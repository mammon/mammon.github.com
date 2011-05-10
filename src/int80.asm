BITS 32
GLOBAL szHello
GLOBAL main
EXTERN printf

SECTION .data
szText1 db 'Getting Current Process ID...',0Ah,0Dh,0
szDone  db 'Done!',0Ah,0Dh,0
szError db 'Error in int 80!',0Ah,0Dh,0
szOutput db '%d',0Ah,0Dh,0

SECTION .text

main:
   push dword szText1
	call printf
	pop ecx
	
GetPID:   
	mov eax, dword 20     ; getpid() syscall
	int 80h
	cmp eax, 0 
	jb Error
	push eax
	push dword szOutput
	call printf
	pop ecx
	pop ecx
	
	push dword szDone
	call printf
	pop ecx
   jmp Exit	
	
Error:
   push dword szError
	call printf
	pop ecx
Exit:
	ret

