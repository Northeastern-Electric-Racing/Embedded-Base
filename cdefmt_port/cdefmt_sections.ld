/*
 * cdefmt linker script fragment.
 *
 */

SECTIONS {
    .cdefmt 0 (INFO) : {
        KEEP(*(.cdefmt.init .cdefmt.init.*))
        . = . + 8;
        KEEP(*(.cdefmt .cdefmt.*))
    }
}

PROVIDE(__cdefmt_build_id = ADDR(.note.gnu.build-id));
