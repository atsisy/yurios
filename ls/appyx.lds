ENTRY(main)
INCLUDE app_vars.lds

SECTIONS
{
  . = SIZEOF_HEADERS;
  .text : { *(.text) }
  "file offset of data" = .;

  . = 0;
  .stack : AT("file offset of data") {
         . = STACK;
  }
  .data : {
        *(.rodata*) *(.data)
  }

  .bss  : { *(.bss) }
  .malloc : {
  . += MALLOC;
  }
}
