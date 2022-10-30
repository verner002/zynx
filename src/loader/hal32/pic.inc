;
; 8259A (32-bit) Static Library
;
; Author: Jakub Verner
; Date: 20-10-2022
;

cpu 486
bits 32

%ifndef _PIC32
%define _PIC32

;
; Initialize PICs
;
; Input: DL (master pic vector offset), DH (slave pic vector offset)
; Output: Nothing
;

init_pics:
push ax
in al, 0x21 ; store master's mask
push ax
in al, 0xa1 ; store slave's mask
push ax
mov al, 0x11
out 0x20, al ; send init. icw
;call wait_pics
out 0xa0, al ; send init. icw
;call wait_pics
mov al, dl
out 0x21, al ; send vector offset icw
;call wait_pics
mov al, dh
out 0xa1, al ; send vector offset icw
;call wait_pics
mov al, 0x04
out 0x21, al ; send cascade icw
;call wait_pics
mov al, 0x02
out 0xa1, al ; send cascade icw
;call wait_pics
dec ax ; mov al, 0x01
out 0x21, al ; send 80x86 icw
;call wait_pics
out 0xa1, al ; send 80x86 icw
;call wait_pics
pop ax
out 0xa1, al ; restore slave's mask
pop ax
out 0x21, al ; restore master's mask
pop ax
ret

;
; Wait For PICs
;
; Input: Nothing
; Output: Nothing
;

;wait_pics:
;push ax
;xor al, al
;out 0x80, al ; dummy write to the post port
;pop ax
;ret

;
; Send EOI
;
; Input: CL (int num)
; Output: Nothing
;

send_eoi:
push ax
mov al, 0x20

cmp cl, 0x08
jb .only_master

out 0xa0, al ; send eoi

.only_master:
out 0x20, al
pop ax
ret

;
; Enable Interrupt Requests
;
; Input: Nothing
; Output: Nothing
;

enable_irqs:
push ax
xor al, al
out 0x21, al
out 0xa1, al
pop ax
ret

;
; Disable Interrupt Requests
;
; Input: Nothing
; Output: Nothing
;

disable_irqs:
push ax
mov al, 0xff
out 0x21, al
out 0xa1, al
pop ax
ret

;
; Enable Interrupt Request
;
; Input: CL (int line num)
; Output: Nothing
;

enable_irq:
push ax
push bx
in al, 0xa1
mov ah, al
in al, 0x21
mov bx, 0x0001
shl bx, cl
not bx
and ax, bx
out 0x21, al
mov al, ah
out 0xa1, al
pop bx
pop ax
ret

;
; Disable Interrupt Request
;
; Input: CL (int line num)
; Output: Nothing
;

disable_irq:
push ax
push bx
in al, 0xa1
mov ah, al
in al, 0x21
mov bx, 0x0001
shl bx, cl
or ax, bx
out 0x21, al
mov al, ah
out 0xa1, al
pop bx
pop ax
ret

%endif