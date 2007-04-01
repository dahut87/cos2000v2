[BITS 16]
[ORG 0x7C00]

section .text

start:
	jmp 	near boot

Disk_ID			db	"COS2000A"	;Fabricant + n° de série Formatage
Sectors_Size		dw	512		;Nombre d"octets/secteur
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

Loading_Ok      db "Secteur en execution",0x0A,0x0D,0
Reg_Names       db "cs ",0
                db "ds ",0
                db "es ",0
                db "fs ",0
                db "gs ",0
                db "ss ",0
                db "eax",0
                db "ebx",0
                db "ecx",0
                db "edx",0
                db "esi",0
                db "edi",0
                db "esp",0
                db "ebp",0
                db "eip",0
                db "FLG",0
                db "cr0",0
                db "cr1",0
                db "cr2",0
                db "cr3",0

Return          db 0x0A,0x0D,0

Numbers equ 20

boot:
        mov     [cs:segms],cs
        mov     [cs:segms+4],ds
        mov     [cs:segms+8],es
        mov     [cs:segms+12],fs
        mov     [cs:segms+16],gs
        mov     [cs:segms+20],ss
        mov     [cs:segms+24],eax
        mov     [cs:segms+28],ebx
        mov     [cs:segms+32],ecx
        mov     [cs:segms+36],edx
        mov     [cs:segms+40],esi
        mov     [cs:segms+44],edi
        mov     [cs:segms+48],esp
        mov     [cs:segms+52],ebp
IP:
        mov     word [cs:segms+56],IP
        mov     word [cs:segms+58],0
        pushfd   
        pop     dword [cs:segms+60]
        mov     eax,cr0
        mov     [cs:segms+64],eax
        mov     eax,cr0
        mov     [cs:segms+68],eax
        mov     eax,cr2
        mov     [cs:segms+72],eax
        mov     eax,cr3
        mov     [cs:segms+76],eax
        cli
        mov  	ax,0x9000
        mov  	ss,ax
        mov  	sp,0xFFFF
        sti
        push 	cs
        push 	cs
        pop  	es
        pop  	ds
        mov  	si,Loading_Ok
        call 	ShowString
        xor  	bx,bx
Show_All_Regs:
        mov 	si,Reg_Names
        shl 	bx,2
        add 	si,bx
        call 	ShowString
        mov 	al,":"
        call 	ShowChar
        mov 	si,segms
        mov 	edx,[bx+si]
        shr 	bx,2
        mov 	cx,32
        cmp 	bx,6
        jae 	Reg_Size_32
        mov 	cx,16
Reg_Size_32:
        call 	ShowHex
        mov 	si,Return
        call 	ShowString
        inc 	bx
        cmp 	bx,Numbers
        jb 	Show_All_Regs
Halting_Cpu:
        jmp 	Halting_Cpu

;==================SHOWHEX==================
;Affiche un nombre hexadécimal EDX de taille CX aprés le curseur
;-> EDX un entier, CX la taille
;<- 
;===========================================
ShowHex:
	push 	ax
	push	bx
	push	cx
	push	edx
 	mov	ax,cx
  	shr	ax,2
   	sub	cx,32
    	neg	cx
     	shl	edx,cl
      	xchg	ax,cx
Hex_Decompose:
	rol	edx,4
 	mov  	bx,dx
  	and  	bx,0x0F
   	mov  	al,[cs:bx+Hex_Table]
    	call 	ShowChar
     	dec  	cl
      	jnz  	Hex_Decompose
       	pop  	edx
      	pop	cx
      	pop	bx
      	pop	ax
      	ret
Hex_Table db "0123456789ABCDEF"

;===================CLS====================
;Efface l"écran
;-> 
;<-
;==========================================
Cls:
        push    ax
        mov     ax,0x0003
        int     0x10
        pop     ax
        ret

;================SHOWCHAR==================
;Affiche un caractère pointé dans AL
;-> AL
;<-
;==========================================
ShowChar:
        push    ax
        push	bx
        mov     ah,0x0E
        mov     bx,0x07
        int     0x10
        pop     bx
        pop	ax
	ret

;===================SHOWSTR================
;Affiche une chaine de caractère pointé par SI
;-> SI pointe une chaine
;<-
;==========================================
ShowString:
        push    ax
        push	bx
        push	si
        cld
Show_Next_Char:
	lodsb
        or      al,al
        jz      String_Showed
        call    ShowChar
        jmp     Show_Next_Char
String_Showed:
        pop     si
        pop	bx
        pop	ax
	ret

;=================WAITKEY=================
;Attend l"appuie d"une touche et
;renvoie en AL la touche appuyée
;-> 
;<- AL
;=========================================
WaitKey:
        mov     ax,0x00
        int     0x16
        ret

times 510-($-$$) db ' '

dw	0xAA55

section .bss

segms      	resb 10000

