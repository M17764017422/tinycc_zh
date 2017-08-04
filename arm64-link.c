#�綨�� TARGET_DEFS_ONLY

#���� EM_TCC_TARGET EM_AARCH64

#���� R_DATA_32  R_AARCH64_ABS32
#���� R_DATA_PTR R_AARCH64_ABS64
#���� R_JMP_SLOT R_AARCH64_JUMP_SLOT
#���� R_GLOB_DAT R_AARCH64_GLOB_DAT
#���� R_COPY     R_AARCH64_COPY
#���� R_RELATIVE R_AARCH64_RELATIVE

#���� R_NUM      R_AARCH64_NUM

#���� ELF_START_ADDR 0x00400000
#���� ELF_PAGE_SIZE 0x1000

#���� PCRELATIVE_DLLPLT 1
#���� RELOCATE_DLLPLT 1

#�� /* !TARGET_DEFS_ONLY */

#���� "tcc.h"

/* Returns 1 for a code relocation, 0 for a data relocation. For unknown
   relocations, returns -1. */
�� code_reloc (�� reloc_type)
{
    ת�� (reloc_type) {
        ���� R_AARCH64_ABS32:
        ���� R_AARCH64_ABS64:
        ���� R_AARCH64_PREL32:
        ���� R_AARCH64_MOVW_UABS_G0_NC:
        ���� R_AARCH64_MOVW_UABS_G1_NC:
        ���� R_AARCH64_MOVW_UABS_G2_NC:
        ���� R_AARCH64_MOVW_UABS_G3:
        ���� R_AARCH64_ADR_PREL_PG_HI21:
        ���� R_AARCH64_ADD_ABS_LO12_NC:
        ���� R_AARCH64_ADR_GOT_PAGE:
        ���� R_AARCH64_LD64_GOT_LO12_NC:
        ���� R_AARCH64_GLOB_DAT:
        ���� R_AARCH64_COPY:
            ���� 0;

        ���� R_AARCH64_JUMP26:
        ���� R_AARCH64_CALL26:
        ���� R_AARCH64_JUMP_SLOT:
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
        ���� R_AARCH64_PREL32:
        ���� R_AARCH64_MOVW_UABS_G0_NC:
        ���� R_AARCH64_MOVW_UABS_G1_NC:
        ���� R_AARCH64_MOVW_UABS_G2_NC:
        ���� R_AARCH64_MOVW_UABS_G3:
        ���� R_AARCH64_ADR_PREL_PG_HI21:
        ���� R_AARCH64_ADD_ABS_LO12_NC:
        ���� R_AARCH64_GLOB_DAT:
        ���� R_AARCH64_JUMP_SLOT:
        ���� R_AARCH64_COPY:
            ���� NO_GOTPLT_ENTRY;

        ���� R_AARCH64_ABS32:
        ���� R_AARCH64_ABS64:
        ���� R_AARCH64_JUMP26:
        ���� R_AARCH64_CALL26:
            ���� AUTO_GOTPLT_ENTRY;

        ���� R_AARCH64_ADR_GOT_PAGE:
        ���� R_AARCH64_LD64_GOT_LO12_NC:
            ���� ALWAYS_GOTPLT_ENTRY;
    }

    tcc_error ("Unknown relocation type: %d", reloc_type);
    ���� -1;
}

ST_FUNC �޷� create_plt_entry(TCCState *s1, �޷� got_offset, �ṹ sym_attr *attr)
{
    Section *plt = s1->plt;
    uint8_t *p;
    �޷� plt_offset;

    �� (s1->output_type == TCC_OUTPUT_DLL)
        tcc_error("DLLs unimplemented!");

    �� (plt->data_offset == 0) {
        section_ptr_add(plt, 32);
    }
    plt_offset = plt->data_offset;

    p = section_ptr_add(plt, 16);
    write32le(p, got_offset);
    write32le(p + 4, (uint64_t) got_offset >> 32);
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
        uint64_t plt = s1->plt->sh_addr;
        uint64_t got = s1->got->sh_addr;
        uint64_t off = (got >> 12) - (plt >> 12);
        �� ((off + ((uint32_t)1 << 20)) >> 21)
            tcc_error("Failed relocating PLT (off=0x%lx, got=0x%lx, plt=0x%lx)", off, got, plt);
        write32le(p, 0xa9bf7bf0); // stp x16,x30,[sp,#-16]!
        write32le(p + 4, (0x90000010 | // adrp x16,...
                          (off & 0x1ffffc) << 3 | (off & 3) << 29));
        write32le(p + 8, (0xf9400211 | // ldr x17,[x16,#...]
                          (got & 0xff8) << 7));
        write32le(p + 12, (0x91000210 | // add x16,x16,#...
                           (got & 0xfff) << 10));
        write32le(p + 16, 0xd61f0220); // br x17
        write32le(p + 20, 0xd503201f); // nop
        write32le(p + 24, 0xd503201f); // nop
        write32le(p + 28, 0xd503201f); // nop
        p += 32;
        �� (p < p_end) {
            uint64_t pc = plt + (p - s1->plt->data);
            uint64_t addr = got + read64le(p);
            uint64_t off = (addr >> 12) - (pc >> 12);
            �� ((off + ((uint32_t)1 << 20)) >> 21)
                tcc_error("Failed relocating PLT (off=0x%lx, addr=0x%lx, pc=0x%lx)", off, addr, pc);
            write32le(p, (0x90000010 | // adrp x16,...
                          (off & 0x1ffffc) << 3 | (off & 3) << 29));
            write32le(p + 4, (0xf9400211 | // ldr x17,[x16,#...]
                              (addr & 0xff8) << 7));
            write32le(p + 8, (0x91000210 | // add x16,x16,#...
                              (addr & 0xfff) << 10));
            write32le(p + 12, 0xd61f0220); // br x17
            p += 16;
        }
    }
}

�� relocate_init(Section *sr) {}

�� relocate(TCCState *s1, ElfW_Rel *rel, �� type, �޷� �� *ptr, addr_t addr, addr_t val)
{
    �� sym_index = ELFW(R_SYM)(rel->r_info);
#�綨�� DEBUG_RELOC
    ElfW(Sym) *sym = &((ElfW(Sym) *)symtab_section->data)[sym_index];
#����

    ת��(type) {
        ���� R_AARCH64_ABS64:
            write64le(ptr, val);
            ����;
        ���� R_AARCH64_ABS32:
            write32le(ptr, val);
            ����;
        ���� R_AARCH64_PREL32:
            write32le(ptr, val - addr);
            ����;
        ���� R_AARCH64_MOVW_UABS_G0_NC:
            write32le(ptr, ((read32le(ptr) & 0xffe0001f) |
                            (val & 0xffff) << 5));
            ����;
        ���� R_AARCH64_MOVW_UABS_G1_NC:
            write32le(ptr, ((read32le(ptr) & 0xffe0001f) |
                            (val >> 16 & 0xffff) << 5));
            ����;
        ���� R_AARCH64_MOVW_UABS_G2_NC:
            write32le(ptr, ((read32le(ptr) & 0xffe0001f) |
                            (val >> 32 & 0xffff) << 5));
            ����;
        ���� R_AARCH64_MOVW_UABS_G3:
            write32le(ptr, ((read32le(ptr) & 0xffe0001f) |
                            (val >> 48 & 0xffff) << 5));
            ����;
        ���� R_AARCH64_ADR_PREL_PG_HI21: {
            uint64_t off = (val >> 12) - (addr >> 12);
            �� ((off + ((uint64_t)1 << 20)) >> 21)
                tcc_error("R_AARCH64_ADR_PREL_PG_HI21 relocation failed");
            write32le(ptr, ((read32le(ptr) & 0x9f00001f) |
                            (off & 0x1ffffc) << 3 | (off & 3) << 29));
            ����;
        }
        ���� R_AARCH64_ADD_ABS_LO12_NC:
            write32le(ptr, ((read32le(ptr) & 0xffc003ff) |
                            (val & 0xfff) << 10));
            ����;
        ���� R_AARCH64_JUMP26:
        ���� R_AARCH64_CALL26:
#�綨�� DEBUG_RELOC
            printf ("reloc %d @ 0x%lx: val=0x%lx name=%s\n", type, addr, val,
                    (�� *) symtab_section->link->data + sym->st_name);
#����
            �� (((val - addr) + ((uint64_t)1 << 27)) & ~(uint64_t)0xffffffc)
                tcc_error("R_AARCH64_(JUMP|CALL)26 relocation failed"
                          " (val=%lx, addr=%lx)", val, addr);
            write32le(ptr, (0x14000000 |
                            (uint32_t)(type == R_AARCH64_CALL26) << 31 |
                            ((val - addr) >> 2 & 0x3ffffff)));
            ����;
        ���� R_AARCH64_ADR_GOT_PAGE: {
            uint64_t off =
                (((s1->got->sh_addr +
                   s1->sym_attrs[sym_index].got_offset) >> 12) - (addr >> 12));
            �� ((off + ((uint64_t)1 << 20)) >> 21)
                tcc_error("R_AARCH64_ADR_GOT_PAGE relocation failed");
            write32le(ptr, ((read32le(ptr) & 0x9f00001f) |
                            (off & 0x1ffffc) << 3 | (off & 3) << 29));
            ����;
        }
        ���� R_AARCH64_LD64_GOT_LO12_NC:
            write32le(ptr,
                      ((read32le(ptr) & 0xfff803ff) |
                       ((s1->got->sh_addr +
                         s1->sym_attrs[sym_index].got_offset) & 0xff8) << 7));
            ����;
        ���� R_AARCH64_COPY:
            ����;
        ���� R_AARCH64_GLOB_DAT:
        ���� R_AARCH64_JUMP_SLOT:
            /* They don't need addend */
#�綨�� DEBUG_RELOC
            printf ("reloc %d @ 0x%lx: val=0x%lx name=%s\n", type, addr,
                    val - rel->r_addend,
                    (�� *) symtab_section->link->data + sym->st_name);
#����
            write64le(ptr, val - rel->r_addend);
            ����;
        ȱʡ:
            fprintf(stderr, "FIXME: handle reloc type %x at %x [%p] to %x\n",
                    type, (�޷�)addr, ptr, (�޷�)val);
            ����;
    }
}

#���� /* !TARGET_DEFS_ONLY */
