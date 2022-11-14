;
; 8237A Static Library
;
; Author: Jakub Verner
; Date: 12-11-2022
;

cpu 486
bits 32

%ifndef _DMA32
%define _DMA32

;
; Initialize DMA Controllers
;
; Input: Nothing
; Output: Nonthing
;

init_dmacs:
push ax
mov al, 0x14
out 0x08, al ; deactive contr
out 0xd0, al ; deactive contr

out 0x0d, al ; master clear, mask on
out 0xda, al ; master clear, mask on

mov al, 0xc0 ; cascade channel 4
out 0xd6, al ; channel 4 is used for cascading

mov al, 0x10
out 0xd0, al ; active contr
out 0x08, al ; active contr
pop ax
ret

;
; Mask DMA Channels (Multichannel)
;
; Input: Nothing
; Output: Nothing
;

mask_dma_chnls:
push ax
mov al, 0x0f
out 0x0f, al
out 0xde, al
pop ax
ret

;
; Unmask DMA Channels (Multichannel)
;
; Input: Nothing
; Output: Nothing
;

unmask_dma_chnls:
push ax
xor al, al
out 0x0f, al
out 0xde, al
pop ax
ret

;
; Mask DMA Channel Multichanel
;
; Input: CL (channel)
; Output: Nothing
;

mask_dma_chnl_mltchnl:
cmp cl, 0x07
ja .return

push ax
mov al, 0x01

cmp cl, 0x03
ja .master

shl al, cl
out 0x0f, al
pop ax
ret

.master:
sub cl, 0x04
shl al, cl
out 0xde, al
pop ax

.return:
ret

;
; Unmask DMA Channel Multichannel
;
; Input: CL (channel)
; Output: Nothing
;

unmask_dma_chnl_mltchnl:
cmp cl, 0x07
ja .return

push ax
push bx
mov bl, 0x01

cmp cl, 0x03
ja .master

shl bl, cl
not bl
in al, 0x0f
and al, bl
out 0x0f, al
pop bx
pop ax
ret

.master:
sub cl, 0x04
shl bl, cl
not bl
in al, 0xde
and al, bl
out 0xde, al
pop bx
pop ax

.return:
ret

;
; Active Master DMA Controller
;
; Input: Nothing
; Output: Noting
;

active_master_dmac:
push ax
mov al, 0x10 ; dakp=0
             ; drwp=0
             ; extw=0
             ; prior=1
             ; komp=0
             ; cond=0
             ; adh0=0
             ; spsp=0
out 0xd0, al
pop ax
ret

;
; Deactive Master DMA Controller
;
; Input: Nothing
; Output: Nothing
;

deactive_master_dmac:
push ax
mov al, 0x14 ; dakp=0
             ; drwp=0
             ; extw=0
             ; prior=1
             ; komp=0
             ; cond=1
             ; adh0=0
             ; spsp=0
out 0xd0, al
pop ax
ret

;
; Active Slave DMA Controller
;
; Input: Nothing
; Output: Nothing
;

active_slave_dmac:
push ax
mov al, 0x10 ; dakp=0
             ; drwp=0
             ; extw=0
             ; prior=1
             ; komp=0
             ; cond=0
             ; adh0=0
             ; spsp=0
out 0x08, al
pop ax
ret

;
; Deactive Slave DMA Controller
;
; Input: Nothing
; Output: Nonthing
;

deactive_slave_dmac:
push ax
mov al, 0x14 ; dakp=0
             ; drwp=0
             ; extw=0
             ; prior=1
             ; komp=0
             ; cond=1
             ; adh0=0
             ; spsp=0
out 0x08, al
pop ax
ret

;
; Initialize Floppy DMA Channel
;
; Input: ES:BX (buffer ptr), CX (count)
; Output: Nothing
;

init_flpy_dma_chnl:
push ax
push dx
mov al, 0x06 ; mask on, channel 2
out 0x0a, al
call wait_dmas

out 0x0c, al ; reset the flip-flop
;call wait_dmas - we do calculation below so this line is not necessary

mov ax, es
mov dx, ax

shl ax, 0x04
shr dh, 0x04

add ax, bx
adc dh, 0x00 ; add carry

out 0x04, al ; low address
call wait_dmas

mov al, ah
out 0x04, al ; hight address
call wait_dmas

mov al, dh
out 0x81, al ; extra address
call wait_dmas

out 0x0c, al ; reset the flip-flop - FIXME: resets automatically?
call wait_dmas

mov al, cl
out 0x05, al ; low counter
call wait_dmas

mov al, ch
out 0x05, al ; high counter
call wait_dmas

mov al, 0x02
out 0x0a, al ; mask off, channel, 2
pop dx
pop ax
ret

;
; Prepare Floppy DMA Channel For Read
;
; Input: Nothing
; Output: Nothing
;

prep_flpy_dma_chnl_read:
push ax
mov al, 0x06 ; mask on, channel 2
out 0x0a, al

mov al, 0x56 ; mod=01
             ; idec=0
             ; auto=1
             ; tra=01
             ; sel=10
out 0x0b, al

mov al, 0x02 ; mask off, channel 2
pop ax
ret

;
; Prepare Floppy DMA Channel For Write
;
; Input: Nothing
; Output: Nothing
;

prep_flpy_dma_chnl_write:
push ax
mov al, 0x06 ; mask on, channel 2
out 0x0a, al

mov al, 0x5a ; mod=01
             ; idec=0
             ; auto=1
             ; tra=10
             ; sel=10
out 0x0b, al

mov al, 0x02 ; mask off, channel 2
pop ax
ret

;
; Wait For DMA Controllers
;
; Input: Nothing
; Output: Nothing
;

wait_dmas:
push ax
xor al, al
out 0x80, al ; dummy write
pop ax
ret

%endif