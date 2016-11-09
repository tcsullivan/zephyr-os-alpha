/**
 * \file
 * \brief Linker script for STM32F103VD chip:
 * - 393216 bytes of rom;
 * - 65536 bytes of ram;
 *
 * \author Copyright (C) 2014-2016 Kamil Szczygiel http://www.distortec.com http://www.freddiechopin.info
 *
 * \par License
 * This Source Code Form is subject to the terms of the Mozilla Public License, v. 2.0. If a copy of the MPL was not
 * distributed with this file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * \warning
 * Automatically generated file - do not edit!
 *
 * \date 2016-11-08 19:50:43
 */

SEARCH_DIR(.);
OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm");
OUTPUT_ARCH(arm);

/*---------------------------------------------------------------------------------------------------------------------+
| stacks sizes
+---------------------------------------------------------------------------------------------------------------------*/

/* Handler mode (core exceptions / interrupts) can use only main stack */
PROVIDE(__main_stack_size = 2048);

/* Thread mode can use main stack (default after reset) or process stack - selected in CONTROL special register */
PROVIDE(__process_stack_size = 2048);

/*---------------------------------------------------------------------------------------------------------------------+
| available memories
+---------------------------------------------------------------------------------------------------------------------*/

MEMORY
{
	rom : org = 0x08000000, len = 393216
	ram : org = 0x20000000, len = 65536
}

PROVIDE(__rom_start = ORIGIN(rom));
PROVIDE(__rom_size = LENGTH(rom));
PROVIDE(__rom_end = __rom_start + __rom_size);

PROVIDE(__ram_start = ORIGIN(ram));
PROVIDE(__ram_size = LENGTH(ram));
PROVIDE(__ram_end = __ram_start + __ram_size);

/*---------------------------------------------------------------------------------------------------------------------+
| entry point
+---------------------------------------------------------------------------------------------------------------------*/

ENTRY(Reset_Handler);

/*---------------------------------------------------------------------------------------------------------------------+
| put data in sections
+---------------------------------------------------------------------------------------------------------------------*/

SECTIONS
{
	.text :
	{
		. = ALIGN(4);
		PROVIDE(__text_start = .);

		/* sub-section: .vectors */

		. = ALIGN(4);
		PROVIDE(__vectors_start = .);

		KEEP(*(.coreVectors));
		KEEP(*(.chipVectors));

		. = ALIGN(4);
		PROVIDE(__vectors_end = .);

		/* end of sub-section: .vectors */

		*(.text* .gnu.linkonce.t.*);
		*(.rodata* .gnu.linkonce.r.*);
		*(.glue_7t .glue_7);

		*(.ARM.extab* .gnu.linkonce.armextab.*);	/* exception unwinding information */
		*(.gcc_except_table);						/* information used for stack unwinding during exception */
		*(.eh_frame_hdr);							/* additional information about .ex_frame section */
		*(.eh_frame);								/* information used for stack unwinding during exception */

		/* sub-section: data_array */

		. = ALIGN(4);
		PROVIDE(__data_array_start = .);

		LONG(LOADADDR(.data)); LONG(ADDR(.data)); LONG(ADDR(.data) + SIZEOF(.data));

		. = ALIGN(4);
		PROVIDE(__data_array_end = .);

		/* end of sub-section: data_array */

		/* sub-section: bss_array */

		. = ALIGN(4);
		PROVIDE(__bss_array_start = .);

		LONG(ADDR(.bss)); LONG(ADDR(.bss) + SIZEOF(.bss));
		LONG(ADDR(.stack)); LONG(ADDR(.stack) + SIZEOF(.stack));

		. = ALIGN(4);
		PROVIDE(__bss_array_end = .);

		/* end of sub-section: bss_array */

		/* sub-sections: init, preinit_array, init_array and fini_array */

		KEEP(*(.init));

		. = ALIGN(4);
		PROVIDE(__preinit_array_start = .);

		KEEP(*(.preinit_array));

		. = ALIGN(4);
		PROVIDE(__preinit_array_end = .);

		. = ALIGN(4);
		PROVIDE(__init_array_start = .);

		KEEP(*(SORT(.init_array.*)));
		KEEP(*(.init_array));

		. = ALIGN(4);
		PROVIDE(__init_array_end = .);

		KEEP(*(.fini));

		. = ALIGN(4);
		PROVIDE(__fini_array_start = .);

		KEEP(*(.fini_array));
		KEEP(*(SORT(.fini_array.*)));

		. = ALIGN(4);
		PROVIDE(__fini_array_end = .);

		/* end of sub-sections: init, preinit_array, init_array and fini_array */

		. = ALIGN(4);
		PROVIDE(__text_end = .);
	} > rom AT > rom

	. = ALIGN(4);
	PROVIDE(__exidx_start = .);

	.ARM.exidx :
	{
		*(.ARM.exidx* .gnu.linkonce.armexidx.*);
	} > rom AT > rom								/* index entries for section unwinding */

	. = ALIGN(4);
	PROVIDE(__exidx_end = .);

	.bss :
	{
		. = ALIGN(4);
		PROVIDE(__bss_start = .);

		*(.bss* .gnu.linkonce.b.*)
		*(COMMON);

		. = ALIGN(4);
		PROVIDE(__bss_end = .);
	} > ram AT > ram

	.data :
	{
		. = ALIGN(4);
		PROVIDE(__data_init_start = LOADADDR(.data));
		PROVIDE(__data_start = .);

		*(.data* .gnu.linkonce.d.*)

		. = ALIGN(4);
		PROVIDE(__data_end = .);
	} > ram AT > rom

	.noinit (NOLOAD) :
	{
		. = ALIGN(4);
		PROVIDE(__noinit_start = .);

		*(.noinit)

		. = ALIGN(4);
		PROVIDE(__noinit_end = .);
	} > ram AT > ram

	.stack :
	{
		. = ALIGN(8);
		PROVIDE(__stack_start = .);
		PROVIDE(__main_stack_start = .);

		. += __main_stack_size;

		. = ALIGN(8);
		PROVIDE(__main_stack_end = .);
		PROVIDE(__process_stack_start = .);

		. += __process_stack_size;

		. = ALIGN(8);
		PROVIDE(__process_stack_end = .);
		PROVIDE(__stack_end = .);
	} > ram AT > ram

	. = ALIGN(4);
	PROVIDE(__heap_start = .);
	PROVIDE(__heap_end = __ram_end);

	.stab 				0 (NOLOAD) : { *(.stab) }
	.stabstr 			0 (NOLOAD) : { *(.stabstr) }
	/* DWARF debug sections.
	* Symbols in the DWARF debugging sections are relative to the beginning
	* of the section so we begin them at 0. */
	/* DWARF 1 */
	.debug				0 : { *(.debug) }
	.line				0 : { *(.line) }
	/* GNU DWARF 1 extensions */
	.debug_srcinfo		0 : { *(.debug_srcinfo) }
	.debug_sfnames		0 : { *(.debug_sfnames) }
	/* DWARF 1.1 and DWARF 2 */
	.debug_aranges		0 : { *(.debug_aranges) }
	.debug_pubnames		0 : { *(.debug_pubnames) }
	/* DWARF 2 */
	.debug_info			0 : { *(.debug_info .gnu.linkonce.wi.*) }
	.debug_abbrev		0 : { *(.debug_abbrev) }
	.debug_line			0 : { *(.debug_line) }
	.debug_frame		0 : { *(.debug_frame) }
	.debug_str			0 : { *(.debug_str) }
	.debug_loc			0 : { *(.debug_loc) }
	.debug_macinfo		0 : { *(.debug_macinfo) }
	/* SGI/MIPS DWARF 2 extensions */
	.debug_weaknames	0 : { *(.debug_weaknames) }
	.debug_funcnames	0 : { *(.debug_funcnames) }
	.debug_typenames	0 : { *(.debug_typenames) }
	.debug_varnames		0 : { *(.debug_varnames) }

	.note.gnu.arm.ident	0 : { KEEP(*(.note.gnu.arm.ident)) }
	.ARM.attributes		0 : { KEEP(*(.ARM.attributes)) }
	/DISCARD/				: { *(.note.GNU-stack) }
}

PROVIDE(__text_size = __text_end - __text_start);
PROVIDE(__vectors_size = __vectors_end - __vectors_start);
PROVIDE(__data_array_size = __data_array_end - __data_array_start);
PROVIDE(__bss_array_size = __bss_array_end - __bss_array_start);
PROVIDE(__exidx_size = __exidx_end - __exidx_start);
PROVIDE(__bss_size = __bss_end - __bss_start);
PROVIDE(__data_size = __data_end - __data_start);
PROVIDE(__noinit_size = __noinit_end - __noinit_start);
PROVIDE(__stack_size = __stack_end - __stack_start);
PROVIDE(__heap_size = __heap_end - __heap_start);

PROVIDE(__bss_start__ = __bss_start);
PROVIDE(__bss_end__ = __bss_end);
