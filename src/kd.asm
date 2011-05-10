BITS 32

%define O_RDWR 2
%define KDMKTONE 0x4B30
EXTERN open
GLOBAL main

section .text
main:
push dword O_RDWR
push dword szTTY
call open
add esp, 8

mov ebx, eax
mov ecx, KDMKTONE
mov dx, 666
shl edx, 16
or dx, 1199
mov eax, 54
int 0x80
;push edx
;push dword KDMKTONE
;push eax
;call ioctl
;add esp, 12

ret

section .data
szTTY db '/dev/tty',0
