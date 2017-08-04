#�綨�� TARGET_DEFS_ONLY

#���� EM_TCC_TARGET EM_X86_64

/* relocation type for 32 bit data relocation */
#���� R_DATA_32   R_X86_64_32S
#���� R_DATA_PTR  R_X86_64_64
#���� R_JMP_SLOT  R_X86_64_JUMP_SLOT
#���� R_GLOB_DAT  R_X86_64_GLOB_DAT
#���� R_COPY      R_X86_64_COPY
#���� R_RELATIVE  R_X86_64_RELATIVE

#���� R_NUM       R_X86_64_NUM

#���� ELF_START_ADDR 0x400000
#���� ELF_PAGE_SIZE  0x200000

#���� PCRELATIVE_DLLPLT 1
#���� RELOCATE_DLLPLT 1

#�� /* !TARGET_DEFS_ONLY */

#���� "tcc.h"

/* Returns 1 for a code relocation, 0 for a data relocation. For unknown
   relocations, returns -1. */
�� code_reloc (�� reloc_type)
{
    ת�� (reloc_type) {
        ���� R_X86_64_32:
        ���� R_X86_64_32S:
        ���� R_X86_64_64:
        ���� R_X86_64_GOTPC32:
        ���� R_X86_64_GOTPC64:
        ���� R_X86_64_GOTPCREL:
        ���� R_X86_64_GOTPCRELX:
        ���� R_X86_64_REX_GOTPCRELX:
        ���� R_X86_64_GOTTPOFF:
        ���� R_X86_64_GOT32:
        ���� R_X86_64_GOT64:
        ���� R_X86_64_GLOB_DAT:
        ���� R_X86_64_COPY:
        ���� R_X86_64_RELATIVE:
        ���� R_X86_64_GOTOFF64:
            ���� 0;

        ���� R_X86_64_PC32:
        ���� R_X86_64_PC64:
        ���� R_X86_64_PLT32:
        ���� R_X86_64_PLTOFF64:
        ���� R_X86_64_JUMP_SLOT:
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
        ���� R_X86_64_GLOB_DAT:
        ���� R_X86_64_JUMP_SLOT:
        ���� R_X86_64_COPY:
        ���� R_X86_64_RELATIVE:
            ���� NO_GOTPLT_ENTRY;

        /* The following relocs wouldn't normally need GOT or PLT
           slots, but we need them for simplicity in the link
           editor part.  See our caller for comments.  */
        ���� R_X86_64_32:
        ���� R_X86_64_32S:
        ���� R_X86_64_64:
        ���� R_X86_64_PC32:
        ���� R_X86_64_PC64:
            ���� AUTO_GOTPLT_ENTRY;

        ���� R_X86_64_GOTTPOFF:
            ���� BUILD_GOT_ONLY;

        ���� R_X86_64_GOT32:
        ���� R_X86_64_GOT64:
        ���� R_X86_64_GOTPC32:
        ���� R_X86_64_GOTPC64:
        ���� R_X86_64_GOTOFF64:
        ���� R_X86_64_GOTPCREL:
        ���� R_X86_64_GOTPCRELX:
        ���� R_X86_64_REX_GOTPCRELX:
        ���� R_X86_64_PLT32:
        ���� R_X86_64_PLTOFF64:
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
    /* On x86-64, the relocation is referred to by _index_ */
    write32le(p + 7, relofs / �󳤶� (ElfW_Rel));
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
        �� x = s1->got->sh_addr - s1->plt->sh_addr - 6;
        add32le(p + 2, x);
        add32le(p + 8, x - 6);
        p += 16;
        �� (p < p_end) {
            add32le(p + 2, x + s1->plt->data - p);
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
        ���� R_X86_64_64:
            �� (s1->output_type == TCC_OUTPUT_DLL) {
                esym_index = s1->sym_attrs[sym_index].dyn_index;
                qrel->r_offset = rel->r_offset;
                �� (esym_index) {
                    qrel->r_info = ELFW(R_INFO)(esym_index, R_X86_64_64);
                    qrel->r_addend = rel->r_addend;
                    qrel++;
                    ����;
                } �� {
                    qrel->r_info = ELFW(R_INFO)(0, R_X86_64_RELATIVE);
                    qrel->r_addend = read64le(ptr) + val;
                    qrel++;
                }
            }
            add64le(ptr, val);
            ����;
        ���� R_X86_64_32:
        ���� R_X86_64_32S:
            �� (s1->output_type == TCC_OUTPUT_DLL) {
                /* XXX: this logic may depend on TCC's codegen
                   now TCC uses R_X86_64_32 even for a 64bit pointer */
                qrel->r_info = ELFW(R_INFO)(0, R_X86_64_RELATIVE);
                /* Use sign extension! */
                qrel->r_addend = (��)read32le(ptr) + val;
                qrel++;
            }
            add32le(ptr, val);
            ����;

        ���� R_X86_64_PC32:
            �� (s1->output_type == TCC_OUTPUT_DLL) {
                /* DLL relocation */
                esym_index = s1->sym_attrs[sym_index].dyn_index;
                �� (esym_index) {
                    qrel->r_offset = rel->r_offset;
                    qrel->r_info = ELFW(R_INFO)(esym_index, R_X86_64_PC32);
                    /* Use sign extension! */
                    qrel->r_addend = (��)read32le(ptr) + rel->r_addend;
                    qrel++;
                    ����;
                }
            }
            ��ת plt32pc32;

        ���� R_X86_64_PLT32:
            /* fallthrough: val already holds the PLT slot address */

        plt32pc32:
        {
            �� �� diff;
            diff = (�� ��)val - addr;
            �� (diff < -2147483648LL || diff > 2147483647LL) {
                tcc_error("internal error: relocation failed");
            }
            add32le(ptr, diff);
        }
            ����;

        ���� R_X86_64_PLTOFF64:
            add64le(ptr, val - s1->got->sh_addr + rel->r_addend);
            ����;

        ���� R_X86_64_PC64:
            �� (s1->output_type == TCC_OUTPUT_DLL) {
                /* DLL relocation */
                esym_index = s1->sym_attrs[sym_index].dyn_index;
                �� (esym_index) {
                    qrel->r_offset = rel->r_offset;
                    qrel->r_info = ELFW(R_INFO)(esym_index, R_X86_64_PC64);
                    qrel->r_addend = read64le(ptr) + rel->r_addend;
                    qrel++;
                    ����;
                }
            }
            add64le(ptr, val - addr);
            ����;

        ���� R_X86_64_GLOB_DAT:
        ���� R_X86_64_JUMP_SLOT:
            /* They don't need addend */
            write64le(ptr, val - rel->r_addend);
            ����;
        ���� R_X86_64_GOTPCREL:
        ���� R_X86_64_GOTPCRELX:
        ���� R_X86_64_REX_GOTPCRELX:
            add32le(ptr, s1->got->sh_addr - addr +
                         s1->sym_attrs[sym_index].got_offset - 4);
            ����;
        ���� R_X86_64_GOTPC32:
            add32le(ptr, s1->got->sh_addr - addr + rel->r_addend);
            ����;
        ���� R_X86_64_GOTPC64:
            add64le(ptr, s1->got->sh_addr - addr + rel->r_addend);
            ����;
        ���� R_X86_64_GOTTPOFF:
            add32le(ptr, val - s1->got->sh_addr);
            ����;
        ���� R_X86_64_GOT32:
            /* we load the got offset */
            add32le(ptr, s1->sym_attrs[sym_index].got_offset);
            ����;
        ���� R_X86_64_GOT64:
            /* we load the got offset */
            add64le(ptr, s1->sym_attrs[sym_index].got_offset);
            ����;
        ���� R_X86_64_GOTOFF64:
            add64le(ptr, val - s1->got->sh_addr);
            ����;
        ���� R_X86_64_RELATIVE:
            /* do nothing */
            ����;
    }
}

#���� /* !TARGET_DEFS_ONLY */
