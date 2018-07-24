struc ELFheader
.Magic                  resb 1;"0x7F"
.MagicStr               resb 3;"ELF"
.Computertype           resb 1;1
.Endianness             resb 1;1
.Original               resb 1;1
.OS                     resb 1;0
.ABI                    resb 1;0
.Dummy                  resb 7;0
.Type                   resw 1;2
.Target                 resw 1;3
.Version                resd 1;1
.Entrypoint             resd 1
.Offsetprogram          resd 1
.Offsetsection          resd 1
.Flags                  resd 1
.SizeELFheader          resw 1
.Sizeprogram            resw 1
.Nbprogram              resw 1
.Sizesection            resw 1
.Nbsection              resw 1
.Index                  resw 1
.end                    equ $
endstruc 

struc Program
.Type                   resd 1
.Offset                 resd 1
.Vadress                resd 1
.PAdress                resd 1
.Size                   resd 1
.Memsize                resd 1
.Flags                  resd 1
.Align                  resd 1
.end                    equ $
endstruc 

struc Sections
.Name                   resd 1
.Type                   resd 1
.Flags                  resd 1
.Vadress                resd 1
.Offset                 resd 1
.Size                   resd 1
.Link                   resd 1
.Info                   resd 1
.Align                  resd 1
.Entrysize              resd 1
.end                    equ $
endstruc 


;type
PT_NULL equ 	0
PT_LOAD equ	1
PT_DYNAMIC equ	2
PT_INTERP equ	3
PT_NOTE equ	4
PT_SHLIB equ	5
PT_PHDR equ	6
PT_TLS equ	7
PT_LOOS equ	0x60000000
PT_HIOS equ	0x6fffffff
PT_LOPROC equ	0x70000000
PT_HIPROC equ	0x7fffffff

;flags
PF_WRITE equ	0x1
PF_ALLOC equ	0x2
PF_X equ	0x1 	;Execute
PF_W equ	0x2 	;Write
PF_R equ	0x4 	;Read
PF_MASKOS equ	0x0ff00000 	;Unspecified
PF_MASKPROC equ	0xf0000000 	;Unspecified

;type
ST_EXIT  equ 1
ST_NULL equ	0
ST_PROGBITS equ	1
ST_SYMTAB equ	2
ST_STRTAB equ	3
ST_RELA equ	4
ST_HASH equ	5
ST_DYNAMIC equ	6
ST_NOTE equ	7
ST_NOBITS equ	8
ST_REL equ	9
ST_SHLIB equ	10
ST_DYNSYM equ	11
ST_INIT_ARRAY equ	14
ST_FINI_ARRAY equ	15
ST_PREINIT_ARRAY equ	16
ST_GROUP equ	17
ST_SYMTAB_SHNDX equ	18
ST_LOOS equ	0x60000000
ST_HIOS equ	0x6fffffff
ST_LOPROC equ	0x70000000
ST_HIPROC equ	0x7fffffff
ST_LOUSER equ	0x80000000
ST_HIUSER equ	0xffffffff

;flags
SF_WRITE equ	0x1
SF_ALLOC equ	0x2
SF_EXECINSTR equ	0x4
SF_MERGE equ	0x10
SF_STRINGS equ	0x20
SF_INFO_LINK equ	0x40
SF_LINK_ORDER equ	0x80
SF_OS_NONCONFORMING equ	0x100
SF_GROUP equ	0x200
SF_TLS equ	0x400
SF_COMPRESSED equ	0x800
SF_MASKOS equ	0x0ff00000
SF_MASKPROC equ	0xf0000000

