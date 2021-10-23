GLOBAL _buildContext , InitFirstProcess
EXTERN irqDispatcher , getCurrentRSP 

%macro pushState 0
	push rax
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro popState 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
	pop rax
%endmacro

; uint64_t _buildContext(uint64_t baseRSP, uint64_t functionAddress);
_buildContext:
    push rbp
    mov rbp, rsp

	mov rsp, rdi

    push 0x0    ; SS
    push rdi    ; RSP
    push 0x206  ; RFLAGS
    push 0x8    ; CS
    push rsi    ; RIP
    pushState

    mov rax, rsp

    mov rsp, rbp
    pop rbp
    ret


InitFirstProcess: 
	
	;mov rdi, 0 ; pasaje de parametro
	;call irqDispatcher

	call getCurrentRSP;
	mov rsp , rax ;; cambio de contexto.

	popState
	sti ;;set interrupts

	iretq