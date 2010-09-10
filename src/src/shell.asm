BITS 32
GLOBAL main
EXTERN printf
EXTERN exit
EXTERN malloc
EXTERN free
EXTERN write
EXTERN read

STDOUT equ 0
STDIN  equ 1
STDERR equ 2

SECTION .text
main:

WaitForInput:
	call DisplayPrompt
	
	push dword 1024
	push dword CmdLineBuf
	push dword 0
	call read
	add esp, 12
	mov [CmdSize], eax
	
	call ProcessCmd
	test eax, eax
	jnz WaitForInput
	
	push dword 0
	call exit
	
DisplayPrompt:
	push dword 9
	push dword szPrompt
	push dword 1
	call write
	add esp, 12
	ret

ProcessCmd:
	push dword 2
	push dword CmdLineBuf
	push dword ' '
	call split
	add esp, 12
	mov [ARGC], ebx
	mov [ARGV], eax
	
	mov ecx, eax
	add ecx, 4
	mov ecx, [ecx]				;ecx now = addr of second ARG
	sub ecx, [eax]				;ecx now = length of first ARG
	mov ebx, ecx
	mov edx, InternalCmdTable
	
.checkInternal:
   mov edi, [eax]				;pointer to first substring
	mov esi, [edx]				;pointer to second substring
	cmp esi, 0
	je .notInternal
	repe
	cmpsb	
	jne .next
	jmp [edx + 4]
.next:
	add edx, 8
	mov ecx, ebx
	jmp .checkInternal
.notInternal:
	;xor eax, eax				;return 0 to exit
	ret

	


;INTERNAL COMMANDS
CmdEcho:
	mov ecx, CmdSize
	sub ecx, ebx
	dec ecx
	push ecx
	add eax, 4
	push dword [eax]
	push dword 0
	call write
	add esp, 12
	ret
	
CmdExit:
	push dword 0
	call exit

;    split( char, string, max_split)
;    returns array of pointers into original string
;      e.g. split( ":", "this:that:theother:null\0", NULL)
;           "this\0that\0theother\0null\0"
;           ptrArray[0] = [ptrArray+0] = "this\0"
;           ptrArray[1] = [ptrArray+4] = "that\0"
;           ptrArray[2] = [ptrArray+8] = "theother\0"
;           ptrArray[3] = [ptrArray+C] = "null\0"

split:
	push ebp
	mov ebp, esp
 	mov ecx, [ebp + 8] 		;max# of splits
 	mov edi,	[ebp + 12] 		;pointer to target string
 	mov ebx, [ebp + 16]		;splitchar
	
	xor eax, eax				;zero out eax for later
	mov edx, esp				;save current stack pos.
	push dword edi				;save ptr to first substring
 	cmp ecx, 0					;is #splits NULL?
	jnz do_split
	mov ecx, 0xFFFF			;yes -- set to MAX
	
do_split:						
	mov bh, byte [edi]
	cmp bl, bh					;equal to delimiter?
	je .splitstr
	cmp al, bh					;end of string?
	je .EOS 
	inc edi						;next char
	loop do_split 
.splitstr:
	mov [edi], byte al	;replace split delimiter with "\0"
	inc edi						;move to first char after delimiter
	push edi
	loop do_split				;loop #splits or till EOS

	
.EOS:
	mov ecx, edx
	sub ecx, esp				;get total size of pushed pointers
	push ecx						
	call malloc					;allocate that much space for array
	test eax, eax
	jz .error
	pop ecx						;clear stack
	mov edi, eax				;set destination to beginning of array
	add edi, ecx
	shr ecx, 2					;divide total size/4 [= # of dwords to move]
	mov ebx, ecx				;save count
	
.store:
	sub edi, 4
	pop dword [edi]
	loop .store

.error:
	mov esp, ebp
	pop ebp
	ret

	
SECTION .data progbits alloc write align=4
;szPrompt db '%s%s ',0
;szHost db 'asmshell',0
;szCaret db'>',0
;ptrHost	dd	szHost
;ptrCaret	dd	szCaret
szPrompt db 'asmshell>',0

CmdLineBuf	times	1024 db 0
CmdSize		dd	0
ARGC			dd	0
ARGV			dd	0
ARG0			times 64 db 0

;internal commands
szEcho	db	'ECHO',0
szExit	db 'EXIT',0

InternalCmdTable:
dd szEcho, CmdEcho
dd szExit, CmdExit
dd 0



szOutput	db '%s',0Ah,0Dh,0
szString	db	'name:password:UID:GID:group:home',0
