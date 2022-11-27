;
; Memory (16-bit) Static Library
;
; Author: Jakub Verner
; Date: 12-10-2022
;

cpu 486
bits 16

%ifndef _MEM16
%define _MEM16

;
; Check A20 Gate
;
; Input: Nothing
; Output: CF (set on a20 disabled)
;

check_a20:
push es
push ax
push si
push di
;xor ax, ax
;mov ds, ax

mov ax, 0xffff
mov es, ax

mov si, 0x7dfe
mov di, 0x7e0e

mov word [es:di], 0x55aa ; change data

cmp word [si], 0x55aa ; has changed?
jnz .return

mov word [es:di], 0xaa55 ; restore data

pop di
pop si
pop ax
pop es
stc
ret

.return:
pop di
pop si
pop ax
pop es
clc
ret

;
; Enable A20 Gate BIOS
;
; Input: Nothing
; Output: Nothing
;

enable_a20_bios:
push ax
mov ax, 0x2403
int 0x15
jc .return

or ah, ah
jnz .return

mov ax, 0x2401
int 0x15

.return:
pop ax
ret

;
; Enable A20 Gate Keyboard Controller
;
; Input: Nothing
; Output: Nothing
;

enable_a20_kbd:
cli
push ax
mov al, 0xad ; disable first ps/2
call wait_for_ps2ib16
out 0x64, al
mov al, 0xd0 ; read controller out. port
call wait_for_ps2ib16
out 0x64, al

call wait_for_ps2ob16
in al, 0x60
or al, 0x02 ; enable a20 gate

push ax
mov al, 0xd1 ; write controller out. port
call wait_for_ps2ib16
out 0x64, al
pop ax

call wait_for_ps2ib16
out 0x60, al

mov al, 0xae ; enable first ps/2
call wait_for_ps2ib16
out 0x64, al
pop ax
sti
ret

;
; Enable A20 Port 0x92
;
; Input: Nothing
; Output: Nothing
;

enable_a20_p92:
cli
push ax
in al, 0x92
or al, 0x02
out 0x92, al
pop ax
sti
ret

;
; Wait For Input Buffer
;
; Input: Nothing
; Output: Nothing
;

wait_for_ps2ib16:
push ax

.again:
in al, 0x64
test al, 0x02
jnz .again

pop ax
ret

;
; Wait For Output Buffer
;
; Input: Nothing
; Output: Nothing
;

wait_for_ps2ob16:
push ax

.again:
in al, 0x64
test al, 0x01
jz .again

pop ax
ret

%endif