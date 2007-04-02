[BITS 16]
[ORG 0x7C00]

section .text

start:
	jmp	near Boot
			
Disk_ID			db	"COS2000A"	;Fabricant + n° de série Formatage
Sectors_Size		dw	512		;Nombre d'octets/secteur
Sectors_Per_Cluster	db	1		;Nombre de secteurs/cluster
Reserved_Sectors	dw	1		;Nombre de secteurs réservé
Fats_Number		db	2		;Nombre de copies de la FAT
Fits_Number		dw	224		;Taille du répertoire racine
Sectors_Per_Disk	dw	2880		;Nombre secteurs du volume si < 32 Mo
Media_Descriptor	db	0xF0		;Descripteur de média
Sectors_Per_Fat		dw	9		;Nombre secteurs/FAT
Sectors_Per_Track	dw	18		;Nombre secteurs/piste
Heads_Number		dw	2		;Nombre de tete de lecture/écriture
Sectors_Hidden		dd	0		;Nombre de secteurs cachés
Sectors_Per_Disk2	dd	0		;Nombre secteurs du volume si > 32 Mo
Boot_Drive		db	0		;Lecteur de démarrage
Reserved		db	0		;NA (pour NT seulement)
Extended_Boot_ID	db	0x29		;Signature Boot étendu 29h
Serial_Number		dd	0x01020304	;N° de série
Disk_Name		db	"COS2000    "	;Nom de volume
Fat_Type		db	"FAT12   "	;Type de système de fichiers

;Cpu_Message		db "CPU test",0
Boot_Message		db "Booting ",0
Finding_Message		db "System  ",0
Loading_Message		db "Loading ",0
System_File		db "SYSTEM  SYS"
Is_Ok			db "[  OK  ]",0x0A,0x0D,0
Is_Failed		db "[Failed]",0x0A,0x0D,"Press a key",0x0A,0x0D,0
The_Dot			db '.',0

Boot_Error:
        mov	si,Is_Failed
        call	ShowString
        mov	ah,0
        int	0x16
        int	0x19

Boot_Ok:
	mov	al,[Stage]
	cmp	al,0
	jz	No_Ok
	mov	si,Is_Ok
	call	ShowString
No_Ok:	
	xor	ah,ah
	mov	si,ax
	add	si,Boot_Message ;Cpu_Message
	call	ShowString
	add	byte [Stage],0x09	
        ret

Stage	db 0

Boot:
        push	cs
        push	cs
        pop	es
        pop	ds
	mov	[Boot_Drive],dl
        cli        
        mov	ax,0x9000
        mov	ss,ax
        mov	sp,0xFFFF
        sti
;	call	Detect_Cpu
;	jc	Boot_Error
        call	Boot_Ok	
        xor	ax,ax
        int	0x13
        jc	Boot_Error
	mov	cx,[Reserved_Sectors]
        add	cx,[Sectors_Hidden]
        adc	cx,[Sectors_Hidden+2]
        mov	di,Fat_Buffer
        call	ReadSector
        jc	Boot_Error
        xor	ax,ax
        mov	al,[Fats_Number]
        mov	bx,[Sectors_Per_Fat]
        mul	bx
        add	cx,ax
        mov	ax,32
        mul	word [Fits_Number]
        div	word [Sectors_Size]
        add	ax,cx
        sub	ax,2
        mov	word [Serial_Number],ax
        xor	dx,dx
	call	Boot_Ok
Find_System:
        mov	di,Buffer
        call	ReadSector
        jc	Near Boot_Error
        xor	bx,bx
Next_Root_Entrie:
        cmp	byte [di],0
        je	near Boot_Error
        push	di
        push	cx
        mov	si,System_File
        mov	cx,11
        rep	cmpsb
        pop	cx
        pop	di
        je	System_Found
        add	di,32
        add	bx,32
        inc	dx
        cmp	dx,[Fits_Number]
        ja	near Boot_Error
        cmp	bx,[Sectors_Size]
        jb	Next_Root_Entrie
        inc	cx
        jmp	Find_System
System_Found:
        mov	cx,[di+26]
        mov	ax,0x0071
        mov	es,ax
        push	es
        mov	di,0x100
        push	di
        call	Boot_Ok
	mov	si,The_Dot
Resume_Loading:
        cmp	cx,0x0FF0
        jae	Finish_Loading
        push	cx
        add	cx,word [Serial_Number]
        call	ReadSector
        pop	cx
        jc	near Boot_Error
	call	ShowString
        add	di,[Sectors_Size]
        call	NextFatGroup
        jc	near Boot_Error
        jmp	Resume_Loading
Finish_Loading:
	call	Boot_Ok
        retf

;====================READSECTOR=======================
;Lit le secteur logique LBA CX et le met en es:di
;-> CX (limité à 65536 secteurs, soit 32 Mo avec secteur 512 octets)
;<- Flag Carry si erreur
;=====================================================
ReadSector:
	pusha
	mov	ax,cx
	xor	dx,dx
	div	word [Sectors_Per_Track]
	inc	dl
	mov	bl,dl
	xor	dx,dx
	div	word [Heads_Number]
	mov	dh, [Boot_Drive]
	xchg	dl,dh
	mov	cx,ax
	xchg	cl,ch
	shl	cl,6
	or	cl, bl
	mov	bx,di
	mov	si, 4
	mov	al, 1
Read_Again:
  	mov	ah, 2
  	int	0x13
  	jnc	Read_Done
  	dec	si
  	jnz	Read_Again
Read_Done:
  	popa
	ret

;===================NEXTFATGROUP======================
;Renvoie en CX le groupe qui succède dans la FAT le groupe CX
;-> CX
;<-
;=====================================================
NextFatGroup:
	push	bx
	push	dx
	push	di
	mov	ax,cx
	mov	bx,ax
	and	bx,0000000000000001b
	shr	ax,1
	mov	cx,3
	mul	cx
	mov	di,Fat_Buffer
	add	di,ax
	cmp	bx,0
	jnz	Even_Group
Odd_Group:
	mov	dx,[di]
	and	dx,0x0FFF
	mov	cx,dx
	jmp	Next_Group_Found
Even_Group:
	mov	dx,[di+1]
	and	dx,0xFFF0
	shr	dx,4
	mov	cx,dx
Next_Group_Found:
	pop	di
	pop	dx
	pop	bx
	ret

;======================SHOWSTR========================
;Affiche la chaine de caractère pointé par ds:si à l'écran
;-> DS, SI
;<- Flag Carry si erreur
;=====================================================
ShowString:
	pusha
Next_Char:
        lodsb
        or	al,al
        jz	End_Show
        mov	ah,0x0E
        mov	bx,0x07
        int	0x10
        jmp	Next_Char
End_Show:
        popa
	ret

;======================DETECTCPU======================
;Detecte si le processeur est un 386 au mini
;->
;<- Flag Carry si erreur
;=====================================================
;Detect_Cpu:
;	push	ax		; test if 8088/8086 is present (flag bits 12-15 will be set)
; 	xor	ah,ah		; ah = 0
;	push	ax		; copy ax into the flags
;	popf			; with bits 12-15 clear
;	pushf			; Read flags back into ax
;	pop	ax
;	and	ah,0xF0		; check if bits 12-15 are set
;	cmp	ah,0xF0
;	je	No_386		; no 386 detected (8088/8086 present)
;				; check for a 286 (bits 12-15 are clear)
;	mov	ah,0xF0		; set bits 12-15
;	push	ax		; copy ax onto the flags
;	popf
;	pushf			; copy the flags into ax
;	pop	ax
;	and	ah,0xF0		; check if bits 12-15 are clear
;	jz	No_386		; no 386 detected (80286 present)
;	clc
;	ret
;No_386:
;	stc	
;	ret

times 510-($-$$) db ' '

dw	0xAA55

Buffer		equ $
Fat_Buffer	equ $+512

section .bss

;Buffer		resb 512
;Fat_Buffer	resb 10000