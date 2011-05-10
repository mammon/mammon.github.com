BITS 32
GLOBAL szHello
GLOBAL main
EXTERN printf
EXTERN ioperm

SECTION .data
szText1 db 'Enabling I/O Port Access',0Ah,0Dh,0
szText2 db 'Disabling I/O Port Acess',0Ah,0Dh,0
szDone  db 'Done!',0Ah,0Dh,0
szError db 'Error in ioperm() call!',0Ah,0Dh,0
szEqual db 'Output/Input bytes are equal.',0Ah,0Dh,0
szChange db 'Output/Input bytes changed.',0Ah,0Dh,0

SECTION .text

main:
   ;Enable I/O Port Access
   push dword szText1
	call printf
	pop ecx
enable_IO:
   push word 1    ; enable mode
	push dword 04h ; four ports
	push dword 40h ; start with port 40
	call ioperm    ; Must be SUID "root" for this call!
	add ESP, 10
	cmp eax, 0
	jne Error
	
	; Port manipulation goes here
	mov bl, 0EEh   ; value to send to speaker timer
SetControl:
   mov al, 96     ; R/W low byte of Counter2, mode 3
	out 43h, al
WritePort:
   mov al, bl
   out 42h, al
ReadPort:
   in al, 42h
	cmp al, bl
	jne ByteChanged
BytesEqual:
   push dword szEqual
	call printf
	pop ecx
	jmp disable_IO
ByteChanged:
   push dword szChange
	call printf
	pop ecx

disable_IO:
	;Disable I/O Port Access : the system will do this if we don't
	push dword szText2
	call printf
	pop ecx       
	push word 0    ; disable mode
	push dword 04h ; four ports
	push dword 40h ; start with port 40h	
	call ioperm
	pop ecx
	pop ecx
	pop cx
	cmp eax, 0
	jne Error
	jmp Exit
Error:
   push dword szError
	call printf
	pop ecx
Exit:
	ret

