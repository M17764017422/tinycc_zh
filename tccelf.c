/*
 *  ELF file handling for TCC
 *
 *  Copyright (c) 2001-2004 Fabrice Bellard
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#���� "tcc.h"

/* Define this to get some debug output during relocation processing.  */
#������ DEBUG_RELOC

/********************************************************/
/* global variables */

ST_DATA Section *text_section, *data_section, *bss_section; /* predefined sections */
ST_DATA Section *common_section;
ST_DATA Section *cur_text_section; /* current section where function code is generated */
#�綨�� CONFIG_TCC_ASM
ST_DATA Section *last_text_section; /* to handle .previous asm directive */
#����
#�綨�� CONFIG_TCC_BCHECK
/* bound check related sections */
ST_DATA Section *bounds_section; /* contains global data bound description */
ST_DATA Section *lbounds_section; /* contains local data bound description */
#����
/* symbol sections */
ST_DATA Section *symtab_section, *strtab_section;
/* debug sections */
ST_DATA Section *stab_section, *stabstr_section;

/* XXX: avoid static variable */
��̬ �� new_undef_sym = 0; /* Is there a new undefined sym since last new_undef_sym() */

/* ------------------------------------------------------------------------- */

ST_FUNC �� tccelf_new(TCCState *s)
{
    /* no section zero */
    dynarray_add(&s->sections, &s->nb_sections, NULL);

    /* create standard sections */
    text_section = new_section(s, ".text", SHT_PROGBITS, SHF_ALLOC | SHF_EXECINSTR);
    data_section = new_section(s, ".data", SHT_PROGBITS, SHF_ALLOC | SHF_WRITE);
    bss_section = new_section(s, ".bss", SHT_NOBITS, SHF_ALLOC | SHF_WRITE);
    common_section = new_section(s, ".common", SHT_NOBITS, SHF_PRIVATE);
    common_section->sh_num = SHN_COMMON;

    /* symbols are always generated for linking stage */
    symtab_section = new_symtab(s, ".symtab", SHT_SYMTAB, 0,
                                ".strtab",
                                ".hashtab", SHF_PRIVATE);
    strtab_section = symtab_section->link;
    s->symtab = symtab_section;

    /* private symbol table for dynamic symbols */
    s->dynsymtab_section = new_symtab(s, ".dynsymtab", SHT_SYMTAB, SHF_PRIVATE,
                                      ".dynstrtab",
                                      ".dynhashtab", SHF_PRIVATE);
    get_sym_attr(s, 0, 1);
}

#�綨�� CONFIG_TCC_BCHECK
ST_FUNC �� tccelf_bounds_new(TCCState *s)
{
    /* create bounds sections */
    bounds_section = new_section(s, ".bounds",
                                 SHT_PROGBITS, SHF_ALLOC);
    lbounds_section = new_section(s, ".lbounds",
                                  SHT_PROGBITS, SHF_ALLOC);
}
#����

ST_FUNC �� tccelf_stab_new(TCCState *s)
{
    stab_section = new_section(s, ".stab", SHT_PROGBITS, 0);
    stab_section->sh_entsize = �󳤶�(Stab_Sym);
    stabstr_section = new_section(s, ".stabstr", SHT_STRTAB, 0);
    put_elf_str(stabstr_section, "");
    stab_section->link = stabstr_section;
    /* put first entry */
    put_stabs("", 0, 0, 0, 0);
}

��̬ �� free_section(Section *s)
{
    tcc_free(s->data);
}

ST_FUNC �� tccelf_delete(TCCState *s1)
{
    �� i;

    /* free all sections */
    ����(i = 1; i < s1->nb_sections; i++)
        free_section(s1->sections[i]);
    dynarray_reset(&s1->sections, &s1->nb_sections);

    ����(i = 0; i < s1->nb_priv_sections; i++)
        free_section(s1->priv_sections[i]);
    dynarray_reset(&s1->priv_sections, &s1->nb_priv_sections);

    /* free any loaded DLLs */
#�綨�� TCC_IS_NATIVE
    ���� ( i = 0; i < s1->nb_loaded_dlls; i++) {
        DLLReference *ref = s1->loaded_dlls[i];
        �� ( ref->handle )
# �綨�� _WIN32
            FreeLibrary((HMODULE)ref->handle);
# ��
            dlclose(ref->handle);
# ����
    }
#����
    /* free loaded dlls array */
    dynarray_reset(&s1->loaded_dlls, &s1->nb_loaded_dlls);
    tcc_free(s1->sym_attrs);
}

ST_FUNC Section *new_section(TCCState *s1, ���� �� *name, �� sh_type, �� sh_flags)
{
    Section *sec;

    sec = tcc_mallocz(�󳤶�(Section) + strlen(name));
    strcpy(sec->name, name);
    sec->sh_type = sh_type;
    sec->sh_flags = sh_flags;
    ת��(sh_type) {
    ���� SHT_HASH:
    ���� SHT_REL:
    ���� SHT_RELA:
    ���� SHT_DYNSYM:
    ���� SHT_SYMTAB:
    ���� SHT_DYNAMIC:
        sec->sh_addralign = 4;
        ����;
    ���� SHT_STRTAB:
        sec->sh_addralign = 1;
        ����;
    ȱʡ:
        sec->sh_addralign =  PTR_SIZE; /* gcc/pcc default alignment */
        ����;
    }

    �� (sh_flags & SHF_PRIVATE) {
        dynarray_add(&s1->priv_sections, &s1->nb_priv_sections, sec);
    } �� {
        sec->sh_num = s1->nb_sections;
        dynarray_add(&s1->sections, &s1->nb_sections, sec);
    }

    ���� sec;
}

ST_FUNC Section *new_symtab(TCCState *s1,
                           ���� �� *symtab_name, �� sh_type, �� sh_flags,
                           ���� �� *strtab_name,
                           ���� �� *hash_name, �� hash_sh_flags)
{
    Section *symtab, *strtab, *hash;
    �� *ptr, nb_buckets;

    symtab = new_section(s1, symtab_name, sh_type, sh_flags);
    symtab->sh_entsize = �󳤶�(ElfW(Sym));
    strtab = new_section(s1, strtab_name, SHT_STRTAB, sh_flags);
    put_elf_str(strtab, "");
    symtab->link = strtab;
    put_elf_sym(symtab, 0, 0, 0, 0, 0, NULL);

    nb_buckets = 1;

    hash = new_section(s1, hash_name, SHT_HASH, hash_sh_flags);
    hash->sh_entsize = �󳤶�(��);
    symtab->hash = hash;
    hash->link = symtab;

    ptr = section_ptr_add(hash, (2 + nb_buckets + 1) * �󳤶�(��));
    ptr[0] = nb_buckets;
    ptr[1] = 1;
    memset(ptr + 2, 0, (nb_buckets + 1) * �󳤶�(��));
    ���� symtab;
}

/* realloc section and set its content to zero */
ST_FUNC �� section_realloc(Section *sec, �޷� �� new_size)
{
    �޷� �� size;
    �޷� �� *data;

    size = sec->data_allocated;
    �� (size == 0)
        size = 1;
    �� (size < new_size)
        size = size * 2;
    data = tcc_realloc(sec->data, size);
    memset(data + sec->data_allocated, 0, size - sec->data_allocated);
    sec->data = data;
    sec->data_allocated = size;
}

/* reserve at least 'size' bytes aligned per 'align' in section
   'sec' from current offset, and return the aligned offset */
ST_FUNC size_t section_add(Section *sec, addr_t size, �� align)
{
    size_t offset, offset1;

    offset = (sec->data_offset + align - 1) & -align;
    offset1 = offset + size;
    �� (sec->sh_type != SHT_NOBITS && offset1 > sec->data_allocated)
        section_realloc(sec, offset1);
    sec->data_offset = offset1;
    �� (align > sec->sh_addralign)
        sec->sh_addralign = align;
    ���� offset;
}

/* reserve at least 'size' bytes in section 'sec' from
   sec->data_offset. */
ST_FUNC �� *section_ptr_add(Section *sec, addr_t size)
{
    size_t offset = section_add(sec, size, 1);
    ���� sec->data + offset;
}

/* reserve at least 'size' bytes from section start */
ST_FUNC �� section_reserve(Section *sec, �޷� �� size)
{
    �� (size > sec->data_allocated)
        section_realloc(sec, size);
    �� (size > sec->data_offset)
        sec->data_offset = size;
}

/* return a reference to a section, and create it if it does not
   exists */
ST_FUNC Section *find_section(TCCState *s1, ���� �� *name)
{
    Section *sec;
    �� i;
    ����(i = 1; i < s1->nb_sections; i++) {
        sec = s1->sections[i];
        �� (!strcmp(name, sec->name))
            ���� sec;
    }
    /* sections are created as PROGBITS */
    ���� new_section(s1, name, SHT_PROGBITS, SHF_ALLOC);
}

/* ------------------------------------------------------------------------- */

ST_FUNC �� put_elf_str(Section *s, ���� �� *sym)
{
    �� offset, len;
    �� *ptr;

    len = strlen(sym) + 1;
    offset = s->data_offset;
    ptr = section_ptr_add(s, len);
    memcpy(ptr, sym, len);
    ���� offset;
}

/* elf symbol hashing function */
��̬ �޷� �� elf_hash(���� �޷� �� *name)
{
    �޷� �� h = 0, g;

    �� (*name) {
        h = (h << 4) + *name++;
        g = h & 0xf0000000;
        �� (g)
            h ^= g >> 24;
        h &= ~g;
    }
    ���� h;
}

/* rebuild hash table of section s */
/* NOTE: we do factorize the hash table code to go faster */
��̬ �� rebuild_hash(Section *s, �޷� �� nb_buckets)
{
    ElfW(Sym) *sym;
    �� *ptr, *hash, nb_syms, sym_index, h;
    �޷� �� *strtab;

    strtab = s->link->data;
    nb_syms = s->data_offset / �󳤶�(ElfW(Sym));

    s->hash->data_offset = 0;
    ptr = section_ptr_add(s->hash, (2 + nb_buckets + nb_syms) * �󳤶�(��));
    ptr[0] = nb_buckets;
    ptr[1] = nb_syms;
    ptr += 2;
    hash = ptr;
    memset(hash, 0, (nb_buckets + 1) * �󳤶�(��));
    ptr += nb_buckets + 1;

    sym = (ElfW(Sym) *)s->data + 1;
    ����(sym_index = 1; sym_index < nb_syms; sym_index++) {
        �� (ELFW(ST_BIND)(sym->st_info) != STB_LOCAL) {
            h = elf_hash(strtab + sym->st_name) % nb_buckets;
            *ptr = hash[h];
            hash[h] = sym_index;
        } �� {
            *ptr = 0;
        }
        ptr++;
        sym++;
    }
}

/* return the symbol number */
ST_FUNC �� put_elf_sym(Section *s, addr_t value, �޷� �� size,
    �� info, �� other, �� shndx, ���� �� *name)
{
    �� name_offset, sym_index;
    �� nbuckets, h;
    ElfW(Sym) *sym;
    Section *hs;

    sym = section_ptr_add(s, �󳤶�(ElfW(Sym)));
    �� (name)
        name_offset = put_elf_str(s->link, name);
    ��
        name_offset = 0;
    /* XXX: endianness */
    sym->st_name = name_offset;
    sym->st_value = value;
    sym->st_size = size;
    sym->st_info = info;
    sym->st_other = other;
    sym->st_shndx = shndx;
    sym_index = sym - (ElfW(Sym) *)s->data;
    hs = s->hash;
    �� (hs) {
        �� *ptr, *base;
        ptr = section_ptr_add(hs, �󳤶�(��));
        base = (�� *)hs->data;
        /* only add global or weak symbols */
        �� (ELFW(ST_BIND)(info) != STB_LOCAL) {
            /* add another hashing entry */
            nbuckets = base[0];
            h = elf_hash((�޷� �� *) name) % nbuckets;
            *ptr = base[2 + h];
            base[2 + h] = sym_index;
            base[1]++;
            /* we resize the hash table */
            hs->nb_hashed_syms++;
            �� (hs->nb_hashed_syms > 2 * nbuckets) {
                rebuild_hash(s, 2 * nbuckets);
            }
        } �� {
            *ptr = 0;
            base[1]++;
        }
    }
    ���� sym_index;
}

/* find global ELF symbol 'name' and return its index. Return 0 if not
   found. */
ST_FUNC �� find_elf_sym(Section *s, ���� �� *name)
{
    ElfW(Sym) *sym;
    Section *hs;
    �� nbuckets, sym_index, h;
    ���� �� *name1;

    hs = s->hash;
    �� (!hs)
        ���� 0;
    nbuckets = ((�� *)hs->data)[0];
    h = elf_hash((�޷� �� *) name) % nbuckets;
    sym_index = ((�� *)hs->data)[2 + h];
    �� (sym_index != 0) {
        sym = &((ElfW(Sym) *)s->data)[sym_index];
        name1 = (�� *) s->link->data + sym->st_name;
        �� (!strcmp(name, name1))
            ���� sym_index;
        sym_index = ((�� *)hs->data)[2 + nbuckets + sym_index];
    }
    ���� 0;
}

/* return elf symbol value, signal error if 'err' is nonzero */
ST_FUNC addr_t get_elf_sym_addr(TCCState *s, ���� �� *name, �� err)
{
    �� sym_index;
    ElfW(Sym) *sym;

    sym_index = find_elf_sym(s->symtab, name);
    sym = &((ElfW(Sym) *)s->symtab->data)[sym_index];
    �� (!sym_index || sym->st_shndx == SHN_UNDEF) {
        �� (err)
            tcc_error("%s not defined", name);
        ���� 0;
    }
    ���� sym->st_value;
}

/* return elf symbol value */
LIBTCCAPI �� *tcc_get_symbol(TCCState *s, ���� �� *name)
{
    ���� (��*)(uintptr_t)get_elf_sym_addr(s, name, 0);
}

#�� �Ѷ��� TCC_IS_NATIVE || �Ѷ��� TCC_TARGET_PE
/* return elf symbol value or error */
ST_FUNC ��* tcc_get_symbol_err(TCCState *s, ���� �� *name)
{
    ���� (��*)(uintptr_t)get_elf_sym_addr(s, name, 1);
}
#����

/* add an elf symbol : check if it is already defined and patch
   it. Return symbol index. NOTE that sh_num can be SHN_UNDEF. */
ST_FUNC �� set_elf_sym(Section *s, addr_t value, �޷� �� size,
                       �� info, �� other, �� shndx, ���� �� *name)
{
    ElfW(Sym) *esym;
    �� sym_bind, sym_index, sym_type, esym_bind;
    �޷� �� sym_vis, esym_vis, new_vis;

    sym_bind = ELFW(ST_BIND)(info);
    sym_type = ELFW(ST_TYPE)(info);
    sym_vis = ELFW(ST_VISIBILITY)(other);

    sym_index = find_elf_sym(s, name);
    esym = &((ElfW(Sym) *)s->data)[sym_index];
    �� (sym_index && esym->st_value == value && esym->st_size == size
        && esym->st_info == info && esym->st_other == other
        && esym->st_shndx == shndx)
        ���� sym_index;

    �� (sym_bind != STB_LOCAL) {
        /* we search global or weak symbols */
        �� (!sym_index)
            ��ת do_def;
        �� (esym->st_shndx != SHN_UNDEF) {
            esym_bind = ELFW(ST_BIND)(esym->st_info);
            /* propagate the most constraining visibility */
            /* STV_DEFAULT(0)<STV_PROTECTED(3)<STV_HIDDEN(2)<STV_INTERNAL(1) */
            esym_vis = ELFW(ST_VISIBILITY)(esym->st_other);
            �� (esym_vis == STV_DEFAULT) {
                new_vis = sym_vis;
            } �� �� (sym_vis == STV_DEFAULT) {
                new_vis = esym_vis;
            } �� {
                new_vis = (esym_vis < sym_vis) ? esym_vis : sym_vis;
            }
            esym->st_other = (esym->st_other & ~ELFW(ST_VISIBILITY)(-1))
                             | new_vis;
            other = esym->st_other; /* in case we have to patch esym */
            �� (shndx == SHN_UNDEF) {
                /* ignore adding of undefined symbol if the
                   corresponding symbol is already defined */
            } �� �� (sym_bind == STB_GLOBAL && esym_bind == STB_WEAK) {
                /* global overrides weak, so patch */
                ��ת do_patch;
            } �� �� (sym_bind == STB_WEAK && esym_bind == STB_GLOBAL) {
                /* weak is ignored if already global */
            } �� �� (sym_bind == STB_WEAK && esym_bind == STB_WEAK) {
                /* keep first-found weak definition, ignore subsequents */
            } �� �� (sym_vis == STV_HIDDEN || sym_vis == STV_INTERNAL) {
                /* ignore hidden symbols after */
            } �� �� ((esym->st_shndx == SHN_COMMON
                            || esym->st_shndx == bss_section->sh_num)
                        && (shndx < SHN_LORESERVE
                            && shndx != bss_section->sh_num)) {
                /* data symbol gets precedence over common/bss */
                ��ת do_patch;
            } �� �� (shndx == SHN_COMMON || shndx == bss_section->sh_num) {
                /* data symbol keeps precedence over common/bss */
            } �� �� (s == tcc_state->dynsymtab_section) {
                /* we accept that two DLL define the same symbol */
            } �� {
#�� 0
                printf("new_bind=%x new_shndx=%x new_vis=%x old_bind=%x old_shndx=%x old_vis=%x\n",
                       sym_bind, shndx, new_vis, esym_bind, esym->st_shndx, esym_vis);
#����
                tcc_error_noabort("'%s' defined twice", name);
            }
        } �� {
        do_patch:
            esym->st_info = ELFW(ST_INFO)(sym_bind, sym_type);
            esym->st_shndx = shndx;
            new_undef_sym = 1;
            esym->st_value = value;
            esym->st_size = size;
            esym->st_other = other;
        }
    } �� {
    do_def:
        sym_index = put_elf_sym(s, value, size,
                                ELFW(ST_INFO)(sym_bind, sym_type), other,
                                shndx, name);
    }
    ���� sym_index;
}

/* put relocation */
ST_FUNC �� put_elf_reloca(Section *symtab, Section *s, �޷� �� offset,
                            �� type, �� symbol, addr_t addend)
{
    �� buf[256];
    Section *sr;
    ElfW_Rel *rel;

    sr = s->reloc;
    �� (!sr) {
        /* if no relocation section, create it */
        snprintf(buf, �󳤶�(buf), REL_SECTION_FMT, s->name);
        /* if the symtab is allocated, then we consider the relocation
           are also */
        sr = new_section(tcc_state, buf, SHT_RELX, symtab->sh_flags);
        sr->sh_entsize = �󳤶�(ElfW_Rel);
        sr->link = symtab;
        sr->sh_info = s->sh_num;
        s->reloc = sr;
    }
    rel = section_ptr_add(sr, �󳤶�(ElfW_Rel));
    rel->r_offset = offset;
    rel->r_info = ELFW(R_INFO)(symbol, type);
#�� SHT_RELX == SHT_RELA
    rel->r_addend = addend;
#��
    �� (addend)
        tcc_error("non-zero addend on REL architecture");
#����
}

ST_FUNC �� put_elf_reloc(Section *symtab, Section *s, �޷� �� offset,
                           �� type, �� symbol)
{
    put_elf_reloca(symtab, s, offset, type, symbol, 0);
}

/* Remove relocations for section S->reloc starting at oldrelocoffset
   that are to the same place, retaining the last of them.  As side effect
   the relocations are sorted.  Possibly reduces the number of relocs.  */
ST_FUNC �� squeeze_multi_relocs(Section *s, size_t oldrelocoffset)
{
    Section *sr = s->reloc;
    ElfW_Rel *r, *dest;
    ssize_t a;
    ElfW(Addr) addr;

    �� (oldrelocoffset + �󳤶�(*r) >= sr->data_offset)
      ����;
    /* The relocs we're dealing with are the result of initializer parsing.
       So they will be mostly in order and there aren't many of them.
       Secondly we need a stable sort (which qsort isn't).  We use
       a simple insertion sort.  */
    ���� (a = oldrelocoffset + �󳤶�(*r); a < sr->data_offset; a += �󳤶�(*r)) {
        ssize_t i = a - �󳤶�(*r);
        addr = ((ElfW_Rel*)(sr->data + a))->r_offset;
        ���� (; i >= (ssize_t)oldrelocoffset &&
               ((ElfW_Rel*)(sr->data + i))->r_offset > addr; i -= �󳤶�(*r)) {
            ElfW_Rel tmp = *(ElfW_Rel*)(sr->data + a);
            *(ElfW_Rel*)(sr->data + a) = *(ElfW_Rel*)(sr->data + i);
            *(ElfW_Rel*)(sr->data + i) = tmp;
        }
    }

    r = (ElfW_Rel*)(sr->data + oldrelocoffset);
    dest = r;
    ���� (; r < (ElfW_Rel*)(sr->data + sr->data_offset); r++) {
        �� (dest->r_offset != r->r_offset)
          dest++;
        *dest = *r;
    }
    sr->data_offset = (�޷� ��*)dest - sr->data + �󳤶�(*r);
}

/* put stab debug information */

ST_FUNC �� put_stabs(���� �� *str, �� type, �� other, �� desc,
                      �޷� �� value)
{
    Stab_Sym *sym;

    sym = section_ptr_add(stab_section, �󳤶�(Stab_Sym));
    �� (str) {
        sym->n_strx = put_elf_str(stabstr_section, str);
    } �� {
        sym->n_strx = 0;
    }
    sym->n_type = type;
    sym->n_other = other;
    sym->n_desc = desc;
    sym->n_value = value;
}

ST_FUNC �� put_stabs_r(���� �� *str, �� type, �� other, �� desc,
                        �޷� �� value, Section *sec, �� sym_index)
{
    put_stabs(str, type, other, desc, value);
    put_elf_reloc(symtab_section, stab_section,
                  stab_section->data_offset - �󳤶�(�޷� ��),
                  R_DATA_32, sym_index);
}

ST_FUNC �� put_stabn(�� type, �� other, �� desc, �� value)
{
    put_stabs(NULL, type, other, desc, value);
}

ST_FUNC �� put_stabd(�� type, �� other, �� desc)
{
    put_stabs(NULL, type, other, desc, 0);
}

ST_FUNC �ṹ sym_attr *get_sym_attr(TCCState *s1, �� index, �� alloc)
{
    �� n;
    �ṹ sym_attr *tab;

    �� (index >= s1->nb_sym_attrs) {
        �� (!alloc)
            ���� s1->sym_attrs;
        /* find immediately bigger power of 2 and reallocate array */
        n = 1;
        �� (index >= n)
            n *= 2;
        tab = tcc_realloc(s1->sym_attrs, n * �󳤶�(*s1->sym_attrs));
        s1->sym_attrs = tab;
        memset(s1->sym_attrs + s1->nb_sym_attrs, 0,
               (n - s1->nb_sym_attrs) * �󳤶�(*s1->sym_attrs));
        s1->nb_sym_attrs = n;
    }
    ���� &s1->sym_attrs[index];
}

/* Browse each elem of type <type> in section <sec> starting at elem <startoff>
   using variable <elem> */
#���� for_each_elem(sec, startoff, elem, type) \
    ���� (elem = (type *) sec->data + startoff; \
         elem < (type *) (sec->data + sec->data_offset); elem++)

/* In an ELF file symbol table, the local symbols must appear below
   the global and weak ones. Since TCC cannot sort it while generating
   the code, we must do it after. All the relocation tables are also
   modified to take into account the symbol table sorting */
��̬ �� sort_syms(TCCState *s1, Section *s)
{
    �� *old_to_new_syms;
    ElfW(Sym) *new_syms;
    �� nb_syms, i;
    ElfW(Sym) *p, *q;
    ElfW_Rel *rel;
    Section *sr;
    �� type, sym_index;

    nb_syms = s->data_offset / �󳤶�(ElfW(Sym));
    new_syms = tcc_malloc(nb_syms * �󳤶�(ElfW(Sym)));
    old_to_new_syms = tcc_malloc(nb_syms * �󳤶�(��));

    /* first pass for local symbols */
    p = (ElfW(Sym) *)s->data;
    q = new_syms;
    ����(i = 0; i < nb_syms; i++) {
        �� (ELFW(ST_BIND)(p->st_info) == STB_LOCAL) {
            old_to_new_syms[i] = q - new_syms;
            *q++ = *p;
        }
        p++;
    }
    /* save the number of local symbols in section header */
    ��( s->sh_size )    /* this 'if' makes IDA happy */
        s->sh_info = q - new_syms;

    /* then second pass for non local symbols */
    p = (ElfW(Sym) *)s->data;
    ����(i = 0; i < nb_syms; i++) {
        �� (ELFW(ST_BIND)(p->st_info) != STB_LOCAL) {
            old_to_new_syms[i] = q - new_syms;
            *q++ = *p;
        }
        p++;
    }

    /* we copy the new symbols to the old */
    memcpy(s->data, new_syms, nb_syms * �󳤶�(ElfW(Sym)));
    tcc_free(new_syms);

    /* now we modify all the relocations */
    ����(i = 1; i < s1->nb_sections; i++) {
        sr = s1->sections[i];
        �� (sr->sh_type == SHT_RELX && sr->link == s) {
            for_each_elem(sr, 0, rel, ElfW_Rel) {
                sym_index = ELFW(R_SYM)(rel->r_info);
                type = ELFW(R_TYPE)(rel->r_info);
                sym_index = old_to_new_syms[sym_index];
                rel->r_info = ELFW(R_INFO)(sym_index, type);
            }
        }
    }

    tcc_free(old_to_new_syms);
}

/* relocate common symbols in the .bss section */
ST_FUNC �� relocate_common_syms(��)
{
    ElfW(Sym) *sym;

    for_each_elem(symtab_section, 1, sym, ElfW(Sym)) {
        �� (sym->st_shndx == SHN_COMMON) {
            /* symbol alignment is in st_value for SHN_COMMONs */
            sym->st_value = section_add(bss_section, sym->st_size,
                                        sym->st_value);
            sym->st_shndx = bss_section->sh_num;
        }
    }
}

/* relocate symbol table, resolve undefined symbols if do_resolve is
   true and output error if undefined symbol. */
ST_FUNC �� relocate_syms(TCCState *s1, Section *symtab, �� do_resolve)
{
    ElfW(Sym) *sym;
    �� sym_bind, sh_num;
    ���� �� *name;

    for_each_elem(symtab, 1, sym, ElfW(Sym)) {
        sh_num = sym->st_shndx;
        �� (sh_num == SHN_UNDEF) {
            name = (�� *) strtab_section->data + sym->st_name;
            /* Use ld.so to resolve symbol for us (for tcc -run) */
            �� (do_resolve) {
#�� �Ѷ��� TCC_IS_NATIVE && !�Ѷ��� TCC_TARGET_PE
                �� *addr = dlsym(RTLD_DEFAULT, name);
                �� (addr) {
                    sym->st_value = (addr_t) addr;
#�綨�� DEBUG_RELOC
                    printf ("relocate_sym: %s -> 0x%lx\n", name, sym->st_value);
#����
                    ��ת found;
                }
#����
            /* if dynamic symbol exist, it will be used in relocate_section */
            } �� �� (s1->dynsym && find_elf_sym(s1->dynsym, name))
                ��ת found;
            /* XXX: _fp_hw seems to be part of the ABI, so we ignore
               it */
            �� (!strcmp(name, "_fp_hw"))
                ��ת found;
            /* only weak symbols are accepted to be undefined. Their
               value is zero */
            sym_bind = ELFW(ST_BIND)(sym->st_info);
            �� (sym_bind == STB_WEAK)
                sym->st_value = 0;
            ��
                tcc_error_noabort("undefined symbol '%s'", name);
        } �� �� (sh_num < SHN_LORESERVE) {
            /* add section base */
            sym->st_value += s1->sections[sym->st_shndx]->sh_addr;
        }
    found: ;
    }
}

/* relocate a given section (CPU dependent) by applying the relocations
   in the associated relocation section */
ST_FUNC �� relocate_section(TCCState *s1, Section *s)
{
    Section *sr = s->reloc;
    ElfW_Rel *rel;
    ElfW(Sym) *sym;
    �� type, sym_index;
    �޷� �� *ptr;
    addr_t tgt, addr;

    relocate_init(sr);

    for_each_elem(sr, 0, rel, ElfW_Rel) {
        ptr = s->data + rel->r_offset;
        sym_index = ELFW(R_SYM)(rel->r_info);
        sym = &((ElfW(Sym) *)symtab_section->data)[sym_index];
        type = ELFW(R_TYPE)(rel->r_info);
        tgt = sym->st_value;
#�� SHT_RELX == SHT_RELA
        tgt += rel->r_addend;
#����
        addr = s->sh_addr + rel->r_offset;
        relocate(s1, rel, type, ptr, addr, tgt);
    }
    /* if the relocation is allocated, we change its symbol table */
    �� (sr->sh_flags & SHF_ALLOC)
        sr->link = s1->dynsym;
}

/* relocate relocation table in 'sr' */
��̬ �� relocate_rel(TCCState *s1, Section *sr)
{
    Section *s;
    ElfW_Rel *rel;

    s = s1->sections[sr->sh_info];
    for_each_elem(sr, 0, rel, ElfW_Rel)
        rel->r_offset += s->sh_addr;
}

/* count the number of dynamic relocations so that we can reserve
   their space */
��̬ �� prepare_dynamic_rel(TCCState *s1, Section *sr)
{
    ElfW_Rel *rel;
    �� sym_index, type, count;

    count = 0;
    for_each_elem(sr, 0, rel, ElfW_Rel) {
        sym_index = ELFW(R_SYM)(rel->r_info);
        type = ELFW(R_TYPE)(rel->r_info);
        ת��(type) {
#�� �Ѷ���(TCC_TARGET_I386)
        ���� R_386_32:
#���� �Ѷ���(TCC_TARGET_X86_64)
        ���� R_X86_64_32:
        ���� R_X86_64_32S:
        ���� R_X86_64_64:
#����
            count++;
            ����;
#�� �Ѷ���(TCC_TARGET_I386)
        ���� R_386_PC32:
#���� �Ѷ���(TCC_TARGET_X86_64)
        ���� R_X86_64_PC32:
#����
            �� (get_sym_attr(s1, sym_index, 0)->dyn_index)
                count++;
            ����;
        ȱʡ:
            ����;
        }
    }
    �� (count) {
        /* allocate the section */
        sr->sh_flags |= SHF_ALLOC;
        sr->sh_size = count * �󳤶�(ElfW_Rel);
    }
    ���� count;
}

��̬ �� build_got(TCCState *s1)
{
    /* if no got, then create it */
    s1->got = new_section(s1, ".got", SHT_PROGBITS, SHF_ALLOC | SHF_WRITE);
    s1->got->sh_entsize = 4;
    set_elf_sym(symtab_section, 0, 4, ELFW(ST_INFO)(STB_GLOBAL, STT_OBJECT),
                0, s1->got->sh_num, "_GLOBAL_OFFSET_TABLE_");
    /* keep space for _DYNAMIC pointer and two dummy got entries */
    section_ptr_add(s1->got, 3 * PTR_SIZE);
}

/* Create a GOT and (for function call) a PLT entry corresponding to a symbol
   in s1->symtab. When creating the dynamic symbol table entry for the GOT
   relocation, use 'size' and 'info' for the corresponding symbol metadata.
   Returns the offset of the GOT or (if any) PLT entry. */
��̬ �ṹ sym_attr * put_got_entry(TCCState *s1, �� dyn_reloc_type,
                                       �޷� �� size,
                                       �� info, �� sym_index)
{
    �� need_plt_entry;
    ���� �� *name;
    ElfW(Sym) *sym;
    �ṹ sym_attr *attr;
    �޷� got_offset;
    �� plt_name[100];
    �� len;

    need_plt_entry = (dyn_reloc_type == R_JMP_SLOT);
    attr = get_sym_attr(s1, sym_index, 1);

    /* In case a function is both called and its address taken 2 GOT entries
       are created, one for taking the address (GOT) and the other for the PLT
       entry (PLTGOT).  */
    �� (need_plt_entry ? attr->plt_offset : attr->got_offset)
        ���� attr;

    /* create the GOT entry */
    got_offset = s1->got->data_offset;
    section_ptr_add(s1->got, PTR_SIZE);

    /* Create the GOT relocation that will insert the address of the object or
       function of interest in the GOT entry. This is a static relocation for
       memory output (dlsym will give us the address of symbols) and dynamic
       relocation otherwise (executable and DLLs). The relocation should be
       done lazily for GOT entry with *_JUMP_SLOT relocation type (the one
       associated to a PLT entry) but is currently done at load time for an
       unknown reason. */

    sym = &((ElfW(Sym) *) symtab_section->data)[sym_index];
    name = (�� *) symtab_section->link->data + sym->st_name;

    �� (s1->dynsym) {
        �� (ELFW(ST_BIND)(sym->st_info) == STB_LOCAL) {
            /* Hack alarm.  We don't want to emit dynamic symbols
               and symbol based relocs for STB_LOCAL symbols, but rather
               want to resolve them directly.  At this point the symbol
               values aren't final yet, so we must defer this.  We will later
               have to create a RELATIVE reloc anyway, so we misuse the
               relocation slot to smuggle the symbol reference until
               fill_local_got_entries.  Not that the sym_index is
               relative to symtab_section, not s1->dynsym!  Nevertheless
               we use s1->dyn_sym so that if this is the first call
               that got->reloc is correctly created.  Also note that
               RELATIVE relocs are not normally created for the .got,
               so the types serves as a marker for later (and is retained
               also for the final output, which is okay because then the
               got is just normal data).  */
            put_elf_reloc(s1->dynsym, s1->got, got_offset, R_RELATIVE,
                          sym_index);
        } �� {
            �� (0 == attr->dyn_index)
                attr->dyn_index = set_elf_sym(s1->dynsym, sym->st_value, size,
                                              info, 0, sym->st_shndx, name);
            put_elf_reloc(s1->dynsym, s1->got, got_offset, dyn_reloc_type,
                          attr->dyn_index);
        }
    } �� {
        put_elf_reloc(symtab_section, s1->got, got_offset, dyn_reloc_type,
                      sym_index);
    }

    �� (need_plt_entry) {
        �� (!s1->plt) {
            s1->plt = new_section(s1, ".plt", SHT_PROGBITS,
                                  SHF_ALLOC | SHF_EXECINSTR);
            s1->plt->sh_entsize = 4;
        }

        attr->plt_offset = create_plt_entry(s1, got_offset, attr);

        /* create a symbol 'sym@plt' for the PLT jump vector */
        len = strlen(name);
        �� (len > �󳤶� plt_name - 5)
            len = �󳤶� plt_name - 5;
        memcpy(plt_name, name, len);
        strcpy(plt_name + len, "@plt");
        attr->plt_sym = put_elf_sym(s1->symtab, attr->plt_offset, sym->st_size,
            ELFW(ST_INFO)(STB_GLOBAL, STT_FUNC), 0, s1->plt->sh_num, plt_name);

    } �� {
        attr->got_offset = got_offset;
    }

    ���� attr;
}

/* build GOT and PLT entries */
ST_FUNC �� build_got_entries(TCCState *s1)
{
    Section *s;
    ElfW_Rel *rel;
    ElfW(Sym) *sym;
    �� i, type, gotplt_entry, reloc_type, sym_index;
    �ṹ sym_attr *attr;

    ����(i = 1; i < s1->nb_sections; i++) {
        s = s1->sections[i];
        �� (s->sh_type != SHT_RELX)
            ����;
        /* no need to handle got relocations */
        �� (s->link != symtab_section)
            ����;
        for_each_elem(s, 0, rel, ElfW_Rel) {
            type = ELFW(R_TYPE)(rel->r_info);
            gotplt_entry = gotplt_entry_type(type);
            sym_index = ELFW(R_SYM)(rel->r_info);
            sym = &((ElfW(Sym) *)symtab_section->data)[sym_index];

            �� (gotplt_entry == NO_GOTPLT_ENTRY) {
                ����;
            }

            /* Automatically create PLT/GOT [entry] if it is an undefined
               reference (resolved at runtime), or the symbol is absolute,
               probably created by tcc_add_symbol, and thus on 64-bit
               targets might be too far from application code.  */
            �� (gotplt_entry == AUTO_GOTPLT_ENTRY) {
                �� (sym->st_shndx == SHN_UNDEF) {
                    ElfW(Sym) *esym;
                    �� dynindex;
                    �� (s1->output_type == TCC_OUTPUT_DLL && ! PCRELATIVE_DLLPLT)
                        ����;
                    /* Relocations for UNDEF symbols would normally need
                       to be transferred into the executable or shared object.
                       If that were done AUTO_GOTPLT_ENTRY wouldn't exist.
                       But TCC doesn't do that (at least for exes), so we
                       need to resolve all such relocs locally.  And that
                       means PLT slots for functions in DLLs and COPY relocs for
                       data symbols.  COPY relocs were generated in
                       bind_exe_dynsyms (and the symbol adjusted to be defined),
                       and for functions we were generated a dynamic symbol
                       of function type.  */
                    �� (s1->dynsym) {
                        /* dynsym isn't set for -run :-/  */
                        dynindex = get_sym_attr(s1, sym_index, 0)->dyn_index;
                        esym = (ElfW(Sym) *)s1->dynsym->data + dynindex;
                        �� (dynindex
                            && (ELFW(ST_TYPE)(esym->st_info) == STT_FUNC
                                || (ELFW(ST_TYPE)(esym->st_info) == STT_NOTYPE
                                    && ELFW(ST_TYPE)(sym->st_info) == STT_FUNC)))
                            ��ת jmp_slot;
                    }
                } �� �� (!(sym->st_shndx == SHN_ABS
#��δ���� TCC_TARGET_ARM
                        && PTR_SIZE == 8
#����
                        ))
                    ����;
            }

#�綨�� TCC_TARGET_X86_64
            �� ((type == R_X86_64_PLT32 || type == R_X86_64_PC32) &&
                (ELFW(ST_VISIBILITY)(sym->st_other) != STV_DEFAULT ||
                 ELFW(ST_BIND)(sym->st_info) == STB_LOCAL)) {
                rel->r_info = ELFW(R_INFO)(sym_index, R_X86_64_PC32);
                ����;
            }
#����
            �� (code_reloc(type)) {
            jmp_slot:
                reloc_type = R_JMP_SLOT;
            } ��
                reloc_type = R_GLOB_DAT;

            �� (!s1->got)
                build_got(s1);

            �� (gotplt_entry == BUILD_GOT_ONLY)
                ����;

            attr = put_got_entry(s1, reloc_type, sym->st_size, sym->st_info,
                                 sym_index);

            �� (reloc_type == R_JMP_SLOT)
                rel->r_info = ELFW(R_INFO)(attr->plt_sym, type);
        }
    }
}

/* put dynamic tag */
��̬ �� put_dt(Section *dynamic, �� dt, addr_t val)
{
    ElfW(Dyn) *dyn;
    dyn = section_ptr_add(dynamic, �󳤶�(ElfW(Dyn)));
    dyn->d_tag = dt;
    dyn->d_un.d_val = val;
}

#��δ���� TCC_TARGET_PE
��̬ �� add_init_array_defines(TCCState *s1, ���� �� *section_name)
{
    Section *s;
    �� end_offset;
    �� sym_start[1024];
    �� sym_end[1024];

    snprintf(sym_start, �󳤶�(sym_start), "__%s_start", section_name + 1);
    snprintf(sym_end, �󳤶�(sym_end), "__%s_end", section_name + 1);

    s = find_section(s1, section_name);
    �� (!s) {
        end_offset = 0;
        s = data_section;
    } �� {
        end_offset = s->data_offset;
    }

    set_elf_sym(symtab_section,
                0, 0,
                ELFW(ST_INFO)(STB_GLOBAL, STT_NOTYPE), 0,
                s->sh_num, sym_start);
    set_elf_sym(symtab_section,
                end_offset, 0,
                ELFW(ST_INFO)(STB_GLOBAL, STT_NOTYPE), 0,
                s->sh_num, sym_end);
}
#����

��̬ �� tcc_add_support(TCCState *s1, ���� �� *filename)
{
    �� buf[1024];
    snprintf(buf, �󳤶�(buf), "%s/%s", s1->tcc_lib_path, filename);
    ���� tcc_add_file(s1, buf);
}

ST_FUNC �� tcc_add_bcheck(TCCState *s1)
{
#�綨�� CONFIG_TCC_BCHECK
    addr_t *ptr;
    �� sym_index;

    �� (0 == s1->do_bounds_check)
        ����;
    /* XXX: add an object file to do that */
    ptr = section_ptr_add(bounds_section, �󳤶�(*ptr));
    *ptr = 0;
    set_elf_sym(symtab_section, 0, 0,
                ELFW(ST_INFO)(STB_GLOBAL, STT_NOTYPE), 0,
                bounds_section->sh_num, "__bounds_start");
    /* pull bcheck.o from libtcc1.a */
    sym_index = set_elf_sym(symtab_section, 0, 0,
                ELFW(ST_INFO)(STB_GLOBAL, STT_NOTYPE), 0,
                SHN_UNDEF, "__bound_init");
    �� (s1->output_type != TCC_OUTPUT_MEMORY) {
        /* add 'call __bound_init()' in .init section */
        Section *init_section = find_section(s1, ".init");
        �޷� �� *pinit = section_ptr_add(init_section, 5);
        pinit[0] = 0xe8;
        write32le(pinit + 1, -4);
        put_elf_reloc(symtab_section, init_section,
            init_section->data_offset - 4, R_386_PC32, sym_index);
            /* R_386_PC32 = R_X86_64_PC32 = 2 */
    }
#����
}

/* add tcc runtime libraries */
ST_FUNC �� tcc_add_runtime(TCCState *s1)
{
    tcc_add_bcheck(s1);
    tcc_add_pragma_libs(s1);
    /* add libc */
    �� (!s1->nostdlib) {
        tcc_add_library_err(s1, "c");
#�綨�� TCC_LIBGCC
        �� (!s1->static_link) {
            �� (TCC_LIBGCC[0] == '/')
                tcc_add_file(s1, TCC_LIBGCC);
            ��
                tcc_add_dll(s1, TCC_LIBGCC, 0);
        }
#����
        tcc_add_support(s1, TCC_LIBTCC1);
        /* add crt end if not memory output */
        �� (s1->output_type != TCC_OUTPUT_MEMORY)
            tcc_add_crt(s1, "crtn.o");
    }
}

/* add various standard linker symbols (must be done after the
   sections are filled (for example after allocating common
   symbols)) */
ST_FUNC �� tcc_add_linker_symbols(TCCState *s1)
{
    �� buf[1024];
    �� i;
    Section *s;

    set_elf_sym(symtab_section,
                text_section->data_offset, 0,
                ELFW(ST_INFO)(STB_GLOBAL, STT_NOTYPE), 0,
                text_section->sh_num, "_etext");
    set_elf_sym(symtab_section,
                data_section->data_offset, 0,
                ELFW(ST_INFO)(STB_GLOBAL, STT_NOTYPE), 0,
                data_section->sh_num, "_edata");
    set_elf_sym(symtab_section,
                bss_section->data_offset, 0,
                ELFW(ST_INFO)(STB_GLOBAL, STT_NOTYPE), 0,
                bss_section->sh_num, "_end");
#��δ���� TCC_TARGET_PE
    /* horrible new standard ldscript defines */
    add_init_array_defines(s1, ".preinit_array");
    add_init_array_defines(s1, ".init_array");
    add_init_array_defines(s1, ".fini_array");
#����

    /* add start and stop symbols for sections whose name can be
       expressed in C */
    ����(i = 1; i < s1->nb_sections; i++) {
        s = s1->sections[i];
        �� (s->sh_type == SHT_PROGBITS &&
            (s->sh_flags & SHF_ALLOC)) {
            ���� �� *p;
            �� ch;

            /* check if section name can be expressed in C */
            p = s->name;
            ����(;;) {
                ch = *p;
                �� (!ch)
                    ����;
                �� (!isid(ch) && !isnum(ch))
                    ��ת next_sec;
                p++;
            }
            snprintf(buf, �󳤶�(buf), "__start_%s", s->name);
            set_elf_sym(symtab_section,
                        0, 0,
                        ELFW(ST_INFO)(STB_GLOBAL, STT_NOTYPE), 0,
                        s->sh_num, buf);
            snprintf(buf, �󳤶�(buf), "__stop_%s", s->name);
            set_elf_sym(symtab_section,
                        s->data_offset, 0,
                        ELFW(ST_INFO)(STB_GLOBAL, STT_NOTYPE), 0,
                        s->sh_num, buf);
        }
    next_sec: ;
    }
}

��̬ �� tcc_output_binary(TCCState *s1, FILE *f,
                              ���� �� *sec_order)
{
    Section *s;
    �� i, offset, size;

    offset = 0;
    ����(i=1;i<s1->nb_sections;i++) {
        s = s1->sections[sec_order[i]];
        �� (s->sh_type != SHT_NOBITS &&
            (s->sh_flags & SHF_ALLOC)) {
            �� (offset < s->sh_offset) {
                fputc(0, f);
                offset++;
            }
            size = s->sh_size;
            fwrite(s->data, 1, size, f);
            offset += size;
        }
    }
}

#�� �Ѷ���(__FreeBSD__) || �Ѷ���(__FreeBSD_kernel__)
#���� HAVE_PHDR       1
#���� EXTRA_RELITEMS  14
#��
#���� HAVE_PHDR      1
#���� EXTRA_RELITEMS 9
#����

ST_FUNC �� fill_got_entry(TCCState *s1, ElfW_Rel *rel)
{
    �� sym_index = ELFW(R_SYM) (rel->r_info);
    ElfW(Sym) *sym = &((ElfW(Sym) *) symtab_section->data)[sym_index];
    �ṹ sym_attr *attr = get_sym_attr(s1, sym_index, 0);
    �޷� offset = attr->got_offset;

    �� (0 == offset)
        ����;
    section_reserve(s1->got, offset + PTR_SIZE);
#�綨�� TCC_TARGET_X86_64
    write64le(s1->got->data + offset, sym->st_value);
#��
    write32le(s1->got->data + offset, sym->st_value);
#����
}

/* Perform relocation to GOT or PLT entries */
ST_FUNC �� fill_got(TCCState *s1)
{
    Section *s;
    ElfW_Rel *rel;
    �� i;

    ����(i = 1; i < s1->nb_sections; i++) {
        s = s1->sections[i];
        �� (s->sh_type != SHT_RELX)
            ����;
        /* no need to handle got relocations */
        �� (s->link != symtab_section)
            ����;
        for_each_elem(s, 0, rel, ElfW_Rel) {
            ת�� (ELFW(R_TYPE) (rel->r_info)) {
                ���� R_X86_64_GOT32:
                ���� R_X86_64_GOTPCREL:
                ���� R_X86_64_GOTPCRELX:
                ���� R_X86_64_REX_GOTPCRELX:
                ���� R_X86_64_PLT32:
                    fill_got_entry(s1, rel);
                    ����;
            }
        }
    }
}

/* See put_got_entry for a description.  This is the second stage
   where GOT references to local defined symbols are rewritten.  */
��̬ �� fill_local_got_entries(TCCState *s1)
{
    ElfW_Rel *rel;
    for_each_elem(s1->got->reloc, 0, rel, ElfW_Rel) {
        �� (ELFW(R_TYPE)(rel->r_info) == R_RELATIVE) {
            �� sym_index = ELFW(R_SYM) (rel->r_info);
            ElfW(Sym) *sym = &((ElfW(Sym) *) symtab_section->data)[sym_index];
            �ṹ sym_attr *attr = get_sym_attr(s1, sym_index, 0);
            �޷� offset = attr->got_offset;
            �� (offset != rel->r_offset - s1->got->sh_addr)
              tcc_error_noabort("huh");
            rel->r_info = ELFW(R_INFO)(0, R_RELATIVE);
#�� SHT_RELX == SHT_RELA
            rel->r_addend = sym->st_value;
#��
            /* All our REL architectures also happen to be 32bit LE.  */
            write32le(s1->got->data + offset, sym->st_value);
#����
        }
    }
}

/* Bind symbols of executable: resolve undefined symbols from exported symbols
   in shared libraries and export non local defined symbols to shared libraries
   if -rdynamic switch was given on command line */
��̬ �� bind_exe_dynsyms(TCCState *s1)
{
    ���� �� *name;
    �� sym_index, index;
    ElfW(Sym) *sym, *esym;
    �� type;

    /* Resolve undefined symbols from dynamic symbols. When there is a match:
       - if STT_FUNC or STT_GNU_IFUNC symbol -> add it in PLT
       - if STT_OBJECT symbol -> add it in .bss section with suitable reloc */
    for_each_elem(symtab_section, 1, sym, ElfW(Sym)) {
        �� (sym->st_shndx == SHN_UNDEF) {
            name = (�� *) symtab_section->link->data + sym->st_name;
            sym_index = find_elf_sym(s1->dynsymtab_section, name);
            �� (sym_index) {
                esym = &((ElfW(Sym) *)s1->dynsymtab_section->data)[sym_index];
                type = ELFW(ST_TYPE)(esym->st_info);
                �� ((type == STT_FUNC) || (type == STT_GNU_IFUNC)) {
                    /* Indirect functions shall have STT_FUNC type in executable
                     * dynsym section. Indeed, a dlsym call following a lazy
                     * resolution would pick the symbol value from the
                     * executable dynsym entry which would contain the address
                     * of the function wanted by the caller of dlsym instead of
                     * the address of the function that would return that
                     * address */
                    �� dynindex
                      = put_elf_sym(s1->dynsym, 0, esym->st_size,
                                    ELFW(ST_INFO)(STB_GLOBAL,STT_FUNC), 0, 0,
                                    name);
                    �� index = sym - (ElfW(Sym) *) symtab_section->data;
                    get_sym_attr(s1, index, 1)->dyn_index = dynindex;
                } �� �� (type == STT_OBJECT) {
                    �޷� �� offset;
                    ElfW(Sym) *dynsym;
                    offset = bss_section->data_offset;
                    /* XXX: which alignment ? */
                    offset = (offset + 16 - 1) & -16;
                    set_elf_sym (s1->symtab, offset, esym->st_size,
                                 esym->st_info, 0, bss_section->sh_num, name);
                    index = put_elf_sym(s1->dynsym, offset, esym->st_size,
                                        esym->st_info, 0, bss_section->sh_num,
                                        name);

                    /* Ensure R_COPY works for weak symbol aliases */
                    �� (ELFW(ST_BIND)(esym->st_info) == STB_WEAK) {
                        for_each_elem(s1->dynsymtab_section, 1, dynsym, ElfW(Sym)) {
                            �� ((dynsym->st_value == esym->st_value)
                                && (ELFW(ST_BIND)(dynsym->st_info) == STB_GLOBAL)) {
                                �� *dynname = (�� *) s1->dynsymtab_section->link->data
                                                + dynsym->st_name;
                                put_elf_sym(s1->dynsym, offset, dynsym->st_size,
                                            dynsym->st_info, 0,
                                            bss_section->sh_num, dynname);
                                ����;
                            }
                        }
                    }

                    put_elf_reloc(s1->dynsym, bss_section,
                                  offset, R_COPY, index);
                    offset += esym->st_size;
                    bss_section->data_offset = offset;
                }
            } �� {
                /* STB_WEAK undefined symbols are accepted */
                /* XXX: _fp_hw seems to be part of the ABI, so we ignore it */
                �� (ELFW(ST_BIND)(sym->st_info) == STB_WEAK ||
                    !strcmp(name, "_fp_hw")) {
                } �� {
                    tcc_error_noabort("undefined symbol '%s'", name);
                }
            }
        } �� �� (s1->rdynamic && ELFW(ST_BIND)(sym->st_info) != STB_LOCAL) {
            /* if -rdynamic option, then export all non local symbols */
            name = (�� *) symtab_section->link->data + sym->st_name;
            set_elf_sym(s1->dynsym, sym->st_value, sym->st_size, sym->st_info,
                        0, sym->st_shndx, name);
        }
    }
}

/* Bind symbols of libraries: export all non local symbols of executable that
   are referenced by shared libraries. The reason is that the dynamic loader
   search symbol first in executable and then in libraries. Therefore a
   reference to a symbol already defined by a library can still be resolved by
   a symbol in the executable. */
��̬ �� bind_libs_dynsyms(TCCState *s1)
{
    ���� �� *name;
    �� sym_index;
    ElfW(Sym) *sym, *esym;

    for_each_elem(s1->dynsymtab_section, 1, esym, ElfW(Sym)) {
        name = (�� *) s1->dynsymtab_section->link->data + esym->st_name;
        sym_index = find_elf_sym(symtab_section, name);
        /* XXX: avoid adding a symbol if already present because of
                -rdynamic ? */
        sym = &((ElfW(Sym) *)symtab_section->data)[sym_index];
        �� (sym_index && sym->st_shndx != SHN_UNDEF)
            set_elf_sym(s1->dynsym, sym->st_value, sym->st_size, sym->st_info,
                        0, sym->st_shndx, name);
        �� �� (esym->st_shndx == SHN_UNDEF) {
            /* weak symbols can stay undefined */
            �� (ELFW(ST_BIND)(esym->st_info) != STB_WEAK)
                tcc_warning("undefined dynamic symbol '%s'", name);
        }
    }
}

/* Export all non local symbols. This is used by shared libraries so that the
   non local symbols they define can resolve a reference in another shared
   library or in the executable. Correspondingly, it allows undefined local
   symbols to be resolved by other shared libraries or by the executable. */
��̬ �� export_global_syms(TCCState *s1)
{
    �� dynindex, index;
    ���� �� *name;
    ElfW(Sym) *sym;

    for_each_elem(symtab_section, 1, sym, ElfW(Sym)) {
        �� (ELFW(ST_BIND)(sym->st_info) != STB_LOCAL) {
            name = (�� *) symtab_section->link->data + sym->st_name;
            dynindex = put_elf_sym(s1->dynsym, sym->st_value, sym->st_size,
                                   sym->st_info, 0, sym->st_shndx, name);
            index = sym - (ElfW(Sym) *) symtab_section->data;
            get_sym_attr(s1, index, 1)->dyn_index = dynindex;
        }
    }
}

/* Allocate strings for section names and decide if an unallocated section
   should be output.
   NOTE: the strsec section comes last, so its size is also correct ! */
��̬ �� alloc_sec_names(TCCState *s1, �� file_type, Section *strsec)
{
    �� i;
    Section *s;

    /* Allocate strings for section names */
    ����(i = 1; i < s1->nb_sections; i++) {
        s = s1->sections[i];
        /* when generating a DLL, we include relocations but we may
           patch them */
        �� (file_type == TCC_OUTPUT_DLL &&
            s->sh_type == SHT_RELX &&
            !(s->sh_flags & SHF_ALLOC)) {
            /* gr: avoid bogus relocs for empty (debug) sections */
            �� (s1->sections[s->sh_info]->sh_flags & SHF_ALLOC)
                prepare_dynamic_rel(s1, s);
            �� �� (s1->do_debug)
                s->sh_size = s->data_offset;
        } �� �� (s1->do_debug ||
            file_type == TCC_OUTPUT_OBJ ||
            (s->sh_flags & SHF_ALLOC) ||
            i == (s1->nb_sections - 1)) {
            /* we output all sections if debug or object file */
            s->sh_size = s->data_offset;
        }
        �� (s->sh_size || (s->sh_flags & SHF_ALLOC))
            s->sh_name = put_elf_str(strsec, s->name);
    }
    strsec->sh_size = strsec->data_offset;
}

/* Info to be copied in dynamic section */
�ṹ dyn_inf {
    Section *dynamic;
    Section *dynstr;
    �޷� �� dyn_rel_off;
    addr_t rel_addr;
    addr_t rel_size;
#�� �Ѷ���(__FreeBSD__) || �Ѷ���(__FreeBSD_kernel__)
    addr_t bss_addr;
    addr_t bss_size;
#����
};

/* Assign sections to segments and decide how are sections laid out when loaded
   in memory. This function also fills corresponding program headers. */
��̬ �� layout_sections(TCCState *s1, ElfW(Phdr) *phdr, �� phnum,
                           Section *interp, Section* strsec,
                           �ṹ dyn_inf *dyninf, �� *sec_order)
{
    �� i, j, k, file_type, sh_order_index, file_offset;
    �޷� �� s_align;
    �� �� tmp;
    addr_t addr;
    ElfW(Phdr) *ph;
    Section *s;

    file_type = s1->output_type;
    sh_order_index = 1;
    file_offset = 0;
    �� (s1->output_format == TCC_OUTPUT_FORMAT_ELF)
        file_offset = �󳤶�(ElfW(Ehdr)) + phnum * �󳤶�(ElfW(Phdr));
    s_align = ELF_PAGE_SIZE;
    �� (s1->section_align)
        s_align = s1->section_align;

    �� (phnum > 0) {
        �� (s1->has_text_addr) {
            �� a_offset, p_offset;
            addr = s1->text_addr;
            /* we ensure that (addr % ELF_PAGE_SIZE) == file_offset %
               ELF_PAGE_SIZE */
            a_offset = (��) (addr & (s_align - 1));
            p_offset = file_offset & (s_align - 1);
            �� (a_offset < p_offset)
                a_offset += s_align;
            file_offset += (a_offset - p_offset);
        } �� {
            �� (file_type == TCC_OUTPUT_DLL)
                addr = 0;
            ��
                addr = ELF_START_ADDR;
            /* compute address after headers */
            addr += (file_offset & (s_align - 1));
        }

        ph = &phdr[0];
        /* Leave one program headers for the program interpreter and one for
           the program header table itself if needed. These are done later as
           they require section layout to be done first. */
        �� (interp)
            ph += 1 + HAVE_PHDR;

        /* dynamic relocation table information, for .dynamic section */
        dyninf->rel_addr = dyninf->rel_size = 0;
#�� �Ѷ���(__FreeBSD__) || �Ѷ���(__FreeBSD_kernel__)
        dyninf->bss_addr = dyninf->bss_size = 0;
#����

        ����(j = 0; j < 2; j++) {
            ph->p_type = PT_LOAD;
            �� (j == 0)
                ph->p_flags = PF_R | PF_X;
            ��
                ph->p_flags = PF_R | PF_W;
            ph->p_align = s_align;

            /* Decide the layout of sections loaded in memory. This must
               be done before program headers are filled since they contain
               info about the layout. We do the following ordering: interp,
               symbol tables, relocations, progbits, nobits */
            /* XXX: do faster and simpler sorting */
            ����(k = 0; k < 5; k++) {
                ����(i = 1; i < s1->nb_sections; i++) {
                    s = s1->sections[i];
                    /* compute if section should be included */
                    �� (j == 0) {
                        �� ((s->sh_flags & (SHF_ALLOC | SHF_WRITE)) !=
                            SHF_ALLOC)
                            ����;
                    } �� {
                        �� ((s->sh_flags & (SHF_ALLOC | SHF_WRITE)) !=
                            (SHF_ALLOC | SHF_WRITE))
                            ����;
                    }
                    �� (s == interp) {
                        �� (k != 0)
                            ����;
                    } �� �� (s->sh_type == SHT_DYNSYM ||
                               s->sh_type == SHT_STRTAB ||
                               s->sh_type == SHT_HASH) {
                        �� (k != 1)
                            ����;
                    } �� �� (s->sh_type == SHT_RELX) {
                        �� (k != 2)
                            ����;
                    } �� �� (s->sh_type == SHT_NOBITS) {
                        �� (k != 4)
                            ����;
                    } �� {
                        �� (k != 3)
                            ����;
                    }
                    sec_order[sh_order_index++] = i;

                    /* section matches: we align it and add its size */
                    tmp = addr;
                    addr = (addr + s->sh_addralign - 1) &
                        ~(s->sh_addralign - 1);
                    file_offset += (��) ( addr - tmp );
                    s->sh_offset = file_offset;
                    s->sh_addr = addr;

                    /* update program header infos */
                    �� (ph->p_offset == 0) {
                        ph->p_offset = file_offset;
                        ph->p_vaddr = addr;
                        ph->p_paddr = ph->p_vaddr;
                    }
                    /* update dynamic relocation infos */
                    �� (s->sh_type == SHT_RELX) {
#�� �Ѷ���(__FreeBSD__) || �Ѷ���(__FreeBSD_kernel__)
                        �� (!strcmp(strsec->data + s->sh_name, ".rel.got")) {
                            dyninf->rel_addr = addr;
                            dyninf->rel_size += s->sh_size; /* XXX only first rel. */
                        }
                        �� (!strcmp(strsec->data + s->sh_name, ".rel.bss")) {
                            dyninf->bss_addr = addr;
                            dyninf->bss_size = s->sh_size; /* XXX only first rel. */
                        }
#��
                        �� (dyninf->rel_size == 0)
                            dyninf->rel_addr = addr;
                        dyninf->rel_size += s->sh_size;
#����
                    }
                    addr += s->sh_size;
                    �� (s->sh_type != SHT_NOBITS)
                        file_offset += s->sh_size;
                }
            }
            �� (j == 0) {
                /* Make the first PT_LOAD segment include the program
                   headers itself (and the ELF header as well), it'll
                   come out with same memory use but will make various
                   tools like binutils strip work better.  */
                ph->p_offset &= ~(ph->p_align - 1);
                ph->p_vaddr &= ~(ph->p_align - 1);
                ph->p_paddr &= ~(ph->p_align - 1);
            }
            ph->p_filesz = file_offset - ph->p_offset;
            ph->p_memsz = addr - ph->p_vaddr;
            ph++;
            �� (j == 0) {
                �� (s1->output_format == TCC_OUTPUT_FORMAT_ELF) {
                    /* if in the middle of a page, we duplicate the page in
                       memory so that one copy is RX and the other is RW */
                    �� ((addr & (s_align - 1)) != 0)
                        addr += s_align;
                } �� {
                    addr = (addr + s_align - 1) & ~(s_align - 1);
                    file_offset = (file_offset + s_align - 1) & ~(s_align - 1);
                }
            }
        }
    }

    /* all other sections come after */
    ����(i = 1; i < s1->nb_sections; i++) {
        s = s1->sections[i];
        �� (phnum > 0 && (s->sh_flags & SHF_ALLOC))
            ����;
        sec_order[sh_order_index++] = i;

        file_offset = (file_offset + s->sh_addralign - 1) &
            ~(s->sh_addralign - 1);
        s->sh_offset = file_offset;
        �� (s->sh_type != SHT_NOBITS)
            file_offset += s->sh_size;
    }

    ���� file_offset;
}

��̬ �� fill_unloadable_phdr(ElfW(Phdr) *phdr, �� phnum, Section *interp,
                                 Section *dynamic)
{
    ElfW(Phdr) *ph;

    /* if interpreter, then add corresponding program header */
    �� (interp) {
        ph = &phdr[0];

        �� (HAVE_PHDR)
        {
            �� len = phnum * �󳤶�(ElfW(Phdr));

            ph->p_type = PT_PHDR;
            ph->p_offset = �󳤶�(ElfW(Ehdr));
            ph->p_vaddr = interp->sh_addr - len;
            ph->p_paddr = ph->p_vaddr;
            ph->p_filesz = ph->p_memsz = len;
            ph->p_flags = PF_R | PF_X;
            ph->p_align = 4; /* interp->sh_addralign; */
            ph++;
        }

        ph->p_type = PT_INTERP;
        ph->p_offset = interp->sh_offset;
        ph->p_vaddr = interp->sh_addr;
        ph->p_paddr = ph->p_vaddr;
        ph->p_filesz = interp->sh_size;
        ph->p_memsz = interp->sh_size;
        ph->p_flags = PF_R;
        ph->p_align = interp->sh_addralign;
    }

    /* if dynamic section, then add corresponding program header */
    �� (dynamic) {
        ph = &phdr[phnum - 1];

        ph->p_type = PT_DYNAMIC;
        ph->p_offset = dynamic->sh_offset;
        ph->p_vaddr = dynamic->sh_addr;
        ph->p_paddr = ph->p_vaddr;
        ph->p_filesz = dynamic->sh_size;
        ph->p_memsz = dynamic->sh_size;
        ph->p_flags = PF_R | PF_W;
        ph->p_align = dynamic->sh_addralign;
    }
}

/* Fill the dynamic section with tags describing the address and size of
   sections */
��̬ �� fill_dynamic(TCCState *s1, �ṹ dyn_inf *dyninf)
{
    Section *dynamic;

    dynamic = dyninf->dynamic;

    /* put dynamic section entries */
    dynamic->data_offset = dyninf->dyn_rel_off;
    put_dt(dynamic, DT_HASH, s1->dynsym->hash->sh_addr);
    put_dt(dynamic, DT_STRTAB, dyninf->dynstr->sh_addr);
    put_dt(dynamic, DT_SYMTAB, s1->dynsym->sh_addr);
    put_dt(dynamic, DT_STRSZ, dyninf->dynstr->data_offset);
    put_dt(dynamic, DT_SYMENT, �󳤶�(ElfW(Sym)));
#�� PTR_SIZE == 8
    put_dt(dynamic, DT_RELA, dyninf->rel_addr);
    put_dt(dynamic, DT_RELASZ, dyninf->rel_size);
    put_dt(dynamic, DT_RELAENT, �󳤶�(ElfW_Rel));
#��
#�� �Ѷ���(__FreeBSD__) || �Ѷ���(__FreeBSD_kernel__)
    put_dt(dynamic, DT_PLTGOT, s1->got->sh_addr);
    put_dt(dynamic, DT_PLTRELSZ, dyninf->rel_size);
    put_dt(dynamic, DT_JMPREL, dyninf->rel_addr);
    put_dt(dynamic, DT_PLTREL, DT_REL);
    put_dt(dynamic, DT_REL, dyninf->bss_addr);
    put_dt(dynamic, DT_RELSZ, dyninf->bss_size);
#��
    put_dt(dynamic, DT_REL, dyninf->rel_addr);
    put_dt(dynamic, DT_RELSZ, dyninf->rel_size);
    put_dt(dynamic, DT_RELENT, �󳤶�(ElfW_Rel));
#����
#����
    �� (s1->do_debug)
        put_dt(dynamic, DT_DEBUG, 0);
    put_dt(dynamic, DT_NULL, 0);
}

/* Relocate remaining sections and symbols (that is those not related to
   dynamic linking) */
��̬ �� final_sections_reloc(TCCState *s1)
{
    �� i;
    Section *s;

    relocate_syms(s1, s1->symtab, 0);

    �� (s1->nb_errors != 0)
        ���� -1;

    /* relocate sections */
    /* XXX: ignore sections with allocated relocations ? */
    ����(i = 1; i < s1->nb_sections; i++) {
        s = s1->sections[i];
#�� �Ѷ���(TCC_TARGET_I386) || �Ѷ���(TCC_MUSL)
        �� (s->reloc && s != s1->got && (s->sh_flags & SHF_ALLOC)) //gr
        /* On X86 gdb 7.3 works in any case but gdb 6.6 will crash if SHF_ALLOC
        checking is removed */
#��
        �� (s->reloc && s != s1->got)
        /* On X86_64 gdb 7.3 will crash if SHF_ALLOC checking is present */
#����
            relocate_section(s1, s);
    }

    /* relocate relocation entries if the relocation tables are
       allocated in the executable */
    ����(i = 1; i < s1->nb_sections; i++) {
        s = s1->sections[i];
        �� ((s->sh_flags & SHF_ALLOC) &&
            s->sh_type == SHT_RELX) {
            relocate_rel(s1, s);
        }
    }
    ���� 0;
}

/* Create an ELF file on disk.
   This function handle ELF specific layout requirements */
��̬ �� tcc_output_elf(TCCState *s1, FILE *f, �� phnum, ElfW(Phdr) *phdr,
                           �� file_offset, �� *sec_order)
{
    �� i, shnum, offset, size, file_type;
    Section *s;
    ElfW(Ehdr) ehdr;
    ElfW(Shdr) shdr, *sh;

    file_type = s1->output_type;
    shnum = s1->nb_sections;

    memset(&ehdr, 0, �󳤶�(ehdr));

    �� (phnum > 0) {
        ehdr.e_phentsize = �󳤶�(ElfW(Phdr));
        ehdr.e_phnum = phnum;
        ehdr.e_phoff = �󳤶�(ElfW(Ehdr));
    }

    /* align to 4 */
    file_offset = (file_offset + 3) & -4;

    /* fill header */
    ehdr.e_ident[0] = ELFMAG0;
    ehdr.e_ident[1] = ELFMAG1;
    ehdr.e_ident[2] = ELFMAG2;
    ehdr.e_ident[3] = ELFMAG3;
    ehdr.e_ident[4] = ELFCLASSW;
    ehdr.e_ident[5] = ELFDATA2LSB;
    ehdr.e_ident[6] = EV_CURRENT;
#�� �Ѷ���(__FreeBSD__) || �Ѷ���(__FreeBSD_kernel__)
    ehdr.e_ident[EI_OSABI] = ELFOSABI_FREEBSD;
#����
#�綨�� TCC_TARGET_ARM
#�綨�� TCC_ARM_EABI
    ehdr.e_ident[EI_OSABI] = 0;
    ehdr.e_flags = EF_ARM_EABI_VER4;
    �� (file_type == TCC_OUTPUT_EXE || file_type == TCC_OUTPUT_DLL)
        ehdr.e_flags |= EF_ARM_HASENTRY;
    �� (s1->float_abi == ARM_HARD_FLOAT)
        ehdr.e_flags |= EF_ARM_VFP_FLOAT;
    ��
        ehdr.e_flags |= EF_ARM_SOFT_FLOAT;
#��
    ehdr.e_ident[EI_OSABI] = ELFOSABI_ARM;
#����
#����
    ת��(file_type) {
    ȱʡ:
    ���� TCC_OUTPUT_EXE:
        ehdr.e_type = ET_EXEC;
        ehdr.e_entry = get_elf_sym_addr(s1, "_start", 1);
        ����;
    ���� TCC_OUTPUT_DLL:
        ehdr.e_type = ET_DYN;
        ehdr.e_entry = text_section->sh_addr; /* XXX: is it correct ? */
        ����;
    ���� TCC_OUTPUT_OBJ:
        ehdr.e_type = ET_REL;
        ����;
    }
    ehdr.e_machine = EM_TCC_TARGET;
    ehdr.e_version = EV_CURRENT;
    ehdr.e_shoff = file_offset;
    ehdr.e_ehsize = �󳤶�(ElfW(Ehdr));
    ehdr.e_shentsize = �󳤶�(ElfW(Shdr));
    ehdr.e_shnum = shnum;
    ehdr.e_shstrndx = shnum - 1;

    fwrite(&ehdr, 1, �󳤶�(ElfW(Ehdr)), f);
    fwrite(phdr, 1, phnum * �󳤶�(ElfW(Phdr)), f);
    offset = �󳤶�(ElfW(Ehdr)) + phnum * �󳤶�(ElfW(Phdr));

    sort_syms(s1, symtab_section);
    ����(i = 1; i < s1->nb_sections; i++) {
        s = s1->sections[sec_order[i]];
        �� (s->sh_type != SHT_NOBITS) {
            �� (offset < s->sh_offset) {
                fputc(0, f);
                offset++;
            }
            size = s->sh_size;
            �� (size)
                fwrite(s->data, 1, size, f);
            offset += size;
        }
    }

    /* output section headers */
    �� (offset < ehdr.e_shoff) {
        fputc(0, f);
        offset++;
    }

    ����(i = 0; i < s1->nb_sections; i++) {
        sh = &shdr;
        memset(sh, 0, �󳤶�(ElfW(Shdr)));
        s = s1->sections[i];
        �� (s) {
            sh->sh_name = s->sh_name;
            sh->sh_type = s->sh_type;
            sh->sh_flags = s->sh_flags;
            sh->sh_entsize = s->sh_entsize;
            sh->sh_info = s->sh_info;
            �� (s->link)
                sh->sh_link = s->link->sh_num;
            sh->sh_addralign = s->sh_addralign;
            sh->sh_addr = s->sh_addr;
            sh->sh_offset = s->sh_offset;
            sh->sh_size = s->sh_size;
        }
        fwrite(sh, 1, �󳤶�(ElfW(Shdr)), f);
    }
}

/* Write an elf, coff or "binary" file */
��̬ �� tcc_write_elf_file(TCCState *s1, ���� �� *filename, �� phnum,
                              ElfW(Phdr) *phdr, �� file_offset, �� *sec_order)
{
    �� fd, mode, file_type;
    FILE *f;

    file_type = s1->output_type;
    �� (file_type == TCC_OUTPUT_OBJ)
        mode = 0666;
    ��
        mode = 0777;
    unlink(filename);
    fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY, mode);
    �� (fd < 0) {
        tcc_error_noabort("could not write '%s'", filename);
        ���� -1;
    }
    f = fdopen(fd, "wb");
    �� (s1->verbose)
        printf("<- %s\n", filename);

#�綨�� TCC_TARGET_COFF
    �� (s1->output_format == TCC_OUTPUT_FORMAT_COFF)
        tcc_output_coff(s1, f);
    ��
#����
    �� (s1->output_format == TCC_OUTPUT_FORMAT_ELF)
        tcc_output_elf(s1, f, phnum, phdr, file_offset, sec_order);
    ��
        tcc_output_binary(s1, f, sec_order);
    fclose(f);

    ���� 0;
}

/* Sort section headers by assigned sh_addr, remove sections
   that we aren't going to output.  */
��̬ �� tidy_section_headers(TCCState *s1, �� *sec_order)
{
    �� i, nnew, l, *backmap;
    Section **snew, *s;
    ElfW(Sym) *sym;

    snew = tcc_malloc(s1->nb_sections * �󳤶�(snew[0]));
    backmap = tcc_malloc(s1->nb_sections * �󳤶�(backmap[0]));
    ���� (i = 0, nnew = 0, l = s1->nb_sections; i < s1->nb_sections; i++) {
        s = s1->sections[sec_order[i]];
        �� (!i || s->sh_name) {
            backmap[sec_order[i]] = nnew;
            snew[nnew] = s;
            ++nnew;
        } �� {
            backmap[sec_order[i]] = 0;
            snew[--l] = s;
        }
    }
    ���� (i = 0; i < nnew; i++) {
        s = snew[i];
        �� (s) {
            s->sh_num = i;
            �� (s->sh_type == SHT_RELX)
                s->sh_info = backmap[s->sh_info];
        }
    }

    for_each_elem(symtab_section, 1, sym, ElfW(Sym))
        �� (sym->st_shndx != SHN_UNDEF && sym->st_shndx < SHN_LORESERVE)
            sym->st_shndx = backmap[sym->st_shndx];
    ��( !s1->static_link ) {
        for_each_elem(s1->dynsym, 1, sym, ElfW(Sym))
            �� (sym->st_shndx != SHN_UNDEF && sym->st_shndx < SHN_LORESERVE)
                sym->st_shndx = backmap[sym->st_shndx];
    }
    ���� (i = 0; i < s1->nb_sections; i++)
        sec_order[i] = i;
    tcc_free(s1->sections);
    s1->sections = snew;
    s1->nb_sections = nnew;
    tcc_free(backmap);
}

/* Output an elf, coff or binary file */
/* XXX: suppress unneeded sections */
��̬ �� elf_output_file(TCCState *s1, ���� �� *filename)
{
    �� i, ret, phnum, shnum, file_type, file_offset, *sec_order;
    �ṹ dyn_inf dyninf = {0};
    ElfW(Phdr) *phdr;
    ElfW(Sym) *sym;
    Section *strsec, *interp, *dynamic, *dynstr;

    file_type = s1->output_type;
    s1->nb_errors = 0;

    /* if linking, also link in runtime libraries (libc, libgcc, etc.) */
    �� (file_type != TCC_OUTPUT_OBJ) {
        tcc_add_runtime(s1);
    }

    phdr = NULL;
    sec_order = NULL;
    interp = dynamic = dynstr = NULL; /* avoid warning */

    �� (file_type != TCC_OUTPUT_OBJ) {
        relocate_common_syms();

        tcc_add_linker_symbols(s1);

        �� (!s1->static_link) {
            �� (file_type == TCC_OUTPUT_EXE) {
                �� *ptr;
                /* allow override the dynamic loader */
                ���� �� *elfint = getenv("LD_SO");
                �� (elfint == NULL)
                    elfint = DEFAULT_ELFINTERP(s1);
                /* add interpreter section only if executable */
                interp = new_section(s1, ".interp", SHT_PROGBITS, SHF_ALLOC);
                interp->sh_addralign = 1;
                ptr = section_ptr_add(interp, 1 + strlen(elfint));
                strcpy(ptr, elfint);
            }

            /* add dynamic symbol table */
            s1->dynsym = new_symtab(s1, ".dynsym", SHT_DYNSYM, SHF_ALLOC,
                                    ".dynstr",
                                    ".hash", SHF_ALLOC);
            dynstr = s1->dynsym->link;

            /* add dynamic section */
            dynamic = new_section(s1, ".dynamic", SHT_DYNAMIC,
                                  SHF_ALLOC | SHF_WRITE);
            dynamic->link = dynstr;
            dynamic->sh_entsize = �󳤶�(ElfW(Dyn));

            build_got(s1);

            �� (file_type == TCC_OUTPUT_EXE) {
                bind_exe_dynsyms(s1);

                �� (s1->nb_errors) {
                    ret = -1;
                    ��ת the_end;
                }

                bind_libs_dynsyms(s1);
            } �� /* shared library case: simply export all global symbols */
                export_global_syms(s1);

            build_got_entries(s1);

            /* add a list of needed dlls */
            ����(i = 0; i < s1->nb_loaded_dlls; i++) {
                DLLReference *dllref = s1->loaded_dlls[i];
                �� (dllref->level == 0)
                    put_dt(dynamic, DT_NEEDED, put_elf_str(dynstr, dllref->name));
            }

            �� (s1->rpath)
                put_dt(dynamic, s1->enable_new_dtags ? DT_RUNPATH : DT_RPATH,
                    put_elf_str(dynstr, s1->rpath));

            /* XXX: currently, since we do not handle PIC code, we
               must relocate the readonly segments */
            �� (file_type == TCC_OUTPUT_DLL) {
                �� (s1->soname)
                    put_dt(dynamic, DT_SONAME, put_elf_str(dynstr, s1->soname));
                put_dt(dynamic, DT_TEXTREL, 0);
            }

            �� (s1->symbolic)
                put_dt(dynamic, DT_SYMBOLIC, 0);

            /* add necessary space for other entries */
            dyninf.dyn_rel_off = dynamic->data_offset;
            dynamic->data_offset += �󳤶�(ElfW(Dyn)) * EXTRA_RELITEMS;
        } �� {
            /* still need to build got entries in case of static link */
            build_got_entries(s1);
        }
    }

    /* we add a section for symbols */
    strsec = new_section(s1, ".shstrtab", SHT_STRTAB, 0);
    put_elf_str(strsec, "");

    /* compute number of sections */
    shnum = s1->nb_sections;

    /* this array is used to reorder sections in the output file */
    sec_order = tcc_malloc(�󳤶�(��) * shnum);
    sec_order[0] = 0;

    /* compute number of program headers */
    ת��(file_type) {
    ȱʡ:
    ���� TCC_OUTPUT_OBJ:
        phnum = 0;
        ����;
    ���� TCC_OUTPUT_EXE:
        �� (!s1->static_link)
            phnum = 4 + HAVE_PHDR;
        ��
            phnum = 2;
        ����;
    ���� TCC_OUTPUT_DLL:
        phnum = 3;
        ����;
    }

    /* Allocate strings for section names */
    alloc_sec_names(s1, file_type, strsec);

    /* allocate program segment headers */
    phdr = tcc_mallocz(phnum * �󳤶�(ElfW(Phdr)));

    /* compute section to program header mapping */
    file_offset = layout_sections(s1, phdr, phnum, interp, strsec, &dyninf,
                                  sec_order);

    /* Fill remaining program header and finalize relocation related to dynamic
       linking. */
    �� (phnum > 0) {
        fill_unloadable_phdr(phdr, phnum, interp, dynamic);
        �� (dynamic) {
            dyninf.dynamic = dynamic;
            dyninf.dynstr = dynstr;

            fill_dynamic(s1, &dyninf);

            /* put in GOT the dynamic section address and relocate PLT */
            write32le(s1->got->data, dynamic->sh_addr);
            �� (file_type == TCC_OUTPUT_EXE
                || (RELOCATE_DLLPLT && file_type == TCC_OUTPUT_DLL))
                relocate_plt(s1);

            /* relocate symbols in .dynsym now that final addresses are known */
            for_each_elem(s1->dynsym, 1, sym, ElfW(Sym)) {
                �� (sym->st_shndx != SHN_UNDEF && sym->st_shndx < SHN_LORESERVE) {
                    /* do symbol relocation */
                    sym->st_value += s1->sections[sym->st_shndx]->sh_addr;
                }
            }
        }
    }

    /* if building executable or DLL, then relocate each section
       except the GOT which is already relocated */
    �� (file_type != TCC_OUTPUT_OBJ) {
        ret = final_sections_reloc(s1);
        �� (ret)
            ��ת the_end;
        tidy_section_headers(s1, sec_order);
    }

    /* Perform relocation to GOT or PLT entries */
    �� (file_type == TCC_OUTPUT_EXE && s1->static_link)
        fill_got(s1);
    �� �� (s1->got)
        fill_local_got_entries(s1);

    /* Create the ELF file with name 'filename' */
    ret = tcc_write_elf_file(s1, filename, phnum, phdr, file_offset, sec_order);
    s1->nb_sections = shnum;
 the_end:
    tcc_free(sec_order);
    tcc_free(phdr);
    ���� ret;
}

LIBTCCAPI �� tcc_output_file(TCCState *s, ���� �� *filename)
{
    �� ret;
#�綨�� TCC_TARGET_PE
    �� (s->output_type != TCC_OUTPUT_OBJ) {
        ret = pe_output_file(s, filename);
    } ��
#����
        ret = elf_output_file(s, filename);
    ���� ret;
}

��̬ �� *load_data(�� fd, �޷� �� file_offset, �޷� �� size)
{
    �� *data;

    data = tcc_malloc(size);
    lseek(fd, file_offset, SEEK_SET);
    read(fd, data, size);
    ���� data;
}

���Ͷ��� �ṹ SectionMergeInfo {
    Section *s;            /* corresponding existing section */
    �޷� �� offset;  /* offset of the new section in the existing section */
    uint8_t new_section;       /* true if section 's' was added */
    uint8_t link_once;         /* true if link once section */
} SectionMergeInfo;

ST_FUNC �� tcc_object_type(�� fd, ElfW(Ehdr) *h)
{
    �� size = read(fd, h, �󳤶� *h);
    �� (size == �󳤶� *h && 0 == memcmp(h, ELFMAG, 4)) {
        �� (h->e_type == ET_REL)
            ���� AFF_BINTYPE_REL;
        �� (h->e_type == ET_DYN)
            ���� AFF_BINTYPE_DYN;
    } �� �� (size >= 8) {
        �� (0 == memcmp(h, ARMAG, 8))
            ���� AFF_BINTYPE_AR;
#�綨�� TCC_TARGET_COFF
        �� (((�ṹ filehdr*)h)->f_magic == COFF_C67_MAGIC)
            ���� AFF_BINTYPE_C67;
#����
    }
    ���� 0;
}

/* load an object file and merge it with current files */
/* XXX: handle correctly stab (debug) info */
ST_FUNC �� tcc_load_object_file(TCCState *s1,
                                �� fd, �޷� �� file_offset)
{
    ElfW(Ehdr) ehdr;
    ElfW(Shdr) *shdr, *sh;
    �� size, i, j, offset, offseti, nb_syms, sym_index, ret, seencompressed;
    �޷� �� *strsec, *strtab;
    �� *old_to_new_syms;
    �� *sh_name, *name;
    SectionMergeInfo *sm_table, *sm;
    ElfW(Sym) *sym, *symtab;
    ElfW_Rel *rel;
    Section *s;

    �� stab_index;
    �� stabstr_index;

    stab_index = stabstr_index = 0;

    lseek(fd, file_offset, SEEK_SET);
    �� (tcc_object_type(fd, &ehdr) != AFF_BINTYPE_REL)
        ��ת fail1;
    /* test CPU specific stuff */
    �� (ehdr.e_ident[5] != ELFDATA2LSB ||
        ehdr.e_machine != EM_TCC_TARGET) {
    fail1:
        tcc_error_noabort("invalid object file");
        ���� -1;
    }
    /* read sections */
    shdr = load_data(fd, file_offset + ehdr.e_shoff,
                     �󳤶�(ElfW(Shdr)) * ehdr.e_shnum);
    sm_table = tcc_mallocz(�󳤶�(SectionMergeInfo) * ehdr.e_shnum);

    /* load section names */
    sh = &shdr[ehdr.e_shstrndx];
    strsec = load_data(fd, file_offset + sh->sh_offset, sh->sh_size);

    /* load symtab and strtab */
    old_to_new_syms = NULL;
    symtab = NULL;
    strtab = NULL;
    nb_syms = 0;
    seencompressed = 0;
    ����(i = 1; i < ehdr.e_shnum; i++) {
        sh = &shdr[i];
        �� (sh->sh_type == SHT_SYMTAB) {
            �� (symtab) {
                tcc_error_noabort("object must contain only one symtab");
            fail:
                ret = -1;
                ��ת the_end;
            }
            nb_syms = sh->sh_size / �󳤶�(ElfW(Sym));
            symtab = load_data(fd, file_offset + sh->sh_offset, sh->sh_size);
            sm_table[i].s = symtab_section;

            /* now load strtab */
            sh = &shdr[sh->sh_link];
            strtab = load_data(fd, file_offset + sh->sh_offset, sh->sh_size);
        }
        �� (sh->sh_flags & SHF_COMPRESSED)
            seencompressed = 1;
    }

    /* now examine each section and try to merge its content with the
       ones in memory */
    ����(i = 1; i < ehdr.e_shnum; i++) {
        /* no need to examine section name strtab */
        �� (i == ehdr.e_shstrndx)
            ����;
        sh = &shdr[i];
        sh_name = (�� *) strsec + sh->sh_name;
        /* ignore sections types we do not handle */
        �� (sh->sh_type != SHT_PROGBITS &&
            sh->sh_type != SHT_RELX &&
#�綨�� TCC_ARM_EABI
            sh->sh_type != SHT_ARM_EXIDX &&
#����
            sh->sh_type != SHT_NOBITS &&
            sh->sh_type != SHT_PREINIT_ARRAY &&
            sh->sh_type != SHT_INIT_ARRAY &&
            sh->sh_type != SHT_FINI_ARRAY &&
            strcmp(sh_name, ".stabstr")
            )
            ����;
        �� (seencompressed
            && (!strncmp(sh_name, ".debug_", �󳤶�(".debug_")-1)
                || (sh->sh_type == SHT_RELX
                    && !strncmp((��*)strsec + shdr[sh->sh_info].sh_name,
                                ".debug_", �󳤶�(".debug_")-1))))
          ����;
        �� (sh->sh_addralign < 1)
            sh->sh_addralign = 1;
        /* find corresponding section, if any */
        ����(j = 1; j < s1->nb_sections;j++) {
            s = s1->sections[j];
            �� (!strcmp(s->name, sh_name)) {
                �� (!strncmp(sh_name, ".gnu.linkonce",
                             �󳤶�(".gnu.linkonce") - 1)) {
                    /* if a 'linkonce' section is already present, we
                       do not add it again. It is a little tricky as
                       symbols can still be defined in
                       it. */
                    sm_table[i].link_once = 1;
                    ��ת next;
                } �� {
                    ��ת found;
                }
            }
        }
        /* not found: create new section */
        s = new_section(s1, sh_name, sh->sh_type, sh->sh_flags & ~SHF_GROUP);
        /* take as much info as possible from the section. sh_link and
           sh_info will be updated later */
        s->sh_addralign = sh->sh_addralign;
        s->sh_entsize = sh->sh_entsize;
        sm_table[i].new_section = 1;
    found:
        �� (sh->sh_type != s->sh_type) {
            tcc_error_noabort("invalid section type");
            ��ת fail;
        }

        /* align start of section */
        offset = s->data_offset;

        �� (0 == strcmp(sh_name, ".stab")) {
            stab_index = i;
            ��ת no_align;
        }
        �� (0 == strcmp(sh_name, ".stabstr")) {
            stabstr_index = i;
            ��ת no_align;
        }

        size = sh->sh_addralign - 1;
        offset = (offset + size) & ~size;
        �� (sh->sh_addralign > s->sh_addralign)
            s->sh_addralign = sh->sh_addralign;
        s->data_offset = offset;
    no_align:
        sm_table[i].offset = offset;
        sm_table[i].s = s;
        /* concatenate sections */
        size = sh->sh_size;
        �� (sh->sh_type != SHT_NOBITS) {
            �޷� �� *ptr;
            lseek(fd, file_offset + sh->sh_offset, SEEK_SET);
            ptr = section_ptr_add(s, size);
            read(fd, ptr, size);
        } �� {
            s->data_offset += size;
        }
    next: ;
    }

    /* gr relocate stab strings */
    �� (stab_index && stabstr_index) {
        Stab_Sym *a, *b;
        �޷� o;
        s = sm_table[stab_index].s;
        a = (Stab_Sym *)(s->data + sm_table[stab_index].offset);
        b = (Stab_Sym *)(s->data + s->data_offset);
        o = sm_table[stabstr_index].offset;
        �� (a < b)
            a->n_strx += o, a++;
    }

    /* second short pass to update sh_link and sh_info fields of new
       sections */
    ����(i = 1; i < ehdr.e_shnum; i++) {
        s = sm_table[i].s;
        �� (!s || !sm_table[i].new_section)
            ����;
        sh = &shdr[i];
        �� (sh->sh_link > 0)
            s->link = sm_table[sh->sh_link].s;
        �� (sh->sh_type == SHT_RELX) {
            s->sh_info = sm_table[sh->sh_info].s->sh_num;
            /* update backward link */
            s1->sections[s->sh_info]->reloc = s;
        }
    }
    sm = sm_table;

    /* resolve symbols */
    old_to_new_syms = tcc_mallocz(nb_syms * �󳤶�(��));

    sym = symtab + 1;
    ����(i = 1; i < nb_syms; i++, sym++) {
        �� (sym->st_shndx != SHN_UNDEF &&
            sym->st_shndx < SHN_LORESERVE) {
            sm = &sm_table[sym->st_shndx];
            �� (sm->link_once) {
                /* if a symbol is in a link once section, we use the
                   already defined symbol. It is very important to get
                   correct relocations */
                �� (ELFW(ST_BIND)(sym->st_info) != STB_LOCAL) {
                    name = (�� *) strtab + sym->st_name;
                    sym_index = find_elf_sym(symtab_section, name);
                    �� (sym_index)
                        old_to_new_syms[i] = sym_index;
                }
                ����;
            }
            /* if no corresponding section added, no need to add symbol */
            �� (!sm->s)
                ����;
            /* convert section number */
            sym->st_shndx = sm->s->sh_num;
            /* offset value */
            sym->st_value += sm->offset;
        }
        /* add symbol */
        name = (�� *) strtab + sym->st_name;
        sym_index = set_elf_sym(symtab_section, sym->st_value, sym->st_size,
                                sym->st_info, sym->st_other,
                                sym->st_shndx, name);
        old_to_new_syms[i] = sym_index;
    }

    /* third pass to patch relocation entries */
    ����(i = 1; i < ehdr.e_shnum; i++) {
        s = sm_table[i].s;
        �� (!s)
            ����;
        sh = &shdr[i];
        offset = sm_table[i].offset;
        ת��(s->sh_type) {
        ���� SHT_RELX:
            /* take relocation offset information */
            offseti = sm_table[sh->sh_info].offset;
            for_each_elem(s, (offset / �󳤶�(*rel)), rel, ElfW_Rel) {
                �� type;
                �޷� sym_index;
                /* convert symbol index */
                type = ELFW(R_TYPE)(rel->r_info);
                sym_index = ELFW(R_SYM)(rel->r_info);
                /* NOTE: only one symtab assumed */
                �� (sym_index >= nb_syms)
                    ��ת invalid_reloc;
                sym_index = old_to_new_syms[sym_index];
                /* ignore link_once in rel section. */
                �� (!sym_index && !sm->link_once
#�綨�� TCC_TARGET_ARM
                    && type != R_ARM_V4BX
#����
                   ) {
                invalid_reloc:
                    tcc_error_noabort("Invalid relocation entry [%2d] '%s' @ %.8x",
                        i, strsec + sh->sh_name, rel->r_offset);
                    ��ת fail;
                }
                rel->r_info = ELFW(R_INFO)(sym_index, type);
                /* offset the relocation offset */
                rel->r_offset += offseti;
#�綨�� TCC_TARGET_ARM
                /* Jumps and branches from a Thumb code to a PLT entry need
                   special handling since PLT entries are ARM code.
                   Unconditional bl instructions referencing PLT entries are
                   handled by converting these instructions into blx
                   instructions. Other case of instructions referencing a PLT
                   entry require to add a Thumb stub before the PLT entry to
                   switch to ARM mode. We set bit plt_thumb_stub of the
                   attribute of a symbol to indicate such a case. */
                �� (type == R_ARM_THM_JUMP24)
                    get_sym_attr(s1, sym_index, 1)->plt_thumb_stub = 1;
#����
            }
            ����;
        ȱʡ:
            ����;
        }
    }

    ret = 0;
 the_end:
    tcc_free(symtab);
    tcc_free(strtab);
    tcc_free(old_to_new_syms);
    tcc_free(sm_table);
    tcc_free(strsec);
    tcc_free(shdr);
    ���� ret;
}

���Ͷ��� �ṹ ArchiveHeader {
    �� ar_name[16];           /* name of this member */
    �� ar_date[12];           /* file mtime */
    �� ar_uid[6];             /* owner uid; printed as decimal */
    �� ar_gid[6];             /* owner gid; printed as decimal */
    �� ar_mode[8];            /* file mode, printed as octal   */
    �� ar_size[10];           /* file size, printed as decimal */
    �� ar_fmag[2];            /* should contain ARFMAG */
} ArchiveHeader;

��̬ �� get_be32(���� uint8_t *b)
{
    ���� b[3] | (b[2] << 8) | (b[1] << 16) | (b[0] << 24);
}

��̬ �� get_be64(���� uint8_t *b)
{
  �� �� ret = get_be32(b);
  ret = (ret << 32) | (�޷�)get_be32(b+4);
  ���� (��)ret;
}

/* load only the objects which resolve undefined symbols */
��̬ �� tcc_load_alacarte(TCCState *s1, �� fd, �� size, �� entrysize)
{
    �� i, bound, nsyms, sym_index, off, ret;
    uint8_t *data;
    ���� �� *ar_names, *p;
    ���� uint8_t *ar_index;
    ElfW(Sym) *sym;

    data = tcc_malloc(size);
    �� (read(fd, data, size) != size)
        ��ת fail;
    nsyms = entrysize == 4 ? get_be32(data) : get_be64(data);
    ar_index = data + entrysize;
    ar_names = (�� *) ar_index + nsyms * entrysize;

    ���� {
        bound = 0;
        ����(p = ar_names, i = 0; i < nsyms; i++, p += strlen(p)+1) {
            sym_index = find_elf_sym(symtab_section, p);
            ��(sym_index) {
                sym = &((ElfW(Sym) *)symtab_section->data)[sym_index];
                ��(sym->st_shndx == SHN_UNDEF) {
                    off = (entrysize == 4
                           ? get_be32(ar_index + i * 4)
                           : get_be64(ar_index + i * 8))
                          + �󳤶�(ArchiveHeader);
                    ++bound;
                    ��(tcc_load_object_file(s1, fd, off) < 0) {
                    fail:
                        ret = -1;
                        ��ת the_end;
                    }
                }
            }
        }
    } ��(bound);
    ret = 0;
 the_end:
    tcc_free(data);
    ���� ret;
}

/* load a '.a' file */
ST_FUNC �� tcc_load_archive(TCCState *s1, �� fd)
{
    ArchiveHeader hdr;
    �� ar_size[11];
    �� ar_name[17];
    �� magic[8];
    �� size, len, i;
    �޷� �� file_offset;

    /* skip magic which was already checked */
    read(fd, magic, �󳤶�(magic));

    ����(;;) {
        len = read(fd, &hdr, �󳤶�(hdr));
        �� (len == 0)
            ����;
        �� (len != �󳤶�(hdr)) {
            tcc_error_noabort("invalid archive");
            ���� -1;
        }
        memcpy(ar_size, hdr.ar_size, �󳤶�(hdr.ar_size));
        ar_size[�󳤶�(hdr.ar_size)] = '\0';
        size = strtol(ar_size, NULL, 0);
        memcpy(ar_name, hdr.ar_name, �󳤶�(hdr.ar_name));
        ����(i = �󳤶�(hdr.ar_name) - 1; i >= 0; i--) {
            �� (ar_name[i] != ' ')
                ����;
        }
        ar_name[i + 1] = '\0';
        file_offset = lseek(fd, 0, SEEK_CUR);
        /* align to even */
        size = (size + 1) & ~1;
        �� (!strcmp(ar_name, "/")) {
            /* coff symbol table : we handle it */
            ��(s1->alacarte_link)
                ���� tcc_load_alacarte(s1, fd, size, 4);
        } �� �� (!strcmp(ar_name, "/SYM64/")) {
            ��(s1->alacarte_link)
                ���� tcc_load_alacarte(s1, fd, size, 8);
        } �� {
            ElfW(Ehdr) ehdr;
            �� (tcc_object_type(fd, &ehdr) == AFF_BINTYPE_REL) {
                �� (tcc_load_object_file(s1, fd, file_offset) < 0)
                    ���� -1;
            }
        }
        lseek(fd, file_offset + size, SEEK_SET);
    }
    ���� 0;
}

#��δ���� TCC_TARGET_PE
/* load a DLL and all referenced DLLs. 'level = 0' means that the DLL
   is referenced by the user (so it should be added as DT_NEEDED in
   the generated ELF file) */
ST_FUNC �� tcc_load_dll(TCCState *s1, �� fd, ���� �� *filename, �� level)
{
    ElfW(Ehdr) ehdr;
    ElfW(Shdr) *shdr, *sh, *sh1;
    �� i, j, nb_syms, nb_dts, sym_bind, ret;
    ElfW(Sym) *sym, *dynsym;
    ElfW(Dyn) *dt, *dynamic;
    �޷� �� *dynstr;
    ���� �� *name, *soname;
    DLLReference *dllref;

    read(fd, &ehdr, �󳤶�(ehdr));

    /* test CPU specific stuff */
    �� (ehdr.e_ident[5] != ELFDATA2LSB ||
        ehdr.e_machine != EM_TCC_TARGET) {
        tcc_error_noabort("bad architecture");
        ���� -1;
    }

    /* read sections */
    shdr = load_data(fd, ehdr.e_shoff, �󳤶�(ElfW(Shdr)) * ehdr.e_shnum);

    /* load dynamic section and dynamic symbols */
    nb_syms = 0;
    nb_dts = 0;
    dynamic = NULL;
    dynsym = NULL; /* avoid warning */
    dynstr = NULL; /* avoid warning */
    ����(i = 0, sh = shdr; i < ehdr.e_shnum; i++, sh++) {
        ת��(sh->sh_type) {
        ���� SHT_DYNAMIC:
            nb_dts = sh->sh_size / �󳤶�(ElfW(Dyn));
            dynamic = load_data(fd, sh->sh_offset, sh->sh_size);
            ����;
        ���� SHT_DYNSYM:
            nb_syms = sh->sh_size / �󳤶�(ElfW(Sym));
            dynsym = load_data(fd, sh->sh_offset, sh->sh_size);
            sh1 = &shdr[sh->sh_link];
            dynstr = load_data(fd, sh1->sh_offset, sh1->sh_size);
            ����;
        ȱʡ:
            ����;
        }
    }

    /* compute the real library name */
    soname = tcc_basename(filename);

    ����(i = 0, dt = dynamic; i < nb_dts; i++, dt++) {
        �� (dt->d_tag == DT_SONAME) {
            soname = (�� *) dynstr + dt->d_un.d_val;
        }
    }

    /* if the dll is already loaded, do not load it */
    ����(i = 0; i < s1->nb_loaded_dlls; i++) {
        dllref = s1->loaded_dlls[i];
        �� (!strcmp(soname, dllref->name)) {
            /* but update level if needed */
            �� (level < dllref->level)
                dllref->level = level;
            ret = 0;
            ��ת the_end;
        }
    }

    /* add the dll and its level */
    dllref = tcc_mallocz(�󳤶�(DLLReference) + strlen(soname));
    dllref->level = level;
    strcpy(dllref->name, soname);
    dynarray_add(&s1->loaded_dlls, &s1->nb_loaded_dlls, dllref);

    /* add dynamic symbols in dynsym_section */
    ����(i = 1, sym = dynsym + 1; i < nb_syms; i++, sym++) {
        sym_bind = ELFW(ST_BIND)(sym->st_info);
        �� (sym_bind == STB_LOCAL)
            ����;
        name = (�� *) dynstr + sym->st_name;
        set_elf_sym(s1->dynsymtab_section, sym->st_value, sym->st_size,
                    sym->st_info, sym->st_other, sym->st_shndx, name);
    }

    /* load all referenced DLLs */
    ����(i = 0, dt = dynamic; i < nb_dts; i++, dt++) {
        ת��(dt->d_tag) {
        ���� DT_NEEDED:
            name = (�� *) dynstr + dt->d_un.d_val;
            ����(j = 0; j < s1->nb_loaded_dlls; j++) {
                dllref = s1->loaded_dlls[j];
                �� (!strcmp(name, dllref->name))
                    ��ת already_loaded;
            }
            �� (tcc_add_dll(s1, name, AFF_REFERENCED_DLL) < 0) {
                tcc_error_noabort("referenced dll '%s' not found", name);
                ret = -1;
                ��ת the_end;
            }
        already_loaded:
            ����;
        }
    }
    ret = 0;
 the_end:
    tcc_free(dynstr);
    tcc_free(dynsym);
    tcc_free(dynamic);
    tcc_free(shdr);
    ���� ret;
}

#���� LD_TOK_NAME 256
#���� LD_TOK_EOF  (-1)

/* return next ld script token */
��̬ �� ld_next(TCCState *s1, �� *name, �� name_size)
{
    �� c;
    �� *q;

 redo:
    ת��(ch) {
    ���� ' ':
    ���� '\t':
    ���� '\f':
    ���� '\v':
    ���� '\r':
    ���� '\n':
        inp();
        ��ת redo;
    ���� '/':
        minp();
        �� (ch == '*') {
            file->buf_ptr = parse_comment(file->buf_ptr);
            ch = file->buf_ptr[0];
            ��ת redo;
        } �� {
            q = name;
            *q++ = '/';
            ��ת parse_name;
        }
        ����;
    ���� '\\':
        ch = handle_eob();
        �� (ch != '\\')
            ��ת redo;
        /* fall through */
    /* case 'a' ... 'z': */
    ���� 'a':
       ���� 'b':
       ���� 'c':
       ���� 'd':
       ���� 'e':
       ���� 'f':
       ���� 'g':
       ���� 'h':
       ���� 'i':
       ���� 'j':
       ���� 'k':
       ���� 'l':
       ���� 'm':
       ���� 'n':
       ���� 'o':
       ���� 'p':
       ���� 'q':
       ���� 'r':
       ���� 's':
       ���� 't':
       ���� 'u':
       ���� 'v':
       ���� 'w':
       ���� 'x':
       ���� 'y':
       ���� 'z':
    /* case 'A' ... 'z': */
    ���� 'A':
       ���� 'B':
       ���� 'C':
       ���� 'D':
       ���� 'E':
       ���� 'F':
       ���� 'G':
       ���� 'H':
       ���� 'I':
       ���� 'J':
       ���� 'K':
       ���� 'L':
       ���� 'M':
       ���� 'N':
       ���� 'O':
       ���� 'P':
       ���� 'Q':
       ���� 'R':
       ���� 'S':
       ���� 'T':
       ���� 'U':
       ���� 'V':
       ���� 'W':
       ���� 'X':
       ���� 'Y':
       ���� 'Z':
    ���� '_':
    ���� '.':
    ���� '$':
    ���� '~':
        q = name;
    parse_name:
        ����(;;) {
            �� (!((ch >= 'a' && ch <= 'z') ||
                  (ch >= 'A' && ch <= 'Z') ||
                  (ch >= '0' && ch <= '9') ||
                  strchr("/.-_+=$:\\,~", ch)))
                ����;
            �� ((q - name) < name_size - 1) {
                *q++ = ch;
            }
            minp();
        }
        *q = '\0';
        c = LD_TOK_NAME;
        ����;
    ���� CH_EOF:
        c = LD_TOK_EOF;
        ����;
    ȱʡ:
        c = ch;
        inp();
        ����;
    }
    ���� c;
}

��̬ �� ld_add_file(TCCState *s1, ���� �� filename[])
{
    �� (filename[0] == '/') {
        �� (CONFIG_SYSROOT[0] == '\0'
            && tcc_add_file_internal(s1, filename, AFF_TYPE_BIN) == 0)
            ���� 0;
        filename = tcc_basename(filename);
    }
    ���� tcc_add_dll(s1, filename, 0);
}

��̬ ���� �� new_undef_syms(��)
{
    �� ret = 0;
    ret = new_undef_sym;
    new_undef_sym = 0;
    ���� ret;
}

��̬ �� ld_add_file_list(TCCState *s1, ���� �� *cmd, �� as_needed)
{
    �� filename[1024], libname[1024];
    �� t, group, nblibs = 0, ret = 0;
    �� **libs = NULL;

    group = !strcmp(cmd, "GROUP");
    �� (!as_needed)
        new_undef_syms();
    t = ld_next(s1, filename, �󳤶�(filename));
    �� (t != '(')
        expect("(");
    t = ld_next(s1, filename, �󳤶�(filename));
    ����(;;) {
        libname[0] = '\0';
        �� (t == LD_TOK_EOF) {
            tcc_error_noabort("unexpected end of file");
            ret = -1;
            ��ת lib_parse_error;
        } �� �� (t == ')') {
            ����;
        } �� �� (t == '-') {
            t = ld_next(s1, filename, �󳤶�(filename));
            �� ((t != LD_TOK_NAME) || (filename[0] != 'l')) {
                tcc_error_noabort("library name expected");
                ret = -1;
                ��ת lib_parse_error;
            }
            pstrcpy(libname, �󳤶� libname, &filename[1]);
            �� (s1->static_link) {
                snprintf(filename, �󳤶� filename, "lib%s.a", libname);
            } �� {
                snprintf(filename, �󳤶� filename, "lib%s.so", libname);
            }
        } �� �� (t != LD_TOK_NAME) {
            tcc_error_noabort("filename expected");
            ret = -1;
            ��ת lib_parse_error;
        }
        �� (!strcmp(filename, "AS_NEEDED")) {
            ret = ld_add_file_list(s1, cmd, 1);
            �� (ret)
                ��ת lib_parse_error;
        } �� {
            /* TODO: Implement AS_NEEDED support. Ignore it for now */
            �� (!as_needed) {
                ret = ld_add_file(s1, filename);
                �� (ret)
                    ��ת lib_parse_error;
                �� (group) {
                    /* Add the filename *and* the libname to avoid future conversions */
                    dynarray_add(&libs, &nblibs, tcc_strdup(filename));
                    �� (libname[0] != '\0')
                        dynarray_add(&libs, &nblibs, tcc_strdup(libname));
                }
            }
        }
        t = ld_next(s1, filename, �󳤶�(filename));
        �� (t == ',') {
            t = ld_next(s1, filename, �󳤶�(filename));
        }
    }
    �� (group && !as_needed) {
        �� (new_undef_syms()) {
            �� i;

            ���� (i = 0; i < nblibs; i ++)
                ld_add_file(s1, libs[i]);
        }
    }
lib_parse_error:
    dynarray_reset(&libs, &nblibs);
    ���� ret;
}

/* interpret a subset of GNU ldscripts to handle the dummy libc.so
   files */
ST_FUNC �� tcc_load_ldscript(TCCState *s1)
{
    �� cmd[64];
    �� filename[1024];
    �� t, ret;

    ch = handle_eob();
    ����(;;) {
        t = ld_next(s1, cmd, �󳤶�(cmd));
        �� (t == LD_TOK_EOF)
            ���� 0;
        �� �� (t != LD_TOK_NAME)
            ���� -1;
        �� (!strcmp(cmd, "INPUT") ||
            !strcmp(cmd, "GROUP")) {
            ret = ld_add_file_list(s1, cmd, 0);
            �� (ret)
                ���� ret;
        } �� �� (!strcmp(cmd, "OUTPUT_FORMAT") ||
                   !strcmp(cmd, "TARGET")) {
            /* ignore some commands */
            t = ld_next(s1, cmd, �󳤶�(cmd));
            �� (t != '(')
                expect("(");
            ����(;;) {
                t = ld_next(s1, filename, �󳤶�(filename));
                �� (t == LD_TOK_EOF) {
                    tcc_error_noabort("unexpected end of file");
                    ���� -1;
                } �� �� (t == ')') {
                    ����;
                }
            }
        } �� {
            ���� -1;
        }
    }
    ���� 0;
}
#���� /* !TCC_TARGET_PE */
