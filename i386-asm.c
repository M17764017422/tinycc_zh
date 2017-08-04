/*
 *  i386 specific functions for TCC assembler
 *
 *  Copyright (c) 2001, 2002 Fabrice Bellard
 *  Copyright (c) 2009 Fr��d��ric Feret (x86_64 support)
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

#���� MAX_OPERANDS 3

#���� TOK_ASM_first TOK_ASM_clc
#���� TOK_ASM_last TOK_ASM_emms
#���� TOK_ASM_alllast TOK_ASM_subps

#���� OPC_B          0x01  /* only used with OPC_WL */
#���� OPC_WL         0x02  /* accepts w, l or no suffix */
#���� OPC_BWL        (OPC_B | OPC_WL) /* accepts b, w, l or no suffix */
#���� OPC_REG        0x04 /* register is added to opcode */
#���� OPC_MODRM      0x08 /* modrm encoding */

#���� OPCT_MASK      0x70
#���� OPC_FWAIT      0x10 /* add fwait opcode */
#���� OPC_SHIFT      0x20 /* shift opcodes */
#���� OPC_ARITH      0x30 /* arithmetic opcodes */
#���� OPC_FARITH     0x40 /* FPU arithmetic opcodes */
#���� OPC_TEST       0x50 /* test opcodes */
#���� OPCT_IS(v,i) (((v) & OPCT_MASK) == (i))

#���� OPC_0F        0x100 /* Is secondary map (0x0f prefix) */
#���� OPC_48        0x200 /* Always has REX prefix */
#�綨�� TCC_TARGET_X86_64
# ���� OPC_WLQ     0x1000  /* accepts w, l, q or no suffix */
# ���� OPC_BWLQ    (OPC_B | OPC_WLQ) /* accepts b, w, l, q or no suffix */
# ���� OPC_WLX     OPC_WLQ
# ���� OPC_BWLX    OPC_BWLQ
#��
# ���� OPC_WLX     OPC_WL
# ���� OPC_BWLX    OPC_BWL
#����

#���� OPC_GROUP_SHIFT 13

/* in order to compress the operand type, we use specific operands and
   we or only with EA  */
ö�� {
    OPT_REG8=0, /* warning: value is hardcoded from TOK_ASM_xxx */
    OPT_REG16,  /* warning: value is hardcoded from TOK_ASM_xxx */
    OPT_REG32,  /* warning: value is hardcoded from TOK_ASM_xxx */
#�綨�� TCC_TARGET_X86_64
    OPT_REG64,  /* warning: value is hardcoded from TOK_ASM_xxx */
#����
    OPT_MMX,    /* warning: value is hardcoded from TOK_ASM_xxx */
    OPT_SSE,    /* warning: value is hardcoded from TOK_ASM_xxx */
    OPT_CR,     /* warning: value is hardcoded from TOK_ASM_xxx */
    OPT_TR,     /* warning: value is hardcoded from TOK_ASM_xxx */
    OPT_DB,     /* warning: value is hardcoded from TOK_ASM_xxx */
    OPT_SEG,
    OPT_ST,
#�綨�� TCC_TARGET_X86_64
    OPT_REG8_LOW, /* %spl,%bpl,%sil,%dil, encoded like ah,ch,dh,bh, but
                     with REX prefix, not used in insn templates */
#����
    OPT_IM8,
    OPT_IM8S,
    OPT_IM16,
    OPT_IM32,
#�綨�� TCC_TARGET_X86_64
    OPT_IM64,
#����
    OPT_EAX,    /* %al, %ax, %eax or %rax register */
    OPT_ST0,    /* %st(0) register */
    OPT_CL,     /* %cl register */
    OPT_DX,     /* %dx register */
    OPT_ADDR,   /* OP_EA with only offset */
    OPT_INDIR,  /* *(expr) */
    /* composite types */
    OPT_COMPOSITE_FIRST,
    OPT_IM,     /* IM8 | IM16 | IM32 */
    OPT_REG,    /* REG8 | REG16 | REG32 | REG64 */
    OPT_REGW,   /* REG16 | REG32 | REG64 */
    OPT_IMW,    /* IM16 | IM32 */
    OPT_MMXSSE, /* MMX | SSE */
    OPT_DISP,   /* Like OPT_ADDR, but emitted as displacement (for jumps) */
    OPT_DISP8,  /* Like OPT_ADDR, but only 8bit (short jumps) */
    /* can be ored with any OPT_xxx */
    OPT_EA = 0x80
};

#���� OP_REG8   (1 << OPT_REG8)
#���� OP_REG16  (1 << OPT_REG16)
#���� OP_REG32  (1 << OPT_REG32)
#���� OP_MMX    (1 << OPT_MMX)
#���� OP_SSE    (1 << OPT_SSE)
#���� OP_CR     (1 << OPT_CR)
#���� OP_TR     (1 << OPT_TR)
#���� OP_DB     (1 << OPT_DB)
#���� OP_SEG    (1 << OPT_SEG)
#���� OP_ST     (1 << OPT_ST)
#���� OP_IM8    (1 << OPT_IM8)
#���� OP_IM8S   (1 << OPT_IM8S)
#���� OP_IM16   (1 << OPT_IM16)
#���� OP_IM32   (1 << OPT_IM32)
#���� OP_EAX    (1 << OPT_EAX)
#���� OP_ST0    (1 << OPT_ST0)
#���� OP_CL     (1 << OPT_CL)
#���� OP_DX     (1 << OPT_DX)
#���� OP_ADDR   (1 << OPT_ADDR)
#���� OP_INDIR  (1 << OPT_INDIR)
#�綨�� TCC_TARGET_X86_64
# ���� OP_REG64 (1 << OPT_REG64)
# ���� OP_REG8_LOW (1 << OPT_REG8_LOW)
# ���� OP_IM64  (1 << OPT_IM64)
# ���� OP_EA32  (OP_EA << 1)
#��
# ���� OP_REG64 0
# ���� OP_REG8_LOW 0
# ���� OP_IM64  0
# ���� OP_EA32  0
#����

#���� OP_EA     0x40000000
#���� OP_REG    (OP_REG8 | OP_REG16 | OP_REG32 | OP_REG64)

#�綨�� TCC_TARGET_X86_64
# ���� TREG_XAX   TREG_RAX
# ���� TREG_XCX   TREG_RCX
# ���� TREG_XDX   TREG_RDX
#��
# ���� TREG_XAX   TREG_EAX
# ���� TREG_XCX   TREG_ECX
# ���� TREG_XDX   TREG_EDX
#����

���Ͷ��� �ṹ ASMInstr {
    uint16_t sym;
    uint16_t opcode;
    uint16_t instr_type;
    uint8_t nb_ops;
    uint8_t op_type[MAX_OPERANDS]; /* see OP_xxx */
} ASMInstr;

���Ͷ��� �ṹ Operand {
    uint32_t type;
    int8_t  reg; /* register, -1 if none */
    int8_t  reg2; /* second register, -1 if none */
    uint8_t shift;
    ExprValue e;
} Operand;

��̬ ���� uint8_t reg_to_size[9] = {
/*
    [OP_REG8] = 0,
    [OP_REG16] = 1,
    [OP_REG32] = 2,
#�綨�� TCC_TARGET_X86_64
    [OP_REG64] = 3,
#����
*/
    0, 0, 1, 0, 2, 0, 0, 0, 3
};

#���� NB_TEST_OPCODES 30

��̬ ���� uint8_t test_bits[NB_TEST_OPCODES] = {
 0x00, /* o */
 0x01, /* no */
 0x02, /* b */
 0x02, /* c */
 0x02, /* nae */
 0x03, /* nb */
 0x03, /* nc */
 0x03, /* ae */
 0x04, /* e */
 0x04, /* z */
 0x05, /* ne */
 0x05, /* nz */
 0x06, /* be */
 0x06, /* na */
 0x07, /* nbe */
 0x07, /* a */
 0x08, /* s */
 0x09, /* ns */
 0x0a, /* p */
 0x0a, /* pe */
 0x0b, /* np */
 0x0b, /* po */
 0x0c, /* l */
 0x0c, /* nge */
 0x0d, /* nl */
 0x0d, /* ge */
 0x0e, /* le */
 0x0e, /* ng */
 0x0f, /* nle */
 0x0f, /* g */
};

��̬ ���� uint8_t segment_prefixes[] = {
 0x26, /* es */
 0x2e, /* cs */
 0x36, /* ss */
 0x3e, /* ds */
 0x64, /* fs */
 0x65  /* gs */
};

��̬ ���� ASMInstr asm_instrs[] = {
#���� ALT(x) x
/* This removes a 0x0f in the second byte */
#���� O(o) ((uint64_t) ((((o) & 0xff00) == 0x0f00) ? ((((o) >> 8) & ~0xff) | ((o) & 0xff)) : (o)))
/* This constructs instr_type from opcode, type and group.  */
#���� T(o,i,g) ((i) | ((g) << OPC_GROUP_SHIFT) | ((((o) & 0xff00) == 0x0f00) ? OPC_0F : 0))
#���� DEF_ASM_OP0(name, opcode)
#���� DEF_ASM_OP0L(name, opcode, group, instr_type) { TOK_ASM_ ## name, O(opcode), T(opcode, instr_type, group), 0, { 0 } },
#���� DEF_ASM_OP1(name, opcode, group, instr_type, op0) { TOK_ASM_ ## name, O(opcode), T(opcode, instr_type, group), 1, { op0 }},
#���� DEF_ASM_OP2(name, opcode, group, instr_type, op0, op1) { TOK_ASM_ ## name, O(opcode), T(opcode, instr_type, group), 2, { op0, op1 }},
#���� DEF_ASM_OP3(name, opcode, group, instr_type, op0, op1, op2) { TOK_ASM_ ## name, O(opcode), T(opcode, instr_type, group), 3, { op0, op1, op2 }},
#�綨�� TCC_TARGET_X86_64
# ���� "x86_64-asm.h"
#��
# ���� "i386-asm.h"
#����
    /* last operation */
    { 0, },
};

��̬ ���� uint16_t op0_codes[] = {
#���� ALT(x)
#���� DEF_ASM_OP0(x, opcode) opcode,
#���� DEF_ASM_OP0L(name, opcode, group, instr_type)
#���� DEF_ASM_OP1(name, opcode, group, instr_type, op0)
#���� DEF_ASM_OP2(name, opcode, group, instr_type, op0, op1)
#���� DEF_ASM_OP3(name, opcode, group, instr_type, op0, op1, op2)
#�綨�� TCC_TARGET_X86_64
# ���� "x86_64-asm.h"
#��
# ���� "i386-asm.h"
#����
};

��̬ ���� �� get_reg_shift(TCCState *s1)
{
    �� shift, v;
    v = asm_int_expr(s1);
    ת��(v) {
    ���� 1:
        shift = 0;
        ����;
    ���� 2:
        shift = 1;
        ����;
    ���� 4:
        shift = 2;
        ����;
    ���� 8:
        shift = 3;
        ����;
    ȱʡ:
        expect("1, 2, 4 or 8 constant");
        shift = 0;
        ����;
    }
    ���� shift;
}

#�綨�� TCC_TARGET_X86_64
��̬ �� asm_parse_numeric_reg(�� t, �޷� �� *type)
{
    �� reg = -1;
    �� (t >= TOK_IDENT && t < tok_ident) {
        ���� �� *s = table_ident[t - TOK_IDENT]->str;
        �� c;
        *type = OP_REG64;
        �� (*s == 'c') {
            s++;
            *type = OP_CR;
        }
        �� (*s++ != 'r')
          ���� -1;
        /* Don't allow leading '0'.  */
        �� ((c = *s++) >= '1' && c <= '9')
          reg = c - '0';
        ��
          ���� -1;
        �� ((c = *s) >= '0' && c <= '5')
          s++, reg = reg * 10 + c - '0';
        �� (reg > 15)
          ���� -1;
        �� ((c = *s) == 0)
          ;
        �� �� (*type != OP_REG64)
          ���� -1;
        �� �� (c == 'b' && !s[1])
          *type = OP_REG8;
        �� �� (c == 'w' && !s[1])
          *type = OP_REG16;
        �� �� (c == 'd' && !s[1])
          *type = OP_REG32;
        ��
          ���� -1;
    }
    ���� reg;
}
#����

��̬ �� asm_parse_reg(�޷� �� *type)
{
    �� reg = 0;
    *type = 0;
    �� (tok != '%')
        ��ת error_32;
    next();
    �� (tok >= TOK_ASM_eax && tok <= TOK_ASM_edi) {
        reg = tok - TOK_ASM_eax;
        *type = OP_REG32;
#�綨�� TCC_TARGET_X86_64
    } �� �� (tok >= TOK_ASM_rax && tok <= TOK_ASM_rdi) {
        reg = tok - TOK_ASM_rax;
        *type = OP_REG64;
    } �� �� (tok == TOK_ASM_rip) {
        reg = -2; /* Probably should use different escape code. */
        *type = OP_REG64;
    } �� �� ((reg = asm_parse_numeric_reg(tok, type)) >= 0
               && (*type == OP_REG32 || *type == OP_REG64)) {
        ;
#����
    } �� {
    error_32:
        expect("register");
    }
    next();
    ���� reg;
}

��̬ �� parse_operand(TCCState *s1, Operand *op)
{
    ExprValue e;
    �� reg, indir;
    ���� �� *p;

    indir = 0;
    �� (tok == '*') {
        next();
        indir = OP_INDIR;
    }

    �� (tok == '%') {
        next();
        �� (tok >= TOK_ASM_al && tok <= TOK_ASM_db7) {
            reg = tok - TOK_ASM_al;
            op->type = 1 << (reg >> 3); /* WARNING: do not change constant order */
            op->reg = reg & 7;
            �� ((op->type & OP_REG) && op->reg == TREG_XAX)
                op->type |= OP_EAX;
            �� �� (op->type == OP_REG8 && op->reg == TREG_XCX)
                op->type |= OP_CL;
            �� �� (op->type == OP_REG16 && op->reg == TREG_XDX)
                op->type |= OP_DX;
        } �� �� (tok >= TOK_ASM_dr0 && tok <= TOK_ASM_dr7) {
            op->type = OP_DB;
            op->reg = tok - TOK_ASM_dr0;
        } �� �� (tok >= TOK_ASM_es && tok <= TOK_ASM_gs) {
            op->type = OP_SEG;
            op->reg = tok - TOK_ASM_es;
        } �� �� (tok == TOK_ASM_st) {
            op->type = OP_ST;
            op->reg = 0;
            next();
            �� (tok == '(') {
                next();
                �� (tok != TOK_PPNUM)
                    ��ת reg_error;
                p = tokc.str.data;
                reg = p[0] - '0';
                �� ((�޷�)reg >= 8 || p[1] != '\0')
                    ��ת reg_error;
                op->reg = reg;
                next();
                skip(')');
            }
            �� (op->reg == 0)
                op->type |= OP_ST0;
            ��ת no_skip;
#�綨�� TCC_TARGET_X86_64
        } �� �� (tok >= TOK_ASM_spl && tok <= TOK_ASM_dil) {
            op->type = OP_REG8 | OP_REG8_LOW;
            op->reg = 4 + tok - TOK_ASM_spl;
        } �� �� ((op->reg = asm_parse_numeric_reg(tok, &op->type)) >= 0) {
            ;
#����
        } �� {
        reg_error:
            tcc_error("unknown register %%%s", get_tok_str(tok, &tokc));
        }
        next();
    no_skip: ;
    } �� �� (tok == '$') {
        /* constant value */
        next();
        asm_expr(s1, &e);
        op->type = OP_IM32;
        op->e = e;
        �� (!op->e.sym) {
            �� (op->e.v == (uint8_t)op->e.v)
                op->type |= OP_IM8;
            �� (op->e.v == (int8_t)op->e.v)
                op->type |= OP_IM8S;
            �� (op->e.v == (uint16_t)op->e.v)
                op->type |= OP_IM16;
#�綨�� TCC_TARGET_X86_64
            �� (op->e.v != (int32_t)op->e.v && op->e.v != (uint32_t)op->e.v)
                op->type = OP_IM64;
#����
        }
    } �� {
        /* address(reg,reg2,shift) with all variants */
        op->type = OP_EA;
        op->reg = -1;
        op->reg2 = -1;
        op->shift = 0;
        �� (tok != '(') {
            asm_expr(s1, &e);
            op->e = e;
        } �� {
            next();
            �� (tok == '%') {
                unget_tok('(');
                op->e.v = 0;
                op->e.sym = NULL;
            } �� {
                /* bracketed offset expression */
                asm_expr(s1, &e);
                �� (tok != ')')
                    expect(")");
                next();
                op->e.v = e.v;
                op->e.sym = e.sym;
            }
            op->e.pcrel = 0;
        }
        �� (tok == '(') {
            �޷� �� type = 0;
            next();
            �� (tok != ',') {
                op->reg = asm_parse_reg(&type);
            }
            �� (tok == ',') {
                next();
                �� (tok != ',') {
                    op->reg2 = asm_parse_reg(&type);
                }
                �� (tok == ',') {
                    next();
                    op->shift = get_reg_shift(s1);
                }
            }
            �� (type & OP_REG32)
                op->type |= OP_EA32;
            skip(')');
        }
        �� (op->reg == -1 && op->reg2 == -1)
            op->type |= OP_ADDR;
    }
    op->type |= indir;
}

/* XXX: unify with C code output ? */
ST_FUNC �� gen_expr32(ExprValue *pe)
{
    �� (pe->pcrel)
        /* If PC-relative, always set VT_SYM, even without symbol,
           so as to force a relocation to be emitted.  */
        gen_addrpc32(VT_SYM, pe->sym, pe->v);
    ��
        gen_addr32(pe->sym ? VT_SYM : 0, pe->sym, pe->v);
}

#�綨�� TCC_TARGET_X86_64
ST_FUNC �� gen_expr64(ExprValue *pe)
{
    gen_addr64(pe->sym ? VT_SYM : 0, pe->sym, pe->v);
}
#����

/* XXX: unify with C code output ? */
��̬ �� gen_disp32(ExprValue *pe)
{
    Sym *sym = pe->sym;
    �� (sym && sym->r == cur_text_section->sh_num) {
        /* same section: we can output an absolute value. Note
           that the TCC compiler behaves differently here because
           it always outputs a relocation to ease (future) code
           elimination in the linker */
        gen_le32(pe->v + sym->jnext - ind - 4);
    } �� {
        �� (sym && sym->type.t == VT_VOID) {
            sym->type.t = VT_FUNC;
            sym->type.ref = NULL;
        }
        gen_addrpc32(VT_SYM, sym, pe->v);
    }
}

/* generate the modrm operand */
��̬ ���� �� asm_modrm(�� reg, Operand *op)
{
    �� mod, reg1, reg2, sib_reg1;

    �� (op->type & (OP_REG | OP_MMX | OP_SSE)) {
        g(0xc0 + (reg << 3) + op->reg);
    } �� �� (op->reg == -1 && op->reg2 == -1) {
        /* displacement only */
#�綨�� TCC_TARGET_X86_64
        g(0x04 + (reg << 3));
        g(0x25);
#��
        g(0x05 + (reg << 3));
#����
        gen_expr32(&op->e);
#�綨�� TCC_TARGET_X86_64
    } �� �� (op->reg == -2) {
        ExprValue *pe = &op->e;
        g(0x05 + (reg << 3));
        gen_addrpc32(pe->sym ? VT_SYM : 0, pe->sym, pe->v);
        ���� ind;
#����
    } �� {
        sib_reg1 = op->reg;
        /* fist compute displacement encoding */
        �� (sib_reg1 == -1) {
            sib_reg1 = 5;
            mod = 0x00;
        } �� �� (op->e.v == 0 && !op->e.sym && op->reg != 5) {
            mod = 0x00;
        } �� �� (op->e.v == (int8_t)op->e.v && !op->e.sym) {
            mod = 0x40;
        } �� {
            mod = 0x80;
        }
        /* compute if sib byte needed */
        reg1 = op->reg;
        �� (op->reg2 != -1)
            reg1 = 4;
        g(mod + (reg << 3) + reg1);
        �� (reg1 == 4) {
            /* add sib byte */
            reg2 = op->reg2;
            �� (reg2 == -1)
                reg2 = 4; /* indicate no index */
            g((op->shift << 6) + (reg2 << 3) + sib_reg1);
        }
        /* add offset */
        �� (mod == 0x40) {
            g(op->e.v);
        } �� �� (mod == 0x80 || op->reg == -1) {
            gen_expr32(&op->e);
        }
    }
    ���� 0;
}

#�綨�� TCC_TARGET_X86_64
#���� REX_W 0x48
#���� REX_R 0x44
#���� REX_X 0x42
#���� REX_B 0x41

��̬ �� asm_rex(�� width64, Operand *ops, �� nb_ops, �� *op_type,
                    �� regi, �� rmi)
{
  �޷� �� rex = width64 ? 0x48 : 0;
  �� saw_high_8bit = 0;
  �� i;
  �� (rmi == -1) {
      /* No mod/rm byte, but we might have a register op nevertheless
         (we will add it to the opcode later).  */
      ����(i = 0; i < nb_ops; i++) {
          �� (op_type[i] & (OP_REG | OP_ST)) {
              �� (ops[i].reg >= 8) {
                  rex |= REX_B;
                  ops[i].reg -= 8;
              } �� �� (ops[i].type & OP_REG8_LOW)
                  rex |= 0x40;
              �� �� (ops[i].type & OP_REG8 && ops[i].reg >= 4)
                  /* An 8 bit reg >= 4 without REG8 is ah/ch/dh/bh */
                  saw_high_8bit = ops[i].reg;
              ����;
          }
      }
  } �� {
      �� (regi != -1) {
          �� (ops[regi].reg >= 8) {
              rex |= REX_R;
              ops[regi].reg -= 8;
          } �� �� (ops[regi].type & OP_REG8_LOW)
              rex |= 0x40;
          �� �� (ops[regi].type & OP_REG8 && ops[regi].reg >= 4)
              /* An 8 bit reg >= 4 without REG8 is ah/ch/dh/bh */
              saw_high_8bit = ops[regi].reg;
      }
      �� (ops[rmi].type & (OP_REG | OP_MMX | OP_SSE | OP_CR | OP_EA)) {
          �� (ops[rmi].reg >= 8) {
              rex |= REX_B;
              ops[rmi].reg -= 8;
          } �� �� (ops[rmi].type & OP_REG8_LOW)
              rex |= 0x40;
          �� �� (ops[rmi].type & OP_REG8 && ops[rmi].reg >= 4)
              /* An 8 bit reg >= 4 without REG8 is ah/ch/dh/bh */
              saw_high_8bit = ops[rmi].reg;
      }
      �� (ops[rmi].type & OP_EA && ops[rmi].reg2 >= 8) {
          rex |= REX_X;
          ops[rmi].reg2 -= 8;
      }
  }
  �� (rex) {
      �� (saw_high_8bit)
          tcc_error("can't encode register %%%ch when REX prefix is required",
                    "acdb"[saw_high_8bit-4]);
      g(rex);
  }
}
#����

��̬ �� maybe_print_stats (��)
{
  ��̬ �� already = 1;
  �� (!already)
    /* print stats about opcodes */
    {
        ���� �ṹ ASMInstr *pa;
        �� freq[4];
        �� op_vals[500];
        �� nb_op_vals, i, j;

        already = 1;
        nb_op_vals = 0;
        memset(freq, 0, �󳤶�(freq));
        ����(pa = asm_instrs; pa->sym != 0; pa++) {
            freq[pa->nb_ops]++;
            //����(i=0;i<pa->nb_ops;i++) {
                ����(j=0;j<nb_op_vals;j++) {
                    //�� (pa->op_type[i] == op_vals[j])
                    �� (pa->instr_type == op_vals[j])
                        ��ת found;
                }
                //op_vals[nb_op_vals++] = pa->op_type[i];
                op_vals[nb_op_vals++] = pa->instr_type;
            found: ;
            //}
        }
        ����(i=0;i<nb_op_vals;i++) {
            �� v = op_vals[i];
            //�� ((v & (v - 1)) != 0)
                printf("%3d: %08x\n", i, v);
        }
        printf("size=%d nb=%d f0=%d f1=%d f2=%d f3=%d\n",
               (��)�󳤶�(asm_instrs),
               (��)�󳤶�(asm_instrs) / (��)�󳤶�(ASMInstr),
               freq[0], freq[1], freq[2], freq[3]);
    }
}

ST_FUNC �� asm_opcode(TCCState *s1, �� opcode)
{
    ���� ASMInstr *pa;
    �� i, modrm_index, modreg_index, reg, v, op1, seg_prefix, pc;
    �� nb_ops, s;
    Operand ops[MAX_OPERANDS], *pop;
    �� op_type[3]; /* decoded op type */
    �� alltypes;   /* OR of all operand types */
    �� autosize;
    �� p66;
#�綨�� TCC_TARGET_X86_64
    �� rex64;
#����

    maybe_print_stats();
    /* force synthetic ';' after prefix instruction, so we can handle */
    /* one-line things like "rep stosb" instead of only "rep\nstosb" */
    �� (opcode >= TOK_ASM_wait && opcode <= TOK_ASM_repnz)
        unget_tok(';');

    /* get operands */
    pop = ops;
    nb_ops = 0;
    seg_prefix = 0;
    alltypes = 0;
    ����(;;) {
        �� (tok == ';' || tok == TOK_LINEFEED)
            ����;
        �� (nb_ops >= MAX_OPERANDS) {
            tcc_error("incorrect number of operands");
        }
        parse_operand(s1, pop);
        �� (tok == ':') {
           �� (pop->type != OP_SEG || seg_prefix)
               tcc_error("incorrect prefix");
           seg_prefix = segment_prefixes[pop->reg];
           next();
           parse_operand(s1, pop);
           �� (!(pop->type & OP_EA)) {
               tcc_error("segment prefix must be followed by memory reference");
           }
        }
        pop++;
        nb_ops++;
        �� (tok != ',')
            ����;
        next();
    }

    s = 0; /* avoid warning */

    /* optimize matching by using a lookup table (no hashing is needed
       !) */
    ����(pa = asm_instrs; pa->sym != 0; pa++) {
        �� it = pa->instr_type & OPCT_MASK;
        s = 0;
        �� (it == OPC_FARITH) {
            v = opcode - pa->sym;
            �� (!((�޷�)v < 8 * 6 && (v % 6) == 0))
                ����;
        } �� �� (it == OPC_ARITH) {
            �� (!(opcode >= pa->sym && opcode < pa->sym + 8*NBWLX))
                ����;
            s = (opcode - pa->sym) % NBWLX;
            �� ((pa->instr_type & OPC_BWLX) == OPC_WLX)
              {
                /* We need to reject the xxxb opcodes that we accepted above.
                   Note that pa->sym for WLX opcodes is the 'w' token,
                   to get the 'b' token subtract one.  */
                �� (((opcode - pa->sym + 1) % NBWLX) == 0)
                    ����;
                s++;
              }
        } �� �� (it == OPC_SHIFT) {
            �� (!(opcode >= pa->sym && opcode < pa->sym + 7*NBWLX))
                ����;
            s = (opcode - pa->sym) % NBWLX;
        } �� �� (it == OPC_TEST) {
            �� (!(opcode >= pa->sym && opcode < pa->sym + NB_TEST_OPCODES))
                ����;
            /* cmovxx is a test opcode but accepts multiple sizes.
               TCC doesn't accept the suffixed mnemonic, instead we 
               simply force size autodetection always.  */
            �� (pa->instr_type & OPC_WLX)
                s = NBWLX - 1;
        } �� �� (pa->instr_type & OPC_B) {
#�綨�� TCC_TARGET_X86_64
            /* Some instructions don't have the full size but only
               bwl form.  insb e.g. */
            �� ((pa->instr_type & OPC_WLQ) != OPC_WLQ
                && !(opcode >= pa->sym && opcode < pa->sym + NBWLX-1))
                ����;
#����
            �� (!(opcode >= pa->sym && opcode < pa->sym + NBWLX))
                ����;
            s = opcode - pa->sym;
        } �� �� (pa->instr_type & OPC_WLX) {
            �� (!(opcode >= pa->sym && opcode < pa->sym + NBWLX-1))
                ����;
            s = opcode - pa->sym + 1;
        } �� {
            �� (pa->sym != opcode)
                ����;
        }
        �� (pa->nb_ops != nb_ops)
            ����;
#�綨�� TCC_TARGET_X86_64
        /* Special case for moves.  Selecting the IM64->REG64 form
           should only be done if we really have an >32bit imm64, and that
           is hardcoded.  Ignore it here.  */
        �� (pa->opcode == 0xb0 && ops[0].type != OP_IM64
            && (ops[1].type & OP_REG) == OP_REG64
            && !(pa->instr_type & OPC_0F))
            ����;
#����
        /* now decode and check each operand */
        alltypes = 0;
        ����(i = 0; i < nb_ops; i++) {
            �� op1, op2;
            op1 = pa->op_type[i];
            op2 = op1 & 0x1f;
            ת��(op2) {
            ���� OPT_IM:
                v = OP_IM8 | OP_IM16 | OP_IM32;
                ����;
            ���� OPT_REG:
                v = OP_REG8 | OP_REG16 | OP_REG32 | OP_REG64;
                ����;
            ���� OPT_REGW:
                v = OP_REG16 | OP_REG32 | OP_REG64;
                ����;
            ���� OPT_IMW:
                v = OP_IM16 | OP_IM32;
                ����;
            ���� OPT_MMXSSE:
                v = OP_MMX | OP_SSE;
                ����;
            ���� OPT_DISP:
            ���� OPT_DISP8:
                v = OP_ADDR;
                ����;
            ȱʡ:
                v = 1 << op2;
                ����;
            }
            �� (op1 & OPT_EA)
                v |= OP_EA;
            op_type[i] = v;
            �� ((ops[i].type & v) == 0)
                ��ת next;
            alltypes |= ops[i].type;
        }
        /* all is matching ! */
        ����;
    next: ;
    }
    �� (pa->sym == 0) {
        �� (opcode >= TOK_ASM_first && opcode <= TOK_ASM_last) {
            �� b;
            b = op0_codes[opcode - TOK_ASM_first];
            �� (b & 0xff00) 
                g(b >> 8);
            g(b);
            ����;
        } �� �� (opcode <= TOK_ASM_alllast) {
            tcc_error("bad operand with opcode '%s'",
                  get_tok_str(opcode, NULL));
        } �� {
            tcc_error("unknown opcode '%s'",
                  get_tok_str(opcode, NULL));
        }
    }
    /* if the size is unknown, then evaluate it (OPC_B or OPC_WL case) */
    autosize = NBWLX-1;
#�綨�� TCC_TARGET_X86_64
    /* XXX the autosize should rather be zero, to not have to adjust this
       all the time.  */
    �� ((pa->instr_type & OPC_BWLQ) == OPC_B)
        autosize = NBWLX-2;
#����
    �� (s == autosize) {
        /* Check for register operands providing hints about the size.
           Start from the end, i.e. destination operands.  This matters
           only for opcodes accepting different sized registers, lar and lsl
           are such opcodes.  */
        ����(i = nb_ops - 1; s == autosize && i >= 0; i--) {
            �� ((ops[i].type & OP_REG) && !(op_type[i] & (OP_CL | OP_DX)))
                s = reg_to_size[ops[i].type & OP_REG];
        }
        �� (s == autosize) {
            �� ((opcode == TOK_ASM_push || opcode == TOK_ASM_pop) &&
                (ops[0].type & (OP_SEG | OP_IM8S | OP_IM32)))
                s = 2;
            �� �� ((opcode == TOK_ASM_push || opcode == TOK_ASM_pop) &&
                     (ops[0].type & OP_EA))
                s = NBWLX - 2;
            ��
                tcc_error("cannot infer opcode suffix");
        }
    }

#�綨�� TCC_TARGET_X86_64
    /* Generate addr32 prefix if needed */
    ����(i = 0; i < nb_ops; i++) {
        �� (ops[i].type & OP_EA32) {
            g(0x67);
            ����;
        }
    }
#����
    /* generate data16 prefix if needed */
    p66 = 0;
    �� (s == 1)
        p66 = 1;
    �� {
        /* accepting mmx+sse in all operands --> needs 0x66 to
           switch to sse mode.  Accepting only sse in an operand --> is
           already SSE insn and needs 0x66/f2/f3 handling.  */
        ���� (i = 0; i < nb_ops; i++)
            �� ((op_type[i] & (OP_MMX | OP_SSE)) == (OP_MMX | OP_SSE)
                && ops[i].type & OP_SSE)
                p66 = 1;
    }
    �� (p66)
        g(0x66);
#�綨�� TCC_TARGET_X86_64
    rex64 = 0;
    �� (pa->instr_type & OPC_48)
        rex64 = 1;
    �� �� (s == 3 || (alltypes & OP_REG64)) {
        /* generate REX prefix */
        �� default64 = 0;
        ����(i = 0; i < nb_ops; i++) {
            �� (op_type[i] == OP_REG64 && pa->opcode != 0xb8) {
                /* If only 64bit regs are accepted in one operand
                   this is a default64 instruction without need for
                   REX prefixes, except for movabs(0xb8).  */
                default64 = 1;
                ����;
            }
        }
        /* XXX find better encoding for the default64 instructions.  */
        �� (((opcode != TOK_ASM_push && opcode != TOK_ASM_pop
              && opcode != TOK_ASM_pushw && opcode != TOK_ASM_pushl
              && opcode != TOK_ASM_pushq && opcode != TOK_ASM_popw
              && opcode != TOK_ASM_popl && opcode != TOK_ASM_popq
              && opcode != TOK_ASM_call && opcode != TOK_ASM_jmp))
            && !default64)
            rex64 = 1;
    }
#����

    /* now generates the operation */
    �� (OPCT_IS(pa->instr_type, OPC_FWAIT))
        g(0x9b);
    �� (seg_prefix)
        g(seg_prefix);

    v = pa->opcode;
    �� (pa->instr_type & OPC_0F)
        v = ((v & ~0xff) << 8) | 0x0f00 | (v & 0xff);
    �� ((v == 0x69 || v == 0x6b) && nb_ops == 2) {
        /* kludge for imul $im, %reg */
        nb_ops = 3;
        ops[2] = ops[1];
        op_type[2] = op_type[1];
    } �� �� (v == 0xcd && ops[0].e.v == 3 && !ops[0].e.sym) {
        v--; /* int $3 case */
        nb_ops = 0;
    } �� �� ((v == 0x06 || v == 0x07)) {
        �� (ops[0].reg >= 4) {
            /* push/pop %fs or %gs */
            v = 0x0fa0 + (v - 0x06) + ((ops[0].reg - 4) << 3);
        } �� {
            v += ops[0].reg << 3;
        }
        nb_ops = 0;
    } �� �� (v <= 0x05) {
        /* arith case */
        v += ((opcode - TOK_ASM_addb) / NBWLX) << 3;
    } �� �� ((pa->instr_type & (OPCT_MASK | OPC_MODRM)) == OPC_FARITH) {
        /* fpu arith case */
        v += ((opcode - pa->sym) / 6) << 3;
    }

    /* search which operand will be used for modrm */
    modrm_index = -1;
    modreg_index = -1;
    �� (pa->instr_type & OPC_MODRM) {
        �� (!nb_ops) {
            /* A modrm opcode without operands is a special case (e.g. mfence).
               It has a group and acts as if there's an register operand 0
               (ax).  */
            i = 0;
            ops[i].type = OP_REG;
            ops[i].reg = 0;
            ��ת modrm_found;
        }
        /* first look for an ea operand */
        ����(i = 0;i < nb_ops; i++) {
            �� (op_type[i] & OP_EA)
                ��ת modrm_found;
        }
        /* then if not found, a register or indirection (shift instructions) */
        ����(i = 0;i < nb_ops; i++) {
            �� (op_type[i] & (OP_REG | OP_MMX | OP_SSE | OP_INDIR))
                ��ת modrm_found;
        }
#�綨�� ASM_DEBUG
        tcc_error("bad op table");
#����
    modrm_found:
        modrm_index = i;
        /* if a register is used in another operand then it is
           used instead of group */
        ����(i = 0;i < nb_ops; i++) {
            �� t = op_type[i];
            �� (i != modrm_index &&
                (t & (OP_REG | OP_MMX | OP_SSE | OP_CR | OP_TR | OP_DB | OP_SEG))) {
                modreg_index = i;
                ����;
            }
        }
    }
#�綨�� TCC_TARGET_X86_64
    asm_rex (rex64, ops, nb_ops, op_type, modreg_index, modrm_index);
#����

    �� (pa->instr_type & OPC_REG) {
        /* mov $im, %reg case */
        �� (v == 0xb0 && s >= 1)
            v += 7;
        ����(i = 0; i < nb_ops; i++) {
            �� (op_type[i] & (OP_REG | OP_ST)) {
                v += ops[i].reg;
                ����;
            }
        }
    }
    �� (pa->instr_type & OPC_B)
        v += s >= 1;
    �� (nb_ops == 1 && pa->op_type[0] == OPT_DISP8) {
        Sym *sym;
        �� jmp_disp;

        /* see if we can really generate the jump with a byte offset */
        sym = ops[0].e.sym;
        �� (!sym)
            ��ת no_short_jump;
        �� (sym->r != cur_text_section->sh_num)
            ��ת no_short_jump;
        jmp_disp = ops[0].e.v + sym->jnext - ind - 2 - (v >= 0xff);
        �� (jmp_disp == (int8_t)jmp_disp) {
            /* OK to generate jump */
            ops[0].e.sym = 0;
            ops[0].e.v = jmp_disp;
            op_type[0] = OP_IM8S;
        } �� {
        no_short_jump:
            /* long jump will be allowed. need to modify the
               opcode slightly */
            �� (v == 0xeb) /* jmp */
                v = 0xe9;
            �� �� (v == 0x70) /* jcc */
                v += 0x0f10;
            ��
                tcc_error("invalid displacement");
        }
    }
    �� (OPCT_IS(pa->instr_type, OPC_TEST))
        v += test_bits[opcode - pa->sym];
    op1 = v >> 16;
    �� (op1)
        g(op1);
    op1 = (v >> 8) & 0xff;
    �� (op1)
        g(op1);
    g(v);

    �� (OPCT_IS(pa->instr_type, OPC_SHIFT)) {
        reg = (opcode - pa->sym) / NBWLX;
        �� (reg == 6)
            reg = 7;
    } �� �� (OPCT_IS(pa->instr_type, OPC_ARITH)) {
        reg = (opcode - pa->sym) / NBWLX;
    } �� �� (OPCT_IS(pa->instr_type, OPC_FARITH)) {
        reg = (opcode - pa->sym) / 6;
    } �� {
        reg = (pa->instr_type >> OPC_GROUP_SHIFT) & 7;
    }

    pc = 0;
    �� (pa->instr_type & OPC_MODRM) {
        /* if a register is used in another operand then it is
           used instead of group */
        �� (modreg_index >= 0)
            reg = ops[modreg_index].reg;
        pc = asm_modrm(reg, &ops[modrm_index]);
    }

    /* emit constants */
#��δ���� TCC_TARGET_X86_64
    �� (!(pa->instr_type & OPC_0F)
        && (pa->opcode == 0x9a || pa->opcode == 0xea)) {
        /* ljmp or lcall kludge */
        gen_expr32(&ops[1].e);
        �� (ops[0].e.sym)
            tcc_error("cannot relocate");
        gen_le16(ops[0].e.v);
        ����;
    }
#����
    ����(i = 0;i < nb_ops; i++) {
        v = op_type[i];
        �� (v & (OP_IM8 | OP_IM16 | OP_IM32 | OP_IM64 | OP_IM8S | OP_ADDR)) {
            /* if multiple sizes are given it means we must look
               at the op size */
            �� ((v | OP_IM8 | OP_IM64) == (OP_IM8 | OP_IM16 | OP_IM32 | OP_IM64)) {
                �� (s == 0)
                    v = OP_IM8;
                �� �� (s == 1)
                    v = OP_IM16;
                �� �� (s == 2 || (v & OP_IM64) == 0)
                    v = OP_IM32;
                ��
                    v = OP_IM64;
            }

            �� ((v & (OP_IM8 | OP_IM8S | OP_IM16)) && ops[i].e.sym)
                tcc_error("cannot relocate");

            �� (v & (OP_IM8 | OP_IM8S)) {
                g(ops[i].e.v);
            } �� �� (v & OP_IM16) {
                gen_le16(ops[i].e.v);
#�綨�� TCC_TARGET_X86_64
            } �� �� (v & OP_IM64) {
                gen_expr64(&ops[i].e);
#����
            } �� �� (pa->op_type[i] == OPT_DISP || pa->op_type[i] == OPT_DISP8) {
                gen_disp32(&ops[i].e);
            } �� {
                gen_expr32(&ops[i].e);
            }
        }
    }

    /* after immediate operands, adjust pc-relative address */
    �� (pc)
        add32le(cur_text_section->data + pc - 4, pc - ind);
}

/* return the constraint priority (we allocate first the lowest
   numbered constraints) */
��̬ ���� �� constraint_priority(���� �� *str)
{
    �� priority, c, pr;

    /* we take the lowest priority */
    priority = 0;
    ����(;;) {
        c = *str;
        �� (c == '\0')
            ����;
        str++;
        ת��(c) {
        ���� 'A':
            pr = 0;
            ����;
        ���� 'a':
        ���� 'b':
        ���� 'c':
        ���� 'd':
        ���� 'S':
        ���� 'D':
            pr = 1;
            ����;
        ���� 'q':
            pr = 2;
            ����;
        ���� 'r':
        ���� 'R':
        ���� 'p':
            pr = 3;
            ����;
        ���� 'N':
        ���� 'M':
        ���� 'I':
        ���� 'e':
        ���� 'i':
        ���� 'm':
        ���� 'g':
            pr = 4;
            ����;
        ȱʡ:
            tcc_error("unknown constraint '%c'", c);
            pr = 0;
        }
        �� (pr > priority)
            priority = pr;
    }
    ���� priority;
}

��̬ ���� �� *skip_constraint_modifiers(���� �� *p)
{
    �� (*p == '=' || *p == '&' || *p == '+' || *p == '%')
        p++;
    ���� p;
}

/* If T (a token) is of the form "%reg" returns the register
   number and type, otherwise return -1.  */
ST_FUNC �� asm_parse_regvar (�� t)
{
    ���� �� *s;
    Operand op;
    �� (t < TOK_IDENT)
        ���� -1;
    s = table_ident[t - TOK_IDENT]->str;
    �� (s[0] != '%')
        ���� -1;
    t = tok_alloc(s+1, strlen(s)-1)->tok;
    unget_tok(t);
    unget_tok('%');
    parse_operand(tcc_state, &op);
    /* Accept only integer regs for now.  */
    �� (op.type & OP_REG)
        ���� op.reg;
    ��
        ���� -1;
}

#���� REG_OUT_MASK 0x01
#���� REG_IN_MASK  0x02

#���� is_reg_allocated(reg) (regs_allocated[reg] & reg_mask)

ST_FUNC �� asm_compute_constraints(ASMOperand *operands,
                                    �� nb_operands, �� nb_outputs,
                                    ���� uint8_t *clobber_regs,
                                    �� *pout_reg)
{
    ASMOperand *op;
    �� sorted_op[MAX_ASM_OPERANDS];
    �� i, j, k, p1, p2, tmp, reg, c, reg_mask;
    ���� �� *str;
    uint8_t regs_allocated[NB_ASM_REGS];

    /* init fields */
    ����(i=0;i<nb_operands;i++) {
        op = &operands[i];
        op->input_index = -1;
        op->ref_index = -1;
        op->reg = -1;
        op->is_memory = 0;
        op->is_rw = 0;
    }
    /* compute constraint priority and evaluate references to output
       constraints if input constraints */
    ����(i=0;i<nb_operands;i++) {
        op = &operands[i];
        str = op->constraint;
        str = skip_constraint_modifiers(str);
        �� (isnum(*str) || *str == '[') {
            /* this is a reference to another constraint */
            k = find_constraint(operands, nb_operands, str, NULL);
            �� ((�޷�)k >= i || i < nb_outputs)
                tcc_error("invalid reference in constraint %d ('%s')",
                      i, str);
            op->ref_index = k;
            �� (operands[k].input_index >= 0)
                tcc_error("cannot reference twice the same operand");
            operands[k].input_index = i;
            op->priority = 5;
        } �� �� ((op->vt->r & VT_VALMASK) == VT_LOCAL
                   && op->vt->sym
                   && (reg = op->vt->sym->r & VT_VALMASK) < VT_CONST) {
            op->priority = 1;
            op->reg = reg;
        } �� {
            op->priority = constraint_priority(str);
        }
    }

    /* sort operands according to their priority */
    ����(i=0;i<nb_operands;i++)
        sorted_op[i] = i;
    ����(i=0;i<nb_operands - 1;i++) {
        ����(j=i+1;j<nb_operands;j++) {
            p1 = operands[sorted_op[i]].priority;
            p2 = operands[sorted_op[j]].priority;
            �� (p2 < p1) {
                tmp = sorted_op[i];
                sorted_op[i] = sorted_op[j];
                sorted_op[j] = tmp;
            }
        }
    }

    ����(i = 0;i < NB_ASM_REGS; i++) {
        �� (clobber_regs[i])
            regs_allocated[i] = REG_IN_MASK | REG_OUT_MASK;
        ��
            regs_allocated[i] = 0;
    }
    /* esp cannot be used */
    regs_allocated[4] = REG_IN_MASK | REG_OUT_MASK;
    /* ebp cannot be used yet */
    regs_allocated[5] = REG_IN_MASK | REG_OUT_MASK;

    /* allocate registers and generate corresponding asm moves */
    ����(i=0;i<nb_operands;i++) {
        j = sorted_op[i];
        op = &operands[j];
        str = op->constraint;
        /* no need to allocate references */
        �� (op->ref_index >= 0)
            ����;
        /* select if register is used for output, input or both */
        �� (op->input_index >= 0) {
            reg_mask = REG_IN_MASK | REG_OUT_MASK;
        } �� �� (j < nb_outputs) {
            reg_mask = REG_OUT_MASK;
        } �� {
            reg_mask = REG_IN_MASK;
        }
        �� (op->reg >= 0) {
            �� (is_reg_allocated(op->reg))
                tcc_error("asm regvar requests register that's taken already");
            reg = op->reg;
            ��ת reg_found;
        }
    try_next:
        c = *str++;
        ת��(c) {
        ���� '=':
            ��ת try_next;
        ���� '+':
            op->is_rw = 1;
            /* FALL THRU */
        ���� '&':
            �� (j >= nb_outputs)
                tcc_error("'%c' modifier can only be applied to outputs", c);
            reg_mask = REG_IN_MASK | REG_OUT_MASK;
            ��ת try_next;
        ���� 'A':
            /* allocate both eax and edx */
            �� (is_reg_allocated(TREG_XAX) ||
                is_reg_allocated(TREG_XDX))
                ��ת try_next;
            op->is_llong = 1;
            op->reg = TREG_XAX;
            regs_allocated[TREG_XAX] |= reg_mask;
            regs_allocated[TREG_XDX] |= reg_mask;
            ����;
        ���� 'a':
            reg = TREG_XAX;
            ��ת alloc_reg;
        ���� 'b':
            reg = 3;
            ��ת alloc_reg;
        ���� 'c':
            reg = TREG_XCX;
            ��ת alloc_reg;
        ���� 'd':
            reg = TREG_XDX;
            ��ת alloc_reg;
        ���� 'S':
            reg = 6;
            ��ת alloc_reg;
        ���� 'D':
            reg = 7;
        alloc_reg:
            �� (is_reg_allocated(reg))
                ��ת try_next;
            ��ת reg_found;
        ���� 'q':
            /* eax, ebx, ecx or edx */
            ����(reg = 0; reg < 4; reg++) {
                �� (!is_reg_allocated(reg))
                    ��ת reg_found;
            }
            ��ת try_next;
        ���� 'r':
        ���� 'R':
        ���� 'p': /* A general address, for x86(64) any register is acceptable*/
            /* any general register */
            ����(reg = 0; reg < 8; reg++) {
                �� (!is_reg_allocated(reg))
                    ��ת reg_found;
            }
            ��ת try_next;
        reg_found:
            /* now we can reload in the register */
            op->is_llong = 0;
            op->reg = reg;
            regs_allocated[reg] |= reg_mask;
            ����;
        ���� 'e':
        ���� 'i':
            �� (!((op->vt->r & (VT_VALMASK | VT_LVAL)) == VT_CONST))
                ��ת try_next;
            ����;
        ���� 'I':
        ���� 'N':
        ���� 'M':
            �� (!((op->vt->r & (VT_VALMASK | VT_LVAL | VT_SYM)) == VT_CONST))
                ��ת try_next;
            ����;
        ���� 'm':
        ���� 'g':
            /* nothing special to do because the operand is already in
               memory, except if the pointer itself is stored in a
               memory variable (VT_LLOCAL case) */
            /* XXX: fix constant case */
            /* if it is a reference to a memory zone, it must lie
               in a register, so we reserve the register in the
               input registers and a load will be generated
               later */
            �� (j < nb_outputs || c == 'm') {
                �� ((op->vt->r & VT_VALMASK) == VT_LLOCAL) {
                    /* any general register */
                    ����(reg = 0; reg < 8; reg++) {
                        �� (!(regs_allocated[reg] & REG_IN_MASK))
                            ��ת reg_found1;
                    }
                    ��ת try_next;
                reg_found1:
                    /* now we can reload in the register */
                    regs_allocated[reg] |= REG_IN_MASK;
                    op->reg = reg;
                    op->is_memory = 1;
                }
            }
            ����;
        ȱʡ:
            tcc_error("asm constraint %d ('%s') could not be satisfied",
                  j, op->constraint);
            ����;
        }
        /* if a reference is present for that operand, we assign it too */
        �� (op->input_index >= 0) {
            operands[op->input_index].reg = op->reg;
            operands[op->input_index].is_llong = op->is_llong;
        }
    }

    /* compute out_reg. It is used to store outputs registers to memory
       locations references by pointers (VT_LLOCAL case) */
    *pout_reg = -1;
    ����(i=0;i<nb_operands;i++) {
        op = &operands[i];
        �� (op->reg >= 0 &&
            (op->vt->r & VT_VALMASK) == VT_LLOCAL  &&
            !op->is_memory) {
            ����(reg = 0; reg < 8; reg++) {
                �� (!(regs_allocated[reg] & REG_OUT_MASK))
                    ��ת reg_found2;
            }
            tcc_error("could not find free output register for reloading");
        reg_found2:
            *pout_reg = reg;
            ����;
        }
    }

    /* print sorted constraints */
#�綨�� ASM_DEBUG
    ����(i=0;i<nb_operands;i++) {
        j = sorted_op[i];
        op = &operands[j];
        printf("%%%d [%s]: \"%s\" r=0x%04x reg=%d\n",
               j,
               op->id ? get_tok_str(op->id, NULL) : "",
               op->constraint,
               op->vt->r,
               op->reg);
    }
    �� (*pout_reg >= 0)
        printf("out_reg=%d\n", *pout_reg);
#����
}

ST_FUNC �� subst_asm_operand(CString *add_str,
                              SValue *sv, �� modifier)
{
    �� r, reg, size, val;
    �� buf[64];

    r = sv->r;
    �� ((r & VT_VALMASK) == VT_CONST) {
        �� (!(r & VT_LVAL) && modifier != 'c' && modifier != 'n' &&
            modifier != 'P')
            cstr_ccat(add_str, '$');
        �� (r & VT_SYM) {
            ���� �� *name = get_tok_str(sv->sym->v, NULL);
            �� (sv->sym->v >= SYM_FIRST_ANOM) {
                /* In case of anonymous symbols ("L.42", used
                   for static data labels) we can't find them
                   in the C symbol table when later looking up
                   this name.  So enter them now into the asm label
                   list when we still know the symbol.  */
                get_asm_sym(tok_alloc(name, strlen(name))->tok, sv->sym);
            }
            cstr_cat(add_str, name, -1);
            �� ((uint32_t)sv->c.i == 0)
                ��ת no_offset;
            cstr_ccat(add_str, '+');
        }
        val = sv->c.i;
        �� (modifier == 'n')
            val = -val;
        snprintf(buf, �󳤶�(buf), "%d", (��)sv->c.i);
        cstr_cat(add_str, buf, -1);
    no_offset:;
#�綨�� TCC_TARGET_X86_64
        �� (r & VT_LVAL)
            cstr_cat(add_str, "(%rip)", -1);
#����
    } �� �� ((r & VT_VALMASK) == VT_LOCAL) {
#�綨�� TCC_TARGET_X86_64
        snprintf(buf, �󳤶�(buf), "%d(%%rbp)", (��)sv->c.i);
#��
        snprintf(buf, �󳤶�(buf), "%d(%%ebp)", (��)sv->c.i);
#����
        cstr_cat(add_str, buf, -1);
    } �� �� (r & VT_LVAL) {
        reg = r & VT_VALMASK;
        �� (reg >= VT_CONST)
            tcc_error("internal compiler error");
        snprintf(buf, �󳤶�(buf), "(%%%s)",
#�綨�� TCC_TARGET_X86_64
                 get_tok_str(TOK_ASM_rax + reg, NULL)
#��
                 get_tok_str(TOK_ASM_eax + reg, NULL)
#����
                 );
        cstr_cat(add_str, buf, -1);
    } �� {
        /* register case */
        reg = r & VT_VALMASK;
        �� (reg >= VT_CONST)
            tcc_error("internal compiler error");

        /* choose register operand size */
        �� ((sv->type.t & VT_BTYPE) == VT_BYTE ||
            (sv->type.t & VT_BTYPE) == VT_BOOL)
            size = 1;
        �� �� ((sv->type.t & VT_BTYPE) == VT_SHORT)
            size = 2;
#�綨�� TCC_TARGET_X86_64
        �� �� ((sv->type.t & VT_BTYPE) == VT_LLONG ||
                 (sv->type.t & VT_BTYPE) == VT_PTR)
            size = 8;
#����
        ��
            size = 4;
        �� (size == 1 && reg >= 4)
            size = 4;

        �� (modifier == 'b') {
            �� (reg >= 4)
                tcc_error("cannot use byte register");
            size = 1;
        } �� �� (modifier == 'h') {
            �� (reg >= 4)
                tcc_error("cannot use byte register");
            size = -1;
        } �� �� (modifier == 'w') {
            size = 2;
        } �� �� (modifier == 'k') {
            size = 4;
#�綨�� TCC_TARGET_X86_64
        } �� �� (modifier == 'q') {
            size = 8;
#����
        }

        ת��(size) {
        ���� -1:
            reg = TOK_ASM_ah + reg;
            ����;
        ���� 1:
            reg = TOK_ASM_al + reg;
            ����;
        ���� 2:
            reg = TOK_ASM_ax + reg;
            ����;
        ȱʡ:
            reg = TOK_ASM_eax + reg;
            ����;
#�綨�� TCC_TARGET_X86_64
        ���� 8:
            reg = TOK_ASM_rax + reg;
            ����;
#����
        }
        snprintf(buf, �󳤶�(buf), "%%%s", get_tok_str(reg, NULL));
        cstr_cat(add_str, buf, -1);
    }
}

/* generate prolog and epilog code for asm statement */
ST_FUNC �� asm_gen_code(ASMOperand *operands, �� nb_operands,
                         �� nb_outputs, �� is_output,
                         uint8_t *clobber_regs,
                         �� out_reg)
{
    uint8_t regs_allocated[NB_ASM_REGS];
    ASMOperand *op;
    �� i, reg;

    /* Strictly speaking %Xbp and %Xsp should be included in the
       call-preserved registers, but currently it doesn't matter.  */
#�綨�� TCC_TARGET_X86_64
#�綨�� TCC_TARGET_PE
    ��̬ uint8_t reg_saved[] = { 3, 6, 7, 12, 13, 14, 15 };
#��
    ��̬ uint8_t reg_saved[] = { 3, 12, 13, 14, 15 };
#����
#��
    ��̬ uint8_t reg_saved[] = { 3, 6, 7 };
#����

    /* mark all used registers */
    memcpy(regs_allocated, clobber_regs, �󳤶�(regs_allocated));
    ����(i = 0; i < nb_operands;i++) {
        op = &operands[i];
        �� (op->reg >= 0)
            regs_allocated[op->reg] = 1;
    }
    �� (!is_output) {
        /* generate reg save code */
        ����(i = 0; i < �󳤶�(reg_saved)/�󳤶�(reg_saved[0]); i++) {
            reg = reg_saved[i];
            �� (regs_allocated[reg]) {
                �� (reg >= 8)
                  g(0x41), reg-=8;
                g(0x50 + reg);
            }
        }

        /* generate load code */
        ����(i = 0; i < nb_operands; i++) {
            op = &operands[i];
            �� (op->reg >= 0) {
                �� ((op->vt->r & VT_VALMASK) == VT_LLOCAL &&
                    op->is_memory) {
                    /* memory reference case (for both input and
                       output cases) */
                    SValue sv;
                    sv = *op->vt;
                    sv.r = (sv.r & ~VT_VALMASK) | VT_LOCAL | VT_LVAL;
                    sv.type.t = VT_PTR;
                    load(op->reg, &sv);
                } �� �� (i >= nb_outputs || op->is_rw) {
                    /* load value in register */
                    load(op->reg, op->vt);
                    �� (op->is_llong) {
                        SValue sv;
                        sv = *op->vt;
                        sv.c.i += 4;
                        load(TREG_XDX, &sv);
                    }
                }
            }
        }
    } �� {
        /* generate save code */
        ����(i = 0 ; i < nb_outputs; i++) {
            op = &operands[i];
            �� (op->reg >= 0) {
                �� ((op->vt->r & VT_VALMASK) == VT_LLOCAL) {
                    �� (!op->is_memory) {
                        SValue sv;
                        sv = *op->vt;
                        sv.r = (sv.r & ~VT_VALMASK) | VT_LOCAL;
                        sv.type.t = VT_PTR;
                        load(out_reg, &sv);

                        sv = *op->vt;
                        sv.r = (sv.r & ~VT_VALMASK) | out_reg;
                        store(op->reg, &sv);
                    }
                } �� {
                    store(op->reg, op->vt);
                    �� (op->is_llong) {
                        SValue sv;
                        sv = *op->vt;
                        sv.c.i += 4;
                        store(TREG_XDX, &sv);
                    }
                }
            }
        }
        /* generate reg restore code */
        ����(i = �󳤶�(reg_saved)/�󳤶�(reg_saved[0]) - 1; i >= 0; i--) {
            reg = reg_saved[i];
            �� (regs_allocated[reg]) {
                �� (reg >= 8)
                  g(0x41), reg-=8;
                g(0x58 + reg);
            }
        }
    }
}

ST_FUNC �� asm_clobber(uint8_t *clobber_regs, ���� �� *str)
{
    �� reg;
    TokenSym *ts;
#�綨�� TCC_TARGET_X86_64
    �޷� �� type;
#����

    �� (!strcmp(str, "memory") ||
        !strcmp(str, "cc") ||
        !strcmp(str, "flags"))
        ����;
    ts = tok_alloc(str, strlen(str));
    reg = ts->tok;
    �� (reg >= TOK_ASM_eax && reg <= TOK_ASM_edi) {
        reg -= TOK_ASM_eax;
    } �� �� (reg >= TOK_ASM_ax && reg <= TOK_ASM_di) {
        reg -= TOK_ASM_ax;
#�綨�� TCC_TARGET_X86_64
    } �� �� (reg >= TOK_ASM_rax && reg <= TOK_ASM_rdi) {
        reg -= TOK_ASM_rax;
    } �� �� ((reg = asm_parse_numeric_reg(reg, &type)) >= 0) {
        ;
#����
    } �� {
        tcc_error("invalid clobber register '%s'", str);
    }
    clobber_regs[reg] = 1;
}
