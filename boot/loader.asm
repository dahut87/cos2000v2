;/*******************************************************************************/
;/* COS2000 - Compatible Operating System - LGPL v3 - Hordé Nicolas             */
;/*                                                                             */
[BITS 16]
[ORG 0x0]

SECTION .text
	push 	cs
	push 	cs
	pop 	ds
	pop 	es
	mov     si,msg0
	call    showstr
;Projection de la FAT12 en mémoire
	mov     si,msg1
	call    showstr
	call    initfat
    call    ok	
;Recherche du système
    mov     si,msg2
    call    showstr	
    mov     si,System_File	
	call    search
    call    ok	
;Activation du BUS A20
	mov     si,msg3
	call    showstr		
    call    EnableA20
    call    ok	
;Chargement d'une GDT pour le mode FLAT REAL
	mov     si,msg4
	call    showstr		
	mov     eax,cs		
	shl     eax,4
	mov     [GDT.Entry1+2],ax
	mov     [GDT.Entry2+2],ax
	shr     eax,16
	mov     [GDT.Entry1+4],al
	mov     [GDT.Entry2+4],al
	mov     eax,cs		
	shl     eax,4
	add     eax,gdt0
	mov     [GDTR.Address],eax
	lgdt    [GDTR]		
    call    ok	
;Passage en mode FLAT REAL
	mov     si,msg5
    call    showstr
	cli
    mov     eax,cr0
    or      al,1
    mov     cr0,eax
    jmp     short $+2
    mov     bx,SYS_DATA_SEL
    mov     fs,bx              
    mov     ds,bx
    mov     es,bx
    mov     gs,bx
    and     al,0FEh
    mov     cr0,eax
    jmp     short $+2
    mov     bx,cs
    mov     fs,bx              
    mov     ds,bx
    mov     es,bx
    mov     gs,bx           
    sti
    call    ok	    
;Chargement du noyau en 0x30000
	mov     si,msg6
	call    showstr		
    mov     ebx,0x300000
	call    load
    call    ok		
;Chargement du fichier PE
	mov     si,msg7
	call    showstr	
	call    initpe
    call    ok	
	
    ;suite:
	;mov    cx,suite
    ;call   debug
	mov     si,msg8
	call    showstr
	cli
;Passage en mode protégé
	mov     al,0x80	
	out     0x70,al
	mov     eax,cr0	
	or      al,1
	mov     cr0,eax
;Exécution du code 32 bits
	jmp     SYS_CODE_SEL:GoPMode32
		
		
Boot_Error:
	mov     si,error
	call    showstr 
    xor     ax,ax
    int     16h
    jmp     0xFFFF:0x0 	
        
ok:
    push    si 
    push    cx
    mov     cl,67
    call    eol
    mov     si,okay
	call    showstr
	pop     cx 
    pop     si
    ret	
        
;======================INITPE========================
;Initialise le PE de l'adresse EBX 
;-> 
;<- Flag Carry si erreur
;=====================================================              
initpe:
    pusha
    push   es
    xor    ax,ax
    mov    es,ax
    mov    [begin],ebx
    cmp    word [es:ebx+ELFheader.Magic],0x457F  ; bien l'entete .elf ?
    jne    near errorelf
    cmp    word [es:ebx+ELFheader.Magic+2],"LF"  ; bien l'entete .elf ?
    jne    near errorelf
    cmp    byte [es:ebx+ELFheader.Computertype],0x1  ; type ok ?
    jne    near errorelf
    cmp    byte [es:ebx+ELFheader.Endianness],0x1  ; type ok ?
    jne    near errorelf
    cmp    byte [es:ebx+ELFheader.Original],0x1  ; type ok ?
    jne    near errorelf
    cmp    byte [es:ebx+ELFheader.OS],0x0  ; type ok ?
    jne    near errorelf
    cmp    byte [es:ebx+ELFheader.ABI],0x0  ; type ok ?
    jne    near errorelf
    cmp    byte [es:ebx+ELFheader.Dummy],0x0  ; type ok ?
    jne    near errorelf
    mov    si,info1
    call   showstr
    mov    esi,[es:ebx+ELFheader.Entrypoint]
    mov    [entriepoint],esi    
    mov    esi,[es:ebx+ELFheader.Offsetprogram]
    add    esi,ebx
    mov    [offsetheader],esi
    mov    esi,[es:ebx+ELFheader.Offsetsection]
    add    esi,ebx
    mov    [offsetsection],esi  
    mov    cx,[es:ebx+ELFheader.Nbprogram]
    mov    [cs:nbprogram],cx
    mov    cx,[es:ebx+ELFheader.Nbsection]
    mov    [cs:nbsection],cx 
    xor    eax,eax
    mov    ax,[es:ebx+ELFheader.Index]
    xor    edx,edx
    mov    dx,[es:ebx+ELFheader.Sizesection]
    mul    edx
    add    eax,esi
    mov    eax,[es:eax+Sections.Offset]
    add    eax,ebx
    mov    [cs:symbol],eax
    xor    eax,eax
    add    ax,[es:ebx+ELFheader.Sizesection]
    add    esi,eax
sections:
    mov    edi,esi
    cmp    dword [es:edi+Sections.Type],0x00
    jz     nothing
    mov    esi,info2
    call   showstr
    push   ds
    push   es
    pop    ds
    mov    esi,[es:edi+Sections.Name]
    add    esi,[cs:symbol]
    call   showstr
    pop    ds
    mov    esi,info3
    call   showstr 
    mov    edx,[es:edi+Sections.Offset]
    push   cx
    mov    cx,32
    call   ShowHex
    pop    cx
    mov    esi,info4
    call   showstr 
    mov    edx,[es:edi+Sections.Size]
    push   cx
    mov    cx,32
    call   ShowHex
    pop    cx
    mov    esi,info5
    call   showstr 
    mov    edx,[es:edi+Sections.Vadress]
    push   cx
    mov    cx,32
    call   ShowHex
    pop    cx
    and    dword [es:edi+Sections.Flags],SF_ALLOC
    jne    itsok
    mov    esi,info6
    call   showstr 
    jmp    itsok2
itsok:
    mov    esi,info10
    call   showstr 
    call   copy2mem
    cmp    dword [es:edi+Sections.Type],ST_NOBITS
    jne    itsok2
    mov    esi,info9
    call   showstr 
    mov    esi,info10
    call   showstr 
    call   zero2mem
itsok2:   
    push   cx
    xor    cx,cx
    mov    edx,[es:edi+Sections.Align]
    mov    esi,info8
    call   showstr 
nextpower:
    cmp    edx,0
    je     powerok
    cmp    edx,1
    je     powerok
    cmp    cx,32
    je     powerok
    inc    cx
    shr    edx,1
    jnc    nextpower
powerok:
    mov    edx,ecx
    mov    cx,4
    call   ShowHex
    pop    cx
    call   showrtn
    mov    esi,edi
nothing:
    add    esi,eax
    dec    cx
    jnz    sections
    mov    esi,info7
    call   showstr 
    mov    edx,[cs:entriepoint]
    mov    cx,32
    call   ShowHex
    clc
    pop    es
    popa
    ret     

errorelf:
    stc
    pop     es
    popa
    ret

;==========DEBUG===========
;CX adresse
;->CX
;<- 
;==========================
debug:
    mov     esi,info11
    call    showstr 
    xor     edx,edx
    mov     dx,cs
    shl     edx,4     
    add     edx,ecx
    mov     cx,32
    call    ShowHex     
infini:
    jmp     infini

;==========COPY2MEM===========
;Copie de es:esi vers es:edi
;->ES:ESI ES:EDI CX
;<- Flag
;=============================
copy2mem:
    push    eax
    push    esi
    push    edi
    push    ecx
    mov     esi,[es:edi+Sections.Offset]
    add     esi,[cs:begin]
    mov     ecx,[es:edi+Sections.Size]
    shr     ecx,2
    inc     ecx
    mov     edi,[es:edi+Sections.Vadress]
copietomem:
    mov     eax,[es:esi]
    mov     [es:edi],eax
    add     edi,4
    add     esi,4       
    dec     ecx
    jnz     copietomem       
    pop     ecx
    pop     edi
    pop     esi
    pop     eax
    ret

;==========ZERO2MEM===========
;Remise à zero de es:edi
;->ES:EDI CX
;<- Flag
;=============================
zero2mem:
    push    eax
    push    esi
    push    edi
    push    ecx
    mov     esi,[es:edi+Sections.Offset]
    add     esi,[cs:begin]
    mov     ecx,[es:edi+Sections.Size]
    shr     ecx,2
    inc     ecx
    mov     edi,[es:edi+Sections.Vadress]
    mov     eax,0
zerotomem:
    mov     [es:edi],eax
    add     edi,4
    dec     ecx
    jnz     zerotomem  
    pop     ecx
    pop     edi
    pop     esi
    pop     eax
    ret

begin dd 0
entriepoint dd 0
offsetheader dd 0
offsetsection dd 0
nbprogram dw 0
nbsection dw 0
symbol dd 0

;==========SHOWHEX===========
;Affiche un nombre hexadécimal EDX de taille CX aprés le curseur
;-> EDX un entier, CX la taille
;<- 
;============================
ShowHex:
    push    ax
    push    bx 
    push    cx 
    push    edx
    push    si
    mov     ax,cx
	shr     ax,2
    sub     cx,32
    neg     cx
    shl     edx,cl
    xor     cx,cx
    inc     cx
Hexaize:
    rol     edx,4
    mov     bx,dx
    and     bx,0fh
    mov     si,ax
    mov     al,[cs:bx+Tab]
    xor     bx,bx  
    mov	    ah,0x09
    mov	    bl,[cs:thecolor]
    int	    0x10
    mov	    ah,0x0E
    int	    0x10 
    mov     ax,si
    dec     al
    jnz     Hexaize
    pop     si
    pop     edx 
    pop     cx 
    pop     bx 
    pop     ax
    ret
Tab db '0123456789ABCDEF'

;============EOL=============
;Va en colonne CX
;-> 
;<- Flag Carry si erreur
;============================   
eol:
    pusha
    mov     bp,cx
    mov     ah,03
    xor     bx,bx
    int     10h
    mov     ah,02
    mov     cx,bp
    mov     dl,cl
    int     10h
    popa
    ret

;============LOAD==============
;Charge le groupe en mémoire en EBX pour le groupe CX
;-> CX
;<- Flag Carry si erreur
;==============================      
load:
    push    ax
    push    ebx
    push    ecx 
    push    edx
    push    esi 
    push    edi
    push    fs
    xor     edx,edx
    xor     ax,ax
    mov     fs,ax
    mov	    si,The_Dot
	mov     dx,[Sectors_Size]
Resume_Loading:
    cmp	    cx,0x0FF0
    jae	    Finish_Loading
    push	cx
    add	    cx,word [data]
    mov     edi,Buffer
    call	ReadSector
    pop	    cx
    jc	    near Boot_Error
	call	showstr
    push    esi 
    push    ebx
    push    cx
    mov     esi,Buffer
    mov     cx,dx
    shr     cx,2
copie:
    mov     eax,[ds:esi]
    mov     [fs:ebx],eax
    add     ebx,4
    add     esi,4       
    dec     cx
    jnz     copie
    pop     cx
    pop     ebx
    pop     esi 
    add	    ebx,edx 
    call	NextFatGroup
    jc	    near Boot_Error
    jmp	    Resume_Loading
Finish_Loading:
    pop     fs
    pop     edi
    pop     esi
    pop     edx
    pop     ecx
    pop     ebx
    pop     ax
    ret

;===========INITFAT=============
;Initialise les variables de la fat
;-> 
;<- Flag Carry si erreur
;===============================
initfat:
    pusha
    push    ds
    xor     ax,ax
    mov     ds,ax
    mov     si,0x7C0B
    mov     di,bootsector
    mov     cx, 512
    rep     movsb
    pop     ds
	mov	    cx,[Reserved_Sectors]
    add	    cx,[Sectors_Hidden]
    adc	    cx,[Sectors_Hidden+2]
    mov	    bx,[Sectors_Per_Fat]
    mov     di,Fat_Buffer
    push    bx
    push    cx
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
    mov     [entries],cx
    mov	    ax,32
    mul	    word [Fits_Number]
    div	    word [Sectors_Size]
    add	    cx,ax
    sub	    cx,2
    mov	    word [data],cx    
    popa
    ret
        
;===========SEARCH============
;Recherche le groupe d'un fichier
;-> si nom du fichier
;<- Flag Carry si erreur CX
;=============================
search:
    push    bx
    push    dx 
    push    di
    mov     cx,[entries]
    mov	    di,Buffer
    call    ReadSector
    jc      Boot_Error
    xor	    bx,bx
Next_Root_Entrie:
    cmp	    byte [di],0
    je	    Boot_Error
    push    si
    push	di
    push	cx
    mov	    cx,32
    rep	    cmpsb
    pop	    cx
    pop	    di
    pop     si
    je	    Found
    add	    di,32
    add	    bx,32
    inc	    dx
    cmp	    dx,[Fits_Number]
    ja	    Boot_Error
    cmp	    bx,[Sectors_Size]
    jb	    Next_Root_Entrie
    inc	    cx
Found:
    mov	    cx,[di+26+32]
    pop     di 
    pop     dx 
    pop     bx
    ret
    
;============SHOWRTN============
;Affiche la chaine de caractère return
;-> 
;<- Flag Carry si erreur
;===============================
showrtn:
    push    ds
    push    ax
    push    esi
    mov     esi,return
    mov     ax,cs
    mov     ds,ax
    call    showstr
    pop     esi
    pop     ax
    pop     ds
    ret

;==============SHOWSTR=============
;Affiche la chaine de caractère pointé par ds:esi à l'écran
;-> DS, ESI
;<- Flag Carry si erreur
;==================================
showstr:
    pushad
    xor  bh,bh
nextchar:
    mov     al,[ds:esi]
    inc     esi
    or	    al,al
    jz	    endshow
    cmp     al,' '
    jb      justchar       
    cmp     al,'#'
    jne     nocolor
    mov     al,[ds:esi]
    inc     esi
    sub     al,'0'
    shl     al,3
    mov     [cs:thecolor],al
    shr     al,2
    add     [cs:thecolor],al
    mov     al,[ds:esi]
    inc         esi
    sub     al,'0'
    add     [cs:thecolor],al
    jmp	    nextchar
nocolor:
    cmp     al,'@'
    jne     nocolor2
    mov     al,[ds:esi]
    inc     esi
    sub     al,'0'
    shl     al,3
    mov     cl,al
    shr     al,2
    add     cl,al
    mov     al,[ds:esi]
    inc     esi
    sub     cl,'0'
    add     cl,al
    call    eol
    jmp	    nextchar
nocolor2:
    xor     bx,bx  
    mov	    ah,0x09
    mov	    bl,[cs:thecolor]
    xor     cx,cx
    inc     cx
    int	    0x10
justchar:
    mov	    ah,0x0E
    int	    0x10 
    jmp	    nextchar
endshow:
    popad
	ret  

;==========READSECTOR============
;Lit le secteur logique LBA CX et le met en es:di
;-> CX (limité à 65536 secteurs, soit 32 Mo avec secteur 512 octets)
;<- Flag Carry si erreur
;================================
ReadSector:
	pusha
	mov	    ax,cx
	xor	    dx,dx
	div	    word [Sectors_Per_Track]
	inc	    dl
	mov	    bl,dl
	xor	    dx,dx
	div	    word [Heads_Number]
	mov	    dh, [Boot_Drive]
	xchg	dl,dh
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
	
;============NEXTFATGROUP===========
;Renvoie en CX le groupe qui succède dans la FAT le groupe CX
;-> CX
;<- CX
;===================================
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

;============EnableA20============
;-> 
;<-
;Ouvre l'autoroute A20	
;=================================
EnableA20:
	cli
	call    ClearKeybBuffer
	call    WaitKeybCommand
	mov     al,0xd1
	out     0x64,al
	call    WaitKeybCommand
	mov     al,0xdf
	out     0x60,al
	call    WaitKeybCommand
	jmp     A20Enabled
WaitKeybCommand:
	in      al,0x64
	test    al,0x02
	jnz     WaitKeybCommand
	ret
ClearKeybBuffer:
	in      al,0x64
	test    al,0x01
	jnz     ReadKeyb
	ret
ReadKeyb:	
	in      al,0x60
	jmp     ClearKeybBuffer
A20Enabled:
	sti
	ret		
			

;=================================
;CODE 32 bits mode protégé
;=================================
GoPMode32:
[BITS 32]
;Initialisation des selecteurs
    mov     ax,ALL_DATA_SEL
    mov     es,ax
    mov     gs,ax
	mov     fs,ax
	mov     ss,ax
	mov     ds,ax
	mov     esp,0x3fffff
	push    KERNEL_SEL
	push    dword [cs:entriepoint]
;Execution du système
	retf
		
section .data

thecolor db 0x07
msg0 db '#12@20-=< Lancement du Chargeur ELF >=-',0x0A,0x0D,0x0A,0x0D,0
msg1 db '#07Initialisation de la FAT',0
msg2 db '#07Recherche du systeme',0
msg3 db '#07Activation adressage 24 bits',0
msg4 db '#07Chargement des descripteurs',0
msg5 db '#07Passage en mode Flat real',0
msg6 db '#07Chargement du systeme',0
msg7 db '#07Mise en place format ELF',0
msg8 db '#07Passage en mode protege',0


info1 db 0x0A,0x0D,'Format ELF i386 reconnu, #08Sections :',0x0A,0x0D,0
info2 db '  ',0
info3 db '@12 | ',0
info4 db ' - ',0
info5 db ' -> ',0
info6 db '@35#12NON ALLOUE #08',0
info7 db 'Point entree en ',0
info8 db '@48 2**',0
info9 db '@15#12  VIDE  #08',0
info10 db '@70MEM',0
info11 db 0x0A,0x0D,'#12Arret debug en ',0

return db 0x0A,0x0D,0

okay db '    #15[  #10OK  #15]',0x0A,0x0D,0
error db '     #15[#12Erreur#15]',0x0A,0x0D,0x0A,0x0D,'    <Appuyez une touche pour redemarrer>',0
The_Dot db '.',0
System_File		    db "As",0,"y",0,"s",0,"t",0,"e",0,0x0F,0,0xB8,"m",0,".",0,"s",0,"y",0,"s",0,0,0,0,0,0xFF,0xFF,0xFF,0xFF
entries dw 0
data dw 0
xy dw 0

GDTR:
.Size:		dw GDT_END 
.Address:	dd 0 

gdt0 equ $			; null entry
GDT:
.Entry0:	dw 0		; limit 15:0
		dw 0		; base 15:0
		db 0		; base 23:16
		db 0		; type
		db 0		; limit 19:16, flags
		db 0		; base 31:24


SYS_CODE_SEL equ $-gdt0		; code segment descriptor

.Entry1:	dw 0xFFFF
		dw 0x0		; base
		db 0x0         	; base
		db 0x9A         ; present, ring 0, code, non-conforming, readable
		db 0xCF   	; 32 bit
		db 0


SYS_DATA_SEL equ $-gdt0		; data segment descriptor

.Entry2:	dw 0xFFFF
		dw 0x0    	; base
		db 0x0	  	; base
		db 0x92         ; present, ring 0, data, expand-up, writable
		db 0xCF		; 32 bit
		db 0


ALL_DATA_SEL equ $-gdt0	; 4meg data segment descriptor

.Entry3:	dw 0xFFFF
		dw 0x0          ; base
		db 0x0          ; base
		db 0x92         ; present, ring 0, data, expand-up, writable
		db 0xCF		; 4k pages, 32 bit
		db 0


KERNEL_SEL equ $-gdt0	; 4g code segment descriptor

.Entry4:	dw 0xffff
		dw 0x0          ; base
		db 0x0          ; base
		db 0x9A         ; present, ring 0, code, non-conforming, readable
		db 0xCF		; 4k pages, 32 bit
		db 0

GDT_END equ $-gdt0 -1

bootsector equ $
Sectors_Size		dw	0		;Nombre d'octets/secteur
Sectors_Per_Cluster	db	0		;Nombre de secteurs/cluster
Reserved_Sectors	dw	0		;Nombre de secteurs réservé
Fats_Number		    db	0		;Nombre de copies de la FAT
Fits_Number		    dw	0		;Taille du répertoire racine
Sectors_Per_Disk	dw	0		;Nombre secteurs du volume si < 32 Mo
Media_Descriptor	db	0		;Descripteur de média
Sectors_Per_Fat		dw	0		;Nombre secteurs/FAT
Sectors_Per_Track	dw	0		;Nombre secteurs/piste
Heads_Number		dw	0		;Nombre de tete de lecture/écriture
Sectors_Hidden		dd	0		;Nombre de secteurs cachés
Sectors_Per_Disk2	dd	0		;Nombre secteurs du volume si > 32 Mo
Boot_Drive		    db	0		;Lecteur de démarrage
Reserved		    db	0		;NA (pour NT seulement)
Extended_Boot_ID	db	0		;Signature Boot étendu 29h

Buffer            equ $
Fat_Buffer        equ $+512
        	
	SECTION .bss

%include "elf.h"

