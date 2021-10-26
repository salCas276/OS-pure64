GLOBAL acquire 
GLOBAL release 

section .text 


acquire:
    push rax 

    _loop:
        mov rax , 1
        xchg rax , qword[rdi]
        cmp rax , 0 
        jne _loop
        
    pop rax 
    ret


;; en rdi tengo la direccion de memoria de lock 
release: 
    mov qword[rdi],0
    ret