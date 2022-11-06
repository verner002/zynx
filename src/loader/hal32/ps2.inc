;
; 8042 (32-bit) Static Library
;
; Author: Jakub Verner
; Date: 03-11-2022
;

cpu 486
bits 32

%ifndef _PS232
%define _PS232

;
; Initialize PS/2
;
; Input: Nothing
; Output: CF (set on err)
;

init_ps2:
push ax
call disable_fps2

mov al, 0xa7
call wait_for_ps2ib
out 0x64, al ; disable sec ps/2 port for sure

in al, 0x60 ; flush out buff

mov al, 0x20
call wait_for_ps2ib
out 0x64, al ; read config byte

call wait_for_ps2ob
in al, 0x60
and al, 0x24 ; disable trans for first ps/2, disable irqs, enable clock sig for first ps/2

push ax
mov al, 0x60
call wait_for_ps2ib
out 0x64, al ; write config byte
pop ax

call wait_for_ps2ib
out 0x60, al

mov al, 0xaa
call wait_for_ps2ib
out 0x64, al ; test ps/2 contr
call wait_for_ps2ob
in al, 0x60

cmp al, 0x55
jnz .terminate

mov al, 0xab
call wait_for_ps2ib
out 0x64, al ; test first ps/1 port
call wait_for_ps2ob
in al, 0x60

or al, al
jnz .terminate

mov al, 0x20
call wait_for_ps2ib
out 0x64, al ; read config byte

call wait_for_ps2ob
in al, 0x60
or al, 0x01 ; enable irq for first ps/2 port

push ax
mov al, 0x60
call wait_for_ps2ib
out 0x64, al ; write config byte
pop ax

call wait_for_ps2ib
out 0x60, al

call enable_fps2
pop ax
clc
ret

.terminate:
pop ax
stc
ret

;
; Enable First PS/2 Port
;
; Input: Nothing
; Output: Nothing
;

enable_fps2:
push ax
call wait_for_ps2ib
mov al, 0xae
out 0x64, al
pop ax
ret

;
; Disable First PS/2 Port
;
; Input: Nothing
; Output: Nothing
;

disable_fps2:
push ax
call wait_for_ps2ib
mov al, 0xad
out 0x64, al
pop ax
ret

;
; Wait For PS/2 Input Buffer
;
; Input: Nothing
; Output: Nothing
;
; TODO: Add timeout.
;

wait_for_ps2ib:
push ax

.again:
in al, 0x64 ; read stat reg
test al, 0x02
jnz .again
pop ax
ret

;
; Wait For PS/2 Output Buffer
;
; Input: Nothing
; Output: Nothing
;
; TODO: Add timeout.
;

wait_for_ps2ob:
push ax

.again:
in al, 0x64 ; read stat reg
test al, 0x01
jz .again
pop ax
ret

%endif