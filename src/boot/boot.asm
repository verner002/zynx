;
; ZyNX Bootloader
;
; Author: Jakub Verner
; Date: 03-11-2022
;

cpu 486
bits 16
org 0x7c00

;
; Magic Numbers
;

%define DRV_NUM_PTR 0x7b00
%define STACK_PTR   0x7c00
%define FAT_PTR     0x7e00
%define KERNEL_PTR  0x9000

;
; Header
;

jmp short main

sect_track dw 0x0012
heads_cyld db 0x02
sects_total dw 0x0b40

sects_clust db 0x08

sects_res db 0x01
sects_fat db 0x0c

label db "ZyNX Samba"
times 0x000b-($-label) db 0x20

;
; .text Section
;

main:
cli
xor ax, ax
mov ds, ax
mov es, ax
mov ss, ax
mov sp, STACK_PTR
mov byte [DRV_NUM_PTR], dl
cld
sti

mov si, rodata.loading
call print_str

movzx ax, byte [sects_res]
mov bx, FAT_PTR
movzx cx, byte [sects_fat]
call read_sects
jc panic

mov al, 0x10
mul byte [sects_clust]
mov dx, ax

xor ax, ax
mov bx, KERNEL_PTR
mov si, rodata.kernel

.read_next_dir_clust:
call read_clust
jc panic

mov di, bx
push dx

.check_next_rec:
mov cx, 0x000b
add di, 0x20

push si
push di
repz cmpsb
pop di
pop si
jz .match_found

dec dx
jnz .check_next_rec
pop dx

call calc_next_clust
jnz .read_next_dir_clust
jmp panic

.match_found:
pop dx
add si, 0x0b
mov ax, word [di+0x000c]

test byte [di+0x000b], 0x01
jnz .read_next_dir_clust

mov di, bx

push ax
mov ax, 0x0200
movzx cx, byte [sects_clust]
mul cx
mov cx, ax
pop ax

.read_next_file_clust:
call read_clust
jc panic

add bx, cx
call calc_next_clust
jnz .read_next_file_clust

cmp dword [di], "zexe"
jnz panic

cmp byte [di+0x0004], 0x19
jnz panic

cmp word [di+0x0005], 0x0386
jnz panic

mov di, word [di+0x0009]

cmp word di, 0x0000
jz panic

mov si, rodata.ok
call print_str

jmp di

;
; Print String
;
; Input: (DS:)SI (null-term char arr ptr)
; Output: Nothing
;

print_str:
;push ax
;push bx
;push si
mov ah, 0x0e
xor bh, bh ; mov bx, 0x000f - for graphic mode
jmp .load_char

.print_char:
int 0x10

.load_char:
lodsb
or al, al
jnz .print_char

;pop si
;pop bx
;pop ax
ret

;
; Read Sector
;
; Input: AX (LBA sector num), (ES:)BX (byte buff ptr)
; Output: CF (set on err)
;

read_sect:
pusha
xor dx, dx
div word [sect_track]
inc dx
mov cl, dl
div byte [heads_cyld]
mov ch, al
mov dh, ah

mov dl, byte [DRV_NUM_PTR]

mov di, 0x0002

.try_again:
mov ax, 0x0201
stc
int 0x13
jnc .return

or di, di
jz .terminate

xor ah, ah
int 0x13

dec di
jmp .try_again

.terminate:
popa
stc
ret

.return:
popa
ret

;
; Read Sectors
;
; Input: AX (LBA sector num), CX (num of sects), (ES:)BX (byte buff ptr)
; Output: CF (set on err)
;

read_sects:
pusha
clc

.read_sect:
jc .terminate
call read_sect
jc .terminate

inc ax
add bx, 0x0200
loop .read_sect
popa
clc ; because of add bx, ...
ret

.terminate:
popa
ret

;
; Read Cluster
;
; Input: AX (clust addr), (ES:)BX (byte buff ptr)
; Output: CF (set on err)
;

read_clust:
pusha
movzx cx, byte [sects_clust]
mul cx

push cx
movzx cx, byte [sects_res]
add ax, cx
movzx cx, byte [sects_fat]
add ax, cx
pop cx

call read_sects
popa
ret

;
; Calculate Next Cluster
;
; Input: AX (clust addr)
; Output: AX (next clust addr), ZF (set on EOF)
;

calc_next_clust:
push di
mov di, ax
shr di, 0x01
add di, ax
and ax, 0x01
mov ax, word [di+0x7e00]
jz .even_clust
shr ax, 0x04

.even_clust:
and ah, 0x0f
cmp ax, 0x0fff
pop di
ret

;
; Panic
;

panic:
mov si, rodata.panic
call print_str
;jmp halt

;
; Halt
;

halt:
cli
hlt
jmp halt

;
; .rodata Section
;

rodata:
.loading db "loading zynx... ", 0x00
.kernel db "loader  exe"
.ok db "ok", 0x0a, 0x0d, 0x00
.panic db "panic!", 0x0a, 0x0d, 0x00

times 0x01fe-($-$$) db 0x00
dw 0xaa55