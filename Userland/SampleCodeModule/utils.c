#include <utils.h>
#include <lib.h>
#include <cpuid.h>
#include <cpuidflags.h>
#include <processApi.h>
#include <string.h>
#include <testSem.h>

#define BUFFER_SIZE 16
#define MAX_PROCS 5
#define ISHEXA(x) (((x) >= 'a' && (x) <= 'f') || ((x) >= 'A' && (x) <= 'F') || ISDIGIT(x))

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
    print_f(1, " - mkfile <filename>: Crea un file en el file system\n");
    print_f(1, " - printFileContent <filename>: Obtengo el contenido escrito en un elemento del file system\n");
    print_f(1, " - printFileInfo <filename>: Obtengo la informacion del inode de un elemento del file system\n");
    print_f(1, " - open <-m> <filename>: Abre un archivo preexistente en el file system para escritura y/o lectura\n");
    print_f(1, " - close <fd>: Cierra un archivo previamente abierto del file system\n");
    print_f(1, " - unlink <filename>: Elimina un archivo del file system una vez que todas sus aperturas sean cerradas\n");
    print_f(1, " - dup <oldfd>: toma un fd y crea otro nuevo que apunta a la misma apertura\n");
    print_f(1, " - dup2 <oldfd> <newfd>: toma un fd viejo y uno nuevo que apuntara a la misma apertura\n");

}

void printmem(_ARGUMENTS) {
    uint8_t arr[BYTES_TO_READ] = {0};
    char buffer[BUFFER_SIZE + 1] = {0};
    uint64_t dir = 0;
    int ans;

    do {
        print_f(1, "Ingrese una direccion de 64 bits a partir de la cual leer:\n0x");
        ans = get_s(buffer, BUFFER_SIZE);
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
        ans = get_s(buffer, 100);
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


void aux(void);

void auxa(int argc,char* argv[]);
void auxb(int argc,char * argv[]);


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
    int count = getProcessesData(descriptorArray);
    print_f(1, "  PID\n\n");
    for(int i = 0; i < count; i++){
        print_f(1, "  %d\n\n", (descriptorArray+i)->pid);
    }
    memfree(descriptorArray);
}
//--------------------------------------------------------------
void printHola(_ARGUMENTS,int foreground){

    createProcessUserland( (uint64_t) &auxa,argc,argv,foreground);
    createProcessUserland( (uint64_t) &auxb,argc,argv,foreground);

    if(foreground){
        waitSon();
        waitSon();
    }
}


void auxa(int argc,char * argv[]){
    int i = 0; 
    while(1) {
         for(int i=0; i<10000000; i++); 
         print_f(1,"%s #%d\n",argv[1], i++);
    }
}

void auxb(int argc,char * argv[]){
    int i = 0; 
    while(1) {
         for(int i=0; i<10000000; i++); 
         print_f(1,"%s #%d\n",argv[1], i++);
    }
}

void createFile(_ARGUMENTS){
    if(argc != 2)
        return;
    createFileAsm(argv[1]);
}

void createFifo(_ARGUMENTS){
    if(argc != 2)
        return;
    createFifoAsm(argv[1]);
}

void printFileContent(_ARGUMENTS){
    if(argc != 2)
        return;

    char* buf = memalloc(MAX_SIZE_BLOCK); 
    if(getFileContent(argv[1], buf) == -1){
        print_f(1, "No existe archivo con ese nombre\n");
        memfree(buf);
        return;
    }

    print_f(1, "%s\n", buf);
    memfree(buf);
}

void printFileInfo(_ARGUMENTS){
    if(argc != 2)
        return;
    fileInfo* buf = memalloc(sizeof(fileInfo));
    if(getFileInfo(argv[1], buf) == -1){
        print_f(1, "No existe archivo con ese nombre\n");
        memfree(buf);
        return;
    }

    char* auxFileType;
    switch (buf->fileType)
    {
    case 0:
        auxFileType = "keyboard";
        break;
    case 1:
        auxFileType = "console";
        break;
    case 2:
        auxFileType = "regular";
        break;
    case 3:
        auxFileType = "fifo";
        break;
    }

    print_f(1, "------------%s-----------\n", argv[1]);
    print_f(1, "Read Index: %d\n", buf->indexes[0]);
    print_f(1, "Write Index: %d\n", buf->indexes[1]);
    print_f(1, "Opening Number: %d\n", buf->openCount);
    print_f(1, "Writer Number: %d\n", buf->writeOpenCount);
    print_f(1, "File Type: %s\n", auxFileType);
    print_f(1, "For unlink: %s\n", buf->forUnlink ? "True" : "False");

    memfree(buf);
}

void printOpen(_ARGUMENTS){
    if(argc != 3 || argv[1][0] != '-' || argv[1][1] != 'm' || argv[1][2]!='=')
        return;
    int fd = openAsm(argv[2], strtoint(&argv[1][3], NULL, 10));
    if(fd == -1){
        print_f(1, "Hubo un error en la creacion\n");
        return;
    }
    print_f(1, "El fd correspondiente es: %d", fd);
}

void printClose(_ARGUMENTS){
    if(argc != 2)
        return;
    if(closeAsm(strtoint(argv[1], NULL, 10)) == -1){
        print_f(1, "Hubo un error con el cerrado\n");
        return;
    }
}

void printUnlink(_ARGUMENTS){
    if(argc != 2)
        return;
    if(unlinkAsm(argv[1] == -1)){
        print_f(1, "Hubo en error desvinculando el archivo\n");
        return;
    }
    print_f(1, "Este sera desvinculado cuando todas sus aperturas se hayan cerrado\n");
}

void dup(_ARGUMENTS){
    if(argc != 2)
        return;
    int buf[40], count;
    int ret = dupAsm(strtoint(argv[1], NULL, 10), buf, &count);
    if(ret == -1){
        print_f(1, "Hubo un error");
        return;
    }
    print_f(1, "V|R\n");
    for(int i=0; i<8; i++){
        print_f(1, "%d|%d\n", i, buf[i]);
    }
}

void dup2(_ARGUMENTS){
    if(argc != 3)
        return;
    int buf[40], count;
    int ret = dup2Asm(strtoint(argv[1], NULL, 10), strtoint(argv[2], NULL, 10), buf, &count);
    if(ret == -1){
        print_f(1, "Hubo un error");
        return;
    }
    print_f(1, "V|R\n");
    for(int i=0; i<8; i++){
        print_f(1, "%d|%d\n", i, buf[i]);
    }
}

void writeFifo(_ARGUMENTS){
    if(argc != 2)
        return;
    char* buf = "Este es un mensaje secreto";
    if(writeFifoAsm(strtoint(argv[1], NULL, 10), buf, strlen(buf)+1) == -1){
        print_f(1, "No se pudo escribir correctamente\n");
        return;
    }
}

void printReadFifo(_ARGUMENTS){
    if(argc != 2)
        return;
    char buf[30];
    if(readFifoAsm(strtoint(argv[1], NULL, 10), buf, 28) == -1){
        print_f(1, "No se pudo leer correctamente\n");
        return;
    }
    print_f(1, "%s\n", buf);
}

static void askAndRead(char* buffer, char* text){
    int ans;
     do {
        print_f(1, "%s\n", text);
        ans = get_s(buffer, BUFFER_SIZE);
    } while (ans == -1);
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


//TO BE INCLUDED
void endless_loop(){
  while(1);
}

#define MAX_PROCESSES 16 //Should be around 80% of the the processes handled by the kernel

enum State {ERROR, RUNNING, BLOCKED, KILLED};

typedef struct P_rq{
  uint32_t pid;
  enum State state;
}p_rq;

void test_processes(_ARGUMENTS,int foreground){
  p_rq p_rqs[MAX_PROCESSES];
  uint8_t rq;
  uint8_t alive = 0;
  uint8_t action;

  while (1){

    // Create MAX_PROCESSES processes
    for(rq = 0; rq < MAX_PROCESSES; rq++){
      p_rqs[rq].pid = createProcessUserland( (uint64_t) &endless_loop, argc , argv ,foreground);  // TODO: Port this call as required

      if (p_rqs[rq].pid == -1){                           // TODO: Port this as required
        print_f(1,"Error creating process\n");               // TODO: Port this as required
        return;
      }else{
        p_rqs[rq].state = RUNNING;
        alive++;
      }
    }

    // Randomly kills, blocks or unblocks processes until every one has been killed
    while (alive > 0){

      for(rq = 0; rq < MAX_PROCESSES; rq++){
        action = GetUniform(2) % 2; 

        switch(action){
          case 0:
            if (p_rqs[rq].state == RUNNING || p_rqs[rq].state == BLOCKED){
              if (kill(0,p_rqs[rq].pid) == -1){          // TODO: Port this as required
                print_f(1,"Error killing process\n");        // TODO: Port this as required
                return;
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
                return;
              }
              p_rqs[rq].state = BLOCKED; 
            }
            break;
        }
      }

      // Randomly unblocks processes
      for(rq = 0; rq < MAX_PROCESSES; rq++)
        if (p_rqs[rq].state == BLOCKED && GetUniform(2) % 2){
          if(kill(2,p_rqs[rq].pid) == -1){            // TODO: Port this as required
            print_f(1,"Error unblocking process\n");         // TODO: Port this as required
            return;
          }
          p_rqs[rq].state = RUNNING; 
        }
    } 
  }

exitUserland();

}

int test_processes_wrapper(_ARGUMENTS,int foreground){
  
  
int pid = createProcessUserland((uint64_t)&test_processes , 0 , (char**)0 , 0);

nice(pid , -3);
//nice(pid , -20);

  if(foreground)
      waitSon(); 

  return 0;
}


//-------------

void sem(_ARGUMENTS,int foreground){

  printSemaphore();

}
