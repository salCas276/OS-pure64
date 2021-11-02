#include "include/utils.h"
#include "include/lib.h"
#include "include/string.h"


// static void askAndRead(char* buffer, char* text);

void api_createReg(_ARGUMENTS,int foreground){
    if(argc != 2){
        print_f(1, "Los parametros no son validos\n");
        return;
    }
    if(createReg(argv[1]) == -1){
        print_f(1, "Hubo un problema con la creacion\n");
        return;
    }
}

void api_createFifo(_ARGUMENTS,int foreground){
    if(argc != 2){
        print_f(1, "Los parametros no son validos\n");
        return;
    }
    if(createFifo(argv[1]) == -1){
        print_f(1, "Hubo un problema con la creacion\n");
        return;
    }
}

void api_unlink(_ARGUMENTS,int foreground){
    if(argc != 2)
        return;
    if(unlink(argv[1]) == -1){
        print_f(1, "Hubo en error desvinculando el archivo\n");
        return;
    }
    print_f(1, "Este sera desvinculado cuando todas sus aperturas se hayan cerrado\n");
}

void api_open(_ARGUMENTS,int foreground){
    int pid = -1, pos = 1;
    if ( argc != 3){
        if ( argc != 4 || argv[1][0] != '-' || argv[1][1] != 'p' || argv[1][2] != '='){
            print_f(1, "Los parametros no son validos\n");
            return;
        }
        pid = strtoint(&argv[1][3], NULL, 10);
        pos = 2;
    }
    int fd = open(pid, argv[pos+1], strtoint(argv[pos], NULL, 10));
    if(fd == -1){
        print_f(1, "Hubo un error en la creacion\n");
        return;
    }
    print_f(1, "El fd correspondiente es: %d", fd);
}

void api_close(_ARGUMENTS,int foreground){
    int pid = -1, pos = 1;
    if ( argc != 2){
        if ( argc != 3 || argv[1][0] != '-' || argv[1][1] != 'p' || argv[1][2] != '='){
            print_f(1, "Los parametros no son validos\n");
            return;
        }
        pid = strtoint(&argv[1][3], NULL, 10);
        pos = 2;
    }
    if(close(pid, strtoint(argv[pos], NULL, 10)) == -1){
        print_f(1, "Hubo un error con el cerrado\n");
        return;
    }
}

void api_read(_ARGUMENTS,int foreground){
    int pid = -1, pos = 1;
    if ( argc != 3){
        if ( argc != 4 || argv[1][0] != '-' || argv[1][1] != 'p' || argv[1][2] != '='){
            print_f(1, "Los parametros no son validos\n");
            return;
        }
        pid = strtoint(&argv[1][3], NULL, 10);
        pos = 2;
    }
    char buf[BUFFER_SIZE];
    if(read(pid, strtoint(argv[pos], NULL, 10), buf, strtoint(argv[pos+1], NULL, 10)) == -1){
        print_f(1, "Hubo un problema con la lectura\n");
        return;
    }
    write(-1, 1, buf, BUFFER_SIZE);
}

void api_write(_ARGUMENTS,int foreground){
    int pid = -1, pos = 1;
    if ( argc != 2){
        if ( argc != 3 || argv[1][0] != '-' || argv[1][1] != 'p' || argv[1][2] != '='){
            print_f(1, "Los parametros no son validos\n");
            return;
        }
        pid = strtoint(&argv[1][3], NULL, 10);
        pos = 2;
    }
    char buf[BUFFER_SIZE];
    //askAndRead(buf, "Ingrese a continuacion lo que desee escribir:");
    int flag = 1;
    while(flag){
        flag = read(pid, 0, buf, BUFFER_SIZE-1);
        if(flag)
            buf[BUFFER_SIZE] = 0;
        if(write(pid, strtoint(argv[pos], NULL, 10), buf, strlen(buf)) == -1){
            print_f(1, "Hubo un problema con la escritura\n");
            return;
        }
    }
}

void api_dup(_ARGUMENTS,int foreground){
    int pid = -1, pos = 1;
    if ( argc != 2){
        if ( argc != 3 || argv[1][0] != '-' || argv[1][1] != 'p' || argv[1][2] != '='){
            print_f(1, "Los parametros no son validos\n");
            return;
        }
        pid = strtoint(&argv[1][3], NULL, 10);
        pos = 2;
    }

    int ret = dup(pid, strtoint(argv[pos], NULL, 10));
    if(ret == -1){
        print_f(1, "Hubo un error");
        return;
    }
    print_f(1, "El nuevo fd es: %d\n", ret);
}

void api_dup2(_ARGUMENTS,int foreground){
    int pid = -1, pos = 1;
    if ( argc != 3){
        if ( argc != 4 || argv[1][0] != '-' || argv[1][1] != 'p' || argv[1][2] != '='){
            print_f(1, "Los parametros no son validos\n");
            return;
        }
        pid = strtoint(&argv[1][3], NULL, 10);
        pos = 2;
    }
    int ret = dup2(pid, strtoint(argv[pos], NULL, 10), strtoint(argv[pos+1], NULL, 10));
    if(ret == -1){
        print_f(1, "Hubo un error");
        return;
    }
}

void api_printFileContent(_ARGUMENTS,int foreground){
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

void api_printFileInfo(_ARGUMENTS,int foreground){
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

void api_printFdTableByPid(_ARGUMENTS,int foreground){
    int pid = -1;
    if ( argc != 1){
        if ( argc != 2 || argv[1][0] != '-' || argv[1][1] != 'p' || argv[1][2] != '='){
            print_f(1, "Los parametros no son validos\n");
            return;
        }
        pid = strtoint(&argv[1][3], NULL, 10);
    }
    int buf[BUFFER_SIZE];
    int count = getFdTableByPid(pid, buf);
    if(count == -1){
        print_f(1, "Hubo un problema con la obtencion de los datos\n");
        return;
    }
    print_f(1, "V|R\n");
    for(int i = 0; i < 10; i++){
        print_f(1, "%d|%d\n", i, buf[i]);
    }
}

void api_printFifosData(_ARGUMENTS,int foreground){
   fifoData* fifosBuf = memalloc(sizeof(fifoData)*MAX_PRINTING_FILES);
   if(fifosBuf <= 0){
       print_f(1, "Hubo un error al reservar memoria\n");
       return;
   }

   int count = getFifosData(fifosBuf);

   if(count == -1){
       print_f(1, "Hubo un problema con la obtencion de las datos\n");
       memfree(fifosBuf);
       return;
   }

   for(int i = 0; i < count; i++){
        print_f(1, "\n------------%s-------------\n", fifosBuf[i].name);
        print_f(1, "Openings: %d ----- W Openings: %d\n", fifosBuf[i].openCount, fifosBuf[i].writeOpenCount);
        print_f(1, "R Index: %d ----- W Index: %d\n", fifosBuf[i].indexes[0], fifosBuf[i].indexes[1]);
        print_f(1, "For Unlink: %s\n", fifosBuf[i].forUnlink ? "true" : "false");
        int j = 0;
        print_f(1, "Bloqueados por lectura: ");
        while(fifosBuf[i].blockedPids[j] != -1) print_f(1, "%d-", fifosBuf[i].blockedPids[j++]);
        print_f(1, "\n");
        j++;
        print_f(1, "Bloqueados por escritura: ");
        while(fifosBuf[i].blockedPids[j] != -1) print_f(1, "%d-", fifosBuf[i].blockedPids[j++]);
        print_f(1, "\n");
        j++;
        print_f(1, "Bloqueados por sem de lectura: ");
        while(fifosBuf[i].blockedPids[j] != -1) print_f(1, "%d-", fifosBuf[i].blockedPids[j++]);
        print_f(1, "\n");
        j++;
        print_f(1, "Bloqueados por sem de escritura: ");
        while(fifosBuf[i].blockedPids[j] != -1) print_f(1, "%d-", fifosBuf[i].blockedPids[j++]);
        print_f(1, "\n");
    }
    memfree(fifosBuf);
}

// static void askAndRead(char* buffer, char* text){
//     print_f(1, "%s\n", text);
//     read(-1, 0, buffer, BUFFER_SIZE);
// }


