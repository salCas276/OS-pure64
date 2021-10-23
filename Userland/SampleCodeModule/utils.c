#include <utils.h>
#include <lib.h>
#include <cpuid.h>
#include <cpuidflags.h>
#include <ProcessAPI.h>
#include <testSem.h>

#define BUFFER_SIZE 16
#define MAX_PROCS 5
#define ISHEXA(x) (((x) >= 'a' && (x) <= 'f') || ((x) >= 'A' && (x) <= 'F') || ISDIGIT(x))



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
    print_f(1, " - kill: Bloquea o mata un proceso.\n     -k=: 0 para matar, 1 para bloquear, 2 para desbloquear.\n ");


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
            print_f(0, "%s", argv[2]);
        } 
    } else if (argc >= 2)
        print_f(0, "%s", argv[1]); 
}


void aux(void);

void auxa(void);
void auxb(void);


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


void printHola(_ARGUMENTS){
    createProcessUserland( (uint64_t) &auxa);
    createProcessUserland( (uint64_t) &auxb);
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


void aux(void){
    while(1) {
        print_f(1,"hola\n");
    }
}

void auxa(void){
    int i = 0; 
    while(1) {
         for(int i=0; i<10000000; i++); 
         print_f(1,"AAAA #%d\n", i++);
    }
}

void auxb(void){
    int i = 0; 
    while(1) {
         for(int i=0; i<10000000; i++); 
         print_f(1,"BBBB #%d\n", i++);
    }
}



//------------
#define TOTAL_PAIR_PROCESSES 1
#define SEM_ID "sem"
#define N 10

 int64_t global;  //shared memory

void slowInc(int64_t *p, int64_t inc){
  int64_t aux = *p;
  aux += inc;
  renounceUserland();
  *p = aux;
}




void semincM1( ){
  uint64_t i;


 openSemaphore(SEM_ID, 1);


  
  for (i = 0; i < N; i++){
    waitSemaphore(SEM_ID);
    slowInc(&global, -1);
    postSemaphore(SEM_ID);
    renounceUserland();
  }

    waitSemaphore(SEM_ID);
    print_f(1,"Final value: %d\n", global);
    postSemaphore(SEM_ID);

    closeSemaphore(SEM_ID);
  
  while(1){
        
    }
}

void seminc1(){
  uint64_t i;

 openSemaphore(SEM_ID, 1);
 

  
  for (i = 0; i < N; i++){
    waitSemaphore(SEM_ID);
    slowInc(&global, 1);
    postSemaphore(SEM_ID);
    renounceUserland();
  }


  
     waitSemaphore(SEM_ID);
     print_f(1,"Final value: %d\n", global);
     postSemaphore(SEM_ID);


  closeSemaphore(SEM_ID);

    while(1){

    }

}

void inc1(){
  uint64_t i;

 for (i = 0; i < N; i++){
    slowInc(&global, 1);
  }

  print_f(1,"Final value: %d\n", global);

  while(1){
        
    }
}


void incM1(){
  uint64_t i;

 for (i = 0; i < N; i++){
   slowInc(&global, -1);
  }

  print_f(1,"Final value: %d\n", global);
  while(1){
        
    }
}


void test_sync(){
  uint64_t i;

  global = 0;

  print_f(1,"CREATING PROCESSES...(WITH SEM)\n");

  for(i = 0; i < TOTAL_PAIR_PROCESSES; i++){
    createProcessUserland((uint64_t)&semincM1);
    createProcessUserland((uint64_t)&seminc1);
    }
}

void test_no_sync(){
  uint64_t i;

  global = 0;

  print_f(1,"CREATING PROCESSES...(WITHOUT SEM)\n");

  for(i = 0; i < TOTAL_PAIR_PROCESSES; i++){
    createProcessUserland((uint64_t)&inc1);
    createProcessUserland( (uint64_t)& incM1);
  }


}
