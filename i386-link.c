#�綨�� TARGET_DEFS_ONLY

#���� EM_TCC_TARGET EM_386

/* relocation type for 32 bit data relocation */
#���� R_DATA_32   R_386_32
#���� R_DATA_PTR  R_386_32
#���� R_JMP_SLOT  R_386_JMP_SLOT
#���� R_GLOB_DAT  R_386_GLOB_DAT
#���� R_COPY      R_386_COPY
#���� R_RELATIVE  R_386_RELATIVE

#���� R_NUM       R_386_NUM

#���� ELF_START_ADDR 0x08048000
#���� ELF_PAGE_SIZE  0x1000

#���� PCRELATIVE_DLLPLT 0
#���� RELOCATE_DLLPLT 0

#�� /* !TARGET_DEFS_ONLY */

#���� "tcc.h"

/* Returns 1 for a code relocation, 0 for a data relocation. For unknown
   relocations, returns -1. */
�� code_reloc (�� reloc_type)
{
    ת�� (reloc_type) {
        ���� R_386_RELATIVE:
        ���� R_386_16:
        ���� R_386_32:
        ���� R_386_GOTPC:
        ���� R_386_GOTOFF:
        ���� R_386_GOT32:
        ���� R_386_GOT32X:
        ���� R_386_GLOB_DAT:
        ���� R_386_COPY:
            ���� 0;

        ���� R_386_PC16:
        ���� R_386_PC32:
        ���� R_386_PLT32:
        ���� R_386_JMP_SLOT:
            ���� 1;
    }

    tcc_error ("Unknown relocation type: %d", reloc_type);
    ���� -1;
}

/* Returns an enumerator to describe whether and when the relocation needs a
   GOT and/or PLT entry to be created. See tcc.h for a description of the
   different values. */
�� gotplt_entry_type (�� reloc_type)
{
    ת�� (reloc_type) {
        ���� R_386_RELATIVE:
        ���� R_386_16:
        ���� R_386_GLOB_DAT:
        ���� R_386_JMP_SLOT:
        ���� R_386_COPY:
            ���� NO_GOTPLT_ENTRY;

        ���� R_386_32:
            /* This relocations shouldn't normally need GOT or PLT
               slots if it weren't for simplicity in the code generator.
               See our caller for comments.  */
            ���� AUTO_GOTPLT_ENTRY;

        ���� R_386_PC16:
        ���� R_386_PC32:
            ���� AUTO_GOTPLT_ENTRY;

        ���� R_386_GOTPC:
        ���� R_386_GOTOFF:
            ���� BUILD_GOT_ONLY;

        ���� R_386_GOT32:
        ���� R_386_GOT32X:
        ���� R_386_PLT32:
            ���� ALWAYS_GOTPLT_ENTRY;
    }

    tcc_error ("Unknown relocation type: %d", reloc_type);
    ���� -1;
}

ST_FUNC �޷� create_plt_entry(TCCState *s1, �޷� got_offset, �ṹ sym_attr *attr)
{
    Section *plt = s1->plt;
    uint8_t *p;
    �� modrm;
    �޷� plt_offset, relofs;

    /* on i386 if we build a DLL, we add a %ebx offset */
    �� (s1->output_type == TCC_OUTPUT_DLL)
        modrm = 0xa3;
    ��
        modrm = 0x25;

    /* empty PLT: create PLT0 entry that pushes the library identifier
       (GOT + PTR_SIZE) and jumps to ld.so resolution routine
       (GOT + 2 * PTR_SIZE) */
    �� (plt->data_offset == 0) {
        p = section_ptr_add(plt, 16);
        p[0] = 0xff; /* pushl got + PTR_SIZE */
        p[1] = modrm + 0x10;
        write32le(p + 2, PTR_SIZE);
        p[6] = 0xff; /* jmp *(got + PTR_SIZE * 2) */
        p[7] = modrm;
        write32le(p + 8, PTR_SIZE * 2);
    }
    plt_offset = plt->data_offset;

    /* The PLT slot refers to the relocation entry it needs via offset.
       The reloc entry is created below, so its offset is the current
       data_offset */
    relofs = s1->got->reloc ? s1->got->reloc->data_offset : 0;

    /* Jump to GOT entry where ld.so initially put the address of ip + 4 */
    p = section_ptr_add(plt, 16);
    p[0] = 0xff; /* jmp *(got + x) */
    p[1] = modrm;
    write32le(p + 2, got_offset);
    p[6] = 0x68; /* push $xxx */
    write32le(p + 7, relofs);
    p[11] = 0xe9; /* jmp plt_start */
    write32le(p + 12, -(plt->data_offset));
    ���� plt_offset;
}

/* relocate the PLT: compute addresses and offsets in the PLT now that final
   address for PLT and GOT are known (see fill_program_header) */
ST_FUNC �� relocate_plt(TCCState *s1)
{
    uint8_t *p, *p_end;

    �� (!s1->plt)
      ����;

    p = s1->plt->data;
    p_end = p + s1->plt->data_offset;

    �� (p < p_end) {
        add32le(p + 2, s1->got->sh_addr);
        add32le(p + 8, s1->got->sh_addr);
        p += 16;
        �� (p < p_end) {
            add32le(p + 2, s1->got->sh_addr);
            p += 16;
        }
    }
}

��̬ ElfW_Rel *qrel; /* ptr to next reloc entry reused */

�� relocate_init(Section *sr)
{
    qrel = (ElfW_Rel *) sr->data;
}

�� relocate(TCCState *s1, ElfW_Rel *rel, �� type, �޷� �� *ptr, addr_t addr, addr_t val)
{
    �� sym_index, esym_index;

    sym_index = ELFW(R_SYM)(rel->r_info);

    ת�� (type) {
        ���� R_386_32:
            �� (s1->output_type == TCC_OUTPUT_DLL) {
                esym_index = s1->sym_attrs[sym_index].dyn_index;
                qrel->r_offset = rel->r_offset;
                �� (esym_index) {
                    qrel->r_info = ELFW(R_INFO)(esym_index, R_386_32);
                    qrel++;
                    ����;
                } �� {
                    qrel->r_info = ELFW(R_INFO)(0, R_386_RELATIVE);
                    qrel++;
                }
            }
            add32le(ptr, val);
            ����;
        ���� R_386_PC32:
            �� (s1->output_type == TCC_OUTPUT_DLL) {
                /* DLL relocation */
                esym_index = s1->sym_attrs[sym_index].dyn_index;
                �� (esym_index) {
                    qrel->r_offset = rel->r_offset;
                    qrel->r_info = ELFW(R_INFO)(esym_index, R_386_PC32);
                    qrel++;
                    ����;
                }
            }
            add32le(ptr, val - addr);
            ����;
        ���� R_386_PLT32:
            add32le(ptr, val - addr);
            ����;
        ���� R_386_GLOB_DAT:
        ���� R_386_JMP_SLOT:
            write32le(ptr, val);
            ����;
        ���� R_386_GOTPC:
            add32le(ptr, s1->got->sh_addr - addr);
            ����;
        ���� R_386_GOTOFF:
            add32le(ptr, val - s1->got->sh_addr);
            ����;
        ���� R_386_GOT32:
        ���� R_386_GOT32X:
            /* we load the got offset */
            add32le(ptr, s1->sym_attrs[sym_index].got_offset);
            ����;
        ���� R_386_16:
            �� (s1->output_format != TCC_OUTPUT_FORMAT_BINARY) {
            output_file:
                tcc_error("can only produce 16-bit binary files");
            }
            write16le(ptr, read16le(ptr) + val);
            ����;
        ���� R_386_PC16:
            �� (s1->output_format != TCC_OUTPUT_FORMAT_BINARY)
                ��ת output_file;
            write16le(ptr, read16le(ptr) + val - addr);
            ����;
        ���� R_386_RELATIVE:
            /* do nothing */
            ����;
        ���� R_386_COPY:
            /* This relocation must copy initialized data from the library
            to the program .bss segment. Currently made like for ARM
            (to remove noise of default case). Is this true?
            */
            ����;
        ȱʡ:
            fprintf(stderr,"FIXME: handle reloc type %d at %x [%p] to %x\n",
                type, (�޷�)addr, ptr, (�޷�)val);
            ����;
    }
}

#���� /* !TARGET_DEFS_ONLY */
