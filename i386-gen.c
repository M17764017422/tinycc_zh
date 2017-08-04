/*
 *  X86 code generator for TCC
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

#�綨�� TARGET_DEFS_ONLY

/* number of available registers */
#���� NB_REGS         5
#���� NB_ASM_REGS     8
#���� CONFIG_TCC_ASM

/* a register can belong to several classes. The classes must be
   sorted from more general to more precise (see gv2() code which does
   assumptions on it). */
#���� RC_INT     0x0001 /* generic integer register */
#���� RC_FLOAT   0x0002 /* generic float register */
#���� RC_EAX     0x0004
#���� RC_ST0     0x0008 
#���� RC_ECX     0x0010
#���� RC_EDX     0x0020
#���� RC_EBX     0x0040

#���� RC_IRET    RC_EAX /* function return: integer register */
#���� RC_LRET    RC_EDX /* function return: second integer register */
#���� RC_FRET    RC_ST0 /* function return: float register */

/* pretty names for the registers */
ö�� {
    TREG_EAX = 0,
    TREG_ECX,
    TREG_EDX,
    TREG_EBX,
    TREG_ST0,
    TREG_ESP = 4
};

/* return registers for function */
#���� REG_IRET TREG_EAX /* single word int return register */
#���� REG_LRET TREG_EDX /* second word return register (for long long) */
#���� REG_FRET TREG_ST0 /* float return register */

/* defined if function parameters must be evaluated in reverse order */
#���� INVERT_FUNC_PARAMS

/* defined if structures are passed as pointers. Otherwise structures
   are directly pushed on stack. */
/* #���� FUNC_STRUCT_PARAM_AS_PTR */

/* pointer size, in bytes */
#���� PTR_SIZE 4

/* long double size and alignment, in bytes */
#���� LDOUBLE_SIZE  12
#���� LDOUBLE_ALIGN 4
/* maximum alignment (for aligned attribute support) */
#���� MAX_ALIGN     8

/******************************************************/
#�� /* ! TARGET_DEFS_ONLY */
/******************************************************/
#���� "tcc.h"

/* define to 1/0 to [not] have EBX as 4th register */
#���� USE_EBX 0

ST_DATA ���� �� reg_classes[NB_REGS] = {
    /* eax */ RC_INT | RC_EAX,
    /* ecx */ RC_INT | RC_ECX,
    /* edx */ RC_INT | RC_EDX,
    /* ebx */ (RC_INT | RC_EBX) * USE_EBX,
    /* st0 */ RC_FLOAT | RC_ST0,
};

��̬ �޷� �� func_sub_sp_offset;
��̬ �� func_ret_sub;
#�綨�� CONFIG_TCC_BCHECK
��̬ addr_t func_bound_offset;
��̬ �޷� �� func_bound_ind;
#����

/* XXX: make it faster ? */
ST_FUNC �� g(�� c)
{
    �� ind1;
    �� (nocode_wanted)
        ����;
    ind1 = ind + 1;
    �� (ind1 > cur_text_section->data_allocated)
        section_realloc(cur_text_section, ind1);
    cur_text_section->data[ind] = c;
    ind = ind1;
}

ST_FUNC �� o(�޷� �� c)
{
    �� (c) {
        g(c);
        c = c >> 8;
    }
}

ST_FUNC �� gen_le16(�� v)
{
    g(v);
    g(v >> 8);
}

ST_FUNC �� gen_le32(�� c)
{
    g(c);
    g(c >> 8);
    g(c >> 16);
    g(c >> 24);
}

/* output a symbol and patch all calls to it */
ST_FUNC �� gsym_addr(�� t, �� a)
{
    �� (t) {
        �޷� �� *ptr = cur_text_section->data + t;
        uint32_t n = read32le(ptr); /* next value */
        write32le(ptr, a - t - 4);
        t = n;
    }
}

ST_FUNC �� gsym(�� t)
{
    gsym_addr(t, ind);
}

/* instruction + 4 bytes data. Return the address of the data */
��̬ �� oad(�� c, �� s)
{
    �� t;
    �� (nocode_wanted)
        ���� s;
    o(c);
    t = ind;
    gen_le32(s);
    ���� t;
}

/* generate jmp to a label */
#���� gjmp2(instr,lbl) oad(instr,lbl)

/* output constant with relocation if 'r & VT_SYM' is true */
ST_FUNC �� gen_addr32(�� r, Sym *sym, �� c)
{
    �� (r & VT_SYM)
        greloc(cur_text_section, sym, ind, R_386_32);
    gen_le32(c);
}

ST_FUNC �� gen_addrpc32(�� r, Sym *sym, �� c)
{
    �� (r & VT_SYM)
        greloc(cur_text_section, sym, ind, R_386_PC32);
    gen_le32(c - 4);
}

/* generate a modrm reference. 'op_reg' contains the addtional 3
   opcode bits */
��̬ �� gen_modrm(�� op_reg, �� r, Sym *sym, �� c)
{
    op_reg = op_reg << 3;
    �� ((r & VT_VALMASK) == VT_CONST) {
        /* constant memory reference */
        o(0x05 | op_reg);
        gen_addr32(r, sym, c);
    } �� �� ((r & VT_VALMASK) == VT_LOCAL) {
        /* currently, we use only ebp as base */
        �� (c == (��)c) {
            /* short reference */
            o(0x45 | op_reg);
            g(c);
        } �� {
            oad(0x85 | op_reg, c);
        }
    } �� {
        g(0x00 | op_reg | (r & VT_VALMASK));
    }
}

/* load 'r' from value 'sv' */
ST_FUNC �� load(�� r, SValue *sv)
{
    �� v, t, ft, fc, fr;
    SValue v1;

#�綨�� TCC_TARGET_PE
    SValue v2;
    sv = pe_getimport(sv, &v2);
#����

    fr = sv->r;
    ft = sv->type.t & ~VT_DEFSIGN;
    fc = sv->c.i;

    ft &= ~(VT_VOLATILE | VT_CONSTANT);

    v = fr & VT_VALMASK;
    �� (fr & VT_LVAL) {
        �� (v == VT_LLOCAL) {
            v1.type.t = VT_INT;
            v1.r = VT_LOCAL | VT_LVAL;
            v1.c.i = fc;
            fr = r;
            �� (!(reg_classes[fr] & RC_INT))
                fr = get_reg(RC_INT);
            load(fr, &v1);
        }
        �� ((ft & VT_BTYPE) == VT_FLOAT) {
            o(0xd9); /* flds */
            r = 0;
        } �� �� ((ft & VT_BTYPE) == VT_DOUBLE) {
            o(0xdd); /* fldl */
            r = 0;
        } �� �� ((ft & VT_BTYPE) == VT_LDOUBLE) {
            o(0xdb); /* fldt */
            r = 5;
        } �� �� ((ft & VT_TYPE) == VT_BYTE || (ft & VT_TYPE) == VT_BOOL) {
            o(0xbe0f);   /* movsbl */
        } �� �� ((ft & VT_TYPE) == (VT_BYTE | VT_UNSIGNED)) {
            o(0xb60f);   /* movzbl */
        } �� �� ((ft & VT_TYPE) == VT_SHORT) {
            o(0xbf0f);   /* movswl */
        } �� �� ((ft & VT_TYPE) == (VT_SHORT | VT_UNSIGNED)) {
            o(0xb70f);   /* movzwl */
        } �� {
            o(0x8b);     /* movl */
        }
        gen_modrm(r, fr, sv->sym, fc);
    } �� {
        �� (v == VT_CONST) {
            o(0xb8 + r); /* mov $xx, r */
            gen_addr32(fr, sv->sym, fc);
        } �� �� (v == VT_LOCAL) {
            �� (fc) {
                o(0x8d); /* lea xxx(%ebp), r */
                gen_modrm(r, VT_LOCAL, sv->sym, fc);
            } �� {
                o(0x89);
                o(0xe8 + r); /* mov %ebp, r */
            }
        } �� �� (v == VT_CMP) {
            oad(0xb8 + r, 0); /* mov $0, r */
            o(0x0f); /* setxx %br */
            o(fc);
            o(0xc0 + r);
        } �� �� (v == VT_JMP || v == VT_JMPI) {
            t = v & 1;
            oad(0xb8 + r, t); /* mov $1, r */
            o(0x05eb); /* jmp after */
            gsym(fc);
            oad(0xb8 + r, t ^ 1); /* mov $0, r */
        } �� �� (v != r) {
            o(0x89);
            o(0xc0 + r + v * 8); /* mov v, r */
        }
    }
}

/* store register 'r' in lvalue 'v' */
ST_FUNC �� store(�� r, SValue *v)
{
    �� fr, bt, ft, fc;

#�綨�� TCC_TARGET_PE
    SValue v2;
    v = pe_getimport(v, &v2);
#����

    ft = v->type.t;
    fc = v->c.i;
    fr = v->r & VT_VALMASK;
    ft &= ~(VT_VOLATILE | VT_CONSTANT);
    bt = ft & VT_BTYPE;
    /* XXX: incorrect if float reg to reg */
    �� (bt == VT_FLOAT) {
        o(0xd9); /* fsts */
        r = 2;
    } �� �� (bt == VT_DOUBLE) {
        o(0xdd); /* fstpl */
        r = 2;
    } �� �� (bt == VT_LDOUBLE) {
        o(0xc0d9); /* fld %st(0) */
        o(0xdb); /* fstpt */
        r = 7;
    } �� {
        �� (bt == VT_SHORT)
            o(0x66);
        �� (bt == VT_BYTE || bt == VT_BOOL)
            o(0x88);
        ��
            o(0x89);
    }
    �� (fr == VT_CONST ||
        fr == VT_LOCAL ||
        (v->r & VT_LVAL)) {
        gen_modrm(r, v->r, v->sym, fc);
    } �� �� (fr != r) {
        o(0xc0 + fr + r * 8); /* mov r, fr */
    }
}

��̬ �� gadd_sp(�� val)
{
    �� (val == (��)val) {
        o(0xc483);
        g(val);
    } �� {
        oad(0xc481, val); /* add $xxx, %esp */
    }
}

#�� �Ѷ��� CONFIG_TCC_BCHECK || �Ѷ��� TCC_TARGET_PE
��̬ �� gen_static_call(�� v)
{
    Sym *sym;

    sym = external_global_sym(v, &func_old_type, 0);
    oad(0xe8, -4);
    greloc(cur_text_section, sym, ind-4, R_386_PC32);
}
#����

/* 'is_jmp' is '1' if it is a jump */
��̬ �� gcall_or_jmp(�� is_jmp)
{
    �� r;
    �� ((vtop->r & (VT_VALMASK | VT_LVAL)) == VT_CONST) {
        �� rt;
        /* constant case */
        �� (vtop->r & VT_SYM) {
            /* relocation case */
            greloc(cur_text_section, vtop->sym, 
                   ind + 1, R_386_PC32);
        } �� {
            /* put an empty PC32 relocation */
            put_elf_reloc(symtab_section, cur_text_section, 
                          ind + 1, R_386_PC32, 0);
        }
        oad(0xe8 + is_jmp, vtop->c.i - 4); /* call/jmp im */
        /* extend the return value to the whole register if necessary
           visual studio and gcc do not always set the whole eax register
           when assigning the return value of a function  */
        rt = vtop->type.ref->type.t;
        ת�� (rt & VT_BTYPE) {
            ���� VT_BYTE:
                �� (rt & VT_UNSIGNED) {
                    o(0xc0b60f); /* movzx %al, %eax */
                }
                �� {
                    o(0xc0be0f); /* movsx %al, %eax */
                }
                ����;
            ���� VT_SHORT:
                �� (rt & VT_UNSIGNED) {
                    o(0xc0b70f); /* movzx %ax, %eax */
                }
                �� {
                    o(0xc0bf0f); /* movsx %ax, %eax */
                }
                ����;
            ȱʡ:
                ����;
        }
    } �� {
        /* otherwise, indirect call */
        r = gv(RC_INT);
        o(0xff); /* call/jmp *r */
        o(0xd0 + r + (is_jmp << 4));
    }
}

��̬ uint8_t fastcall_regs[3] = { TREG_EAX, TREG_EDX, TREG_ECX };
��̬ uint8_t fastcallw_regs[2] = { TREG_ECX, TREG_EDX };

/* Return the number of registers needed to return the struct, or 0 if
   returning via struct pointer. */
ST_FUNC �� gfunc_sret(CType *vt, �� variadic, CType *ret, �� *ret_align, �� *regsize)
{
#�綨�� TCC_TARGET_PE
    �� size, align;
    *ret_align = 1; // Never have to re-align return values for x86
    *regsize = 4;
    size = type_size(vt, &align);
    �� (size > 8 || (size & (size - 1)))
        ���� 0;
    �� (size == 8)
        ret->t = VT_LLONG;
    �� �� (size == 4)
        ret->t = VT_INT;
    �� �� (size == 2)
        ret->t = VT_SHORT;
    ��
        ret->t = VT_BYTE;
    ret->ref = NULL;
    ���� 1;
#��
    *ret_align = 1; // Never have to re-align return values for x86
    ���� 0;
#����
}

/* Generate function call. The function address is pushed first, then
   all the parameters in call order. This functions pops all the
   parameters and the function address. */
ST_FUNC �� gfunc_call(�� nb_args)
{
    �� size, align, r, args_size, i, func_call;
    Sym *func_sym;
    
    args_size = 0;
    ����(i = 0;i < nb_args; i++) {
        �� ((vtop->type.t & VT_BTYPE) == VT_STRUCT) {
            size = type_size(&vtop->type, &align);
            /* align to stack align size */
            size = (size + 3) & ~3;
            /* allocate the necessary size on stack */
            oad(0xec81, size); /* sub $xxx, %esp */
            /* generate structure store */
            r = get_reg(RC_INT);
            o(0x89); /* mov %esp, r */
            o(0xe0 + r);
            vset(&vtop->type, r | VT_LVAL, 0);
            vswap();
            vstore();
            args_size += size;
        } �� �� (is_float(vtop->type.t)) {
            gv(RC_FLOAT); /* only one float register */
            �� ((vtop->type.t & VT_BTYPE) == VT_FLOAT)
                size = 4;
            �� �� ((vtop->type.t & VT_BTYPE) == VT_DOUBLE)
                size = 8;
            ��
                size = 12;
            oad(0xec81, size); /* sub $xxx, %esp */
            �� (size == 12)
                o(0x7cdb);
            ��
                o(0x5cd9 + size - 4); /* fstp[s|l] 0(%esp) */
            g(0x24);
            g(0x00);
            args_size += size;
        } �� {
            /* simple type (currently always same size) */
            /* XXX: implicit cast ? */
            r = gv(RC_INT);
            �� ((vtop->type.t & VT_BTYPE) == VT_LLONG) {
                size = 8;
                o(0x50 + vtop->r2); /* push r */
            } �� {
                size = 4;
            }
            o(0x50 + r); /* push r */
            args_size += size;
        }
        vtop--;
    }
    save_regs(0); /* save used temporary registers */
    func_sym = vtop->type.ref;
    func_call = func_sym->f.func_call;
    /* fast call case */
    �� ((func_call >= FUNC_FASTCALL1 && func_call <= FUNC_FASTCALL3) ||
        func_call == FUNC_FASTCALLW) {
        �� fastcall_nb_regs;
        uint8_t *fastcall_regs_ptr;
        �� (func_call == FUNC_FASTCALLW) {
            fastcall_regs_ptr = fastcallw_regs;
            fastcall_nb_regs = 2;
        } �� {
            fastcall_regs_ptr = fastcall_regs;
            fastcall_nb_regs = func_call - FUNC_FASTCALL1 + 1;
        }
        ����(i = 0;i < fastcall_nb_regs; i++) {
            �� (args_size <= 0)
                ����;
            o(0x58 + fastcall_regs_ptr[i]); /* pop r */
            /* XXX: incorrect for struct/floats */
            args_size -= 4;
        }
    }
#��δ���� TCC_TARGET_PE
    �� �� ((vtop->type.ref->type.t & VT_BTYPE) == VT_STRUCT)
        args_size -= 4;
#����
    gcall_or_jmp(0);

    �� (args_size && func_call != FUNC_STDCALL)
        gadd_sp(args_size);
    vtop--;
}

#�綨�� TCC_TARGET_PE
#���� FUNC_PROLOG_SIZE (10 + USE_EBX)
#��
#���� FUNC_PROLOG_SIZE (9 + USE_EBX)
#����

/* generate function prolog of type 't' */
ST_FUNC �� gfunc_prolog(CType *func_type)
{
    �� addr, align, size, func_call, fastcall_nb_regs;
    �� param_index, param_addr;
    uint8_t *fastcall_regs_ptr;
    Sym *sym;
    CType *type;

    sym = func_type->ref;
    func_call = sym->f.func_call;
    addr = 8;
    loc = 0;
    func_vc = 0;

    �� (func_call >= FUNC_FASTCALL1 && func_call <= FUNC_FASTCALL3) {
        fastcall_nb_regs = func_call - FUNC_FASTCALL1 + 1;
        fastcall_regs_ptr = fastcall_regs;
    } �� �� (func_call == FUNC_FASTCALLW) {
        fastcall_nb_regs = 2;
        fastcall_regs_ptr = fastcallw_regs;
    } �� {
        fastcall_nb_regs = 0;
        fastcall_regs_ptr = NULL;
    }
    param_index = 0;

    ind += FUNC_PROLOG_SIZE;
    func_sub_sp_offset = ind;
    /* if the function returns a structure, then add an
       implicit pointer parameter */
    func_vt = sym->type;
    func_var = (sym->f.func_type == FUNC_ELLIPSIS);
#�綨�� TCC_TARGET_PE
    size = type_size(&func_vt,&align);
    �� (((func_vt.t & VT_BTYPE) == VT_STRUCT)
        && (size > 8 || (size & (size - 1)))) {
#��
    �� ((func_vt.t & VT_BTYPE) == VT_STRUCT) {
#����
        /* XXX: fastcall case ? */
        func_vc = addr;
        addr += 4;
        param_index++;
    }
    /* define parameters */
    �� ((sym = sym->next) != NULL) {
        type = &sym->type;
        size = type_size(type, &align);
        size = (size + 3) & ~3;
#�綨�� FUNC_STRUCT_PARAM_AS_PTR
        /* structs are passed as pointer */
        �� ((type->t & VT_BTYPE) == VT_STRUCT) {
            size = 4;
        }
#����
        �� (param_index < fastcall_nb_regs) {
            /* save FASTCALL register */
            loc -= 4;
            o(0x89);     /* movl */
            gen_modrm(fastcall_regs_ptr[param_index], VT_LOCAL, NULL, loc);
            param_addr = loc;
        } �� {
            param_addr = addr;
            addr += size;
        }
        sym_push(sym->v & ~SYM_FIELD, type,
                 VT_LOCAL | lvalue_type(type->t), param_addr);
        param_index++;
    }
    func_ret_sub = 0;
    /* pascal type call ? */
    �� (func_call == FUNC_STDCALL)
        func_ret_sub = addr - 8;
#��δ���� TCC_TARGET_PE
    �� �� (func_vc)
        func_ret_sub = 4;
#����

#�綨�� CONFIG_TCC_BCHECK
    /* leave some room for bound checking code */
    �� (tcc_state->do_bounds_check) {
        func_bound_offset = lbounds_section->data_offset;
        func_bound_ind = ind;
        oad(0xb8, 0); /* lbound section pointer */
        oad(0xb8, 0); /* call to function */
    }
#����
}

/* generate function epilog */
ST_FUNC �� gfunc_epilog(��)
{
    addr_t v, saved_ind;

#�綨�� CONFIG_TCC_BCHECK
    �� (tcc_state->do_bounds_check
     && func_bound_offset != lbounds_section->data_offset) {
        addr_t saved_ind;
        addr_t *bounds_ptr;
        Sym *sym_data;

        /* add end of table info */
        bounds_ptr = section_ptr_add(lbounds_section, �󳤶�(addr_t));
        *bounds_ptr = 0;

        /* generate bound local allocation */
        saved_ind = ind;
        ind = func_bound_ind;
        sym_data = get_sym_ref(&char_pointer_type, lbounds_section, 
                               func_bound_offset, lbounds_section->data_offset);
        greloc(cur_text_section, sym_data,
               ind + 1, R_386_32);
        oad(0xb8, 0); /* mov %eax, xxx */
        gen_static_call(TOK___bound_local_new);
        ind = saved_ind;

        /* generate bound check local freeing */
        o(0x5250); /* save returned value, if any */
        greloc(cur_text_section, sym_data, ind + 1, R_386_32);
        oad(0xb8, 0); /* mov %eax, xxx */
        gen_static_call(TOK___bound_local_delete);
        o(0x585a); /* restore returned value, if any */
    }
#����

    /* align local size to word & save local variables */
    v = (-loc + 3) & -4;

#�� USE_EBX
    o(0x8b);
    gen_modrm(TREG_EBX, VT_LOCAL, NULL, -(v+4));
#����

    o(0xc9); /* leave */
    �� (func_ret_sub == 0) {
        o(0xc3); /* ret */
    } �� {
        o(0xc2); /* ret n */
        g(func_ret_sub);
        g(func_ret_sub >> 8);
    }
    saved_ind = ind;
    ind = func_sub_sp_offset - FUNC_PROLOG_SIZE;
#�綨�� TCC_TARGET_PE
    �� (v >= 4096) {
        oad(0xb8, v); /* mov stacksize, %eax */
        gen_static_call(TOK___chkstk); /* call __chkstk, (does the stackframe too) */
    } ��
#����
    {
        o(0xe58955);  /* push %ebp, mov %esp, %ebp */
        o(0xec81);  /* sub esp, stacksize */
        gen_le32(v);
#�綨�� TCC_TARGET_PE
        o(0x90);  /* adjust to FUNC_PROLOG_SIZE */
#����
    }
    o(0x53 * USE_EBX); /* push ebx */
    ind = saved_ind;
}

/* generate a jump to a label */
ST_FUNC �� gjmp(�� t)
{
    ���� gjmp2(0xe9, t);
}

/* generate a jump to a fixed address */
ST_FUNC �� gjmp_addr(�� a)
{
    �� r;
    r = a - ind - 2;
    �� (r == (��)r) {
        g(0xeb);
        g(r);
    } �� {
        oad(0xe9, a - ind - 5);
    }
}

ST_FUNC �� gtst_addr(�� inv, �� a)
{
    �� v = vtop->r & VT_VALMASK;
    �� (v == VT_CMP) {
        inv ^= (vtop--)->c.i;
        a -= ind + 2;
        �� (a == (��)a) {
            g(inv - 32);
            g(a);
        } �� {
            g(0x0f);
            oad(inv - 16, a - 4);
        }
    } �� �� ((v & ~1) == VT_JMP) {
        �� ((v & 1) != inv) {
            gjmp_addr(a);
            gsym(vtop->c.i);
        } �� {
            gsym(vtop->c.i);
            o(0x05eb);
            gjmp_addr(a);
        }
        vtop--;
    }
}

/* generate a test. set 'inv' to invert test. Stack entry is popped */
ST_FUNC �� gtst(�� inv, �� t)
{
    �� v = vtop->r & VT_VALMASK;
    �� (nocode_wanted) {
        ;
    } �� �� (v == VT_CMP) {
        /* fast case : can jump directly since flags are set */
        g(0x0f);
        t = gjmp2((vtop->c.i - 16) ^ inv, t);
    } �� �� (v == VT_JMP || v == VT_JMPI) {
        /* && or || optimization */
        �� ((v & 1) == inv) {
            /* insert vtop->c jump list in t */
            uint32_t n1, n = vtop->c.i;
            �� (n) {
                �� ((n1 = read32le(cur_text_section->data + n)))
                    n = n1;
                write32le(cur_text_section->data + n, t);
                t = vtop->c.i;
            }
        } �� {
            t = gjmp(t);
            gsym(vtop->c.i);
        }
    }
    vtop--;
    ���� t;
}

/* generate an integer binary operation */
ST_FUNC �� gen_opi(�� op)
{
    �� r, fr, opc, c;

    ת��(op) {
    ���� '+':
    ���� TOK_ADDC1: /* add with carry generation */
        opc = 0;
    gen_op8:
        �� ((vtop->r & (VT_VALMASK | VT_LVAL | VT_SYM)) == VT_CONST) {
            /* constant case */
            vswap();
            r = gv(RC_INT);
            vswap();
            c = vtop->c.i;
            �� (c == (��)c) {
                /* generate inc and dec for smaller code */
                �� (c==1 && opc==0 && op != TOK_ADDC1) {
                    o (0x40 | r); // inc
                } �� �� (c==1 && opc==5 && op != TOK_SUBC1) {
                    o (0x48 | r); // dec
                } �� {
                    o(0x83);
                    o(0xc0 | (opc << 3) | r);
                    g(c);
                }
            } �� {
                o(0x81);
                oad(0xc0 | (opc << 3) | r, c);
            }
        } �� {
            gv2(RC_INT, RC_INT);
            r = vtop[-1].r;
            fr = vtop[0].r;
            o((opc << 3) | 0x01);
            o(0xc0 + r + fr * 8); 
        }
        vtop--;
        �� (op >= TOK_ULT && op <= TOK_GT) {
            vtop->r = VT_CMP;
            vtop->c.i = op;
        }
        ����;
    ���� '-':
    ���� TOK_SUBC1: /* sub with carry generation */
        opc = 5;
        ��ת gen_op8;
    ���� TOK_ADDC2: /* add with carry use */
        opc = 2;
        ��ת gen_op8;
    ���� TOK_SUBC2: /* sub with carry use */
        opc = 3;
        ��ת gen_op8;
    ���� '&':
        opc = 4;
        ��ת gen_op8;
    ���� '^':
        opc = 6;
        ��ת gen_op8;
    ���� '|':
        opc = 1;
        ��ת gen_op8;
    ���� '*':
        gv2(RC_INT, RC_INT);
        r = vtop[-1].r;
        fr = vtop[0].r;
        vtop--;
        o(0xaf0f); /* imul fr, r */
        o(0xc0 + fr + r * 8);
        ����;
    ���� TOK_SHL:
        opc = 4;
        ��ת gen_shift;
    ���� TOK_SHR:
        opc = 5;
        ��ת gen_shift;
    ���� TOK_SAR:
        opc = 7;
    gen_shift:
        opc = 0xc0 | (opc << 3);
        �� ((vtop->r & (VT_VALMASK | VT_LVAL | VT_SYM)) == VT_CONST) {
            /* constant case */
            vswap();
            r = gv(RC_INT);
            vswap();
            c = vtop->c.i & 0x1f;
            o(0xc1); /* shl/shr/sar $xxx, r */
            o(opc | r);
            g(c);
        } �� {
            /* we generate the shift in ecx */
            gv2(RC_INT, RC_ECX);
            r = vtop[-1].r;
            o(0xd3); /* shl/shr/sar %cl, r */
            o(opc | r);
        }
        vtop--;
        ����;
    ���� '/':
    ���� TOK_UDIV:
    ���� TOK_PDIV:
    ���� '%':
    ���� TOK_UMOD:
    ���� TOK_UMULL:
        /* first operand must be in eax */
        /* XXX: need better constraint for second operand */
        gv2(RC_EAX, RC_ECX);
        r = vtop[-1].r;
        fr = vtop[0].r;
        vtop--;
        save_reg(TREG_EDX);
        /* save EAX too if used otherwise */
        save_reg_upstack(TREG_EAX, 1);
        �� (op == TOK_UMULL) {
            o(0xf7); /* mul fr */
            o(0xe0 + fr);
            vtop->r2 = TREG_EDX;
            r = TREG_EAX;
        } �� {
            �� (op == TOK_UDIV || op == TOK_UMOD) {
                o(0xf7d231); /* xor %edx, %edx, div fr, %eax */
                o(0xf0 + fr);
            } �� {
                o(0xf799); /* cltd, idiv fr, %eax */
                o(0xf8 + fr);
            }
            �� (op == '%' || op == TOK_UMOD)
                r = TREG_EDX;
            ��
                r = TREG_EAX;
        }
        vtop->r = r;
        ����;
    ȱʡ:
        opc = 7;
        ��ת gen_op8;
    }
}

/* generate a floating point operation 'v = t1 op t2' instruction. The
   two operands are guaranteed to have the same floating point type */
/* XXX: need to use ST1 too */
ST_FUNC �� gen_opf(�� op)
{
    �� a, ft, fc, swapped, r;

    /* convert constants to memory references */
    �� ((vtop[-1].r & (VT_VALMASK | VT_LVAL)) == VT_CONST) {
        vswap();
        gv(RC_FLOAT);
        vswap();
    }
    �� ((vtop[0].r & (VT_VALMASK | VT_LVAL)) == VT_CONST)
        gv(RC_FLOAT);

    /* must put at least one value in the floating point register */
    �� ((vtop[-1].r & VT_LVAL) &&
        (vtop[0].r & VT_LVAL)) {
        vswap();
        gv(RC_FLOAT);
        vswap();
    }
    swapped = 0;
    /* swap the stack if needed so that t1 is the register and t2 is
       the memory reference */
    �� (vtop[-1].r & VT_LVAL) {
        vswap();
        swapped = 1;
    }
    �� (op >= TOK_ULT && op <= TOK_GT) {
        /* load on stack second operand */
        load(TREG_ST0, vtop);
        save_reg(TREG_EAX); /* eax is used by FP comparison code */
        �� (op == TOK_GE || op == TOK_GT)
            swapped = !swapped;
        �� �� (op == TOK_EQ || op == TOK_NE)
            swapped = 0;
        �� (swapped)
            o(0xc9d9); /* fxch %st(1) */
        �� (op == TOK_EQ || op == TOK_NE)
            o(0xe9da); /* fucompp */
        ��
            o(0xd9de); /* fcompp */
        o(0xe0df); /* fnstsw %ax */
        �� (op == TOK_EQ) {
            o(0x45e480); /* and $0x45, %ah */
            o(0x40fC80); /* cmp $0x40, %ah */
        } �� �� (op == TOK_NE) {
            o(0x45e480); /* and $0x45, %ah */
            o(0x40f480); /* xor $0x40, %ah */
            op = TOK_NE;
        } �� �� (op == TOK_GE || op == TOK_LE) {
            o(0x05c4f6); /* test $0x05, %ah */
            op = TOK_EQ;
        } �� {
            o(0x45c4f6); /* test $0x45, %ah */
            op = TOK_EQ;
        }
        vtop--;
        vtop->r = VT_CMP;
        vtop->c.i = op;
    } �� {
        /* no memory reference possible for long double operations */
        �� ((vtop->type.t & VT_BTYPE) == VT_LDOUBLE) {
            load(TREG_ST0, vtop);
            swapped = !swapped;
        }
        
        ת��(op) {
        ȱʡ:
        ���� '+':
            a = 0;
            ����;
        ���� '-':
            a = 4;
            �� (swapped)
                a++;
            ����;
        ���� '*':
            a = 1;
            ����;
        ���� '/':
            a = 6;
            �� (swapped)
                a++;
            ����;
        }
        ft = vtop->type.t;
        fc = vtop->c.i;
        �� ((ft & VT_BTYPE) == VT_LDOUBLE) {
            o(0xde); /* fxxxp %st, %st(1) */
            o(0xc1 + (a << 3));
        } �� {
            /* if saved lvalue, then we must reload it */
            r = vtop->r;
            �� ((r & VT_VALMASK) == VT_LLOCAL) {
                SValue v1;
                r = get_reg(RC_INT);
                v1.type.t = VT_INT;
                v1.r = VT_LOCAL | VT_LVAL;
                v1.c.i = fc;
                load(r, &v1);
                fc = 0;
            }

            �� ((ft & VT_BTYPE) == VT_DOUBLE)
                o(0xdc);
            ��
                o(0xd8);
            gen_modrm(a, r, vtop->sym, fc);
        }
        vtop--;
    }
}

/* convert integers to fp 't' type. Must handle 'int', 'unsigned int'
   and 'long long' cases. */
ST_FUNC �� gen_cvt_itof(�� t)
{
    save_reg(TREG_ST0);
    gv(RC_INT);
    �� ((vtop->type.t & VT_BTYPE) == VT_LLONG) {
        /* signed long long to float/double/long double (unsigned case
           is handled generically) */
        o(0x50 + vtop->r2); /* push r2 */
        o(0x50 + (vtop->r & VT_VALMASK)); /* push r */
        o(0x242cdf); /* fildll (%esp) */
        o(0x08c483); /* add $8, %esp */
    } �� �� ((vtop->type.t & (VT_BTYPE | VT_UNSIGNED)) == 
               (VT_INT | VT_UNSIGNED)) {
        /* unsigned int to float/double/long double */
        o(0x6a); /* push $0 */
        g(0x00);
        o(0x50 + (vtop->r & VT_VALMASK)); /* push r */
        o(0x242cdf); /* fildll (%esp) */
        o(0x08c483); /* add $8, %esp */
    } �� {
        /* int to float/double/long double */
        o(0x50 + (vtop->r & VT_VALMASK)); /* push r */
        o(0x2404db); /* fildl (%esp) */
        o(0x04c483); /* add $4, %esp */
    }
    vtop->r = TREG_ST0;
}

/* convert fp to int 't' type */
ST_FUNC �� gen_cvt_ftoi(�� t)
{
    �� bt = vtop->type.t & VT_BTYPE;
    �� (bt == VT_FLOAT)
        vpush_global_sym(&func_old_type, TOK___fixsfdi);
    �� �� (bt == VT_LDOUBLE)
        vpush_global_sym(&func_old_type, TOK___fixxfdi);
    ��
        vpush_global_sym(&func_old_type, TOK___fixdfdi);
    vswap();
    gfunc_call(1);
    vpushi(0);
    vtop->r = REG_IRET;
    vtop->r2 = REG_LRET;
}

/* convert from one floating point type to another */
ST_FUNC �� gen_cvt_ftof(�� t)
{
    /* all we have to do on i386 is to put the float in a register */
    gv(RC_FLOAT);
}

/* computed goto support */
ST_FUNC �� ggoto(��)
{
    gcall_or_jmp(1);
    vtop--;
}

/* bound check support functions */
#�綨�� CONFIG_TCC_BCHECK

/* generate a bounded pointer addition */
ST_FUNC �� gen_bounded_ptr_add(��)
{
    /* prepare fast i386 function call (args in eax and edx) */
    gv2(RC_EAX, RC_EDX);
    /* save all temporary registers */
    vtop -= 2;
    save_regs(0);
    /* do a fast function call */
    gen_static_call(TOK___bound_ptr_add);
    /* returned pointer is in eax */
    vtop++;
    vtop->r = TREG_EAX | VT_BOUNDED;
    /* address of bounding function call point */
    vtop->c.i = (cur_text_section->reloc->data_offset - �󳤶�(Elf32_Rel));
}

/* patch pointer addition in vtop so that pointer dereferencing is
   also tested */
ST_FUNC �� gen_bounded_ptr_deref(��)
{
    addr_t func;
    ��  size, align;
    Elf32_Rel *rel;
    Sym *sym;

    size = 0;
    /* XXX: put that code in generic part of tcc */
    �� (!is_float(vtop->type.t)) {
        �� (vtop->r & VT_LVAL_BYTE)
            size = 1;
        �� �� (vtop->r & VT_LVAL_SHORT)
            size = 2;
    }
    �� (!size)
        size = type_size(&vtop->type, &align);
    ת��(size) {
    ����  1: func = TOK___bound_ptr_indir1; ����;
    ����  2: func = TOK___bound_ptr_indir2; ����;
    ����  4: func = TOK___bound_ptr_indir4; ����;
    ����  8: func = TOK___bound_ptr_indir8; ����;
    ���� 12: func = TOK___bound_ptr_indir12; ����;
    ���� 16: func = TOK___bound_ptr_indir16; ����;
    ȱʡ:
        tcc_error("unhandled size when dereferencing bounded pointer");
        func = 0;
        ����;
    }

    /* patch relocation */
    /* XXX: find a better solution ? */
    rel = (Elf32_Rel *)(cur_text_section->reloc->data + vtop->c.i);
    sym = external_global_sym(func, &func_old_type, 0);
    �� (!sym->c)
        put_extern_sym(sym, NULL, 0, 0);
    rel->r_info = ELF32_R_INFO(sym->c, ELF32_R_TYPE(rel->r_info));
}
#����

/* Save the stack pointer onto the stack */
ST_FUNC �� gen_vla_sp_save(�� addr) {
    /* mov %esp,addr(%ebp)*/
    o(0x89);
    gen_modrm(TREG_ESP, VT_LOCAL, NULL, addr);
}

/* Restore the SP from a location on the stack */
ST_FUNC �� gen_vla_sp_restore(�� addr) {
    o(0x8b);
    gen_modrm(TREG_ESP, VT_LOCAL, NULL, addr);
}

/* Subtract from the stack pointer, and push the resulting value onto the stack */
ST_FUNC �� gen_vla_alloc(CType *type, �� align) {
#�綨�� TCC_TARGET_PE
    /* alloca does more than just adjust %rsp on Windows */
    vpush_global_sym(&func_old_type, TOK_alloca);
    vswap(); /* Move alloca ref past allocation size */
    gfunc_call(1);
#��
    �� r;
    r = gv(RC_INT); /* allocation size */
    /* sub r,%rsp */
    o(0x2b);
    o(0xe0 | r);
    /* We align to 16 bytes rather than align */
    /* and ~15, %esp */
    o(0xf0e483);
    vpop();
#����
}

/* end of X86 code generator */
/*************************************************************/
#����
/*************************************************************/
