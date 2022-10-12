;
; Hight Memory Static Library
;
; Author: Jakub Verner
; Date: 12-10-2022
;

%ifndef _HIMEM16
%define _HIMEM16

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
;xor ax, ax
;mov ds, ax

mov ax, 0xffff
mov es, ax

mov si, 0x7dfe
mov di, 0x7e0e

mov byte [es:di], 0xaa

cmp byte [si], 0xaa
jnz .return

inc si
inc di

mov byte [si], 0x55

cmp byte [si], 0x55
jnz .return

pop si
pop ax
pop es
stc
ret

.return:
pop si
pop ax
pop es
clc
ret

;
; Enable A20 Gate
;
; Input: Nothing
; Output: Nothing
;

enable_a20:
cli
push ax
mov al, 0xad ; disable first ps/2
call wait_for_in_buff
out 0x64, al
mov al, 0xd0 ; read controller out. port
call wait_for_in_buff
out 0x64, al

call wait_for_out_buff
in al, 0x60
or al, 0x02 ; enable a20 gate

push ax
mov al, 0xd1 ; write controller out. port
call wait_for_in_buff
out 0x64, al
pop ax

call wait_for_in_buff
out 0x60, al

mov al, 0xae ; enable first ps/2
call wait_for_in_buff
out 0x64, al
pop ax
sti
ret

;
; Wait For Input Buffer
;
; Input: Nothing
; Output: Nothing
;

wait_for_in_buff:
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

wait_for_out_buff:
push ax

.again:
in al, 0x64
test al, 0x01
jz .again

pop ax
ret

%endif