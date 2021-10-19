#include <utils.h>
#include <lib.h>
#include <cpuid.h>
#include <cpuidflags.h>
#include <ProcessAPI.h>


#define BUFFER_SIZE 16
#define MAX_PROCS 5
#define ISHEXA(x) (((x) >= 'a' && (x) <= 'f') || ((x) >= 'A' && (x) <= 'F') || ISDIGIT(x))



void printDate() {
	dateType currDate;
	fillDate(&currDate);
	print_f(1, "Date: %d/%d/%d\nTime: %d:%d:%d (UTC)\n", currDate.day, 
                                                         currDate.month, 
                                                         currDate.year + 2000, 
                                                         currDate.hour, 
                                                         currDate.minute, 
                                                         currDate.second);
}

void help() {
    print_f(1, "Los comandos disponibles son:\n");
    print_f(1, " - help: Muestra los comandos disponibles\n");
    print_f(1, " - inforeg: Muestra el estado de los registros\n");
    print_f(1, " - printmem: Imprime la memoria a partir de la direccion dada\n");
    print_f(1, " - printDate: Imprime informacion acerca del tiempo\n");
    print_f(1, " - printFeatures: Muestra caracteristicas del microprocesador\n");
    print_f(1, " - printQuadraticRoots: Resuelve una funcion cuadratica\n");
    print_f(1, " - invalidOpcode: Genera excepcion por operacion invalida\n");
    print_f(1, " - divisionByZero: Genera excepcion por division por 0\n");
    print_f(1, " - ps: Imprime una lista con los procesos actuales y sus datos\n");
}

void printmem() {
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

void printFeatures() {
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

void printQuadraticRoots(){
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

void aux(void);

void printHola(){
    createProcessUserland( (uint64_t) &aux);
}

void printProcessesData(){
    processDescriptor * descriptorArray = memalloc(MAX_PROCS*sizeof(processDescriptor));
    int count = getProcessesData(descriptorArray);
    print_f(1, "  PID\n\n");
    for(int i = 0; i < count; i++){
        print_f(1, "  %d\n\n", (descriptorArray+i)->pid);
    }
    memfree(descriptorArray);
}


void aux(void){
    while(1){
        print_f(1,"hola\n");
    }

}

