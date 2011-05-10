BITS 32
GLOBAL main
EXTERN printf
EXTERN exit

SECTION .text
main:
	push dword szKmem
    call myfunc
    add esp, 4
	
.exit:	
	push dword 0
	call exit 

myfunc:
    enter 8, 0

    les ebx, [ebp + 8]
    mov eax, [ebx]
    mov [ebp - 8], eax
    mov eax, [ebx + 5]
    mov [ebp], eax
	push dword [ebp + 8]
	push dword szOutput
	call printf
	add esp, 8
	push dword [ebp]
	push dword szOutput
	call printf
	add esp, 8
	push dword [ebp - 8]
	push dword szOutput
	call printf
	add esp, 8

    leave
    ret

SECTION .data 
szKmem 	db '/dev/kmem',0
szOutput	db '%.8X',0Ah,0Dh,0
