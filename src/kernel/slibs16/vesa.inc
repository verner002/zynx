;
; VESA VBE Static Library
;
; Author: Jakub Verner
; Date: 27-11-2022
;

%ifndef _VESA16
%define _VESA16

;
; Get VESA BIOS Information
;
; Input: Nothing
; Output: (ES:)DI (ptr to vbe info blk), CF (set on vesa not supp)
;

get_vbe_info:
push ax
push bx
mov ax, 0x4f00
;xor ax,ax
;mov es, ax
mov di, 0xd000
mov dword [di], 'VBE2'
int 0x10

cmp ax, 0x004f
jnz .terminate

cmp dword [di], 'VESA'
jnz .terminate

pop bx
pop ax
clc
ret

.terminate:
pop bx
pop ax
;xor di, di
stc
ret

;
; Get VESA BIOS Video Mode Information
;
; Input: CX (vesa mode num)
; Output: (ES:)DI (ptr to vbe mode info blk), CF (set on err)
;

get_vbe_vmode_info:
push ax
mov ax, 0x4f01
;xor ax, ax
;mov es, ax
mov di, 0xd200
int 0x10

cmp ax, 0x004f
jnz .terminate
pop ax
clc
ret

.terminate:
pop ax
stc
ret

;
; Set VESA BIOS Video Mode
;
; Input: BX (vesa video mode)
; Output: CF (set on err)
;

set_vbe_vmode:
push ax
mov ax, 0x4f02
int 0x10

cmp ax, 0x004f
jnz .terminate

pop ax
clc
ret

.terminate:
pop ax
stc
ret

%endif