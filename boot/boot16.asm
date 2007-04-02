[BITS 16]
[ORG 0x7C00]

section .text

start:
	jmp	near Boot
			
Disk_ID			db	"COS2000A"	;Fabricant + n° de série Formatage
Sectors_Size		dw	512		;Nombre d'octets/secteur
Sectors_Per_Cluster	db	4		;Nombre de secteurs/cluster
Reserved_Sectors	dw	1		;Nombre de secteurs réservé
Fats_Number		db	2		;Nombre de copies de la FAT
Fits_Number		dw	512		;Taille du répertoire racine
Sectors_Per_Disk	dw	2880		;Nombre secteurs du volume si < 32 Mo
Media_Descriptor	db	0xF8		;Descripteur de média
Sectors_Per_Fat		dw	207		;Nombre secteurs/FAT
Sectors_Per_Track	dw	18		;Nombre secteurs/piste
Heads_Number		dw	38		;Nombre de tete de lecture/écriture
Sectors_Hidden		dd	16		;Nombre de secteurs cachés
Sectors_Per_Disk2	dd	39		;Nombre secteurs du volume si > 32 Mo
Boot_Drive		db	0x80		;Lecteur de démarrage
Reserved		db	0		;NA (pour NT seulement)
Extended_Boot_ID	db	0x29		;Signature Boot étendu 29h
Serial_Number		dd	0x01020304	;N° de série
Disk_Name		db	"COS2000    "	;Nom de volume
Fat_Type		db	"FAT16   "	;Type de système de fichiers

;Cpu_Message		db "CPU test",0
;Boot_Message		db "Booting ",0
;Finding_Message	db "System  ",0
;Loading_Message	db "Loading ",0
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
;	mov	al,[Stage]
;	cmp	al,0
;	jz	No_Ok
	mov	si,Is_Ok
	call	ShowString
;No_Ok:	
;	xor	ah,ah
;	mov	si,ax
;	add	si,Boot_Message ;Cpu_Message
;	call	ShowString
;	add	byte [Stage],0x09	
       ret
;
;Stage	db 0

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
;       call	Boot_Ok
        xor	ax,ax
        int	0x13
;        jc	Boot_Error
	xor	eax,eax
        add	ax,[Reserved_Sectors]
        add	ecx,eax
        mov	di,Fat_Buffer
        call	ReadSector
        jc	Boot_Error
        xor	eax,eax
        mov	al,[Fats_Number]
        mov	bx,[Sectors_Per_Fat]
        mul	bx
        add	ecx,eax
        mov	ax,32
        mul	word [Fits_Number]
        div	word [Sectors_Size]
        add	eax,ecx
        mov	dword [Serial_Number],eax
        xor	dx,dx
;        call	Boot_Ok
Find_System:
        mov	di,Buffer
        call	ReadSector
        jc	near Boot_Error
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
        inc	ecx
        jmp	Find_System
System_Found:
	xor	ecx,ecx
        mov	cx,[di+26]	
        mov	ax,0x0071
        mov	es,ax
        push	es
        mov	di,0x100
        push	di
;        call	Boot_Ok
        mov	si,The_Dot
Resume_Loading:
        cmp	cx,0x0FFF0
        jae	Finish_Loading
        call	ReadGroup
        jc	near Boot_Error
        call	ShowString
        mov  	bx,cx
        shl  	bx,1
        mov  	cx,[bx+Fat_Buffer]
        jmp  	Resume_Loading
Finish_Loading:
	call	Boot_Ok
        retf

;====================READSECTOR=======================
;Lit le secteur logique LBA ECX et le met en es:di
;-> ECX (limité à 2^32 secteurs, soit 2 To avec secteur 512 octets)
;<- Flag Carry si erreur
;=====================================================
ReadSector:
        pushad
        mov     ax, cx
        ror     ecx,16
        mov     dx,cx
        rol     ecx,16
        cmp     ecx,4128705
        ja      Extended_CHS
        div     word [Sectors_Per_Track]
        inc     dl
        mov     bl, dl
        xor     dx,dx
        div     word [Heads_Number]
        mov     dh, [Boot_Drive]
        xchg    dl, dh
        mov     cx, ax
        xchg    cl, ch
        shl     cl, 6
        or      cl, bl
        mov     bx, di
        mov     si, 4
        mov     al, 1
Read_Again:
        mov     ah, 2
        int     0x13
        jnc     Read_Done
        dec     si
        jnz     Read_Again
Read_Done:
        popad
        ret
Extended_CHS:
        mov     si,ECHS_Block
        mov     byte [si+Sizes],0x10
        mov     byte [si+Reserve],0x01
        mov     byte [si+NumSectors],0x01
        mov     [si+Adressseg],es
        mov     [si+Adressoff],di
        mov     [si+SectorLow],ax
        mov     [si+SectorHigh],dx
        mov     di,4
        mov     dl,[Boot_Drive]
Read_AgainX:
        mov     ah, 42h
        int     13h
        jnc     Read_Done
        dec     di
        jnz     Read_AgainX

;======================READGROUP======================
;lit le groupe cx en es:di et incr‚mente DI
;-> cx, es:di
;<- di, Flag Carry si erreur
;=====================================================
ReadGroup:
        push 	ax 
	push	cx 
	push	dx
        mov  	al,[Sectors_Per_Cluster]
        sub  	cx,2
        xor  	ah,ah
        mul  	cx
        mov  	cx,dx
        shl  	ecx,16
        mov  	cx,ax
        add  	ecx,dword [Serial_Number]
        mov  	al,[Sectors_Per_Cluster]
read:
        call 	ReadSector
        jc	errors
        inc 	ecx
        add  	di,[Sectors_Size]
        dec  	al
        jnz  	read
        clc	
errors:
        pop  	dx 
	pop	cx 
	pop	ax
        ret      

;======================SHOWSTR========================
;Affiche la chaine de caractère pointé par ds:si à l'écran
;-> DS, SI
;<- Flag Carry si erreur
;=====================================================
ShowString:
        ;push 	ax
        ;push	bx
        ;push	si
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
        ;pop 	si
        ;pop	bx
        ;pop	ax
	popa
        ret

times 510-($-$$) db ' '

dw	0xAA55

ECHS_Block	equ $
Buffer		equ $+26
Fat_Buffer	equ $+26+512

section .bss

%include "echs.h"

;ECHS_Block	resb 24
;Buffer		resb 512
;Fat_Buffer	resb 10000