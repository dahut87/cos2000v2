OUTPUT_FORMAT("elf32-i386", "elf32-i386",
          "elf32-i386")
OUTPUT_ARCH(i386)

ENTRY(mymain)

SECTIONS
{
   . = 0x50000;
   .text  ALIGN(16): { 
        *(.text) 
    }
   . = 0x200000;
   .data  ALIGN(16): { 
        *(.data) 
    }
   .bss  ALIGN(16): { 
        *(.bss) 
    }
}
