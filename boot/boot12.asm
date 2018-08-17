[BITS 16]
[ORG 0x7C00]

section .text

start:
    jmp	near Boot
			
Disk_ID			    db	"COS2000A"	;Fabricant + n° de série Formatage
Sectors_Size		dw	512		;Nombre d'octets/secteur
Sectors_Per_Cluster	db	1		;Nombre de secteurs/cluster
Reserved_Sectors	dw	1		;Nombre de secteurs réservé
Fats_Number		    db	2		;Nombre de copies de la FAT
Fits_Number		    dw	224		;Taille du répertoire racine
Sectors_Per_Disk	dw	2880		;Nombre secteurs du volume si < 32 Mo
Media_Descriptor	db	0xF0		;Descripteur de média
Sectors_Per_Fat		dw	9		;Nombre secteurs/FAT
Sectors_Per_Track	dw	18		;Nombre secteurs/piste
Heads_Number		dw	2		;Nombre de tete de lecture/écriture
Sectors_Hidden		dd	0		;Nombre de secteurs cachés
Sectors_Per_Disk2	dd	0		;Nombre secteurs du volume si > 32 Mo
Boot_Drive		    db	0		;Lecteur de démarrage
Reserved		    db	0		;NA (pour NT seulement)
Extended_Boot_ID	db	0x29		;Signature Boot étendu 29h
Serial_Number		dd	0x01020304	;N° de série
Disk_Name		    db	"COS2000    "	;Nom de volume
Fat_Type		    db	"FAT12   "	;Type de système de fichiers

Boot_Message		db "Cos2000",0
Entre_Message       db "Fichier",0
Loading_Message		db "Charger",0
System_File		    db "Al",0,"o",0,"a",0,"d",0,"e",0,0x0F,0,0x38,"r",0,".",0,"s",0,"y",0,"s",0,0,0,0,0,0xFF,0xFF,0xFF,0xFF
Is_Ok			    db " [  OK  ]",0x0A,0x0D,0
Is_Failed		    db " [ERREUR]",0x0A,0x0D,0
The_Dot			    db '.',0

Boot_Error:
    mov	    si,Is_Failed
    call	ShowString
    xor     ax,ax
    int	    0x16
    int	    0x19

Boot_Ok:
	mov	    si,Is_Ok
	call	ShowString
	ret

Boot:
    push	cs
    push	cs
    pop	    es
    pop	    ds
	mov	    [Boot_Drive],dl
    cli        
    mov	    ax,0x9000
    mov	    ss,ax
    mov	    sp,0xFFFF
    sti
    mov     si,Boot_Message
    call    ShowString	
; Initialisation du lecteur de disquette
    xor	    ax,ax
    int	    0x13
    jc	    Boot_Error
; Calcul de la position de la FAT12
	mov	    cx,[Reserved_Sectors]
    add	    cx,[Sectors_Hidden]
    adc	    cx,[Sectors_Hidden+2]
    mov	    bx,[Sectors_Per_Fat]
    mov     di,Fat_Buffer
    push    bx
    push    cx
; Lecture de la FAT en mémoire
readfat:
    call    ReadSector
    jc      Boot_Error
	inc     cx
	add     di,[Sectors_Size]
	dec     bx
	jnz     readfat          
    pop     cx
    pop     bx
    xor	    ax,ax
    mov	    al,[Fats_Number]
    mul	    bx
    add	    cx,ax
    mov	    ax,32
    mul	    word [Fits_Number]
    div	    word [Sectors_Size]
    add	    ax,cx
    sub	    ax,2
    mov	    word [Serial_Number],ax
    xor	    dx,dx
	call	Boot_Ok
    mov     si,Loading_Message
    call    ShowString	
; Recherche du système dans les entrèes de répertoire
Find_System:
    mov	    di,Buffer
    call	ReadSector
    jc	    Near Boot_Error
    xor	    bx,bx
Next_Root_Entrie:
    cmp	    byte [di],0
    je	    near Boot_Error
    push	di
    push	cx
    mov	    si,System_File
    mov	    cx,32
    rep	    cmpsb
    pop	    cx
    pop	    di
    je	    System_Found
    add	    di,32
    add	    bx,32
    inc	    dx
    cmp	    dx,[Fits_Number]
    ja	    near Boot_Error
    cmp	    bx,[Sectors_Size]
    jb	    Next_Root_Entrie
    inc	    cx
    jmp	    Find_System
System_Found:
; Système trouvé
    call    Boot_Ok
    mov     si,Entre_Message
    call    ShowString	
    mov	    cx,[di+26+32]
    mov	    ax,0x0080
    mov	    es,ax
    push	es
    mov	    di,0x0
    push	di
	mov	    si,The_Dot
Resume_Loading:
; Chargement des secteur en mémoire à l'adresse 0080:0000
    cmp	    cx,0x0FF0
    jae	    Finish_Loading
    push	cx
    add	    cx,word [Serial_Number]
    call	ReadSector
    pop	    cx
    jc      near Boot_Error
	call    ShowString
    add	    di,[Sectors_Size]
    call	NextFatGroup
    jc	    near Boot_Error
    jmp	    Resume_Loading
Finish_Loading:
	call	Boot_Ok
; Exécution du chargeur ELF
    retf

;====================READSECTOR=======================
;Lit le secteur logique LBA CX et le met en es:di
;-> CX (limité à 65536 secteurs, soit 32 Mo avec secteur 512 octets)
;<- Flag Carry si erreur
;=====================================================
ReadSector:
	pusha
	mov     ax,cx
	xor	    dx,dx
	div	    word [Sectors_Per_Track]
	inc	    dl
	mov	    bl,dl
	xor	    dx,dx
	div     word [Heads_Number]
	mov     dh, [Boot_Drive]
	xchg    dl,dh
	mov	    cx,ax
	xchg	cl,ch
	shl	    cl,6
	or	    cl, bl
	mov	    bx,di
	mov	    si, 4
	mov	    al, 1
Read_Again:
  	mov	    ah, 2
  	int	    0x13
  	jnc	    Read_Done
  	dec	    si
  	jnz	    Read_Again
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
	mov	    ax,cx
	mov	    bx,ax
	and	    bx,0000000000000001b
	shr	    ax,1
	mov	    cx,3
	mul	    cx
	mov	    di,Fat_Buffer
	add	    di,ax
	cmp	    bx,0
	jnz	    Even_Group
Odd_Group:
	mov	    dx,[di]
	and	    dx,0x0FFF
	mov	    cx,dx
	jmp	    Next_Group_Found
Even_Group:
	mov	    dx,[di+1]
	and	    dx,0xFFF0
	shr	    dx,4
	mov	    cx,dx
Next_Group_Found:
	pop	    di
	pop	    dx
	pop	    bx
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
	or	    al,al
	jz	    End_Show
	mov	    ah,0x0E
	mov	    bx,0x07
	int	    0x10
	jmp	    Next_Char
End_Show:
	popa
	ret

times 510-($-$$) db ' '

dw	0xAA55

Buffer		equ $
Fat_Buffer	equ $+512

section .bss

;Buffer		resb 512
;Fat_Buffer	resb 10000
