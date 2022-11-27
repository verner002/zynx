;
; Disk (16-bit) Static Library
;
; Author: Jakub Verner
; Date: 19-11-2022
;

%ifndef _DISK16
%define _DISK16

;
; Reset Disk System
;
; Input: DL (drv num)
; Output: CF (set on err)
;

;rst_disk_sys:
;push ax
;xor ah, ah
;int 0x13
;pop ax
;ret

;
; Read Sector
;
; Input: AX (LBA sector num), (ES:)BX (byte buff ptr)
; Output: CF (set on err)
;

read_sect:
pusha
xor dx, dx
div word [SECTS_TRACK_PTR]
inc dx
mov cl, dl
div byte [HEADS_CYLD_PTR]
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
jc .inc_es

.continue:
call read_sect
jc .terminate

inc ax
add bx, 0x0200

loop .read_sect
popa
clc ; because of add bx, ...
ret

.inc_es:
mov dx, es
add dx, 0x1000
jz .terminate
mov es, dx
jmp .continue

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
movzx cx, byte [SECTS_CLUST_PTR]
mul cx

push cx
movzx cx, byte [SECTS_RES_PTR]
add ax, cx
movzx cx, byte [SECTS_FAT_PTR]
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
mov ax, word [di+FAT_PTR]
jz .even_clust
shr ax, 0x04

.even_clust:
and ah, 0x0f
cmp ax, 0x0fff
pop di
ret

;
; Load File
;
; Input: (DS must equal 0x0000)SI (path char arr ptr), (ES must equal 0x0000)BX (byte buff ptr)
; Output: CF (set on err)
;

load_file:
pusha
mov al, 0x10
mul byte [SECTS_CLUST_PTR]
mov dx, ax ; num of recs to check

xor ax, ax

.read_next_dir_clust:
call read_clust
jc .terminate

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

.terminate:
popa
stc
ret

.match_found:
pop dx
add si, 0x0b
mov ax, word [es:di+0x000c]

test byte [es:di+0x000b], 0x01
jnz .read_next_dir_clust

push ax
mov ax, 0x0200
movzx cx, byte [SECTS_CLUST_PTR]
mul cx
mov cx, ax
pop ax

.read_next_file_clust:
call read_clust
jc .terminate

add bx, cx
call calc_next_clust
jnz .read_next_file_clust
popa
clc
ret

%endif