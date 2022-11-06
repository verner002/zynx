;
; Keyboard (32-bit) Static Library
;
; Author: Jakub Verner
; Date: 06-11-2022
;

cpu 486
bits 32

%ifndef _KBD32
%define _KBD32

;
; Reset Keyboard
;
; Input: Nothing
; Output: CF (set on err)
;

reset_kbd:
push ax
push cx
mov cl, 0x03

.again:
mov al, 0xff
call send_kbd_cmd
jc .terminate

call read_kbd_ans
jc .terminate

cmp al, 0xfa
jz .get_reset_stat

cmp al, 0xfe
jnz .terminate

dec cl
jnz .again

.terminate:
pop cx
pop ax
stc
ret

.get_reset_stat:
call read_kbd_ans
jc .terminate

cmp al, 0xaa
jnz .terminate

pop cx
pop ax
clc
ret

;
; Set LEDs Keyboard
;
; Input: DL (leds bitmap)
; Output: CF (set on err)
;

set_leds_kbd:
push ax
push cx
push dx
mov cl, 0x03

.again_set_leds:
mov al, 0xed
call send_kbd_cmd
jc .terminate

call read_kbd_ans
jc .terminate

cmp al, 0xfa
jz .send_data

cmp al, 0xfe
jnz .terminate

dec cl
jnz .again_set_leds

.terminate:
pop dx
pop cx
pop ax
stc
ret

.send_data:
and dl, 0x07
mov cl, 0x03

.again_send_data:
mov al, dl
call send_kbd_cmd
jc .terminate

call read_kbd_ans
jc .terminate

cmp al, 0xfa
jz .return

cmp al, 0xfe
jnz .terminate

dec cl
jnz .again_send_data
jmp .terminate

.return:
pop dx
pop cx
pop ax
;clc
ret

;
; Echo Keyboard
;
; Input: Nothing
; Output: CF (set on echo fail)
;

echo_kbd:
push ax
push cx
mov cl, 0x03

.again:
mov al, 0xee
call send_kbd_cmd
jc .terminate

call read_kbd_ans
jc .terminate

cmp al, 0xee
jz .return

cmp al, 0xfe
jnz .terminate

dec cl
jnz .again

.terminate:
pop cx
pop ax
stc
ret

.return:
pop cx
pop ax
;clc
ret

;
; Identify Keyboard
;
; Input: Nothing
; Output: Nothing
;

ident_kbd:
push ax
push cx
mov cl, 0x03

.again_ident:
mov al, 0xf2
call send_kbd_cmd
jc .terminate

call read_kbd_ans
jc .terminate

cmp al, 0xfa
jz .read_ident

dec cl
jnz .again_ident

.terminate:
pop cx
pop ax
stc
ret

.read_ident:
call read_kbd_ans
jc .at_kbd

cmp al, 0xab
jnz .unknown_kbd

cmp al, 0x41
jz .mf2_trans_kbd

cmp al, 0xc1
jz .mf2_kbd

cmp al, 0x83
jz .mf2_kbd

.unknown_kbd:
pop cx
pop ax
mov al, 0xff
clc
ret

.mf2_kbd:
pop cx
pop ax
xor al, al
clc
ret

.mf2_trans_kbd:
pop cx
pop ax
mov al, 0x01
clc
ret

.at_kbd:
pop cx
pop ax
mov al, 0x02
clc
ret

;
; Enable Keyboard Scanning
;
; Input: Nothing
; Output: CF (set on timeout)
;

enable_kbd_scan:
push ax
push cx
mov cl, 0x03

.again:
mov al, 0xf4
call send_kbd_cmd
jc .terminate

call read_kbd_ans
jc .terminate

cmp al, 0xfa
jz .return

dec cl
jnz .again

.terminate:
pop cx
pop ax
stc
ret

.return:
pop cx
pop ax
clc
ret

;
; Disable Keyboard Scanning
;
; Input: Nothing
; Output: CF (set on timeout)
;

disable_kbd_scan:
push ax
push cx
mov cl, 0x03

.again:
mov al, 0xf5
call send_kbd_cmd
jc .terminate

call read_kbd_ans
jc .terminate

cmp al, 0xfa
jz .return

dec cl
jnz .again

.terminate:
pop cx
pop ax
stc
ret

.return:
pop cx
pop ax
clc
ret

;
; Send Keyboard Command
;
; Input: AL (cmd)
; Output: CF (set on timeout)
;

send_kbd_cmd:
push ax
mov cl, 0x03

.again:
in al, 0x64
test al, 0x02
jz .send_cmd
dec cl
jnz .again
pop ax
stc
ret

.send_cmd:
pop ax
out 0x60, al
clc
ret

;
; Read Keyboard Command
;
; Input: Nothing
; Output: AL (answer), CF (set on timeout)
;

read_kbd_ans:
push ax
mov cl, 0x03

.again:
in al, 0x64
test al, 0x01
jnz .read_answer
dec cl
jnz .again
pop ax
stc
ret

.read_answer:
pop ax
in al, 0x60
clc
ret

%endif