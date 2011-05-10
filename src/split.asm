BITS 32
GLOBAL main
EXTERN printf
EXTERN exit
EXTERN malloc
EXTERN free

SECTION .text
main:
	mov ax, ds
	mov es, ax

	push dword szString
	push dword szOutput
	call printf
	add esp, 8
	
	push dword ":"
	push dword szString
	push dword 0
	call split
	add esp, 12
	
	mov ecx, ebx
	mov ebx, eax
printarray:
	push ecx						;printf hoses ecx!!!!!
	push dword [ds:ebx]
	push dword szOutput
	call printf
	add esp, 8
	add ebx, 4					;skip to next array element
	pop ecx
	loop printarray
	
	push dword [ptrarray]
	call free
	add esp, 4

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
	je EOS 
	inc edi						;next char
	loop do_split 
.splitstr:
	mov [edi], byte al	;replace split delimiter with "\0"
	inc edi						;move to first char after delimiter
	push edi
	loop do_split				;loop #splits or till EOS

	
EOS:
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

	
SECTION .data 
szOutput	db '%s',0Ah,0Dh,0
szString	db	'name:password:UID:GID:group:home',0
