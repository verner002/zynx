;
; VGA (32-bit) Static Library
;
; Author: Jakub Verner
; Date: 19-10-2022
;

%ifndef _VGA32
%define _VGA32

_80x25_text:
; misc
db  0x67
; seq
db  0x03, 0x00, 0x03, 0x00, 0x02
; crtc
db  0x5f, 0x4f, 0x50, 0x82, 0x55, 0x81, 0xbf, 0x1f
db  0x00, 0x4f, 0x0d, 0x0e, 0x00, 0x00, 0x00, 0x50
db  0x9c, 0x0e, 0x8f, 0x28, 0x1f, 0x96, 0xb9, 0xa3
db  0xff
; gc
db  0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x0e, 0x00
db  0xff
; ac
db  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07
db  0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f
db  0x0c, 0x00, 0x0f, 0x08, 0x00

;_90x60_text:
; misc
;db  0xe7
; seq
;db  0x03, 0x01, 0x03, 0x00, 0x02
; crtc
;db  0x6b, 0x59, 0x5a, 0x82, 0x60, 0x8d, 0x0b, 0x3e
;db  0x00, 0x47, 0x06, 0x07, 0x00, 0x00, 0x00, 0x00
;db  0xea, 0x0C, 0xdf, 0x2d, 0x08, 0xe8, 0x05, 0xa3
;db  0xff
; gc
;db  0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x0e, 0x00
;db  0xff
; ac
;db  0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x14, 0x07
;db  0x38, 0x39, 0x3a, 0x3b, 0x3c, 0x3d, 0x3e, 0x3f
;db  0x0c, 0x00, 0x0f, 0x08, 0x00

;
; Initialize VGA
;
; Input: Nothing
; Output: Nothing
;

init_vga:
push esi
mov esi, _80x25_text
call load_regs
pop esi
ret

;
; Load Registers
;
; Input: ESI (vga regs struct)
; Output: Nothing
;

load_regs:
pushad
call unlock_crtc_regs

lodsb
call write_misc_reg

or byte [esi+0x0003], 0x80
and byte [esi+0x0011], 0x7f

xor cl, cl
mov dx, 0x03c4

.load_seq:
lodsb
call write_reg
inc cx
cmp cl, 0x05
jb .load_seq

xor cl, cl
mov dx, 0x03d4

.load_crtc:
lodsb
call write_reg
inc cx
cmp cl, 0x19
jb .load_crtc

xor cl, cl
mov dx, 0x03ce

.load_gc:
lodsb
call write_reg
inc cx
cmp cl, 0x09
jb .load_gc

xor cl, cl
mov dx, 0x03c0

.load_ac:
lodsb
call write_ac_reg
inc cx
cmp cl, 0x15
jb .load_ac

mov dx, 0x03da
in al, dx

mov dx, 0x03c0
mov al, 0x20
out dx, al

call lock_crtc_regs
popa
ret


;
; Set Cursor Position
;
; Input: DH (row), DL (column)
; Output: Nothing
;

set_cur_pos:
pushad
movzx ax, dh
mov bl, 0x50
mul bl
movzx bx, dl
add ax, bx

mov cl, 0x0f
mov dx, 0x03d4
call write_reg

mov al, ah
mov cl, 0x0e
call write_reg
popad
ret

;
; Enable Cursor
;
; Input: Nothing
; Output: Nothing
;

enable_cur:
pushad
mov cl, 0x0a
mov dx, 0x03d4
call read_reg

and al, 0xdf
call write_reg
popad
ret

;
; Disable Cursor
;
; Input: Nothing
; Output: Nothing
;

disable_cur:
pushad
mov cl, 0x0a
mov dx, 0x03d4
call read_reg

or al, 0x20
call write_reg
popad
ret

;
; Read DAC Mask
;
; Input: Nothing
; Output: AL (dac mask)
;

read_dac_mask:
push dx
mov dx, 0x03c6
in al, dx
pop dx
ret

;
; Write DAC Mask
;
; Input: AL (dac mask)
; Output: Nothing
;

write_dac_mask:
push dx
mov dx, 0x03c6
out dx, al
pop dx
ret

;
; Unlock CRTC Registers
;
; Input: Nothing
; Output: Nothing
;

unlock_crtc_regs:
push dx
mov dx, 0x3d4
in al, dx
push ax
mov al, 0x03
out dx, al
inc dx
in al, dx
or al, 0x80
out dx, al
dec dx
mov al, 0x11
out dx, al
inc dx
in al, dx
and al, 0x7f
out dx, al
dec dx
pop ax
out dx, al
pop dx
ret

;
; Lock CRTC Registers
;
; Input: Nothing
; Output: Nothing
;

lock_crtc_regs:
push dx
mov dx, 0x3d4
in al, dx
push ax
mov al, 0x03
out dx, al
inc dx
in al, dx
and al, 0x7f
out dx, al
dec dx
mov al, 0x11
out dx, al
inc dx
in al, dx
or al, 0x80
out dx, al
dec dx
pop ax
out dx, al
pop dx
ret

;
; Read Register
;
; Input: DX (addr reg), CL (index)
; Output: AL (reg val)
;

read_reg:
push cx
push dx
in al, dx ; store addr reg val for later

push ax
mov al, cl
out dx, al
inc dx ; data reg
in al, dx
dec dx ; addr reg
mov ch, al
pop ax

out dx, al ; restore addr reg val

mov al, ch
pop dx
pop cx
ret

;
; Write Registers
;
; Input: DX (addr reg), CL (index), AL (reg val)
; Output: Nothing
;

write_reg:
push cx
push dx
mov ch, al

in al, dx ; store addr reg val for later

push ax
mov al, cl
out dx, al
inc dx ; data reg
mov al, ch
out dx, al
dec dx ; addr reg
pop ax

out dx, al ; restore addr reg val

pop dx
pop cx
ret

;
; Read Attribute Controller Register
;
; Input: Nothing
; Output: Nothing
;

;read_ac_reg:
;ret

;
; Write Attribute Controller Register
;
; Input: CL (index), AL (ac reg val)
; Output: Nothing
;

write_ac_reg:
push cx
push dx
mov ch, al

mov dx, 0x03da
in al, dx

mov dx, 0x03c0
in al, dx

push ax
mov al, cl
out dx, al
mov al, ch
out dx, al
pop ax

out dx, al

pop dx
pop cx
ret

;
; Read Miscellaneous Output Register
;
; Input: Nothing
; Output: AL (misc reg val)
;

;read_misc_reg:
;push dx
;mov dx, 0x03cc
;in al, dx
;pop dx
;ret

;
; Write Miscellaneous Output Register
;
; Input: AL (misc reg val)
; Output: Nothing
;

write_misc_reg:
push dx
mov dx, 0x03c2
out dx, al
pop dx
ret

%endif