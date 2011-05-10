BITS 32
GLOBAL main

EXTERN write
EXTERN open
EXTERN close
EXTERN dup
EXTERN execlp
EXTERN printf
EXTERN ioctl

%define O_RDWR 2
%define VT_OPENQRY   0x5600    ;find available vt
%define VT_ACTIVATE  0x5606    ;make vt active
%define VT_WAITACTIVE   0x5607    ;wait for vt active
%define VT_DISALLOCATE  0x5608   ;free memory associated to vt
					 


section .code
main:
		  push dword 0
		  push dword O_RDWR
		  push dword szTty
		  call open
		  add esp, 12
		  ;cmp eax, 0
		  ;jl
		  mov [hVC], eax

		  ;push dword vstat
		  ;push dword VT_GETSTATE
		  ;push dword [hVC]
		  ;call ioctl
		  ;add esp, 12

		  
		  push dword VCNum 
		  push dword VT_OPENQRY
		  push dword [hVC]
		  call ioctl
		  add esp, 12
		  cmp eax, 0
		  jb  _out


		  push dword VCNum
		  push dword VT_ACTIVATE
		  push dword [hVC]
		  call ioctl
		  add esp, 12

		  push dword VCNum
		  push dword VT_WAITACTIVE
		  push dword [hVC]
		  call ioctl
		  add esp, 12

		  ;push dword 0
		  ;call close
		  ;push dword 1
		  ;call close
		  ;push dword 2
		  ;call close
		  ;add esp, 12

		  ;push dword [hVC]
		  ;call close
		  ;add esp, 4

		  mov eax,  [VCNum]
		  mov ebx, devicename.num
		  cmp eax, 9
		  jbe .low
		  mov dl, 10
		  div dl
		  add al, 30
		  mov [ebx], al
		  inc ebx
		  mov al, ah

.low:	 add al, 30
       mov [ebx], al

		  ;push dword 0
		  ;push dword O_RDWR
		  ;push dword devicename
		  ;call open
		  ;add esp, 12

		 ; mov eax, [hVC]
_out:
		  push dword devicename
		  call printf
		  add esp, 4
		  
		  ;push eax 
		  ;call dup
		  ;push dword [hVC]
		  ;call dup
		  ;add esp, 8

		  ;push dword 0
		  ;push dword szVi
		  ;push dword szViPath
		  ;call execlp
		  ;add esp, 12

		  ret

section .data
szTty		db	'/dev/tty',0
szTtyErr	db	'Unable to open /dev/tty',0Ah,0Dh,0

hVC		dd	0
VCNum		dd	0

vtmode:
.mode		db	0
.waitv	db	0
.relsig	db	0
.acqsig	db	0
.frsig	db	0
vstat:
.v_active	db	0
.v_signal	db	0
.v_state		db	0
devicename:	db "/dev/tty"
.num:			db 0 
		      dw 0
szVi:			db "vi",0
szViPath:	db "/usr/bin/vi",0

