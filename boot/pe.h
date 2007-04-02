struc mzheader
.magic                  resb 2;"MZ"
.cblp                   resw 1
.cp                     resw 1
.crlc                   resw 1
.cparhdr                resw 1
.minalloc               resw 1
.maxalloc               resw 1
.ss                     resw 1
.sp                     resw 1
.csum                   resw 1
.ip                     resw 1
.cs                     resw 1
.lfarlc                 resw 1
.ovno                   resw 1
.res                    resw 4
.oemid                  resw 1
.oeminfo                resw 1
.res2                   resw 10
.lfanew                 resd 1
.end                    equ $
endstruc 

struc peheader
.Signature              resd 1
.Machine                resw 1
.NumberOfSections       resw 1
.TimeDateStamp          resd 1
.PointerToSymbolTable   resd 1
.NumberOfSymbols        resd 1
.SizeOfOptionalHeader   resw 1
.Characteristics        resw 1
.Magic                  resw 1
.MajorLinkerVersion     resb 1
.MinorLinkerVersion     resb 1
.SizeOfCode             resd 1
.SizeOfInitializedData  resd 1
.SizeOfUninitialzedData resd 1
.AddressOfEntryPoint    resd 1
.BaseOfCode             resd 1
.BaseOfData             resd 1
.ImageBase              resd 1
.SectionAlignment       resd 1
.FileAlignment          resd 1
.MajorOperSystemVersion resw 1
.MinorOperSystemVersion resw 1
.MajorImageVersion      resw 1
.MinorImageVersion      resw 1
.MajorSubsystemVersion  resw 1
.MinorSubsystemVersion  resw 1
.Reserved1              resd 1
.SizeOfImage            resd 1
.SizeOfHeaders          resd 1
.CheckSum               resd 1
.Subsystem              resw 1
.DllCharacteristics     resw 1
.SizeOfStackReserve     resd 1
.SizeOfStackCommit      resd 1
.SizeOfHeapReserve      resd 1
.SizeOfHeapCommit       resd 1
.LoaderFlags            resd 1
.NumberOfRvaAndSizes    resd 1
.export                 resd 2
.import                 resd 2
.misc_sectionz          resd 28
.end                    equ $
endstruc 

struc sections
.SectionName            resb 8
.VirtualSize            resd 1
.VirtualAddress         resd 1
.SizeOfRawData          resd 1
.PointerToRawData       resd 1
.PointerToRelocations   resd 1
.PointerToLinenumbers   resd 1
.NumberOfRelocations    resw 1
.NumberOfLinenumbers    resw 1
.Characteristics        resd 1
.end                    equ $
endstruc 
