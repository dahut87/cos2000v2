[BITS 16]
[ORG 0x0100]

section .text

start:
        mov     di,disk
        call    detectdisk
        mov     si,di
        mov     di,thedrive
        call    getalldriveinfos
        mov     [numbers],cx
        mov     al,3
        mov     word [sel],0
main:
        call    cls
        mov     si,thedrive
        call    showalldriveinfos
        call    selection
waitafter:
        call    waitkey
        call    lower
        cmp     ah,0x50
        jne     tre1
        mov     ax,[sel]
        inc     al
        cmp     ax,[numbers]
        jz      waitafter
        mov     [sel],ax
        add     al,3
        call    selection
        jmp     waitafter
tre1:
        cmp     ah,0x48
        jne     tre2
        mov     ax,[sel]
        cmp     ax,0
        jz      waitafter
        dec     al
        mov     [sel],ax
        add     al,3
        call    selection
        jmp     waitafter
tre2:
        cmp     al,0x0D
        jne     tre3
        mov     bx,[sel]
        shl     bx,4
        add     bx,thedrive
        mov     bp,[bx]
        call    initdisk
        mov     di,thepart  
        call    IsMbr
        je      okmbr2
        call    getvolume
        mov     cx,1
        jmp     nombr2
okmbr2:
        call    getpartitions
nombr2:
        call    cls
        mov     si,di
        call    showdriveinfos
        call    waitkey
        call    lower
        mov     al,[cs:old]
        jmp     main
tre3:
        cmp     al,'i'
        jne     tre4
        mov     bx,[sel]
        shl     bx,4
        add     bx,thedrive
        mov     bp,[bx]
        call    initdisk
        mov     al,[cs:old]
        cmp     byte [bx+1],1
        je      noerase
        mov     si,erasing
        call    showstr
        call    choice
        jnz     near main
noerase:
        cmp     byte [bx+2],1
        jne     noreinst
        mov     si,remplacing
        call    showstr
        call    choice
        jnz     near main
noreinst:
        cmp     byte [bx+3],0
        jne     nolbas
        cmp     dword [bx+4],8300
        jb      nolbas
        mov     si,nowereLBA
        call    showstr
        call    choice
        jnz     near main
nolbas:
        mov     si,really
        call    showstr
        call    choice
        jnz     near main
        call    install
        mov     si,installed
        call    showstr
        mov     si,disk  
        mov     di,thedrive
        call    getalldriveinfos
        push    ax
        call    waitkey
        pop     ax
        jmp     main
tre4:
        cmp     al,0x1B
        jne     tre5
        call    cls
        ret
tre5:
        cmp     al,'u'
        jne     tre6
        mov     bx,[sel]
        shl     bx,4
        add     bx,thedrive
        mov     bp,[bx]
        mov     al,[cs:old]  
        cmp     byte [bx+2],1
        je      letsgo
        mov     si,noinst
        call    showstr
        push    ax
        call    waitkey
        pop     ax
        jmp     main
letsgo:
        mov     si,uninst
        call    showstr
        call    choice
        jnz     near main
        call    uninstall
        mov     si,uninstalled
        call    showstr   
        mov     si,disk  
        mov     di,thedrive
        call    getalldriveinfos
        push    ax
        call    waitkey
        pop     ax
        jmp     main
tre6:
        cmp     al,'r'
        jne     near waitafter
        call    reboot

;=============================================================================

;                               DONNEES FIXES

;=============================================================================
msg2 db "ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄ¿"
     db "³     Volume     ³  Type  ³ Installed ³   LBA   ³   Size MByte   ³ Partitions  ³"
     db "ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÄ´",0
tab2 db "ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄÄÙ"
     db "[ENTER] informations [ESC] quit [R] reboot [I] install MBROL [U] uninstall MBROL",0   
noinst          db 0x0A,0x0D,"MBROL is not yet installed, press a key...",0
really          db 0x0A,0x0D,"Do you really want to install MBROL ?",0
erasing         db 0x0A,0x0D,"Mbrol need a partionned disk, on this disk data will be erased, do it ?",0
remplacing      db 0x0A,0x0D,"Mbrol is already installed, do you want to replace old installation ?",0
nowereLBA       db 0x0A,0x0D,"BIOS or this drive don''t support LBA and it necessite LBA adressing, install ?",0
installed       db 0x0A,0x0D,"MBROL is installed, press a key to continue...",0
uninstalled     db 0x0A,0x0D,"MBROL is uninstalled, press a key to continue...",0
uninst          db 0x0A,0x0D,"Do you really want to uninstall MBROL ?",0
processing      db 0x0A,0x0D,"Processing",0

floppy          db "Floppy Disk",0
hard            db "Hard   Disk",0
boot            db " Boot ",0
mbr             db " Mbr  ",0
yes1            db "   Yes   ",0
no1             db "   No    ",0
yes2            db "  Yes  ",0
no2             db "  No   ",0
choiced         db 0x0A,0x0D,"[y/n]:",0
                       
msg db "ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÂÄÄÄÄÄÄÄÄÄÄÄÄÄ¿"
    db "³      Name     ³    Type    ³ Boot ³   Statut  ³   Size MByte   ³ StartSector ³"
    db "ÃÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÅÄÄÄÄÄÄÄÄÄÄÄÄÄ´",0
tab db "ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÁÄÄÄÄÄÄÄÄÄÄÄÄÄÙ"
    db "Press any key to close...",0
sep db " ³ ",0

partkind db 0x01,"FAT12     ",0
         db 0x04,"FAT16     ",0
         db 0x05,"Extended  ",0
         db 0x06,"FAT16B    ",0
         db 0x07,"IFS       ",0
         db 0x0B,"FAT32     ",0
         db 0x0C,"FAT32X    ",0
         db 0x0E,"FAT16X    ",0
         db 0x0F,"ExtendedX ",0
         db 0x81,"Linux 1.4b",0
         db 0x82,"Linux SWAP",0
         db 0x83,"Linux EXT2",0
         db 0x11,"FAT12 Hid ",0
         db 0x14,"FAT16  Hid",0
         db 0x16,"FAT16B Hid",0
         db 0x17,"IFS Hid   ",0
         db 0x1B,"FAT32 Hid ",0
         db 0x1C,"FAT32X Hid",0
         db 0x1E,"FAT16X Hid",0
         db 0xFF,"Unknowed  ",0

noname    db "Unknowed   ","Unkowned  ",0
unknowed  db "Movable  ",0
primary   db "Primary  ",0
secondary db "Secondary",0
yes       db "Yes ",0
no        db "No  ",0
mo        db " MB",0

;=============================================================================

;                           ROUTINES PARTICULIERES

;=============================================================================
;converti al en lowercase
lower:
        cmp     al,'A'
        jb      noupper
        cmp     al,'Z'
        ja      noupper
        add     al,'a'-'A'
noupper:
        ret

;redemarrage … chaud
reboot:
        mov     ax,0x40
        mov     ds,ax
        mov     bx,0x1234
        mov     [ds:0x0072],bx
	   jmp	  0xFFFF:0x0000

;Dialogue qui demande si Yes or No et Renvoie zero si YES et 
choice:
        push    ax 
	   push	  si
        mov     si,choiced
        call    showstr
waiter:
        call    waitkey
        call    lower
        cmp     al,'y'
        je      iyes
        cmp     al,'n'
        je      ino
        jmp     waiter
iyes:
        call    showchar
        cmp     al,al
        jmp     endof
ino:
        call    showchar
        cmp     al,' '
        jmp     endof
endof:
        pop     si 
		pop	ax
        ret           

;install MBROL sur le disque BP
install:
        push    ax 
	push	ecx 
	push	si 
	push	di
        mov     si,processing
        call    showstr
        call    initdisk
        mov     ecx,0
        mov     di,buffer
        call    readsector
        inc     ecx
        mov     si,di
        cmp     dword [di+2],"MBRO"
        je      nosave
        call    writesector
nosave:
        add     si,0x01BE
        mov     di,prog+0x01BE
        mov     cx,16*4/4
        cld
        rep     movsd
        mov     ecx,0
        mov     si,prog
        call    writesector
        inc     ecx
copying:
        inc     ecx
        add     si,512   
        call    writesector
        mov     al,'.'
        call    showchar
        cmp     ecx,7
        jb      copying
        pop     di 
	pop	si 
	pop	ecx 
	pop	ax
        ret

;desinstalle MBROL sur le disque BP
uninstall:
        push    ecx 
		push	si 
	push	di
        call    initdisk
        mov     ecx,1
        mov     di,buffer
        call    readsector
        dec     ecx
        mov     si,di
        call    writesector
        pop     di 
	pop	si 
	pop	ecx
        ret     

;Affiche des infos sur le volume
showalldriveinfos:
        push    ax 
	push	bx 
	push	cx 
	push	edx 
	push	si 
        mov     si,msg2
        call    showstr
        mov     bx,thedrive
        mov     cx,[numbers]
showall:
        mov     al,'³'
        call    showchar
        mov     al,' '
        call    showchar
        mov     al,[bx]
        mov     si,floppy
        cmp     al,0x80
        jb      disks
        mov     si,hard
disks:
        call    showstr
        and     al,0xF
        push    ax
        mov     al,' '
        call    showchar
        mov     al,'0'
        call    showchar 
        pop     ax
        add     al,'0'
        call    showchar
        mov     si,sep
        call    showstr
        mov     si,boot
        cmp     byte [bx+1],1
        jne     booter
        mov     si,mbr
booter:
        call    showstr
        mov     si,sep
        call    showstr
        mov     si,no1
        cmp     byte [bx+2],1
        jne     noinsts
        mov     si,yes1
noinsts:
        call    showstr
        mov     si,sep
        call    showstr
        mov     si,no2
        cmp     byte [bx+3],1
        jne     lbas
        mov     si,yes2
lbas:
        call    showstr
        mov     si,sep
        call    showstr
        mov     edx,[bx+4]
        call    showint
        mov     si,mo
        call    showstr
        mov     si,sep
        call    showstr
        mov     dx,[bx+8]
        call    showint
        mov     al,' '
        call    showchar 
        mov     al,'³'
        call    showchar
        add     bx,16
        dec     cx
        jnz     near showall
        mov     si,tab2
        call    showstr
        pop     di 
	pop	edx 
	pop	cx 
	pop	bx 
	pop	ax
        ret

;renvoie en ES:DI les infos sur les disques DS:SI et Nb dans CX
getalldriveinfos:
        push    ax 
	push	bx 
	push	dx 
	push	si 
	push	di 
	push	bp
        xor     bx,bx
        xor     cx,cx
show2:
        xor     dh,dh
        mov     dl,[si]
        inc     si
        cmp     dl,0xFF
        je      endofshow2
        inc     cx           
        mov     bp,dx
        call    initdisk
        mov     [es:di],dl
        call    IsMbr
        sete    [es:di+1]
        call    IsInstalled
        sete    [es:di+2]
        call    IsLBA
        setnc   [es:di+3]
        push    cx
        call    getcapacity
        mov     [es:di+4],ecx
        mov     cx,1
        cmp     byte [si-1],0x80
        jb      noone
        push    es 
	push	di
        push    cs
        pop     es
        mov     di,dummys
        call    getpartitions
        pop     di 
	pop	es
noone:
        mov     [es:di+8],cx
        add     di,16
        pop     cx
        jmp     show2
endofshow2:
        pop     bp 
	pop	di 
	pop	si 
	pop	dx 
	pop	bx 
	pop	ax
        ret

;===========ISInstalled==============
;Zero si Mbr sur le premier secteur du support BP
;-> BP
;<- 
;====================================
IsInstalled:
        push    ax 
	push	ecx 
	push	di 
	push	es
        mov     ecx,0
        mov     di,buffer
        push    cs
        pop     es
        call    readsector
        cmp     dword [di+2],"MBRO"
        pop     es 
	pop	di 
	pop	ecx 
	pop	ax
        ret    

;Affiche des infos sur le volume
showdriveinfos:
        push    ax 
	push	bx 
	push	cx 
	push	edx 
	push	si 
        mov     si,msg
        call    showstr
        mov     bx,thepart
showall2:
        mov     al,'³'
        call    showchar
        mov     al,' '
        call    showchar
        mov     al,'-'
        call    showchar
        mov     al,'>'
        call    showchar
        mov     si,bx
        add     si,28
        call    showstr
        mov     si,sep
        call    showstr
        mov     al,[bx+4]
        cmp     al,0xFF
        je      useboot
        mov     si,partkind+1
search:
        cmp     byte [si-1],0xFF
        je      notboot
        cmp     [si-1],al
        je      notboot
        add     si,12
        jmp     search
useboot:
        mov     si,bx
        add     si,40
notboot:
        call    showstr
        mov     si,sep
        call    showstr
        mov     si,yes
        cmp     byte [bx],0x80
        je      okboot
        mov     si,no
okboot:
        call    showstr
        mov     si,sep
        call    showstr
        mov     si,primary
        cmp     dword [bx+16],0
        je      okprimary
        mov     si,secondary
okprimary:
        cmp     byte [bx+4],0xFF
        jne     wasno
        mov     si,unknowed
wasno:
        call    showstr
        mov     si,sep
        call    showstr
        mov     edx,[bx+12]
        shr     edx,11
        call    showint
        mov     si,mo
        call    showstr
        mov     si,sep
        call    showstr
        mov     edx,[bx+22]
        call    showint
        mov     al,' '
        call    showchar
        mov     al,'³'
        call    showchar
        add     bx,64
        dec     cx
        jnz     near showall2
        mov     si,tab
        call    showstr
        pop     si 
	pop	edx 
	pop	cx 
	pop	bx 
	pop	ax
        ret

;met en surlign‚e la ligne al et seulement al
selection:
        push    cx
        mov     ch,0x70
        call    selectline
        xchg    al,[cs:old]
        cmp     al,[cs:old]
        je      nochange  
        mov     ch,0x07
        call    selectline
nochange: 
        pop     cx
        ret

old db 20

;selectionne la ligne al de couleur ch
selectline:
        push    ax 
	push	bx 
	push	cx 
	push	ds
        mov     bh,160
        mul     bh
        mov     bx,ax
        mov     ax,0xb800
        mov     ds,ax
        inc     bx
        mov     cl,80
select:
        mov     [bx],ch
        add     bx,2
        dec     cl
        jnz     select
        pop     ds 
	pop	cx 
	pop	bx 
	pop	ax
        ret

;=============================================================================

;                  ROUTINES UNIVERSELLES INSPIREE DE COS2000

;=============================================================================

;============Getcapacity=============
;Renvoie la capacit‚ du disque Bp dans ECX
;-> BP
;<-
;====================================
getcapacity:
        mov     ecx,0
        ret

;===============IsLBA================
;Renvoie zero = 1 si LBA  
;-> BP
;<- 
;====================================
IsLBA:
        push    ax 
	push	bx 
	push	cx 
	push	dx
        mov     dx,bp
        mov     ah,0x41
        mov     bx,0x55AA
        int     0x13
        pop     dx 
	pop	cx 
	pop	bx 
	pop	ax
        ret

;=========GETDRIVEINFOS==============
;Envoie en es:di les lecteurs logiques sur les volumes point‚ par ds:si  
;-> BP, ES:DI
;<- CX leur nombre
;====================================
getdriveinfos:
        push    ax 
	push	bx 
	push	dx 
	push	si 
	push	di 
	push	bp
        push    di
        xor     bx,bx
show:
        xor     dh,dh
        mov     dl,[si]
        inc     si
        cmp     dl,0xFF
        je      endofshow
        mov     bp,dx
        call    initdisk
        call    IsMbr
        je      okmbr
        call    getvolume
        inc     bx
        add     di,64
        jmp     show
okmbr:
        call    getpartitions
        mov     ax,cx
        shl     ax,6
        add     di,ax
        add     bx,cx
        jmp     show
endofshow:
        pop     si
        mov     al,'C'
        mov     cx,bx
        push    cx
        push si
checkp:
        cmp     byte [si+4],5
        je     notprim
        cmp     byte [si+4],0x0F
        je     notprim       
        cmp     byte [si+16],0
        jnz     notprim
        cmp     byte [si+26],0x80
        jb      notprim
        mov     [si+51],al
        inc     al        
notprim:
        add     si,64
        dec     cx
        jnz     checkp
        pop     si 
	pop	cx
        push    cx 
	push	si
checkl:
        cmp     byte [si+4],5
        je      notext
        cmp     byte [si+4],0x0F
        je      notext
        cmp     byte [si+16],0x00
        jz      notext    
        cmp     byte [si+26],0x80
        jb      notext      
        mov     [si+51],al
        inc     al        
notext:
        add     si,64
        dec     cx
        jnz     checkl
        pop     si
        pop	cx
        mov     al,'A'
        push    cx
        push	si
checkn:
        cmp     byte [si+26],0x80
        jae     notlec      
        mov     [si+51],al
        inc     al        
notlec:
        add     si,64
        dec     cx
        jnz     checkn
        pop     si
        pop	cx
        mov     al,'*'
        push    cx 
	push	si
checko:
        cmp     byte [si+4],5
        je      ext2
        cmp     byte [si+4],0x0F
        jne     notext2
ext2:
        mov     [si+51],al
notext2:
        add     si,64
        dec     cx
        jnz     checko
        pop     si
        pop	cx
        pop     bp 
	pop	di 
	pop	si 
	pop	dx 
	pop	bx 
	pop	ax
        ret

;=============GETVOLUME==============
;Envoie en es:di les infos disque EBP
;-> BP, ES:DI
;<- 
;====================================
getvolume:
        push    ax 
	push	ecx 
	push	edx 
	push	di 
        push    di
        mov     cx,64
        mov     al,0
        cld
        rep     stosb
        pop     di
        mov     byte [es:di+4],0xFF
        mov     ecx,0
        mov     [di+26],bp
        push    di
        add     di,28
        call    getbootinfos
        pop     di
        mov     [di+12],edx
        pop     di 
	pop	edx 
	pop	ecx 
	pop	ax
        ret          

;==========GETBOOTINFOS==============
;Envoie en es:di le nom du volume & le FS d'adresse LBA ECX sur disque EBP
;-> BP, ES:DI, ECX
;<- 
;====================================
getbootinfos:
        push    ax 
	push	cx 
	push	si 
	push	di
        push    di
        push	es
        push    cs
        pop     es
        mov     di,buffer
        mov     si,di
        call    readsector
        pop     es
        pop	di
        cmp     byte [si+0x13],0
        je      more32
        xor     edx,edx
        mov     dx,[si+0x13]
        jmp     more16
more32:
        mov     edx,[si+0x20]
more16:
        cmp     byte  [si+0x42],0x29
        je      fat32
        cmp     byte  [si+0x26],0x29
        je      fat16
unk:
        mov     si,noname
        jmp     rel
fat32:
        add     si,0x47
        jmp     rel
fat16:
        add     si,0x2B
rel:        
        mov     cx,11
        cld
        rep     movsb
        mov     al,0
        stosb
        mov     cx,8
        rep     movsb
        mov     ax,'  '
        stosw
        mov     al,0
        stosb
        pop     di 
	pop	si 
	pop	cx 
	pop	ax
        ret

;==========GETPARTITION==============
;Envoie en es:di la liste des partitions du disque BP
;-> BP, ES:DI 
;<- renvoie leur nombre en CX
;====================================
getpartitions:
        push    eax 
	push	ebx 
	push	edx 	
	push	si 
	push	di
        mov     ecx,0
        mov     word [wheres],0
        mov     [theprimary],ecx
againpart:  
        xor     eax,eax
        mov     ebx,ecx
        push    di
        mov     di,partition
        CALL    readsector
        mov     si, di
        pop     di
        add     si,0x01BE
        mov     cl,4
showevery:
        cmp     dword [si+NumSector],0x00000000
        je      nothings
        mov     edx,[si+StartSector]
        cmp     byte [si+Kind],0x05
        je      is
        cmp     byte [si+Kind],0x0F
        jne     isnot
is:
        mov     eax,[theprimary]
        add     eax,edx   
        cmp     dword [theprimary],0
        jne     nothings
isnot:
        push    ecx 
	push	di 
	push	si
        mov     cx,[wheres]
        shl     cx,6
        add     di,cx
        mov     ecx,16/4
        rep     movsd
        pop     si
        mov     [di],ebx
        mov     [di+4],si
        sub     word [di+4],partition
        mov     [di+6],ebx
        add     [di+6],edx
        mov     ecx,[di+6]
        mov     [di+10],bp
        add     di,12
        push    edx
        call    getbootinfos
        pop     edx 
	pop	di 
	pop	ecx
        inc     word [wheres]
nothings:
        add     si,16
        dec     cl
        jnz     showevery
        mov     ecx,eax
        cmp     dword [theprimary],0x00000000
        jne     noth
        mov     [theprimary],eax
noth:
        cmp     ecx,0
        jnz     near againpart
        mov     cx,[wheres]
        pop     di 
	pop	si 
	pop	edx 
	pop	ebx 
	pop	eax
        ret

;==============ISMBR=================
;Zero si Mbr sur le premier secteur du support BP
;-> BP
;<- 
;====================================
IsMbr:
        push    ax 
	push	ecx 
	push	di 
	push	es
        mov     ecx,0
        mov     di,buffer
        push    cs
        pop     es
        call    readsector
        add     di,0x01BE
        mov     cl,4
isgood:
        mov     al,[cs:di]
        and     al,0x7F
        cmp     al,0
        jne     noMbr
        add     di,16
        dec     cl
        jne     isgood
        cmp     word [cs:di],0xAA55
noMbr:
        pop     es 
	pop	di 
	pop	ecx 
	pop	ax
        ret

;=============DETECTDISK=============
;Renvoie en ES:DI les disques d‚tect‚s fini par 0FFh
;-> ES:DI
;<- 
;====================================
detectdisk:
        push    ax 
	push	bx 
	push	cx 
	push	dx 
	push	si 
	push	di
        mov     si,PossibleDisks
DetectAllDisk:
        mov     dl,[cs:si]
        inc     si
        cmp     dl,0xFF
        je      EndOfDetection
        mov     cl,4
retry:
        push    cx
        mov     ax,0x0201
        mov     cx,0x0101
        mov     dh,0x00
        mov     bx,buffer
        int     0x13
        pop     cx
        jnc     isdetected
        dec     cl
        jnz     retry
        jmp     DetectAllDisk
isdetected:
        mov     [es:di],dl
        inc     di
        jmp     DetectAllDisk
EndOfDetection:
        mov     byte [di],0xFF
        pop     di 
	pop	si 
	pop	dx 
	pop	cx 
	pop	bx 
	pop	ax
        ret

PossibleDisks db 0x00,0x01,0x02,0x03,0x04,0x05,0x80,0x81,0x82,0x83,0x84,0x85,0xFF

;=============INITDISK===============
;Initialise le p‚riph‚rique BP pour une utilisation ult‚rieure
;-> BP
;<- 
;====================================
initdisk:
        push ax 
	push	bx 
	push	cx 
	push	dx 
	push	di 
	push	es
        mov     dx,bp
        mov     ah,8
        int     0x13
        and     cx,0x3F
        mov     [cs:sectorspertrack],cx
        mov     cl,dh
        inc     cl
        mov     [cs:headsperdrive],cx
        pop     es 
	pop	di 
	pop	dx 
	pop	cx 
	pop	bx 
	pop	ax
        ret

;=============WRITESECTOR (Fonction 01H)===============
;Ecrit le secteur ECX du disque BP depuis ds:si
;-> AH=1
;<- Flag Carry si erreur
;=====================================================
writesector:
        push    ax 
	push	bx 
	push	ecx 
	push	dx 
	push	si 
	push	di
        mov     ax,bp
        mov     [cs:temp],al
        mov     ax, cx
        ror     ecx, 16
        mov     dx, cx
        rol     ecx,16
        div     word [cs:sectorspertrack]
        inc     dl
        mov     bl, dl
        xor     dx,dx                  
        div     word [cs:headsperdrive]
        mov     dh, [cs:temp]
        xchg    dl, dh
        mov     cx, ax
        xchg    cl, ch
        shl     cl, 6
        or      cl, bl      
        mov     bx, si
        mov     si, 4
        mov     al, 1
TryAgain2:
        mov     ah, 3
        int     0x13
        jnc     Done2
        dec     si
        jnz     TryAgain2
Done2:
        pop   di 
	pop	si 
	pop	dx 
	pop	ecx 
	pop	bx 
	pop	ax
        ret

;=============READSECTOR===============
;Lit le secteur ECX du disque BP et le met en es:di
;-> 
;<- 
;======================================
readsector:
        push    ax 
	push	bx 
	push	ecx 
	push	dx 
	push	si
        mov     ax,bp
        mov     [cs:temp],al
        mov     ax, cx
        ror     ecx, 16
        mov     dx, cx
        rol     ecx,16
        cmp     ecx,4128705
        ja      extended
        div     word [cs:sectorspertrack]
        inc     dl
        mov     bl, dl
        xor     dx,dx                  
        div     word [cs:headsperdrive]
        mov     dh, [cs:temp]
        xchg    dl, dh
        mov     cx, ax
        xchg    cl, ch
        shl     cl, 6
        or      cl, bl      
        mov     bx, di
        mov     si, 4
        mov     al, 1
TryAgain:
        mov     ah, 2
        int     0x13
        jnc     Done
        dec     si
        jnz     TryAgain
Done:
        pop     si 
	pop	dx 
	pop	ecx 
	pop	bx 
	pop	ax
        ret
extended:
        push    di
        push    ds
        push    cs
        pop     ds
        mov     si,block        
        mov     byte [si+Sizes],0x10
        mov     byte [si+Reserve],0x01
        mov     word [si+NumSectors],0x0001
        mov     [si+Adressseg],es
        mov     [si+Adressoff],di
        mov     [si+SectorLow],ax
        mov     [si+SectorHigh],dx
        mov     di,4             
        mov     dl,[temp]
TryAgainX:
        mov     ah, 0x42
        int     0x13
        jnc     DoneX
        dec     di
        jnz     TryAgainX
DoneX:
        pop   ds 
	pop	di 
	pop	si 
	pop	dx 
	pop	ecx 
	pop	bx 
	pop	ax
        ret

headsperdrive   dw 16
sectorspertrack dw 38
block           times 24 db 0
temp            db 0            

;============CLS==============
;Efface l'‚cran
;-> 
;<-
;=============================
cls:
        push    ax
        mov     ax,0x0003
        int     0x10
        pop     ax
        ret

;==========SHOWSTR============
;Affiche une chaine de caractŠre point‚ par SI
;-> SI pointe une chaine
;<-
;=============================
showstr:
        push    ax 
	push	bx 
	push	si
        cld
again:
	lodsb
        or      al,al
        jz      fin
        call    showchar
        jmp     again
fin:
        pop     si
	pop bx
	pop ax
	ret

;==========SHOWPASSTR============
;Affiche une chaine de caractŠre point‚ par SI
;-> SI pointe une chaine
;<-
;=============================
showpasstr:
        push    ax 
	push	bx 
	push	cx 
	push	si
        mov     cl,[si]
        inc     si
        cld
again2:
	lodsb
        call    showchar
        dec     cl
        jnz     again2
fin2:
        pop     si 
	pop	cx 
	pop	bx 
	pop	ax
	ret

;==========WAITKEY=============
;Attend l'appuie d'une toouche et
;renvoie en AL la touche appuyer
;-> 
;<- AL
;==============================
waitkey:
        mov     ax,0
        int     0x16
        ret

;==========SHOWCHAR============
;Affiche un caractŠre point‚ dans AL
;-> AL
;<-
;==============================
showchar:
        push    ax 
	push	bx
        mov     ah,0x0E
        mov     bx,0x07
        int     0x10
        pop     bx 
	pop	ax
	ret

;==========SHOWINT============
;Affiche un entier EDX apr‚s le curseur
;-> EDX un entier
;<-
;=============================
showint:
        push    eax 
	push	bx 
	push	cx 
	push	edx 
	push	esi 
	push	di 
	push	ds 
	push	es
        push    cs
        push    cs
        pop     ds
        pop     es
        mov     cx,10
        mov     di,showbuffer
        mov     al,' '
        cld
        rep     stosb
        xor     cx,cx
        mov     eax, edx
        mov     esi, 10
        mov     bx,di
        mov     byte [cs:di+1],0
decint:
        xor     edx,edx
        div     esi
        add     dl,'0'
        inc     cx
        mov     [cs:bx],dl
        dec     bx
        cmp     ax, 0
        jne     decint
showinteger:
        mov     si,showbuffer
        call    showstr
        pop     es 
	pop	ds 
	pop	di 
	pop	esi 
	pop	edx 
	pop	cx 
	pop	bx 
	pop	eax
	ret
	
;=============================================================================

;                         VARIABLES INITIALISEE

;=============================================================================

sel        dw 0              
wheres     dw 0
numbers    dw 0
theprimary dd 0
prog:

incbin "mbr.bin"
incbin "loader.bin"
;=============================================================================

;                         VARIABLES NON INITIALISEE

;=============================================================================

showbuffer equ $
disk       equ $+12
partition  equ $+12+26
buffer     equ $+12+26+512
thepart    equ $+12+26+512+512
thedrive   equ $+12+26+512+512+10000
dummys     equ $+12+26+512+512+10000+10000

section .bss

%include "partition.h"
%include "echs.h"

;showbuffer resb 12
;disk       resb 26
;partition  resb 512
;buffer     resb 512
;thepart    resb 10000
;thedrive   resb 10000
;dummys     resb 10000

