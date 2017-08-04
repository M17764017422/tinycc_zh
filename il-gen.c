/*
 *  CIL code generator for TCC
 * 
 *  Copyright (c) 2002 Fabrice Bellard
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

#���� this code has bit-rotted since 2003

/* number of available registers */
#���� NB_REGS             3

/* a register can belong to several classes. The classes must be
   sorted from more general to more precise (see gv2() code which does
   assumptions on it). */
#���� RC_ST      0x0001  /* any stack entry */
#���� RC_ST0     0x0002  /* top of stack */
#���� RC_ST1     0x0004  /* top - 1 */

#���� RC_INT     RC_ST
#���� RC_FLOAT   RC_ST
#���� RC_IRET    RC_ST0 /* function return: integer register */
#���� RC_LRET    RC_ST0 /* function return: second integer register */
#���� RC_FRET    RC_ST0 /* function return: float register */

/* pretty names for the registers */
ö�� {
    REG_ST0 = 0,
    REG_ST1,
    REG_ST2,
};

���� �� reg_classes[NB_REGS] = {
    /* ST0 */ RC_ST | RC_ST0,
    /* ST1 */ RC_ST | RC_ST1,
    /* ST2 */ RC_ST,
};

/* return registers for function */
#���� REG_IRET REG_ST0 /* single word int return register */
#���� REG_LRET REG_ST0 /* second word return register (for long long) */
#���� REG_FRET REG_ST0 /* float return register */

/* defined if function parameters must be evaluated in reverse order */
/* #���� INVERT_FUNC_PARAMS */

/* defined if structures are passed as pointers. Otherwise structures
   are directly pushed on stack. */
/* #���� FUNC_STRUCT_PARAM_AS_PTR */

/* pointer size, in bytes */
#���� PTR_SIZE 4

/* long double size and alignment, in bytes */
#���� LDOUBLE_SIZE  8
#���� LDOUBLE_ALIGN 8

/* function call context */
���Ͷ��� �ṹ GFuncContext {
    �� func_call; /* func call type (FUNC_STDCALL or FUNC_CDECL) */
} GFuncContext;

/******************************************************/
/* opcode definitions */

#���� IL_OP_PREFIX 0xFE

ö�� ILOPCodes {
#���� OP(name, str, n) IL_OP_ ## name = n,
#���� "il-opcodes.h"
#������ OP
};

�� *il_opcodes_str[] = {
#���� OP(name, str, n) [n] = str,
#���� "il-opcodes.h"
#������ OP
};

/******************************************************/

/* arguments variable numbers start from there */
#���� ARG_BASE 0x70000000

��̬ FILE *il_outfile;

��̬ �� out_byte(�� c)
{
    *(�� *)ind++ = c;
}

��̬ �� out_le32(�� c)
{
    out_byte(c);
    out_byte(c >> 8);
    out_byte(c >> 16);
    out_byte(c >> 24);
}

��̬ �� init_outfile(��)
{
    �� (!il_outfile) {
        il_outfile = stdout;
        fprintf(il_outfile, 
                ".assembly extern mscorlib\n"
                "{\n"
                ".ver 1:0:2411:0\n"
                "}\n\n");
    }
}

��̬ �� out_op1(�� op)
{
    �� (op & 0x100)
        out_byte(IL_OP_PREFIX);
    out_byte(op & 0xff);
}

/* output an opcode with prefix */
��̬ �� out_op(�� op)
{
    out_op1(op);
    fprintf(il_outfile, " %s\n", il_opcodes_str[op]);
}

��̬ �� out_opb(�� op, �� c)
{
    out_op1(op);
    out_byte(c);
    fprintf(il_outfile, " %s %d\n", il_opcodes_str[op], c);
}

��̬ �� out_opi(�� op, �� c)
{
    out_op1(op);
    out_le32(c);
    fprintf(il_outfile, " %s 0x%x\n", il_opcodes_str[op], c);
}

/* XXX: not complete */
��̬ �� il_type_to_str(�� *buf, �� buf_size, 
                           �� t, ���� �� *varstr)
{
    �� bt;
    Sym *s, *sa;
    �� buf1[256];
    ���� �� *tstr;

    t = t & VT_TYPE;
    bt = t & VT_BTYPE;
    buf[0] = '\0';
    �� (t & VT_UNSIGNED)
        pstrcat(buf, buf_size, "unsigned ");
    ת��(bt) {
    ���� VT_VOID:
        tstr = "void";
        ��ת add_tstr;
    ���� VT_BOOL:
        tstr = "bool";
        ��ת add_tstr;
    ���� VT_BYTE:
        tstr = "int8";
        ��ת add_tstr;
    ���� VT_SHORT:
        tstr = "int16";
        ��ת add_tstr;
    ���� VT_ENUM:
    ���� VT_INT:
    ���� VT_LONG:
        tstr = "int32";
        ��ת add_tstr;
    ���� VT_LLONG:
        tstr = "int64";
        ��ת add_tstr;
    ���� VT_FLOAT:
        tstr = "float32";
        ��ת add_tstr;
    ���� VT_DOUBLE:
    ���� VT_LDOUBLE:
        tstr = "float64";
    add_tstr:
        pstrcat(buf, buf_size, tstr);
        ����;
    ���� VT_STRUCT:
        tcc_error("structures not handled yet");
        ����;
    ���� VT_FUNC:
        s = sym_find((�޷�)t >> VT_STRUCT_SHIFT);
        il_type_to_str(buf, buf_size, s->t, varstr);
        pstrcat(buf, buf_size, "(");
        sa = s->next;
        �� (sa != NULL) {
            il_type_to_str(buf1, �󳤶�(buf1), sa->t, NULL);
            pstrcat(buf, buf_size, buf1);
            sa = sa->next;
            �� (sa)
                pstrcat(buf, buf_size, ", ");
        }
        pstrcat(buf, buf_size, ")");
        ��ת no_var;
    ���� VT_PTR:
        s = sym_find((�޷�)t >> VT_STRUCT_SHIFT);
        pstrcpy(buf1, �󳤶�(buf1), "*");
        �� (varstr)
            pstrcat(buf1, �󳤶�(buf1), varstr);
        il_type_to_str(buf, buf_size, s->t, buf1);
        ��ת no_var;
    }
    �� (varstr) {
        pstrcat(buf, buf_size, " ");
        pstrcat(buf, buf_size, varstr);
    }
 no_var: ;
}


/* patch relocation entry with value 'val' */
�� greloc_patch1(Reloc *p, �� val)
{
}

/* output a symbol and patch all calls to it */
�� gsym_addr(t, a)
{
}

/* output jump and return symbol */
��̬ �� out_opj(�� op, �� c)
{
    out_op1(op);
    out_le32(0);
    �� (c == 0) {
        c = ind - (��)cur_text_section->data;
    }
    fprintf(il_outfile, " %s L%d\n", il_opcodes_str[op], c);
    ���� c;
}

�� gsym(�� t)
{
    fprintf(il_outfile, "L%d:\n", t);
}

/* load 'r' from value 'sv' */
�� load(�� r, SValue *sv)
{
    �� v, fc, ft;

    v = sv->r & VT_VALMASK;
    fc = sv->c.i;
    ft = sv->t;

    �� (sv->r & VT_LVAL) {
        �� (v == VT_LOCAL) {
            �� (fc >= ARG_BASE) {
                fc -= ARG_BASE;
                �� (fc >= 0 && fc <= 4) {
                    out_op(IL_OP_LDARG_0 + fc);
                } �� �� (fc <= 0xff) {
                    out_opb(IL_OP_LDARG_S, fc);
                } �� {
                    out_opi(IL_OP_LDARG, fc);
                }
            } �� {
                �� (fc >= 0 && fc <= 4) {
                    out_op(IL_OP_LDLOC_0 + fc);
                } �� �� (fc <= 0xff) {
                    out_opb(IL_OP_LDLOC_S, fc);
                } �� {
                    out_opi(IL_OP_LDLOC, fc);
                }
            }
        } �� �� (v == VT_CONST) {
                /* XXX: handle globals */
                out_opi(IL_OP_LDSFLD, 0);
        } �� {
            �� ((ft & VT_BTYPE) == VT_FLOAT) {
                out_op(IL_OP_LDIND_R4);
            } �� �� ((ft & VT_BTYPE) == VT_DOUBLE) {
                out_op(IL_OP_LDIND_R8);
            } �� �� ((ft & VT_BTYPE) == VT_LDOUBLE) {
                out_op(IL_OP_LDIND_R8);
            } �� �� ((ft & VT_TYPE) == VT_BYTE)
                out_op(IL_OP_LDIND_I1);
            �� �� ((ft & VT_TYPE) == (VT_BYTE | VT_UNSIGNED))
                out_op(IL_OP_LDIND_U1);
            �� �� ((ft & VT_TYPE) == VT_SHORT)
                out_op(IL_OP_LDIND_I2);
            �� �� ((ft & VT_TYPE) == (VT_SHORT | VT_UNSIGNED))
                out_op(IL_OP_LDIND_U2);
            ��
                out_op(IL_OP_LDIND_I4);
        } 
    } �� {
        �� (v == VT_CONST) {
            /* XXX: handle globals */
            �� (fc >= -1 && fc <= 8) {
                out_op(IL_OP_LDC_I4_M1 + fc + 1); 
            } �� {
                out_opi(IL_OP_LDC_I4, fc);
            }
        } �� �� (v == VT_LOCAL) {
            �� (fc >= ARG_BASE) {
                fc -= ARG_BASE;
                �� (fc <= 0xff) {
                    out_opb(IL_OP_LDARGA_S, fc);
                } �� {
                    out_opi(IL_OP_LDARGA, fc);
                }
            } �� {
                �� (fc <= 0xff) {
                    out_opb(IL_OP_LDLOCA_S, fc);
                } �� {
                    out_opi(IL_OP_LDLOCA, fc);
                }
            }
        } �� {
            /* XXX: do it */
        }
    }
}

/* store register 'r' in lvalue 'v' */
�� store(�� r, SValue *sv)
{
    �� v, fc, ft;

    v = sv->r & VT_VALMASK;
    fc = sv->c.i;
    ft = sv->t;
    �� (v == VT_LOCAL) {
        �� (fc >= ARG_BASE) {
            fc -= ARG_BASE;
            /* XXX: check IL arg store semantics */
            �� (fc <= 0xff) {
                out_opb(IL_OP_STARG_S, fc);
            } �� {
                out_opi(IL_OP_STARG, fc);
            }
        } �� {
            �� (fc >= 0 && fc <= 4) {
                out_op(IL_OP_STLOC_0 + fc);
            } �� �� (fc <= 0xff) {
                out_opb(IL_OP_STLOC_S, fc);
            } �� {
                out_opi(IL_OP_STLOC, fc);
            }
        }
    } �� �� (v == VT_CONST) {
        /* XXX: handle globals */
        out_opi(IL_OP_STSFLD, 0);
    } �� {
        �� ((ft & VT_BTYPE) == VT_FLOAT)
            out_op(IL_OP_STIND_R4);
        �� �� ((ft & VT_BTYPE) == VT_DOUBLE)
            out_op(IL_OP_STIND_R8);
        �� �� ((ft & VT_BTYPE) == VT_LDOUBLE)
            out_op(IL_OP_STIND_R8);
        �� �� ((ft & VT_BTYPE) == VT_BYTE)
            out_op(IL_OP_STIND_I1);
        �� �� ((ft & VT_BTYPE) == VT_SHORT)
            out_op(IL_OP_STIND_I2);
        ��
            out_op(IL_OP_STIND_I4);
    }
}

/* start function call and return function call context */
�� gfunc_start(GFuncContext *c, �� func_call)
{
    c->func_call = func_call;
}

/* push function parameter which is in (vtop->t, vtop->c). Stack entry
   is then popped. */
�� gfunc_param(GFuncContext *c)
{
    �� ((vtop->t & VT_BTYPE) == VT_STRUCT) {
        tcc_error("structures passed as value not handled yet");
    } �� {
        /* simply push on stack */
        gv(RC_ST0);
    }
    vtop--;
}

/* generate function call with address in (vtop->t, vtop->c) and free function
   context. Stack entry is popped */
�� gfunc_call(GFuncContext *c)
{
    �� buf[1024];

    �� ((vtop->r & (VT_VALMASK | VT_LVAL)) == VT_CONST) {
        /* XXX: more info needed from tcc */
        il_type_to_str(buf, �󳤶�(buf), vtop->t, "xxx");
        fprintf(il_outfile, " call %s\n", buf);
    } �� {
        /* indirect call */
        gv(RC_INT);
        il_type_to_str(buf, �󳤶�(buf), vtop->t, NULL);
        fprintf(il_outfile, " calli %s\n", buf);
    }
    vtop--;
}

/* generate function prolog of type 't' */
�� gfunc_prolog(�� t)
{
    �� addr, u, func_call;
    Sym *sym;
    �� buf[1024];

    init_outfile();

    /* XXX: pass function name to gfunc_prolog */
    il_type_to_str(buf, �󳤶�(buf), t, funcname);
    fprintf(il_outfile, ".method static %s il managed\n", buf);
    fprintf(il_outfile, "{\n");
    /* XXX: cannot do better now */
    fprintf(il_outfile, " .maxstack %d\n", NB_REGS);
    fprintf(il_outfile, " .locals (int32, int32, int32, int32, int32, int32, int32, int32)\n");
    
    �� (!strcmp(funcname, "main"))
        fprintf(il_outfile, " .entrypoint\n");
        
    sym = sym_find((�޷�)t >> VT_STRUCT_SHIFT);
    func_call = sym->r;

    addr = ARG_BASE;
    /* if the function returns a structure, then add an
       implicit pointer parameter */
    func_vt = sym->t;
    func_var = (sym->c == FUNC_ELLIPSIS);
    �� ((func_vt & VT_BTYPE) == VT_STRUCT) {
        func_vc = addr;
        addr++;
    }
    /* define parameters */
    �� ((sym = sym->next) != NULL) {
        u = sym->t;
        sym_push(sym->v & ~SYM_FIELD, u,
                 VT_LOCAL | lvalue_type(sym->type.t), addr);
        addr++;
    }
}

/* generate function epilog */
�� gfunc_epilog(��)
{
    out_op(IL_OP_RET);
    fprintf(il_outfile, "}\n\n");
}

/* generate a jump to a label */
�� gjmp(�� t)
{
    ���� out_opj(IL_OP_BR, t);
}

/* generate a jump to a fixed address */
�� gjmp_addr(�� a)
{
    /* XXX: handle syms */
    out_opi(IL_OP_BR, a);
}

/* generate a test. set 'inv' to invert test. Stack entry is popped */
�� gtst(�� inv, �� t)
{
    �� v, *p, c;

    v = vtop->r & VT_VALMASK;
    �� (v == VT_CMP) {
        c = vtop->c.i ^ inv;
        ת��(c) {
        ���� TOK_EQ:
            c = IL_OP_BEQ;
            ����;
        ���� TOK_NE:
            c = IL_OP_BNE_UN;
            ����;
        ���� TOK_LT:
            c = IL_OP_BLT;
            ����;
        ���� TOK_LE:
            c = IL_OP_BLE;
            ����;
        ���� TOK_GT:
            c = IL_OP_BGT;
            ����;
        ���� TOK_GE:
            c = IL_OP_BGE;
            ����;
        ���� TOK_ULT:
            c = IL_OP_BLT_UN;
            ����;
        ���� TOK_ULE:
            c = IL_OP_BLE_UN;
            ����;
        ���� TOK_UGT:
            c = IL_OP_BGT_UN;
            ����;
        ���� TOK_UGE:
            c = IL_OP_BGE_UN;
            ����;
        }
        t = out_opj(c, t);
    } �� �� (v == VT_JMP || v == VT_JMPI) {
        /* && or || optimization */
        �� ((v & 1) == inv) {
            /* insert vtop->c jump list in t */
            p = &vtop->c.i;
            �� (*p != 0)
                p = (�� *)*p;
            *p = t;
            t = vtop->c.i;
        } �� {
            t = gjmp(t);
            gsym(vtop->c.i);
        }
    }
    vtop--;
    ���� t;
}

/* generate an integer binary operation */
�� gen_opi(�� op)
{
    gv2(RC_ST1, RC_ST0);
    ת��(op) {
    ���� '+':
        out_op(IL_OP_ADD);
        ��ת std_op;
    ���� '-':
        out_op(IL_OP_SUB);
        ��ת std_op;
    ���� '&':
        out_op(IL_OP_AND);
        ��ת std_op;
    ���� '^':
        out_op(IL_OP_XOR);
        ��ת std_op;
    ���� '|':
        out_op(IL_OP_OR);
        ��ת std_op;
    ���� '*':
        out_op(IL_OP_MUL);
        ��ת std_op;
    ���� TOK_SHL:
        out_op(IL_OP_SHL);
        ��ת std_op;
    ���� TOK_SHR:
        out_op(IL_OP_SHR_UN);
        ��ת std_op;
    ���� TOK_SAR:
        out_op(IL_OP_SHR);
        ��ת std_op;
    ���� '/':
    ���� TOK_PDIV:
        out_op(IL_OP_DIV);
        ��ת std_op;
    ���� TOK_UDIV:
        out_op(IL_OP_DIV_UN);
        ��ת std_op;
    ���� '%':
        out_op(IL_OP_REM);
        ��ת std_op;
    ���� TOK_UMOD:
        out_op(IL_OP_REM_UN);
    std_op:
        vtop--;
        vtop[0].r = REG_ST0;
        ����;
    ���� TOK_EQ:
    ���� TOK_NE:
    ���� TOK_LT:
    ���� TOK_LE:
    ���� TOK_GT:
    ���� TOK_GE:
    ���� TOK_ULT:
    ���� TOK_ULE:
    ���� TOK_UGT:
    ���� TOK_UGE:
        vtop--;
        vtop[0].r = VT_CMP;
        vtop[0].c.i = op;
        ����;
    }
}

/* generate a floating point operation 'v = t1 op t2' instruction. The
   two operands are guaranteed to have the same floating point type */
�� gen_opf(�� op)
{
    /* same as integer */
    gen_opi(op);
}

/* convert integers to fp 't' type. Must handle 'int', 'unsigned int'
   and 'long long' cases. */
�� gen_cvt_itof(�� t)
{
    gv(RC_ST0);
    �� (t == VT_FLOAT)
        out_op(IL_OP_CONV_R4);
    ��
        out_op(IL_OP_CONV_R8);
}

/* convert fp to int 't' type */
/* XXX: handle long long case */
�� gen_cvt_ftoi(�� t)
{
    gv(RC_ST0);
    ת��(t) {
    ���� VT_INT | VT_UNSIGNED:
        out_op(IL_OP_CONV_U4);
        ����;
    ���� VT_LLONG:
        out_op(IL_OP_CONV_I8);
        ����;
    ���� VT_LLONG | VT_UNSIGNED:
        out_op(IL_OP_CONV_U8);
        ����;
    ȱʡ:
        out_op(IL_OP_CONV_I4);
        ����;
    }
}

/* convert from one floating point type to another */
�� gen_cvt_ftof(�� t)
{
    gv(RC_ST0);
    �� (t == VT_FLOAT) {
        out_op(IL_OP_CONV_R4);
    } �� {
        out_op(IL_OP_CONV_R8);
    }
}

/* end of CIL code generator */
/*************************************************************/

