;----------High-Level Call
;Usage:
;   ccall FuncName, param1, param2, param 3...
;   pcall FuncName, param1, param2, param 3...
;   stdcall FuncName, param1, param2, param 3...
;   fastcall FuncName, param1, param2, param 3...
%macro pcall 2-9 ;Pascal Call: push first value first, no cleanup
%define _j %1
%rep %0-1
    %rotate 1
    push %1
%endrep
    call _j
%endmacro

%macro ccall 2-9 ;C Call: push last value first, cleanup
%define _j %1
%assign __params %0-1
%rep %0-1
    %rotate -1
    push %1
%endrep
    call _j
    %assign __params __params * 4
    add esp, __params
%endmacro

%macro stdcall 2-9 ;Windows StdCall: push last value first, no cleanup
%define _j %1
%rep %0-1
    %rotate -1
    push %1
%endrep
    call _j
%endmacro

%macro fastcall 2-9 ;Borland FastCall: use registers, no cleanup
%define _j %1
%assign __pnum 1
%rep %0-4
    %rotate -1
    %if __pnum = 1
     mov eax, %1
    %elif __pnum = 2
     mov edx, %1
    %elif __pnum = 3
     mov ebx, %1
    %else
     push %1
    %endif
    %assign __pnum __pnum+1
%endrep
    call _j
%endmacro

;----------IF-THEN-ELSE Loops
;Usage:
;   IF Value, Cond, Value
;    ;if code here
;   ELSIF Value, Cond, Value
;    ;else-if code here
;   ELSE
;    ;else code here
;   ENDIF
;Where Value is a variable, register, or integer -- but not EAX or EBX!!
;Where Cond is the conditional -- a, ae, b, be, e, g, ge, le, ne, nz, z, etc..
%macro IF 3
 %push if
 %assign __curr 1
; %if %1 != eax
    mov eax, %1
; %endif
; %if %3 != ebx
    mov ebx, %3
; %endif
    cmp eax, ebx
    j%+2 %%if_code
    jmp %$loc(__curr)
%%if_code:
%endmacro

%macro ELSIF 3
  %ifctx if
    jmp %$end_if
%$loc(__curr):
  %assign __curr __curr+1
;  %if %1 != eax
    mov eax, %1
;  %endif
;  %if %3 != ebx
    mov ebx, %3
;  %endif
    cmp eax, ebx
    j%+2 %%elsif_code
    jmp %$loc(__curr)
%%elsif_code:
 %else
   %error "'ELSIF' can only be used following 'IF'"
 %endif
%endmacro

%macro ELSE 0
 %ifctx if
    jmp %$end_if
%$loc(__curr):
%assign __curr __curr+1
 %else
  %error "'ELSE' can only be used following an 'IF'"
 %endif
%endmacro

%macro ENDIF 0
%$loc(__curr):
%$end_if:
%pop
%endmacro

;----------SWITCH-CASE Blocks
;Usage:
;   SWITCH Variable
;   CASE Int
;        ;case1 code here
;        BREAK
;   CASE Int
;        ;case2 code here
;        BREAK
;   DEFAULT
;        ;default code here
;        BREAK
;   ENDSWITCH
;Where Variable is a variable of INT to DWORD value [eax is used to store]
;Where Int is an immediate INT to DWORD value [ebx is used to store]
%macro SWITCH 1
%push switch
%assign __curr 1
    mov eax, %1
    jmp %$loc(__curr)
%endmacro

%macro CASE 1
%ifctx switch
    %$loc(__curr):
    %assign __curr __curr+1
    mov ebx, %1
    cmp eax, ebx
    jne %$loc(__curr)
%else
    %error "'CASE' can only be used in a 'SWITCH' block"
%endif
%endmacro

%macro DEFAULT 0
%ifctx switch
    %$loc(__curr):
%else
    %error "'DEFAULT' can only be used in a 'SWITCH' block"
%endif
%endmacro

%macro BREAK 0
    jmp %$endswitch
%endmacro

%macro ENDSWITCH 0
    %ifctx switch
    %$endswitch:
    %pop
    %else
         %error "'ENDSWITCH' can only be used in an 'SWITCH' block"
    %endif
%endmacro

;----------DO-FOR and DO-WHILE Loops
;Usage:
;   DO
;    ;code to do here
;   FOR min, cond, max, step
;Where Min is a value or variable of INT to DWORD value [eax is used]
;Where Cond is condition to stop at -- a, ae, b, be, g, ge, l, le, n, ne, z
;Where Max is an INT to DWORD value [ebx is used to store]
;Where Step is an INT to DWORD value to be added to Min at each loop
;e.g. FOR 0, e, 100, 1 == for(x=0; x<100, x++) do code...;
;   DO
;    ;code to do here
;   WHILE variable, condition, value
;Where variable is a value or variable of INT to DWORD value [eax is used]
;Where condition is a conditional -- a, ae, b, be, g, ge, l, le, n, ne, z
;Where Value is an INT to DWORD value [ebx is used to store]
;e.g. WHILE Counter, l, 100 == while( counter < 100 ) do code...;
%macro DO 0
 %push do
    jmp %$init_loop
%$start_loop:
    push eax
%endmacro

%macro FOR 4
 %ifctx do
    pop eax
    add eax, %4
    cmp eax, %3
    j%+2 %%end_loop
    jmp %$start_loop
%$init_loop:
    mov eax, %1
    jmp %$start_loop
%%end_loop:
 %pop
 %else
  %error "'FOR' must be preceded by a 'DO'"
 %endif
%endmacro

%macro WHILE 3
 %ifctx do
    pop eax
    mov ebx, %3
    cmp eax, ebx
    j%+2 %%end_loop
    jmp %$start_loop
%$init_loop:
    mov eax, %1
    jmp %$start_loop
%%end_loop:
 %pop
 %else
  %error "'WHILE' must be preceded by a 'DO'"
 %endif
%endmacro

;----------Procedure Declarations
;Usage:
;PROC ProcName Parameter1, Parameter2, Parameter3...
;LOCALDD Dword_Variable [opt]
;LOCALDW Word_Variable [opt]
;LOCALDB Byte_Variable [opt]
;   ;procedure code here
;ENDP
;
;ENTRYPROC
    ;procedure code here
;ENDP
;
;Note that parameters are set up to EQU to ebp-x
;Note that locals are set up to EQU to ebp+x
;Note that neither LOCALS or Parameters are necessary, as in ENTRYPROC

%macro PROC 1-9
GLOBAL %1
%1:
 %assign _i 4
 %rep %0-1
    %2 equ [ebp-_i]
 %assign _i _i+4
 %rotate 1
 %endrep
    push ebp
    mov ebp, esp
 %push local
 %assign __ll 0
%endmacro

%macro ENDP 0
 %ifctx local
  %pop
 %endif
    pop ebp
%endmacro

%macro ENTRYPROC 0
PROC main
%endmacro

%macro LOCALVAR 1
    sub esp, 4
    %1 equ [ebp + __ll]
%endmacro

%macro LOCALDB 1
  %assign __ll __ll+1
  LOCALVAR %1
%endmacro

%macro LOCALDW 1
  %assign __ll __ll+2
  LOCALVAR %1
%endmacro

%macro LOCALDD 1
  %assign __ll __ll+4
  LOCALVAR %1
%endmacro

;----------Data Declarations
;Usage:
;CHAR Name, String
;INT Name, Byte
;WORD Name, Word
;DWORD Name, Dword
;e.g.    CHAR  UserName, "Joe User"
;        INT   Timeout,  30
;        WORD  Logins
;        DWORD Password
;Note that not specifying a value causes the variable to default to 0
;Note that strings are automatically zero-terminated
%macro CHAR 1-2 0
%1: DB   %2,0
%endmacro

%macro INT 1-2 0
%1: DB   %2
%endmacro

%macro WORD 1-2 0
%1: DW   %2
%endmacro

%macro DWORD 1-2 0
%1: DD   %2
%endmacro
