[BITS 16]
[ORG 0x100]

section .text

start:
	mov	ah,09
	mov	dx,msg
	int	0x21
	cmp	byte [0x80],2
	jne	near error
	mov	ax,0x3D02
	mov	dx,name
	int	0x21
	jc	near error
	mov	bx,ax
	mov	ax,0x4202
	xor	cx,cx
	xor	dx,dx
	int	0x21
	jc	error
	cmp	dx,0
	jne	error
	cmp	ax,512
	jne	error
	mov	ax,0x4200
	xor	cx,cx
	xor	dx,dx
	int	0x21
	jc	error
	mov	ah,0x3F
	mov	cx,512
	mov	dx,buffer2
	int	0x21
	jc	error
	cmp	word [buffer2+510],0xAA55
	jne	error
	mov	al,[0x82]
	cmp	al,'z'
	ja	error
	cmp	al,'a'
	jb	verif
	sub	al,'a'-'A'
verif:
	cmp	al,'Z'
	ja	error
	cmp	al,'A'
	jb	error
	sub	al,'A'
	mov	bp,ax
	mov	[segs],cs	
	mov	cx,0xFFFF
	mov	bx,packet
	int	0x25
	pop	ax
	mov	si,buffer+3
	mov	di,buffer2+3
	mov	cx,59
	rep	movsb
	mov	ax,bp
	mov	word [offs],buffer2
	mov	cx,0xFFFF
	mov	bx,packet
	int	0x26
	pop	ax	
	jc	error
	mov	ah,09
	mov	dx,msgok
	int	0x21
	ret

error:
	mov	ah,09
	mov	dx,msgerror
	int	0x21
	ret


packet dd 0
	dw 1
offs	dw buffer
segs	dw 0
name db "boot.bin",0
msg 	db 0x0D,0x0A,"CopyBoot V1.0 by nico",0x0D,0x0A,"Copyright 2002",0x0D,0x0A,'$'
msgok 	db "Installation of bootsector realized",0x0D,0x0A,'$'
msgerror db "Installation of bootsector failed",0x0D,0x0A,'$'
buffer equ $
buffer2 equ $+512