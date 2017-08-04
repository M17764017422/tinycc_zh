#�綨�� TARGET_DEFS_ONLY

#���� EM_TCC_TARGET EM_C60

/* relocation type for 32 bit data relocation */
#���� R_DATA_32   R_C60_32
#���� R_DATA_PTR  R_C60_32
#���� R_JMP_SLOT  R_C60_JMP_SLOT
#���� R_GLOB_DAT  R_C60_GLOB_DAT
#���� R_COPY      R_C60_COPY
#���� R_RELATIVE  R_C60_RELATIVE

#���� R_NUM       R_C60_NUM

#���� ELF_START_ADDR 0x00000400
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
        ���� R_C60_32:
        ���� R_C60LO16:
        ���� R_C60HI16:
        ���� R_C60_GOT32:
        ���� R_C60_GOTOFF:
        ���� R_C60_GOTPC:
        ���� R_C60_COPY:
            ���� 0;

        ���� R_C60_PLT32:
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
        ���� R_C60_32:
        ���� R_C60LO16:
        ���� R_C60HI16:
        ���� R_C60_COPY:
            ���� NO_GOTPLT_ENTRY;

        ���� R_C60_GOTOFF:
        ���� R_C60_GOTPC:
            ���� BUILD_GOT_ONLY;

        ���� R_C60_PLT32:
        ���� R_C60_GOT32:
            ���� ALWAYS_GOTPLT_ENTRY;
    }

    tcc_error ("Unknown relocation type: %d", reloc_type);
    ���� -1;
}

ST_FUNC �޷� create_plt_entry(TCCState *s1, �޷� got_offset, �ṹ sym_attr *attr)
{
    tcc_error("C67 got not implemented");
    ���� 0;
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
        /* XXX: TODO */
        �� (p < p_end) {
            /* XXX: TODO */
        }
   }
}

�� relocate_init(Section *sr) {}

�� relocate(TCCState *s1, ElfW_Rel *rel, �� type, �޷� �� *ptr, addr_t addr, addr_t val)
{
    ת��(type) {
        ���� R_C60_32:
            *(�� *)ptr += val;
            ����;
        ���� R_C60LO16:
            {
                uint32_t orig;

                /* put the low 16 bits of the absolute address add to what is
                   already there */
                orig  =   ((*(�� *)(ptr  )) >> 7) & 0xffff;
                orig |=  (((*(�� *)(ptr+4)) >> 7) & 0xffff) << 16;

                /* patch both at once - assumes always in pairs Low - High */
                *(�� *) ptr    = (*(�� *) ptr    & (~(0xffff << 7)) ) |
                                   (((val+orig)      & 0xffff) << 7);
                *(�� *)(ptr+4) = (*(�� *)(ptr+4) & (~(0xffff << 7)) ) |
                                  ((((val+orig)>>16) & 0xffff) << 7);
            }
            ����;
        ���� R_C60HI16:
            ����;
        ȱʡ:
            fprintf(stderr,"FIXME: handle reloc type %x at %x [%p] to %x\n",
                    type, (�޷�) addr, ptr, (�޷�) val);
            ����;
    }
}

#���� /* !TARGET_DEFS_ONLY */
