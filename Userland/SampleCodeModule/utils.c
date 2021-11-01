#include <utils.h>
#include <lib.h>
#include <cpuid.h>
#include <cpuidflags.h>
#include <processApi.h>
#include <string.h>
#include <testSem.h>
#include <string.h>


#define MAX_PROCS 20
#define ISHEXA(x) (((x) >= 'a' && (x) <= 'f') || ((x) >= 'A' && (x) <= 'F') || ISDIGIT(x))

void * memset(void*destiation,int32_t c, uint64_t length);
static void askAndRead(char* buffer, char* text);

void printDate(_ARGUMENTS) {
	dateType currDate;
	fillDate(&currDate);
	print_f(1, "Date: %d/%d/%d\nTime: %d:%d:%d (UTC)\n", currDate.day, 
                                                         currDate.month, 
                                                         currDate.year + 2000, 
                                                         currDate.hour, 
                                                         currDate.minute, 
                                                         currDate.second);
}

void help(_ARGUMENTS) {
    print_f(1, "Los comandos disponibles son:\n");
    print_f(1, " - help: Muestra los comandos disponibles\n");
    print_f(1, " - inforeg: Muestra el estado de los registros\n");
    print_f(1, " - printmem: Imprime la memoria a partir de la direccion dada\n");
    print_f(1, " - printDate: Imprime informacion acerca del tiempo\n");
    print_f(1, " - printFeatures: Muestra caracteristicas del microprocesador\n");
    print_f(1, " - printQuadraticRoots: Resuelve una funcion cuadratica\n");
   // print_f(1, " - invalidOpcode: Genera excepcion por operacion invalida\n");
   //  print_f(1, " - divisionByZero: Genera excepcion por division por 0\n");
    print_f(1, " - echo: Imprime un argumento. \n     -m: en mayuscula.\n");
    print_f(1, " - ps: Imprime una lista con los procesos actuales y sus datos\n");
    print_f(1, " - nice: Modifica la prioridad de un proceso.\n     -p=PID: pid.\n     -b=B: bonus a agregar.\n");
    print_f(1, " - kill: Bloquea o mata un proceso.\n     -k=: 0 para matar, 1 para bloquear, 2 para desbloquear.\n");
    print_f(1, " - mkfifo <filename>: Crea un pipe con nombre en el file system\n");
    print_f(1, " - mkreg <filename>: Crea un file en el file system\n");
    print_f(1, " - unlink <filename>: Elimina un archivo del file system una vez que todas sus aperturas sean cerradas\n");
    print_f(1, " - open [-p] <m> <filename>: Abre un archivo preexistente en el file system para escritura y/o lectura\n");
    print_f(1, " - close [-p] <fd>: Cierra un archivo previamente abierto del file system\n");
    print_f(1, " - write [-p] <fd>: Escribe el texto ingresado en el fd idicado\n");
    print_f(1, " - read [-p] <fd> <count>: Lee una cantidad de bytes count del fd idicado\n");
    print_f(1, " - dup [-p] <oldfd>: Toma un fd y crea otro nuevo que apunta a la misma apertura\n");
    print_f(1, " - dup2 [-p] <oldfd> <newfd>: Toma un fd viejo y uno nuevo que apuntara a la misma apertura\n");
    print_f(1, " - printFileContent <filename>: Imprime el contenido escrito en un elemento del file system\n");
    print_f(1, " - printFileInfo <filename>: Imprime la informacion del inode de un elemento del file system\n");
    print_f(1, " - printFdTable [-p]: Imprimie la tabla de file descriptors del proces indicado\n");
    print_f(1, " - pipe: Imprime informacion sobre todos los pipes con nombre abiertos\n");

}

void printmem(_ARGUMENTS) {
    uint8_t arr[BYTES_TO_READ] = {0};
    char buffer[BUFFER_SIZE + 1] = {0};
    uint64_t dir = 0;
    int ans;

    do {
        print_f(1, "Ingrese una direccion de 64 bits a partir de la cual leer:\n0x");
        ans = read(-1, 0, buffer, 100);
    } while (ans == -1);
    
    for (int i = 0; i < ans; i++) {
        if (!ISHEXA(buffer[i])) {
            print_f(1, "\nNo es una direccion valida\n");
            return;
        }
    }

    sscan(buffer, "%x", &dir);

    // fillMem debería devolver la cantidad de bytes leídos correctamente
    // Actualmente como no hay validación, siempre es 32
    // print_f(1, "%x\n", fillMem(dir, arr, BYTES_TO_READ));
    fillMem(dir, arr, BYTES_TO_READ);
    for (int i = 0; i < BYTES_TO_READ; i++)
        print_f(1, "\n0x%x: %xh", dir + i, arr[i]);
    
    print_f(1, "\n");
}

void printFeatures(_ARGUMENTS) {
    // Chequear si se pude usar cpuig
    uint32_t eax, ebx, ecx, edx;

    if (supports_cpuid()){
        print_f(1, "CPUID: YES\n");

        __get_cpuid(1, &eax, &ebx, &ecx, &edx);
        print_f(1, "MX: %s\n", (edx & CPUID_FEAT_EDX_MMX) == 0 ? "NO":"YES");
        print_f(1, "SSE: %s\n", (edx & CPUID_FEAT_EDX_SSE) == 0 ? "NO":"YES");
        print_f(1, "SSE2: %s\n", (edx & CPUID_FEAT_EDX_SSE2) == 0 ? "NO":"YES");
        print_f(1, "SSE3: %s\n", (ecx & CPUID_FEAT_ECX_SSE3) == 0 ? "NO":"YES");
        print_f(1, "SSE41: %s\n", (ecx & CPUID_FEAT_ECX_SSE4_1) == 0 ? "NO":"YES");
        print_f(1, "SSE42: %s\n", (ecx & CPUID_FEAT_ECX_SSE4_2) == 0 ? "NO":"YES");
        print_f(1, "AESNI: %s\n", (ecx & CPUID_FEAT_ECX_AES) == 0 ? "NO":"YES");
        print_f(1, "PCLMULQDQ: %s\n", (ecx & CPUID_FEAT_ECX_PCLMUL) == 0 ? "NO":"YES");
        print_f(1, "AVX: %s\n", (ecx & CPUID_FEAT_ECX_AVX) == 0 ? "NO":"YES");
        print_f(1, "VAESNI: %s\n", supports_vaesni() == 0 ? "NO":"YES");
        print_f(1, "VPCLMULQDQ: %s\n", supports_vpclmulqdq() == 0 ? "NO":"YES");
        print_f(1, "F16C: %s\n", supports_f16c() == 0 ?  "NO":"YES");
        print_f(1, "FMA: %s\n", supports_fma() == 0 ? "NO":"YES");
        print_f(1, "AVX2: %s\n", supports_avx2() == 0 ? "NO":"YES");
    } else {
        print_f(1, "CPUID: NO\n");
    }

}

void printQuadraticRoots(_ARGUMENTS){
    double a, b, c, x1, x2;
    char buffer[100 + 1] = {0};
    int ans;

    do {
        print_f(1, "Ingresar coeficientes a, b y c: ");
        ans = read(-1, 0, buffer, -1);
    } while (ans == -1);

    sscan(buffer, "%g %g %g", &a, &b, &c);

    put_char(1, '\n');

    print_f(1, "%c(x) = %gx%c%c%gx%c%g\n\n", 13, a, 14, b >= 0 ? '+':0, b, c >= 0 ? '+':0, c);
    ans = _quadratic(&a, &b, &c, &x1, &x2);
    switch (ans){
        case 0:
            print_f(1, "Sol = {x%c = %g, x%c = %g}\n", 11, x1, 12, x2);
            break;
        case 1:
            print_f(1, "%c tiene raices complejas.\n", 13);
            break;
        case 2:
            print_f(1, "%c no es una funcion cuadratica.\n", 13);
            break;
    };
}

void echo(_ARGUMENTS) {
    if (argc > 2 && argv[1][0] == '-') {
        if ( argv[1][1] == 'm' ) {
            for (int i=0; argv[2][i]; i++)
                if (argv[2][i] >= 'a' && argv[2][i] <= 'z') argv[2][i] += 'A'-'a'; 
            print_f(1, "%s", argv[2]);
        } 
    } else if (argc >= 2)
        print_f(1, "%s", argv[1]); 
}



void nicecmd(_ARGUMENTS) {
    if ( argc != 3 || argv[1][0] != '-' || argv[1][1] != 'p' || argv[1][2] != '=' ||  argv[2][0] != '-' || argv[2][1] != 'b' || argv[2][2] != '=') return; 
    int pid = strtoint( &argv[1][3], NULL, 10); 
    if ( pid < 0 ) return; 
    int bonus = strtoint(&argv[2][3], NULL, 10); 
    if ( bonus > 19 || bonus < -20 ) return; 
    nice(pid, bonus);
}


void killcmd(_ARGUMENTS) {
    if ( argc != 3 || argv[1][0] != '-' || argv[1][1] != 'k' || argv[1][2] != '=') {
        print_f(2, "Cantidad de parametros invalida\n");
        return; 
    }  
    int pid = strtoint( &argv[2][0], NULL, 10); 
    int sg = strtoint( &argv[1][3], NULL, 10); 

    if (pid == 0) {
        print_f(2, "No puedes matar a la shell\n");
        return; 
    }

    if ( sg >= 3 ) {
        print_f(2, "No existe una senal con ese codigo\n");
        return;  
    }

    if ( kill(sg, pid) != 0 ) {
        print_f(2, "No existe ese pid\n");
    } 
}



void printProcessesData(_ARGUMENTS){
    processDescriptor * descriptorArray = memalloc(MAX_PROCS*sizeof(processDescriptor));
    
    if(!descriptorArray){
      print_f(1,"El sistema no tiene memoria disponible");
      return;
    }

    int count = getProcessesData(descriptorArray);
    print_f(1, "NAME     PID     PRIORITY     FOREGROUND\n");
    for(int i = 0; i < count; i++){
        print_f(1, "%s     %d     %d     %d\n", (descriptorArray+i)->name, (descriptorArray+i)->pid,(descriptorArray+i)->priority,(descriptorArray+i)->foreground);
    }
    memfree(descriptorArray);
}
//--------------------------------------------------------------
void loop(_ARGUMENTS){
    int id = getPid();
    open(-1, "f", 0);
    char buf[10];
    read(-1, 3, buf, 10);
    while(1) {
        print_f(1,"%d----%s\n",id,argv[1]);
         
        for(int i=0 ; i < 1000000;i++)
        renounceUserland();
    }
}

void loop_wrapper(_ARGUMENTS,int foreground){

    createProcessUserland( (uint64_t) &loop,argc,argv,foreground);

    if(foreground){
        waitSon();
    }
}


//------------------------------------------------------------------------------------------


static uint32_t m_z = 362436069;
static uint32_t m_w = 521288629;

uint32_t GetUint(){
  m_z = 36969 * (m_z & 65535) + (m_z >> 16);
  m_w = 18000 * (m_w & 65535) + (m_w >> 16);
  return (m_z << 16) + m_w;
}

uint32_t GetUniform(uint32_t max){
  uint32_t u = GetUint();
  return (u + 1.0) * 2.328306435454494e-10 * max;
}

uint8_t memcheck(void *start, uint8_t value, uint32_t size){
  uint8_t *p = (uint8_t *) start;
  uint32_t i;

  for (i = 0; i < size; i++, p++)
    if (*p != value)
      return 0;

  return 1;
}

//------------------------------------------------------------------

void endless_loop(_ARGUMENTS){
  while(1);
}

#define MAX_PROCESSES 14 

enum State {ERROR, RUNNING, BLOCKED, KILLED};

typedef struct P_rq{
  uint32_t pid;
  enum State state;
}p_rq;

void test_processes(_ARGUMENTS){

  p_rq p_rqs[MAX_PROCESSES];
  uint8_t rq;
  uint8_t alive = 0;
  uint8_t action;


char ** arguments = memalloc(sizeof(char*));

char * name = memalloc(12*sizeof(char));
if(!arguments || !name)
  exitUserland();  
strcpy(name,"endlessLoop");

arguments[0]=name;


  while (1){

    // Create MAX_PROCESSES processes
    for(rq = 0; rq < MAX_PROCESSES; rq++){
      p_rqs[rq].pid = createProcessUserland( (uint64_t) &endless_loop, 1 , arguments ,0);  

      if (p_rqs[rq].pid == -1){                           
        print_f(1,"El sistema no tiene memoria disponible\n");               
        exitUserland();
      }else{
        p_rqs[rq].state = RUNNING;
        alive++;
        renounceUserland();
      }
    }

    // Randomly kills, blocks or unblocks processes until every one has been killed
    while (alive > 0){

      for(rq = 0; rq < MAX_PROCESSES; rq++){
        action = GetUniform(2) % 2 ; 

        switch(action){
          case 0:
            if (p_rqs[rq].state == RUNNING || p_rqs[rq].state == BLOCKED){
              if (kill(0,p_rqs[rq].pid) == -1){          // TODO: Port this as required
                print_f(1,"Error killing process\n");        // TODO: Port this as required
                exitUserland();
              }
              p_rqs[rq].state = KILLED; 
              alive--;
              renounceUserland();
            }
            break;

            case 1:
            if (p_rqs[rq].state == RUNNING){
              if(kill(1,p_rqs[rq].pid) == -1){          // TODO: Port this as required
                print_f(1,"Error blocking process\n");       // TODO: Port this as required
                exitUserland();
              }
              p_rqs[rq].state = BLOCKED;
               renounceUserland(); 
            }
            break;
        }
      }

      // Randomly unblocks processes
      for(rq = 0; rq < MAX_PROCESSES; rq++)
        if (p_rqs[rq].state == BLOCKED && GetUniform(2) % 2){
          if(kill(2,p_rqs[rq].pid) == -1){            // TODO: Port this as required
            print_f(1,"Error unblocking process\n");         // TODO: Port this as required
            exitUserland();
          }
          p_rqs[rq].state = RUNNING; 
           renounceUserland();
        }
    
     
    } 
  }


}

void test_processes_wrapper(_ARGUMENTS,int foreground){
  
char ** arguments = memalloc(sizeof(char*));

arguments[0]=argv[0];

int pid = createProcessUserland((uint64_t)&test_processes , 1 , arguments , foreground);


if(pid < 0){
  print_f(1,"Error al crear el proceso");
  return;
}

nice(pid , -5);

if(foreground)
  waitSon(); 

}


//-------------

void sem(_ARGUMENTS,int foreground){

  semaphoreDescriptor * data = memalloc(20*sizeof(semaphoreDescriptor));
  int quantity = getSemaphoreData((uint64_t)data);
  for(int i = 0 ; i < quantity ; i++){
    print_f(1,"id:%s\n",data[i].sem_id);
    print_f(1,"valor:%d\n",data[i].value);
    print_f(1,"Procesos Bloqueados: ");
    for(int j = 0; data[i].blocked[j] > 0 ;j++){
      print_f(1,"%d ",data[i].blocked[j]);
    }
    memfree(data[i].blocked);
    print_f(1,"\n------\n");
  }


  memfree(data);

}


//-------------

#define TOTAL_PAIR_PROCESSES 2
#define N 1000000
#define SEM_ID "sem"

int64_t global;  //shared memory

void slowInc(int64_t *p, int64_t inc){
  int64_t aux = *p;
  aux += inc;
  renounceUserland();
  *p = aux;
}

void inc(_ARGUMENTS){
  uint64_t i;
  uint64_t sem , value ; 

  sem = strtoint(argv[1],0,10);
  value = strtoint(argv[2],0,10);


  if (sem && openSemaphore(SEM_ID, 1) < 0 ){
    print_f(1,"ERROR OPENING SEM\n");
    return;
  }
  
  for (i = 0; i < N; i++){
    if (sem) waitSemaphore(SEM_ID);
    slowInc(&global, value);
    if (sem) postSemaphore(SEM_ID);
  }

  if (sem) closeSemaphore(SEM_ID);

  exitUserland();
  
}

void test_sync(_ARGUMENTS ){
  uint64_t i;

  global = 0;

  char * buffer ; 

  if(!strcmp(argv[1],"1"))
    print_f(1,"CREATING PROCESSES...(WITH SEM)\n" );
  else 
    print_f(1,"CREATING PROCESSES...(WITH NO SEM)\n" );

  char * arg1 = memalloc(2*sizeof(char));

  if(!arg1){
    print_f(1,"El sistema no tiene memoria disponible");
    memfree(argv);
    exitUserland();
  }

  buffer = "1";
  strcpy(arg1,buffer);

  char * arg2 = memalloc(3*sizeof(char));

  if(!arg2){
    print_f(1,"El sistema no tiene memoria disponible");
    memfree(argv);
    exitUserland();
  }

  buffer = "-1";
  strcpy(arg2,buffer);

  char * nameInc = memalloc(4*sizeof(char));
  strcpy(nameInc,"inc");

  char * argP1[3]={nameInc,argv[1],arg1};
  char * argP2[3]={nameInc,argv[1],arg2};

  for(i = 0; i < TOTAL_PAIR_PROCESSES; i++){
    if(createProcessUserland((uint64_t)&inc, 3,argP1,1) < 0 || createProcessUserland((uint64_t)&inc, 3, argP2,1) < 0 ){
      print_f(1,"El sistema no tiene memoria disponible");
      exitUserland();
    }
  }

   for(int j = 0 ; j < ( i * 2) ; j++)
     waitSon();

  print_f(1,"Final value: %d\n",global);


  memfree(argv);
  exitUserland();

}


void test_sync_wrapper(_ARGUMENTS , int foreground ){

  char * buffer ; 
  char * sem = memalloc(2*sizeof(char));

  if(!sem){
    print_f(1,"El sistema no tiene memoria disponible");
    return;
  }
  
  buffer = "1";
  strcpy(sem,buffer);

  char ** arguments = (char**) memalloc(2*sizeof(char*));
  
  if(!arguments){
    print_f(1,"El sistema no tiene memoria disponible");
    return;
  }


  arguments[0] = argv[0]; 
  arguments[1]= sem;


  if(createProcessUserland((uint64_t )&test_sync , 2 , arguments ,foreground ) < 0 ){
    print_f(1,"El sistema no tiene memoria");
    return;
  }

  if(foreground)
    waitSon();
  
}


void test_no_sync_wrapper(_ARGUMENTS , int foreground ){

  char * buffer ; 
  char * sem = memalloc(2*sizeof(char));

  if(!sem){
    print_f(1,"El sistema no tiene memoria disponible");
    return;
  }

  buffer = "0";
  strcpy(sem,buffer);

  char ** arguments = (char**) memalloc(2*sizeof(char*));
  
  if(!arguments){
    print_f(1,"El sistema no tiene memoria disponible");
    return;
  }

  arguments[0] = argv[0];
  arguments[1]= sem;

  if(createProcessUserland((uint64_t )&test_sync , 2 , arguments ,foreground ) < 0 ){
    print_f(1,"El sistema no tiene memoria");
    return;
  }

  if(foreground)
    waitSon();
  
}


//-------------------------------------------------------------------------------------



#define MAX_BLOCKS 128
#define MAX_MEMORY 838861

typedef struct MM_rq{
  void *address;
  uint32_t size;
}mm_rq;

void test_mm(_ARGUMENTS){
  mm_rq mm_rqs[MAX_BLOCKS];
  uint8_t rq;
  uint32_t total;

  while (1){
    rq = 0;
    total = 0;

    // Request as many blocks as we can
    while(rq < MAX_BLOCKS && total < MAX_MEMORY){
      mm_rqs[rq].size = GetUniform(MAX_MEMORY - total - 1) + 1;
      mm_rqs[rq].address = memalloc(mm_rqs[rq].size); 
      if(!mm_rqs[rq].address)
          break;
      total += mm_rqs[rq].size;
      rq++;
    }

    // Set
    uint32_t i;
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address != NULL)
        memset(mm_rqs[i].address, i, mm_rqs[i].size); 
    // Check
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address != NULL)
        if(!memcheck(mm_rqs[i].address, i, mm_rqs[i].size)) 
          print_f(1,"ERROR!\n"); 

    // Free
    for (i = 0; i < rq; i++)
      if (mm_rqs[i].address != NULL)
        memfree(mm_rqs[i].address); 
  }

}

void test_mm_wrapper(_ARGUMENTS,int foreground){

  char ** arguments = memalloc(sizeof(char*));
  arguments[0]=argv[0];

  if(createProcessUserland((uint64_t)&test_mm,1,arguments,0) < 0 ){
    print_f(1,"El sistema no tiene memoria");
    return;
  }

  if(foreground)
   waitSon();
}



//--------------------------------------------------------------------

#define MINOR_WAIT 10000000// TODO: To prevent a process from flooding the screen
#define WAIT      100000// TODO: Long enough to see theese processes beeing run at least twice


void bussy_wait(uint64_t n){
  uint64_t i;
  for (i = 0; i < n; i++);
  }

void endless_loopPid(){
  uint64_t pid = getPid();

  while(1){
    print_f(1,"%d ",pid);
    bussy_wait(MINOR_WAIT);
  }
}

#define TOTAL_PROCESSES 3

void test_prio(_ARGUMENTS){
  uint64_t pids[TOTAL_PROCESSES];
  uint64_t i;

  char ** arguments = memalloc(sizeof(char*));
  char * name = memalloc(12*sizeof(char));
  strcpy(name,"endlessLoop");
  arguments[0]=name;
  
  for(i = 0; i < TOTAL_PROCESSES; i++){
    pids[i] = createProcessUserland((uint64_t)&endless_loopPid,1,arguments,0);
    if(pids[i] < 0)
      exitUserland();
  }

    for (i = 0; i < 10000; i++);
      renounceUserland();

  print_f(1,"\nCHANGING PRIORITIES...\n");

  for(i = 0; i < TOTAL_PROCESSES; i++){
    switch (i % 3){
      case 0:
         //lowest priority , by default 
        break;
      case 1:
        nice(pids[i], -20); //medium priority
        break;
      case 2:
        nice(pids[i], -20); //highest priority
        nice(pids[i], -20); 
        break;
    }
  }


    for (i = 0; i < 10000; i++);
      renounceUserland();
  bussy_wait(WAIT);
  print_f(1,"\nBLOCKING...\n");

  for(i = 0; i < TOTAL_PROCESSES; i++)
    kill(1,pids[i]);
  
  
    for (i = 0; i < 10000; i++);
      renounceUserland();
  bussy_wait(WAIT);
  print_f(1,"CHANGING PRIORITIES WHILE BLOCKED...\n");
 
  for(i = 0; i < TOTAL_PROCESSES; i++){
    switch (i % 3){
      case 0:
        nice(pids[i],20);
        nice(pids[i],20);
        nice(pids[i],-20);
        break;
      case 1:
        nice(pids[i],20);
        nice(pids[i],20);
        nice(pids[i],-20);
        break;
      case 2:
        nice(pids[i],20);
        nice(pids[i],20);
        nice(pids[i],-20);
        break;
    }
  }

  print_f(1,"UNBLOCKING...\n");

  for(i = 0; i < TOTAL_PROCESSES; i++)
    kill(2,pids[i]);

  
  for (i = 0; i < 10000; i++);
    renounceUserland();
  bussy_wait(WAIT);
  print_f(1,"\nKILLING...\n");

  for(i = 0; i < TOTAL_PROCESSES; i++)
    kill(0,pids[i]);

  exitUserland();
}

void test_prio_wrapper(_ARGUMENTS,int foreground){

  char ** arguments = memalloc(sizeof(char*));
  arguments[0]=argv[0];

  if(createProcessUserland((uint64_t)&test_prio,1,arguments,foreground) < 0){
    print_f(1,"El sistema no tiene memoria");
    return ; 
  }

  if(foreground)
    waitSon();
  
}

void cat(_ARGUMENTS) {  
  char* aux[] = {argv[0], "1"};
  api_write(argc+1, aux);
  exitUserland(); 
}

void cat_wrapper(_ARGUMENTS, int foreground) {
    if(createProcessUserland( (uint64_t) &cat, argc, argv, foreground)  < 0)
      print_f(1,"El sistema no tiene memoria");
    if(foreground)
      waitSon();
}

void wc(_ARGUMENTS) {
  char c; 
  char buf[256];

  read(-1, 0, buf, 256);
  int lineCount = 0, count = 0;

  while(buf[count]){
    if(buf[count] == '\n')
      lineCount++;
    count++;
  }

  print_f(1, "La cantidad de lineas es: %d\n", lineCount);
  exitUserland();
}

void filter(_ARGUMENTS) {
  char buf[256];
  int i = 0, vowCount = 0;
  
  read(-1, 0, buf, 256);

  while(buf[i]){
    buf[i-vowCount] = buf[i];
    if(buf[i] == 'a' || buf[i] == 'e' || buf[i] == 'i' || buf[i] == 'o' || buf[i] == 'u')
      vowCount++;
    i++;
  }
  
  buf[i-vowCount] = 0;

  write(-1, 1, buf, strlen(buf));

  exitUserland(); 
}

void wc_wrapper(_ARGUMENTS, int foreground) {

    if(createProcessUserland( (uint64_t) &wc, argc, argv, foreground)  < 0) // Cambiar aca la funcion a la que apunta 
      print_f(1,"El sistema no tiene memoria");
    
    if(foreground)
      waitSon();
}

void filter_wrapper(_ARGUMENTS, int foreground) {
    if(createProcessUserland( (uint64_t) &filter, argc, argv, foreground)  < 0) // Cambiar aca la funcion a la que apunta 
      print_f(1,"El sistema no tiene memoria");
    if(foreground)
      waitSon();
}

static void askAndRead(char* buffer, char* text){
    print_f(1, "%s\n", text);
    read(-1, 0, buffer, BUFFER_SIZE);
}

void printMemState(_ARGUMENTS, int foreground) {


    memstateType * state = memalloc(sizeof(memstateType)); 
    getMemState(state); 
    print_f(1, "MEMSTATE\nTotal memory: %d\nFree memory: %d\nOccupied memory: %d\n", state->totalMemory, state->free, state->occupied); 
    memfree(state); 

}
