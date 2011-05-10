BITS 32

%define O_RDWR 2
%define KDGETLED        0x4B31			;grep KDGETLED /usr/include/linux/*
%define KDSETLED        0x4B32			;grep KDSETLED /usr/include/linux/*
EXTERN open
EXTERN sleep
GLOBAL main

section .text
main:
push dword O_RDWR
push dword szTTY
call open
add esp, 8

xor edx, edx
mov ecx, KDGETLED
mov ebx, eax
mov eax, 54
int 0x80

mov ecx, 10 
.here:
push ecx
or edx, 0x07
mov ecx, KDSETLED
mov eax, 54
int 0x80

mov ecx, 0xFFFFFF
.delay:
loop .delay

and edx, 0
mov ecx, KDSETLED
mov eax, 54
int 0x80

mov ecx, 0xFFFFFF
.delay2:
loop .delay2

pop ecx
loop .here

ret

section .data
szTTY db '/dev/tty',0
