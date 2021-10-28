#include "memoryManager.h"

#define MEMALLOC 1048576/*0x100000*/

/* 
 * Header de 8 bytes para cada posible bloque de
 * memoria. La direccion retornada por malloc es la
 * inmediata siguiente a este header 
 */  
#define HEADER_SIZE 8

/*
 * Minimo tamaño posible para un bloque 
 * El minimo es 16 bytes porque tenemos headers 
 * de 8 bytes y queremos mantenernos alineados a 
 * 8 bytes
 */
#define MIN_ALLOC_LOG2 4
#define MIN_ALLOC ((size_t)1 << MIN_ALLOC_LOG2)

/*
 * Tamaño del maximo bloque asignable 
 * Coincide con el tamaño del mapa de memoria
 */
#define MAX_ALLOC_LOG2  20 //1 MB
#define MAX_ALLOC ((size_t)1 << MAX_ALLOC_LOG2)

/*
 * x,y bloques de memoria. xRy <-> tam(x) = tam(y). Hay 
 * BUCKET_COUNT clases de equivalencia
 */
#define BUCKET_COUNT (MAX_ALLOC_LOG2 - MIN_ALLOC_LOG2 + 1)

/*
 * Las listas de libres se almacenan como listas doblemente
 * encadenadas. Cada posible tamaño de bloque tiene 
 * asociada una lista. 
 */
typedef struct list_t {
  struct list_t *prev, *next;
} list_t;

/*
 * Todo bucket corresponde a una clase de equivalencia / 
 * posible tamaño de asignacion
 * Todo balde guarda una lista de libres 
 * El bucket con index 0 es aquel para los bloques de tamaño
 * MAX_ALLOC (i.e. todo el espacio de memoria).
 */
static list_t buckets[BUCKET_COUNT];


// Tamaño del minimo bucket alguna vez inicializado (creo) 
// Altura actual del arbol binario 
// Los ancestros por arriba de esta al
static size_t bucket_limit;

/*
 * Arbol binario compacto, escrito en forma de array. 
 * Todo posible bloque asignable de tamaño estrictamente mayor 
 * a MIN_ALLOC tiene un nodo representativo en este arbol. 
 * Obs: un bit en este array = un nodo del arbol 
 *
 * Dado un indice del arbol, la linearizacion permite 
 * moverse al padre o cualquier hijo haciendo aritmetica
 *
 * Dado un indice index: 
 *    - ir al padre:          index = (index - 1) / 2
 *    - ir al hijo izquierdo: index = index*2 + 1
 *    - ir al hijo derecho:   index = index*2 + 2
 *    - ir al hermano:        index = ((index - 1) ^ 1) + 1;
 *
 * Un nodo tiene tres estados, definidos de manera recursiva: 
 *    - UNUSED: ambos hijos estan libres 
 *    - SPLIT: algun hijo esta USED, el otro esta UNUSED
 *    - USED: ningun hijo esta UNUSED 
 *
 * Necesitamos dos bits para guardar esta informacion. 
 * Sin embargo, podemos diferenciar entre UNUSED y USED por
 * contexto, por lo que solo hace falta almacenar SPLIT/!SPLIT,
 * lo cual ocupa solo un bit
 *
 * No guardamos informacion sobre los nodos de tamaño MIN_ALLOC
 * pues nunca estan SPLIT, siempre estan UNUSED o USED 
 * y esta informacion la contiene el padre. 
 * Asi, ahorramos muchisimo espacio y es muy practico. 
 */
static uint8_t node_is_split[(1 << (BUCKET_COUNT - 1)) / 8];


/*
 * Posicion de comienzo de la memoria libre. 
 * Todo asignacion de memoria devuelta por malloc() sera
 * un offset de este puntero entre 0 y MAX_ALLOC
 */
static uint8_t *base_ptr = NULL; 


/*
 * Maxima direccion de memoria alguna vez utilizada por 
 * el asignador de memoria. 
 * La syscall BRK (hay que implementarla) pide mas memoria 
 * al kernel, moviendo este puntero un poco mas para adelante. 
 * La totalidad de la memoria MAX_ALLOC no se reserva de una, 
 * se va corriendo este puntero a medida que se necesite, 
 * siendo el limite, obviamente, base_ptr + MAX_ALLOC
 */
static uint8_t *max_ptr;

/*
 * Funcion para actualizar max_ptr. 
 * Requiere que este implementada la syscall BRK, 
 * por eso esta comentada. 
 * Devuelve true si se pudo mover el puntero 
 * (estaba dentro de lo permitido) o false si 
 * no hay mas movimiento hacia adelante posible 
 */ 
static int update_max_ptr(uint8_t *new_value) {
  if (new_value > max_ptr) {
    // Reemplazar este if feo por una syscall linda que 
    // comente mas abajo 
    if ( new_value > base_ptr + MEMALLOC)
      return 0; 
    // if (brk(new_value)) {
    //   return 0;
    // }
    max_ptr = new_value;
  }
  return 1;
}

/*
 * Devuelve una lista vacía. Como son listas circulares, 
 * una lista vacía es una lista donde ambos extremos
 * apuntan a sí misma 
 */
static void list_init(list_t *list) {
  list->prev = list;
  list->next = list;
}

/*
 * Apendea una entrada a una lista. 
 * Asume que la entrada no esta en ninguna lista
 * pues obreescribe los punteros
 */
static void list_push(list_t *list, list_t *entry) {
  list_t *prev = list->prev;
  entry->prev = prev;
  entry->next = list;
  prev->next = entry;
  list->prev = entry;
}

/*
 * Borra la entrada de cualquiera sea la lista en la que este
 * Asume que la entrada esta en una lista 
 * No hay que proveer la lista pues es circular, por lo que 
 * los punteros se actualizan automaticamente si se elimina el 
 * primer o ultimo elemento
 */
static void list_remove(list_t *entry) {
  list_t *prev = entry->prev;
  list_t *next = entry->next;
  prev->next = next;
  next->prev = prev;
}

/*
 * Devuelve y borra el primer elemento de la lista o NULL
 * si estaba vacia
 */
static list_t *list_pop(list_t *list) {
  list_t *back = list->prev;
  if (back == list) return NULL;
  list_remove(back);
  return back;
}

/*
 * Dado el un indice y un bucket, devuelve la posicion de
 * memoria que ese indice representa en O(1)
 * El #bucket en realidad no es necesario, podemos inferirlo 
 * con un loop, pero no seria O(1) 
 * CREO que seria O(log(n)). Ache igual. 
 */ 
static uint8_t *ptr_for_node(size_t index, size_t bucket) {
  return base_ptr + ((index - (1 << bucket) + 1) << (MAX_ALLOC_LOG2 - bucket));
}

/*
 * Dada una direccion de memoria y un bucket, 
 * devuelve el nodo. El bucket es necesario, pues una
 * misma direccion de memoria puede conllevar bloques de 
 * distinto tamaño 
 */
static size_t node_for_ptr(uint8_t *ptr, size_t bucket) {
  return ((ptr - base_ptr) >> (MAX_ALLOC_LOG2 - bucket)) + (1 << bucket) - 1;
}

/*
 * Dado el indice de un nodo, devuelve si el PADRE esta 
 * partido 
 */
static int parent_is_split(size_t index) {
  index = (index - 1) / 2;
  return (node_is_split[index / 8] >> (index % 8)) & 1;
}

/*
 * Dado el indice de un nodo, cambia el valor de SPLIT
 * del padre
 */
static void flip_parent_is_split(size_t index) {
  index = (index - 1) / 2;
  node_is_split[index / 8] ^= 1 << (index % 8);
}

/*
 * Dado un tamaño, devuelve el indice del bucket mas
 * chico que puede almacenar a ese bloque
*/
static size_t bucket_for_request(size_t request) {
  size_t bucket = BUCKET_COUNT - 1;
  size_t size = MIN_ALLOC;

  while (size < request) {
    bucket--;
    size *= 2;
  }

  return bucket;
}

/*
 * El arbol siempre tiene su raiz en el limite de
 * buckets actual. 
 * Un llamado a esta funcion hace crecer el arbol duplicando 
 * su tamaño hasta que la raiz yazca en el indice del bucket 
 * pasado como parametro 
 * Cada duplicacion baja el bucket limit por 1
 */
static int lower_bucket_limit(size_t bucket) {
  while (bucket < bucket_limit) {
    size_t root = node_for_ptr(base_ptr, bucket_limit);
    uint8_t *right_child;

    /*
     * If the parent isn't SPLIT, that means the node at the current bucket
     * limit is UNUSED and our address space is entirely free. In that case,
     * clear the root free list, increase the bucket limit, and add a single
     * block with the newly-expanded address space to the new root free list.
     */
    if (!parent_is_split(root)) {
      list_remove((list_t *)base_ptr);
      list_init(&buckets[--bucket_limit]);
      list_push(&buckets[bucket_limit], (list_t *)base_ptr);
      continue;
    }

    /*
     * Otherwise, the tree is currently in use. Create a parent node for the
     * current root node in the SPLIT state with a right child on the free
     * list. Make sure to reserve the memory for the free list entry before
     * writing to it. Note that we do not need to flip the "is split" flag for
     * our current parent because it's already on (we know because we just
     * checked it above).
     */
    right_child = ptr_for_node(root + 1, bucket_limit);
    if (!update_max_ptr(right_child + sizeof(list_t))) {
      return 0;
    }
    list_push(&buckets[bucket_limit], (list_t *)right_child);
    list_init(&buckets[--bucket_limit]);

    /*
     * Set the grandparent's SPLIT flag so if we need to lower the bucket limit
     * again, we'll know that the new root node we just added is in use.
     */
    root = (root - 1) / 2;
    if (root != 0) {
      flip_parent_is_split(root);
    }
  }
  return 1;
}

void * malloc(size_t request) {

  size_t original_bucket, bucket;

  // Validar si el tamaño pedido es posible 
  if (request + HEADER_SIZE > MAX_ALLOC) return NULL;
  

  /*
   * Si es el primer llamado a malloc() hay que 
   * inicializar el estado global del mem manager 
   * Al principo, el arbol tiene un solo nodo que representa 
   * el tamaño mas chico posible asignable 
   * Mas memoria se reserva despues de ser necesario 
   */
  if (base_ptr == NULL) {
    
    // Esto es muy pro. Por el momento hardcodeamos base_ptr 
    // base_ptr = max_ptr = (uint8_t *)brk(0);
    max_ptr = base_ptr = ((uint8_t *)MEMBASE);  // uint8_t *
    bucket_limit = BUCKET_COUNT - 1;

    int i = update_max_ptr(base_ptr + sizeof(list_t));

    list_init(&buckets[BUCKET_COUNT - 1]);

    list_push(&buckets[BUCKET_COUNT - 1], (list_t *)base_ptr);
  }

  // BUCKETS [][][][][][][][][][.]
  // bucket            128|
  // orgbuck           128|
  /*
   * Encuentra el bucket con el minimo tamaño para 
   * satisfacer este pedido 
   * Aun no chequea que haya espacio disponible. 
   */
  bucket = bucket_for_request(request + HEADER_SIZE);
  original_bucket = bucket;


  /*
   * Busca un bucket con una lista no vacia que sea lo 
   * suficientemente grande o mas grande de lo que necesitamos
   * Si no hay un bucket exacto, tenemos que splitear un 
   * cacho mas grande para tener un match
   */
  while (bucket + 1 != 0) { 
    size_t size, bytes_needed, i;
    uint8_t *ptr;
  

  /*
   * Puede que necesitemos hacer crecer al arbol 
   * Trata de hacerlo crecer y si no podes retorna null.
   */
    if (!lower_bucket_limit(bucket)) { // No podes mover 
      return NULL;
    }



  /*
   * Trata de hacer un pop de un bloque de la lista libre
   * Si devuelve null, vas a tener que romper 
   * un bloque mas grande 
   */
    ptr = (uint8_t *)list_pop(&buckets[bucket]); 



    if (!ptr) {
      /*
       * Si no estamos en la raiz del arbol o es imposible 
       * hacer crecer al arbol, continua al siguiente bucket.
       */
      if (bucket != bucket_limit || bucket == 0) {
        bucket--;
        continue;
      }

      /*
       * 
       * Otherwise, grow the tree one more level and then pop a block off the
       * free list again. Since we know the root of the tree is used (because
       * the free list was empty), this will add a parent above this node in
       * the SPLIT state and then add the new right child node to the free list
       * for this bucket. Popping the free list will give us this right child.
       */
      if (!lower_bucket_limit(bucket - 1)) {
        return NULL;
      }
      ptr = (uint8_t *)list_pop(&buckets[bucket]);
    }

    /*
     * Trata de expandir la direccion. 
     * Si nos quedamos sin espacio, pone el bloque 
     * devuelta en la lista libre y falla
     */
    size = (size_t)1 << (MAX_ALLOC_LOG2 - bucket);
    bytes_needed = bucket < original_bucket ? size / 2 + sizeof(list_t) : size;
    if (!update_max_ptr(ptr + bytes_needed)) {
      list_push(&buckets[bucket], (list_t *)ptr);
      return NULL;
    }

    /*
     * If we got a node off the free list, change the node from UNUSED to USED.
     * This involves flipping our parent's "is split" bit because that bit is
     * the exclusive-or of the UNUSED flags of both children, and our UNUSED
     * flag (which isn't ever stored explicitly) has just changed.
     *
     * Note that we shouldn't ever need to flip the "is split" bit of our
     * grandparent because we know our buddy is USED so it's impossible for our
     * grandparent to be UNUSED (if our buddy chunk was UNUSED, our parent
     * wouldn't ever have been split in the first place).
     */
    i = node_for_ptr(ptr, bucket);
    if (i != 0) {
      flip_parent_is_split(i);
    }

    /*
     * If the node we got is larger than we need, split it down to the correct
     * size and put the new unused child nodes on the free list in the
     * corresponding bucket. This is done by repeatedly moving to the left
     * child, splitting the parent, and then adding the right child to the free
     * list.
     */
    while (bucket < original_bucket) {
      i = i * 2 + 1;
      bucket++;
      flip_parent_is_split(i);
      list_push(&buckets[bucket], (list_t *)ptr_for_node(i + 1, bucket));
    }

    /*
     * Now that we have a memory address, write the block header (just the size
     * of the allocation) and return the address immediately after the header.
     */
    *(size_t *)ptr = request;
    return ptr + HEADER_SIZE;
  }
  return NULL; 
}

void free(void *ptr) {
  size_t bucket, i;

  /*
   * Ignore any attempts to free a NULL pointer.
   */
  if (!ptr) {
    return;
  }

  /*
   * We were given the address returned by "malloc" so get back to the actual
   * address of the node by subtracting off the size of the block header. Then
   * look up the index of the node corresponding to this address.
   */
  ptr = (uint8_t *)ptr - HEADER_SIZE;
  bucket = bucket_for_request(*(size_t *)ptr + HEADER_SIZE);
  i = node_for_ptr((uint8_t *)ptr, bucket);

  /*
   * Traverse up to the root node, flipping USED blocks to UNUSED and merging
   * UNUSED buddies together into a single UNUSED parent.
   */
  while (i != 0) {
    /*
     * Change this node from UNUSED to USED. This involves flipping our
     * parent's "is split" bit because that bit is the exclusive-or of the
     * UNUSED flags of both children, and our UNUSED flag (which isn't ever
     * stored explicitly) has just changed.
     */
    flip_parent_is_split(i);

    /*
     * If the parent is now SPLIT, that means our buddy is USED, so don't merge
     * with it. Instead, stop the iteration here and add ourselves to the free
     * list for our bucket.
     *
     * Also stop here if we're at the current root node, even if that root node
     * is now UNUSED. Root nodes don't have a buddy so we can't merge with one.
     */
    if (parent_is_split(i) || bucket == bucket_limit) {
      break;
    }

    /*
     * If we get here, we know our buddy is UNUSED. In this case we should
     * merge with that buddy and continue traversing up to the root node. We
     * need to remove the buddy from its free list here but we don't need to
     * add the merged parent to its free list yet. That will be done once after
     * this loop is finished.
     */
    list_remove((list_t *)ptr_for_node(((i - 1) ^ 1) + 1, bucket));
    i = (i - 1) / 2;
    bucket--;
  }

  /*
   * Add ourselves to the free list for our bucket. We add to the back of the
   * list because "malloc" takes from the back of the list and we want a "free"
   * followed by a "malloc" of the same size to ideally use the same address
   * for better memory locality.
   */
  list_push(&buckets[bucket], (list_t *)ptr_for_node(i, bucket));
}

uint8_t * getMemBase() {
  return base_ptr; 
}