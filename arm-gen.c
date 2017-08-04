/*
 *  ARMv4 code generator for TCC
 *
 *  Copyright (c) 2003 Daniel Gl?ckner
 *  Copyright (c) 2012 Thomas Preud'homme
 *
 *  Based on i386-gen.c by Fabrice Bellard
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

#�� �Ѷ���(TCC_ARM_EABI) && !�Ѷ���(TCC_ARM_VFP)
#���� "Currently TinyCC only supports float computation with VFP instructions"
#����

/* number of available registers */
#�綨�� TCC_ARM_VFP
#���� NB_REGS            13
#��
#���� NB_REGS             9
#����

#��δ���� TCC_CPU_VERSION
# ���� TCC_CPU_VERSION 5
#����

/* a register can belong to several classes. The classes must be
   sorted from more general to more precise (see gv2() code which does
   assumptions on it). */
#���� RC_INT     0x0001 /* generic integer register */
#���� RC_FLOAT   0x0002 /* generic float register */
#���� RC_R0      0x0004
#���� RC_R1      0x0008
#���� RC_R2      0x0010
#���� RC_R3      0x0020
#���� RC_R12     0x0040
#���� RC_F0      0x0080
#���� RC_F1      0x0100
#���� RC_F2      0x0200
#���� RC_F3      0x0400
#�綨�� TCC_ARM_VFP
#���� RC_F4      0x0800
#���� RC_F5      0x1000
#���� RC_F6      0x2000
#���� RC_F7      0x4000
#����
#���� RC_IRET    RC_R0  /* function return: integer register */
#���� RC_LRET    RC_R1  /* function return: second integer register */
#���� RC_FRET    RC_F0  /* function return: float register */

/* pretty names for the registers */
ö�� {
    TREG_R0 = 0,
    TREG_R1,
    TREG_R2,
    TREG_R3,
    TREG_R12,
    TREG_F0,
    TREG_F1,
    TREG_F2,
    TREG_F3,
#�綨�� TCC_ARM_VFP
    TREG_F4,
    TREG_F5,
    TREG_F6,
    TREG_F7,
#����
    TREG_SP = 13,
    TREG_LR,
};

#�綨�� TCC_ARM_VFP
#���� T2CPR(t) (((t) & VT_BTYPE) != VT_FLOAT ? 0x100 : 0)
#����

/* return registers for function */
#���� REG_IRET TREG_R0 /* single word int return register */
#���� REG_LRET TREG_R1 /* second word return register (for long long) */
#���� REG_FRET TREG_F0 /* float return register */

#�綨�� TCC_ARM_EABI
#���� TOK___divdi3 TOK___aeabi_ldivmod
#���� TOK___moddi3 TOK___aeabi_ldivmod
#���� TOK___udivdi3 TOK___aeabi_uldivmod
#���� TOK___umoddi3 TOK___aeabi_uldivmod
#����

/* defined if function parameters must be evaluated in reverse order */
#���� INVERT_FUNC_PARAMS

/* defined if structures are passed as pointers. Otherwise structures
   are directly pushed on stack. */
/* #���� FUNC_STRUCT_PARAM_AS_PTR */

/* pointer size, in bytes */
#���� PTR_SIZE 4

/* long double size and alignment, in bytes */
#�綨�� TCC_ARM_VFP
#���� LDOUBLE_SIZE  8
#����

#��δ���� LDOUBLE_SIZE
#���� LDOUBLE_SIZE  8
#����

#�綨�� TCC_ARM_EABI
#���� LDOUBLE_ALIGN 8
#��
#���� LDOUBLE_ALIGN 4
#����

/* maximum alignment (for aligned attribute support) */
#���� MAX_ALIGN     8

#���� CHAR_IS_UNSIGNED

/******************************************************/
#�� /* ! TARGET_DEFS_ONLY */
/******************************************************/
#���� "tcc.h"

ö�� float_abi float_abi;

ST_DATA ���� �� reg_classes[NB_REGS] = {
    /* r0 */ RC_INT | RC_R0,
    /* r1 */ RC_INT | RC_R1,
    /* r2 */ RC_INT | RC_R2,
    /* r3 */ RC_INT | RC_R3,
    /* r12 */ RC_INT | RC_R12,
    /* f0 */ RC_FLOAT | RC_F0,
    /* f1 */ RC_FLOAT | RC_F1,
    /* f2 */ RC_FLOAT | RC_F2,
    /* f3 */ RC_FLOAT | RC_F3,
#�綨�� TCC_ARM_VFP
 /* d4/s8 */ RC_FLOAT | RC_F4,
/* d5/s10 */ RC_FLOAT | RC_F5,
/* d6/s12 */ RC_FLOAT | RC_F6,
/* d7/s14 */ RC_FLOAT | RC_F7,
#����
};

��̬ �� func_sub_sp_offset, last_itod_magic;
��̬ �� leaffunc;

#�� �Ѷ���(TCC_ARM_EABI) && �Ѷ���(TCC_ARM_VFP)
��̬ CType float_type, double_type, func_float_type, func_double_type;
ST_FUNC �� arm_init(�ṹ TCCState *s)
{
    float_type.t = VT_FLOAT;
    double_type.t = VT_DOUBLE;
    func_float_type.t = VT_FUNC;
    func_float_type.ref = sym_push(SYM_FIELD, &float_type, FUNC_CDECL, FUNC_OLD);
    func_double_type.t = VT_FUNC;
    func_double_type.ref = sym_push(SYM_FIELD, &double_type, FUNC_CDECL, FUNC_OLD);

    float_abi = s->float_abi;
#��δ���� TCC_ARM_HARDFLOAT
    tcc_warning("soft float ABI currently not supported: default to softfp");
#����
}
#��
#���� func_float_type func_old_type
#���� func_double_type func_old_type
#���� func_ldouble_type func_old_type
ST_FUNC �� arm_init(�ṹ TCCState *s)
{
#�� 0
#�� !�Ѷ��� (TCC_ARM_VFP)
    tcc_warning("Support for FPA is deprecated and will be removed in next"
                " release");
#����
#�� !�Ѷ��� (TCC_ARM_EABI)
    tcc_warning("Support for OABI is deprecated and will be removed in next"
                " release");
#����
#����
}
#����

��̬ �� two2mask(�� a,�� b) {
  ���� (reg_classes[a]|reg_classes[b])&~(RC_INT|RC_FLOAT);
}

��̬ �� regmask(�� r) {
  ���� reg_classes[r]&~(RC_INT|RC_FLOAT);
}

/******************************************************/

#�� �Ѷ���(TCC_ARM_EABI) && !�Ѷ���(CONFIG_TCC_ELFINTERP)
���� �� *default_elfinterp(�ṹ TCCState *s)
{
    �� (s->float_abi == ARM_HARD_FLOAT)
        ���� "/lib/ld-linux-armhf.so.3";
    ��
        ���� "/lib/ld-linux.so.3";
}
#����

�� o(uint32_t i)
{
  /* this is a good place to start adding big-endian support*/
  �� ind1;
  �� (nocode_wanted)
    ����;
  ind1 = ind + 4;
  �� (!cur_text_section)
    tcc_error("compiler error! This happens f.ex. if the compiler\n"
         "can't evaluate constant expressions outside of a function.");
  �� (ind1 > cur_text_section->data_allocated)
    section_realloc(cur_text_section, ind1);
  cur_text_section->data[ind++] = i&255;
  i>>=8;
  cur_text_section->data[ind++] = i&255;
  i>>=8;
  cur_text_section->data[ind++] = i&255;
  i>>=8;
  cur_text_section->data[ind++] = i;
}

��̬ uint32_t stuff_const(uint32_t op, uint32_t c)
{
  �� try_neg=0;
  uint32_t nc = 0, negop = 0;

  ת��(op&0x1F00000)
  {
    ���� 0x800000: //add
    ���� 0x400000: //sub
      try_neg=1;
      negop=op^0xC00000;
      nc=-c;
      ����;
    ���� 0x1A00000: //mov
    ���� 0x1E00000: //mvn
      try_neg=1;
      negop=op^0x400000;
      nc=~c;
      ����;
    ���� 0x200000: //xor
      ��(c==~0)
        ���� (op&0xF010F000)|((op>>16)&0xF)|0x1E00000;
      ����;
    ���� 0x0: //and
      ��(c==~0)
        ���� (op&0xF010F000)|((op>>16)&0xF)|0x1A00000;
    ���� 0x1C00000: //bic
      try_neg=1;
      negop=op^0x1C00000;
      nc=~c;
      ����;
    ���� 0x1800000: //orr
      ��(c==~0)
        ���� (op&0xFFF0FFFF)|0x1E00000;
      ����;
  }
  ���� {
    uint32_t m;
    �� i;
    ��(c<256) /* catch undefined <<32 */
      ���� op|c;
    ����(i=2;i<32;i+=2) {
      m=(0xff>>i)|(0xff<<(32-i));
      ��(!(c&~m))
        ���� op|(i<<7)|(c<<i)|(c>>(32-i));
    }
    op=negop;
    c=nc;
  } ��(try_neg--);
  ���� 0;
}


//only add,sub
�� stuff_const_harder(uint32_t op, uint32_t v) {
  uint32_t x;
  x=stuff_const(op,v);
  ��(x)
    o(x);
  �� {
    uint32_t a[16], nv, no, o2, n2;
    �� i,j,k;
    a[0]=0xff;
    o2=(op&0xfff0ffff)|((op&0xf000)<<4);;
    ����(i=1;i<16;i++)
      a[i]=(a[i-1]>>2)|(a[i-1]<<30);
    ����(i=0;i<12;i++)
      ����(j=i<4?i+12:15;j>=i+4;j--)
        ��((v&(a[i]|a[j]))==v) {
          o(stuff_const(op,v&a[i]));
          o(stuff_const(o2,v&a[j]));
          ����;
        }
    no=op^0xC00000;
    n2=o2^0xC00000;
    nv=-v;
    ����(i=0;i<12;i++)
      ����(j=i<4?i+12:15;j>=i+4;j--)
        ��((nv&(a[i]|a[j]))==nv) {
          o(stuff_const(no,nv&a[i]));
          o(stuff_const(n2,nv&a[j]));
          ����;
        }
    ����(i=0;i<8;i++)
      ����(j=i+4;j<12;j++)
        ����(k=i<4?i+12:15;k>=j+4;k--)
          ��((v&(a[i]|a[j]|a[k]))==v) {
            o(stuff_const(op,v&a[i]));
            o(stuff_const(o2,v&a[j]));
            o(stuff_const(o2,v&a[k]));
            ����;
          }
    no=op^0xC00000;
    nv=-v;
    ����(i=0;i<8;i++)
      ����(j=i+4;j<12;j++)
        ����(k=i<4?i+12:15;k>=j+4;k--)
          ��((nv&(a[i]|a[j]|a[k]))==nv) {
            o(stuff_const(no,nv&a[i]));
            o(stuff_const(n2,nv&a[j]));
            o(stuff_const(n2,nv&a[k]));
            ����;
          }
    o(stuff_const(op,v&a[0]));
    o(stuff_const(o2,v&a[4]));
    o(stuff_const(o2,v&a[8]));
    o(stuff_const(o2,v&a[12]));
  }
}

uint32_t encbranch(�� pos, �� addr, �� fail)
{
  addr-=pos+8;
  addr/=4;
  ��(addr>=0x1000000 || addr<-0x1000000) {
    ��(fail)
      tcc_error("FIXME: function bigger than 32MB");
    ���� 0;
  }
  ���� 0x0A000000|(addr&0xffffff);
}

�� decbranch(�� pos)
{
  �� x;
  x=*(uint32_t *)(cur_text_section->data + pos);
  x&=0x00ffffff;
  ��(x&0x800000)
    x-=0x1000000;
  ���� x*4+pos+8;
}

/* output a symbol and patch all calls to it */
�� gsym_addr(�� t, �� a)
{
  uint32_t *x;
  �� lt;
  ��(t) {
    x=(uint32_t *)(cur_text_section->data + t);
    t=decbranch(lt=t);
    ��(a==lt+4)
      *x=0xE1A00000; // nop
    �� {
      *x &= 0xff000000;
      *x |= encbranch(lt,a,1);
    }
  }
}

�� gsym(�� t)
{
  gsym_addr(t, ind);
}

#�綨�� TCC_ARM_VFP
��̬ uint32_t vfpr(�� r)
{
  ��(r<TREG_F0 || r>TREG_F7)
    tcc_error("compiler error! register %i is no vfp register",r);
  ���� r - TREG_F0;
}
#��
��̬ uint32_t fpr(�� r)
{
  ��(r<TREG_F0 || r>TREG_F3)
    tcc_error("compiler error! register %i is no fpa register",r);
  ���� r - TREG_F0;
}
#����

��̬ uint32_t intr(�� r)
{
  ��(r == TREG_R12)
    ���� 12;
  ��(r >= TREG_R0 && r <= TREG_R3)
    ���� r - TREG_R0;
  �� (r >= TREG_SP && r <= TREG_LR)
    ���� r + (13 - TREG_SP);
  tcc_error("compiler error! register %i is no int register",r);
}

��̬ �� calcaddr(uint32_t *base, �� *off, �� *sgn, �� maxoff, �޷� shift)
{
  ��(*off>maxoff || *off&((1<<shift)-1)) {
    uint32_t x, y;
    x=0xE280E000;
    ��(*sgn)
      x=0xE240E000;
    x|=(*base)<<16;
    *base=14; // lr
    y=stuff_const(x,*off&~maxoff);
    ��(y) {
      o(y);
      *off&=maxoff;
      ����;
    }
    y=stuff_const(x,(*off+maxoff)&~maxoff);
    ��(y) {
      o(y);
      *sgn=!*sgn;
      *off=((*off+maxoff)&~maxoff)-*off;
      ����;
    }
    stuff_const_harder(x,*off&~maxoff);
    *off&=maxoff;
  }
}

��̬ uint32_t mapcc(�� cc)
{
  ת��(cc)
  {
    ���� TOK_ULT:
      ���� 0x30000000; /* CC/LO */
    ���� TOK_UGE:
      ���� 0x20000000; /* CS/HS */
    ���� TOK_EQ:
      ���� 0x00000000; /* EQ */
    ���� TOK_NE:
      ���� 0x10000000; /* NE */
    ���� TOK_ULE:
      ���� 0x90000000; /* LS */
    ���� TOK_UGT:
      ���� 0x80000000; /* HI */
    ���� TOK_Nset:
      ���� 0x40000000; /* MI */
    ���� TOK_Nclear:
      ���� 0x50000000; /* PL */
    ���� TOK_LT:
      ���� 0xB0000000; /* LT */
    ���� TOK_GE:
      ���� 0xA0000000; /* GE */
    ���� TOK_LE:
      ���� 0xD0000000; /* LE */
    ���� TOK_GT:
      ���� 0xC0000000; /* GT */
  }
  tcc_error("unexpected condition code");
  ���� 0xE0000000; /* AL */
}

��̬ �� negcc(�� cc)
{
  ת��(cc)
  {
    ���� TOK_ULT:
      ���� TOK_UGE;
    ���� TOK_UGE:
      ���� TOK_ULT;
    ���� TOK_EQ:
      ���� TOK_NE;
    ���� TOK_NE:
      ���� TOK_EQ;
    ���� TOK_ULE:
      ���� TOK_UGT;
    ���� TOK_UGT:
      ���� TOK_ULE;
    ���� TOK_Nset:
      ���� TOK_Nclear;
    ���� TOK_Nclear:
      ���� TOK_Nset;
    ���� TOK_LT:
      ���� TOK_GE;
    ���� TOK_GE:
      ���� TOK_LT;
    ���� TOK_LE:
      ���� TOK_GT;
    ���� TOK_GT:
      ���� TOK_LE;
  }
  tcc_error("unexpected condition code");
  ���� TOK_NE;
}

/* load 'r' from value 'sv' */
�� load(�� r, SValue *sv)
{
  �� v, ft, fc, fr, sign;
  uint32_t op;
  SValue v1;

  fr = sv->r;
  ft = sv->type.t;
  fc = sv->c.i;

  ��(fc>=0)
    sign=0;
  �� {
    sign=1;
    fc=-fc;
  }

  v = fr & VT_VALMASK;
  �� (fr & VT_LVAL) {
    uint32_t base = 0xB; // fp
    ��(v == VT_LLOCAL) {
      v1.type.t = VT_PTR;
      v1.r = VT_LOCAL | VT_LVAL;
      v1.c.i = sv->c.i;
      load(TREG_LR, &v1);
      base = 14; /* lr */
      fc=sign=0;
      v=VT_LOCAL;
    } �� ��(v == VT_CONST) {
      v1.type.t = VT_PTR;
      v1.r = fr&~VT_LVAL;
      v1.c.i = sv->c.i;
      v1.sym=sv->sym;
      load(TREG_LR, &v1);
      base = 14; /* lr */
      fc=sign=0;
      v=VT_LOCAL;
    } �� ��(v < VT_CONST) {
      base=intr(v);
      fc=sign=0;
      v=VT_LOCAL;
    }
    ��(v == VT_LOCAL) {
      ��(is_float(ft)) {
        calcaddr(&base,&fc,&sign,1020,2);
#�綨�� TCC_ARM_VFP
        op=0xED100A00; /* flds */
        ��(!sign)
          op|=0x800000;
        �� ((ft & VT_BTYPE) != VT_FLOAT)
          op|=0x100;   /* flds -> fldd */
        o(op|(vfpr(r)<<12)|(fc>>2)|(base<<16));
#��
        op=0xED100100;
        ��(!sign)
          op|=0x800000;
#�� LDOUBLE_SIZE == 8
        �� ((ft & VT_BTYPE) != VT_FLOAT)
          op|=0x8000;
#��
        �� ((ft & VT_BTYPE) == VT_DOUBLE)
          op|=0x8000;
        �� �� ((ft & VT_BTYPE) == VT_LDOUBLE)
          op|=0x400000;
#����
        o(op|(fpr(r)<<12)|(fc>>2)|(base<<16));
#����
      } �� ��((ft & (VT_BTYPE|VT_UNSIGNED)) == VT_BYTE
                || (ft & VT_BTYPE) == VT_SHORT) {
        calcaddr(&base,&fc,&sign,255,0);
        op=0xE1500090;
        �� ((ft & VT_BTYPE) == VT_SHORT)
          op|=0x20;
        �� ((ft & VT_UNSIGNED) == 0)
          op|=0x40;
        ��(!sign)
          op|=0x800000;
        o(op|(intr(r)<<12)|(base<<16)|((fc&0xf0)<<4)|(fc&0xf));
      } �� {
        calcaddr(&base,&fc,&sign,4095,0);
        op=0xE5100000;
        ��(!sign)
          op|=0x800000;
        �� ((ft & VT_BTYPE) == VT_BYTE || (ft & VT_BTYPE) == VT_BOOL)
          op|=0x400000;
        o(op|(intr(r)<<12)|fc|(base<<16));
      }
      ����;
    }
  } �� {
    �� (v == VT_CONST) {
      op=stuff_const(0xE3A00000|(intr(r)<<12),sv->c.i);
      �� (fr & VT_SYM || !op) {
        o(0xE59F0000|(intr(r)<<12));
        o(0xEA000000);
        ��(fr & VT_SYM)
          greloc(cur_text_section, sv->sym, ind, R_ARM_ABS32);
        o(sv->c.i);
      } ��
        o(op);
      ����;
    } �� �� (v == VT_LOCAL) {
      op=stuff_const(0xE28B0000|(intr(r)<<12),sv->c.i);
      �� (fr & VT_SYM || !op) {
        o(0xE59F0000|(intr(r)<<12));
        o(0xEA000000);
        ��(fr & VT_SYM) // needed ?
          greloc(cur_text_section, sv->sym, ind, R_ARM_ABS32);
        o(sv->c.i);
        o(0xE08B0000|(intr(r)<<12)|intr(r));
      } ��
        o(op);
      ����;
    } �� ��(v == VT_CMP) {
      o(mapcc(sv->c.i)|0x3A00001|(intr(r)<<12));
      o(mapcc(negcc(sv->c.i))|0x3A00000|(intr(r)<<12));
      ����;
    } �� �� (v == VT_JMP || v == VT_JMPI) {
      �� t;
      t = v & 1;
      o(0xE3A00000|(intr(r)<<12)|t);
      o(0xEA000000);
      gsym(sv->c.i);
      o(0xE3A00000|(intr(r)<<12)|(t^1));
      ����;
    } �� �� (v < VT_CONST) {
      ��(is_float(ft))
#�綨�� TCC_ARM_VFP
        o(0xEEB00A40|(vfpr(r)<<12)|vfpr(v)|T2CPR(ft)); /* fcpyX */
#��
        o(0xEE008180|(fpr(r)<<12)|fpr(v));
#����
      ��
        o(0xE1A00000|(intr(r)<<12)|intr(v));
      ����;
    }
  }
  tcc_error("load unimplemented!");
}

/* store register 'r' in lvalue 'v' */
�� store(�� r, SValue *sv)
{
  SValue v1;
  �� v, ft, fc, fr, sign;
  uint32_t op;

  fr = sv->r;
  ft = sv->type.t;
  fc = sv->c.i;

  ��(fc>=0)
    sign=0;
  �� {
    sign=1;
    fc=-fc;
  }

  v = fr & VT_VALMASK;
  �� (fr & VT_LVAL || fr == VT_LOCAL) {
    uint32_t base = 0xb; /* fp */
    ��(v < VT_CONST) {
      base=intr(v);
      v=VT_LOCAL;
      fc=sign=0;
    } �� ��(v == VT_CONST) {
      v1.type.t = ft;
      v1.r = fr&~VT_LVAL;
      v1.c.i = sv->c.i;
      v1.sym=sv->sym;
      load(TREG_LR, &v1);
      base = 14; /* lr */
      fc=sign=0;
      v=VT_LOCAL;
    }
    ��(v == VT_LOCAL) {
       ��(is_float(ft)) {
        calcaddr(&base,&fc,&sign,1020,2);
#�綨�� TCC_ARM_VFP
        op=0xED000A00; /* fsts */
        ��(!sign)
          op|=0x800000;
        �� ((ft & VT_BTYPE) != VT_FLOAT)
          op|=0x100;   /* fsts -> fstd */
        o(op|(vfpr(r)<<12)|(fc>>2)|(base<<16));
#��
        op=0xED000100;
        ��(!sign)
          op|=0x800000;
#�� LDOUBLE_SIZE == 8
        �� ((ft & VT_BTYPE) != VT_FLOAT)
          op|=0x8000;
#��
        �� ((ft & VT_BTYPE) == VT_DOUBLE)
          op|=0x8000;
        �� ((ft & VT_BTYPE) == VT_LDOUBLE)
          op|=0x400000;
#����
        o(op|(fpr(r)<<12)|(fc>>2)|(base<<16));
#����
        ����;
      } �� ��((ft & VT_BTYPE) == VT_SHORT) {
        calcaddr(&base,&fc,&sign,255,0);
        op=0xE14000B0;
        ��(!sign)
          op|=0x800000;
        o(op|(intr(r)<<12)|(base<<16)|((fc&0xf0)<<4)|(fc&0xf));
      } �� {
        calcaddr(&base,&fc,&sign,4095,0);
        op=0xE5000000;
        ��(!sign)
          op|=0x800000;
        �� ((ft & VT_BTYPE) == VT_BYTE || (ft & VT_BTYPE) == VT_BOOL)
          op|=0x400000;
        o(op|(intr(r)<<12)|fc|(base<<16));
      }
      ����;
    }
  }
  tcc_error("store unimplemented");
}

��̬ �� gadd_sp(�� val)
{
  stuff_const_harder(0xE28DD000,val);
}

/* 'is_jmp' is '1' if it is a jump */
��̬ �� gcall_or_jmp(�� is_jmp)
{
  �� r;
  �� ((vtop->r & (VT_VALMASK | VT_LVAL)) == VT_CONST) {
    uint32_t x;
    /* constant case */
    x=encbranch(ind,ind+vtop->c.i,0);
    ��(x) {
      �� (vtop->r & VT_SYM) {
        /* relocation case */
        greloc(cur_text_section, vtop->sym, ind, R_ARM_PC24);
      } ��
        put_elf_reloc(symtab_section, cur_text_section, ind, R_ARM_PC24, 0);
      o(x|(is_jmp?0xE0000000:0xE1000000));
    } �� {
      ��(!is_jmp)
        o(0xE28FE004); // add lr,pc,#4
      o(0xE51FF004);   // ldr pc,[pc,#-4]
      �� (vtop->r & VT_SYM)
        greloc(cur_text_section, vtop->sym, ind, R_ARM_ABS32);
      o(vtop->c.i);
    }
  } �� {
    /* otherwise, indirect call */
    r = gv(RC_INT);
    ��(!is_jmp)
      o(0xE1A0E00F);       // mov lr,pc
    o(0xE1A0F000|intr(r)); // mov pc,r
  }
}

��̬ �� unalias_ldbl(�� btype)
{
#�� LDOUBLE_SIZE == 8
    �� (btype == VT_LDOUBLE)
      btype = VT_DOUBLE;
#����
    ���� btype;
}

/* Return whether a structure is an homogeneous float aggregate or not.
   The answer is true if all the elements of the structure are of the same
   primitive float type and there is less than 4 elements.

   type: the type corresponding to the structure to be tested */
��̬ �� is_hgen_float_aggr(CType *type)
{
  �� ((type->t & VT_BTYPE) == VT_STRUCT) {
    �ṹ Sym *ref;
    �� btype, nb_fields = 0;

    ref = type->ref->next;
    btype = unalias_ldbl(ref->type.t & VT_BTYPE);
    �� (btype == VT_FLOAT || btype == VT_DOUBLE) {
      ����(; ref && btype == unalias_ldbl(ref->type.t & VT_BTYPE); ref = ref->next, nb_fields++);
      ���� !ref && nb_fields <= 4;
    }
  }
  ���� 0;
}

�ṹ avail_regs {
  �з� �� avail[3]; /* 3 holes max with only float and double alignments */
  �� first_hole; /* first available hole */
  �� last_hole; /* last available hole (none if equal to first_hole) */
  �� first_free_reg; /* next free register in the sequence, hole excluded */
};

#���� AVAIL_REGS_INITIALIZER (�ṹ avail_regs) { { 0, 0, 0}, 0, 0, 0 }

/* Find suitable registers for a VFP Co-Processor Register Candidate (VFP CPRC
   param) according to the rules described in the procedure call standard for
   the ARM architecture (AAPCS). If found, the registers are assigned to this
   VFP CPRC parameter. Registers are allocated in sequence unless a hole exists
   and the parameter is a single float.

   avregs: opaque structure to keep track of available VFP co-processor regs
   align: alignment constraints for the param, as returned by type_size()
   size: size of the parameter, as returned by type_size() */
�� assign_vfpreg(�ṹ avail_regs *avregs, �� align, �� size)
{
  �� first_reg = 0;

  �� (avregs->first_free_reg == -1)
    ���� -1;
  �� (align >> 3) { /* double alignment */
    first_reg = avregs->first_free_reg;
    /* alignment constraint not respected so use next reg and record hole */
    �� (first_reg & 1)
      avregs->avail[avregs->last_hole++] = first_reg++;
  } �� { /* no special alignment (float or array of float) */
    /* if single float and a hole is available, assign the param to it */
    �� (size == 4 && avregs->first_hole != avregs->last_hole)
      ���� avregs->avail[avregs->first_hole++];
    ��
      first_reg = avregs->first_free_reg;
  }
  �� (first_reg + size / 4 <= 16) {
    avregs->first_free_reg = first_reg + size / 4;
    ���� first_reg;
  }
  avregs->first_free_reg = -1;
  ���� -1;
}

/* Returns whether all params need to be passed in core registers or not.
   This is the case for function part of the runtime ABI. */
�� floats_in_core_regs(SValue *sval)
{
  �� (!sval->sym)
    ���� 0;

  ת�� (sval->sym->v) {
    ���� TOK___floatundisf:
    ���� TOK___floatundidf:
    ���� TOK___fixunssfdi:
    ���� TOK___fixunsdfdi:
#��δ���� TCC_ARM_VFP
    ���� TOK___fixunsxfdi:
#����
    ���� TOK___floatdisf:
    ���� TOK___floatdidf:
    ���� TOK___fixsfdi:
    ���� TOK___fixdfdi:
      ���� 1;

    ȱʡ:
      ���� 0;
  }
}

/* Return the number of registers needed to return the struct, or 0 if
   returning via struct pointer. */
ST_FUNC �� gfunc_sret(CType *vt, �� variadic, CType *ret, �� *ret_align, �� *regsize) {
#�綨�� TCC_ARM_EABI
    �� size, align;
    size = type_size(vt, &align);
    �� (float_abi == ARM_HARD_FLOAT && !variadic &&
        (is_float(vt->t) || is_hgen_float_aggr(vt))) {
        *ret_align = 8;
        *regsize = 8;
        ret->ref = NULL;
        ret->t = VT_DOUBLE;
        ���� (size + 7) >> 3;
    } �� �� (size <= 4) {
        *ret_align = 4;
        *regsize = 4;
        ret->ref = NULL;
        ret->t = VT_INT;
        ���� 1;
    } ��
        ���� 0;
#��
    ���� 0;
#����
}

/* Parameters are classified according to how they are copied to their final
   destination for the function call. Because the copying is performed class
   after class according to the order in the union below, it is important that
   some constraints about the order of the members of this union are respected:
   - CORE_STRUCT_CLASS must come after STACK_CLASS;
   - CORE_CLASS must come after STACK_CLASS, CORE_STRUCT_CLASS and
     VFP_STRUCT_CLASS;
   - VFP_STRUCT_CLASS must come after VFP_CLASS.
   See the comment for the main loop in copy_params() for the reason. */
ö�� reg_class {
        STACK_CLASS = 0,
        CORE_STRUCT_CLASS,
        VFP_CLASS,
        VFP_STRUCT_CLASS,
        CORE_CLASS,
        NB_CLASSES
};

�ṹ param_plan {
    �� start; /* first reg or addr used depending on the class */
    �� end; /* last reg used or next free addr depending on the class */
    SValue *sval; /* pointer to SValue on the value stack */
    �ṹ param_plan *prev; /*  previous element in this class */
};

�ṹ plan {
    �ṹ param_plan *pplans; /* array of all the param plans */
    �ṹ param_plan *clsplans[NB_CLASSES]; /* per class lists of param plans */
};

#���� add_param_plan(plan,pplan,class)                        \
    ���� {                                                        \
        pplan.prev = plan->clsplans[class];                     \
        plan->pplans[plan ## _nb] = pplan;                      \
        plan->clsplans[class] = &plan->pplans[plan ## _nb++];   \
    } ��(0)

/* Assign parameters to registers and stack with alignment according to the
   rules in the procedure call standard for the ARM architecture (AAPCS).
   The overall assignment is recorded in an array of per parameter structures
   called parameter plans. The parameter plans are also further organized in a
   number of linked lists, one per class of parameter (see the comment for the
   definition of union reg_class).

   nb_args: number of parameters of the function for which a call is generated
   float_abi: float ABI in use for this function call
   plan: the structure where the overall assignment is recorded
   todo: a bitmap that record which core registers hold a parameter

   Returns the amount of stack space needed for parameter passing

   Note: this function allocated an array in plan->pplans with tcc_malloc. It
   is the responsibility of the caller to free this array once used (ie not
   before copy_params). */
��̬ �� assign_regs(�� nb_args, �� float_abi, �ṹ plan *plan, �� *todo)
{
  �� i, size, align;
  �� ncrn /* next core register number */, nsaa /* next stacked argument address*/;
  �� plan_nb = 0;
  �ṹ param_plan pplan;
  �ṹ avail_regs avregs = AVAIL_REGS_INITIALIZER;

  ncrn = nsaa = 0;
  *todo = 0;
  plan->pplans = tcc_malloc(nb_args * �󳤶�(*plan->pplans));
  memset(plan->clsplans, 0, �󳤶�(plan->clsplans));
  ����(i = nb_args; i-- ;) {
    �� j, start_vfpreg = 0;
    CType type = vtop[-i].type;
    type.t &= ~VT_ARRAY;
    size = type_size(&type, &align);
    size = (size + 3) & ~3;
    align = (align + 3) & ~3;
    ת��(vtop[-i].type.t & VT_BTYPE) {
      ���� VT_STRUCT:
      ���� VT_FLOAT:
      ���� VT_DOUBLE:
      ���� VT_LDOUBLE:
      �� (float_abi == ARM_HARD_FLOAT) {
        �� is_hfa = 0; /* Homogeneous float aggregate */

        �� (is_float(vtop[-i].type.t)
            || (is_hfa = is_hgen_float_aggr(&vtop[-i].type))) {
          �� end_vfpreg;

          start_vfpreg = assign_vfpreg(&avregs, align, size);
          end_vfpreg = start_vfpreg + ((size - 1) >> 2);
          �� (start_vfpreg >= 0) {
            pplan = (�ṹ param_plan) {start_vfpreg, end_vfpreg, &vtop[-i]};
            �� (is_hfa)
              add_param_plan(plan, pplan, VFP_STRUCT_CLASS);
            ��
              add_param_plan(plan, pplan, VFP_CLASS);
            ����;
          } ��
            ����;
        }
      }
      ncrn = (ncrn + (align-1)/4) & ~((align/4) - 1);
      �� (ncrn + size/4 <= 4 || (ncrn < 4 && start_vfpreg != -1)) {
        /* The parameter is allocated both in core register and on stack. As
         * such, it can be of either class: it would either be the last of
         * CORE_STRUCT_CLASS or the first of STACK_CLASS. */
        ���� (j = ncrn; j < 4 && j < ncrn + size / 4; j++)
          *todo|=(1<<j);
        pplan = (�ṹ param_plan) {ncrn, j, &vtop[-i]};
        add_param_plan(plan, pplan, CORE_STRUCT_CLASS);
        ncrn += size/4;
        �� (ncrn > 4)
          nsaa = (ncrn - 4) * 4;
      } �� {
        ncrn = 4;
        ����;
      }
      ����;
      ȱʡ:
      �� (ncrn < 4) {
        �� is_long = (vtop[-i].type.t & VT_BTYPE) == VT_LLONG;

        �� (is_long) {
          ncrn = (ncrn + 1) & -2;
          �� (ncrn == 4)
            ����;
        }
        pplan = (�ṹ param_plan) {ncrn, ncrn, &vtop[-i]};
        ncrn++;
        �� (is_long)
          pplan.end = ncrn++;
        add_param_plan(plan, pplan, CORE_CLASS);
        ����;
      }
    }
    nsaa = (nsaa + (align - 1)) & ~(align - 1);
    pplan = (�ṹ param_plan) {nsaa, nsaa + size, &vtop[-i]};
    add_param_plan(plan, pplan, STACK_CLASS);
    nsaa += size; /* size already rounded up before */
  }
  ���� nsaa;
}

#������ add_param_plan

/* Copy parameters to their final destination (core reg, VFP reg or stack) for
   function call.

   nb_args: number of parameters the function take
   plan: the overall assignment plan for parameters
   todo: a bitmap indicating what core reg will hold a parameter

   Returns the number of SValue added by this function on the value stack */
��̬ �� copy_params(�� nb_args, �ṹ plan *plan, �� todo)
{
  �� size, align, r, i, nb_extra_sval = 0;
  �ṹ param_plan *pplan;
  �� pass = 0;

   /* Several constraints require parameters to be copied in a specific order:
      - structures are copied to the stack before being loaded in a reg;
      - floats loaded to an odd numbered VFP reg are first copied to the
        preceding even numbered VFP reg and then moved to the next VFP reg.

      It is thus important that:
      - structures assigned to core regs must be copied after parameters
        assigned to the stack but before structures assigned to VFP regs because
        a structure can lie partly in core registers and partly on the stack;
      - parameters assigned to the stack and all structures be copied before
        parameters assigned to a core reg since copying a parameter to the stack
        require using a core reg;
      - parameters assigned to VFP regs be copied before structures assigned to
        VFP regs as the copy might use an even numbered VFP reg that already
        holds part of a structure. */
again:
  ����(i = 0; i < NB_CLASSES; i++) {
    ����(pplan = plan->clsplans[i]; pplan; pplan = pplan->prev) {

      �� (pass
          && (i != CORE_CLASS || pplan->sval->r < VT_CONST))
        ����;

      vpushv(pplan->sval);
      pplan->sval->r = pplan->sval->r2 = VT_CONST; /* disable entry */
      ת��(i) {
        ���� STACK_CLASS:
        ���� CORE_STRUCT_CLASS:
        ���� VFP_STRUCT_CLASS:
          �� ((pplan->sval->type.t & VT_BTYPE) == VT_STRUCT) {
            �� padding = 0;
            size = type_size(&pplan->sval->type, &align);
            /* align to stack align size */
            size = (size + 3) & ~3;
            �� (i == STACK_CLASS && pplan->prev)
              padding = pplan->start - pplan->prev->end;
            size += padding; /* Add padding if any */
            /* allocate the necessary size on stack */
            gadd_sp(-size);
            /* generate structure store */
            r = get_reg(RC_INT);
            o(0xE28D0000|(intr(r)<<12)|padding); /* add r, sp, padding */
            vset(&vtop->type, r | VT_LVAL, 0);
            vswap();
            vstore(); /* memcpy to current sp + potential padding */

            /* Homogeneous float aggregate are loaded to VFP registers
               immediately since there is no way of loading data in multiple
               non consecutive VFP registers as what is done for other
               structures (see the use of todo). */
            �� (i == VFP_STRUCT_CLASS) {
              �� first = pplan->start, nb = pplan->end - first + 1;
              /* vpop.32 {pplan->start, ..., pplan->end} */
              o(0xECBD0A00|(first&1)<<22|(first>>1)<<12|nb);
              /* No need to write the register used to a SValue since VFP regs
                 cannot be used for gcall_or_jmp */
            }
          } �� {
            �� (is_float(pplan->sval->type.t)) {
#�綨�� TCC_ARM_VFP
              r = vfpr(gv(RC_FLOAT)) << 12;
              �� ((pplan->sval->type.t & VT_BTYPE) == VT_FLOAT)
                size = 4;
              �� {
                size = 8;
                r |= 0x101; /* vpush.32 -> vpush.64 */
              }
              o(0xED2D0A01 + r); /* vpush */
#��
              r = fpr(gv(RC_FLOAT)) << 12;
              �� ((pplan->sval->type.t & VT_BTYPE) == VT_FLOAT)
                size = 4;
              �� �� ((pplan->sval->type.t & VT_BTYPE) == VT_DOUBLE)
                size = 8;
              ��
                size = LDOUBLE_SIZE;

              �� (size == 12)
                r |= 0x400000;
              �� ��(size == 8)
                r|=0x8000;

              o(0xED2D0100|r|(size>>2)); /* some kind of vpush for FPA */
#����
            } �� {
              /* simple type (currently always same size) */
              /* XXX: implicit cast ? */
              size=4;
              �� ((pplan->sval->type.t & VT_BTYPE) == VT_LLONG) {
                lexpand_nr();
                size = 8;
                r = gv(RC_INT);
                o(0xE52D0004|(intr(r)<<12)); /* push r */
                vtop--;
              }
              r = gv(RC_INT);
              o(0xE52D0004|(intr(r)<<12)); /* push r */
            }
            �� (i == STACK_CLASS && pplan->prev)
              gadd_sp(pplan->prev->end - pplan->start); /* Add padding if any */
          }
          ����;

        ���� VFP_CLASS:
          gv(regmask(TREG_F0 + (pplan->start >> 1)));
          �� (pplan->start & 1) { /* Must be in upper part of double register */
            o(0xEEF00A40|((pplan->start>>1)<<12)|(pplan->start>>1)); /* vmov.f32 s(n+1), sn */
            vtop->r = VT_CONST; /* avoid being saved on stack by gv for next float */
          }
          ����;

        ���� CORE_CLASS:
          �� ((pplan->sval->type.t & VT_BTYPE) == VT_LLONG) {
            lexpand_nr();
            gv(regmask(pplan->end));
            pplan->sval->r2 = vtop->r;
            vtop--;
          }
          gv(regmask(pplan->start));
          /* Mark register as used so that gcall_or_jmp use another one
             (regs >=4 are free as never used to pass parameters) */
          pplan->sval->r = vtop->r;
          ����;
      }
      vtop--;
    }
  }

  /* second pass to restore registers that were saved on stack by accident.
     Maybe redundant after the "lvalue_save" patch in tccgen.c:gv() */
  �� (++pass < 2)
    ��ת again;

  /* Manually free remaining registers since next parameters are loaded
   * manually, without the help of gv(int). */
  save_regs(nb_args);

  ��(todo) {
    o(0xE8BD0000|todo); /* pop {todo} */
    ����(pplan = plan->clsplans[CORE_STRUCT_CLASS]; pplan; pplan = pplan->prev) {
      �� r;
      pplan->sval->r = pplan->start;
      /* An SValue can only pin 2 registers at best (r and r2) but a structure
         can occupy more than 2 registers. Thus, we need to push on the value
         stack some fake parameter to have on SValue for each registers used
         by a structure (r2 is not used). */
      ���� (r = pplan->start + 1; r <= pplan->end; r++) {
        �� (todo & (1 << r)) {
          nb_extra_sval++;
          vpushi(0);
          vtop->r = r;
        }
      }
    }
  }
  ���� nb_extra_sval;
}

/* Generate function call. The function address is pushed first, then
   all the parameters in call order. This functions pops all the
   parameters and the function address. */
�� gfunc_call(�� nb_args)
{
  �� r, args_size;
  �� def_float_abi = float_abi;
  �� todo;
  �ṹ plan plan;

#�綨�� TCC_ARM_EABI
  �� variadic;

  �� (float_abi == ARM_HARD_FLOAT) {
    variadic = (vtop[-nb_args].type.ref->f.func_type == FUNC_ELLIPSIS);
    �� (variadic || floats_in_core_regs(&vtop[-nb_args]))
      float_abi = ARM_SOFTFP_FLOAT;
  }
#����
  /* cannot let cpu flags if other instruction are generated. Also avoid leaving
     VT_JMP anywhere except on the top of the stack because it would complicate
     the code generator. */
  r = vtop->r & VT_VALMASK;
  �� (r == VT_CMP || (r & ~1) == VT_JMP)
    gv(RC_INT);

  args_size = assign_regs(nb_args, float_abi, &plan, &todo);

#�綨�� TCC_ARM_EABI
  �� (args_size & 7) { /* Stack must be 8 byte aligned at fct call for EABI */
    args_size = (args_size + 7) & ~7;
    o(0xE24DD004); /* sub sp, sp, #4 */
  }
#����

  nb_args += copy_params(nb_args, &plan, todo);
  tcc_free(plan.pplans);

  /* Move fct SValue on top as required by gcall_or_jmp */
  vrotb(nb_args + 1);
  gcall_or_jmp(0);
  �� (args_size)
      gadd_sp(args_size); /* pop all parameters passed on the stack */
#�� �Ѷ���(TCC_ARM_EABI) && �Ѷ���(TCC_ARM_VFP)
  ��(float_abi == ARM_SOFTFP_FLOAT && is_float(vtop->type.ref->type.t)) {
    ��((vtop->type.ref->type.t & VT_BTYPE) == VT_FLOAT) {
      o(0xEE000A10); /*vmov s0, r0 */
    } �� {
      o(0xEE000B10); /* vmov.32 d0[0], r0 */
      o(0xEE201B10); /* vmov.32 d0[1], r1 */
    }
  }
#����
  vtop -= nb_args + 1; /* Pop all params and fct address from value stack */
  leaffunc = 0; /* we are calling a function, so we aren't in a leaf function */
  float_abi = def_float_abi;
}

/* generate function prolog of type 't' */
�� gfunc_prolog(CType *func_type)
{
  Sym *sym,*sym2;
  �� n, nf, size, align, rs, struct_ret = 0;
  �� addr, pn, sn; /* pn=core, sn=stack */
  CType ret_type;

#�綨�� TCC_ARM_EABI
  �ṹ avail_regs avregs = AVAIL_REGS_INITIALIZER;
#����

  sym = func_type->ref;
  func_vt = sym->type;
  func_var = (func_type->ref->f.func_type == FUNC_ELLIPSIS);

  n = nf = 0;
  �� ((func_vt.t & VT_BTYPE) == VT_STRUCT &&
      !gfunc_sret(&func_vt, func_var, &ret_type, &align, &rs))
  {
    n++;
    struct_ret = 1;
    func_vc = 12; /* Offset from fp of the place to store the result */
  }
  ����(sym2 = sym->next; sym2 && (n < 4 || nf < 16); sym2 = sym2->next) {
    size = type_size(&sym2->type, &align);
#�綨�� TCC_ARM_EABI
    �� (float_abi == ARM_HARD_FLOAT && !func_var &&
        (is_float(sym2->type.t) || is_hgen_float_aggr(&sym2->type))) {
      �� tmpnf = assign_vfpreg(&avregs, align, size);
      tmpnf += (size + 3) / 4;
      nf = (tmpnf > nf) ? tmpnf : nf;
    } ��
#����
    �� (n < 4)
      n += (size + 3) / 4;
  }
  o(0xE1A0C00D); /* mov ip,sp */
  �� (func_var)
    n=4;
  �� (n) {
    ��(n>4)
      n=4;
#�綨�� TCC_ARM_EABI
    n=(n+1)&-2;
#����
    o(0xE92D0000|((1<<n)-1)); /* save r0-r4 on stack if needed */
  }
  �� (nf) {
    �� (nf>16)
      nf=16;
    nf=(nf+1)&-2; /* nf => HARDFLOAT => EABI */
    o(0xED2D0A00|nf); /* save s0-s15 on stack if needed */
  }
  o(0xE92D5800); /* save fp, ip, lr */
  o(0xE1A0B00D); /* mov fp, sp */
  func_sub_sp_offset = ind;
  o(0xE1A00000); /* nop, leave space for stack adjustment in epilog */

#�綨�� TCC_ARM_EABI
  �� (float_abi == ARM_HARD_FLOAT) {
    func_vc += nf * 4;
    avregs = AVAIL_REGS_INITIALIZER;
  }
#����
  pn = struct_ret, sn = 0;
  �� ((sym = sym->next)) {
    CType *type;
    type = &sym->type;
    size = type_size(type, &align);
    size = (size + 3) >> 2;
    align = (align + 3) & ~3;
#�綨�� TCC_ARM_EABI
    �� (float_abi == ARM_HARD_FLOAT && !func_var && (is_float(sym->type.t)
        || is_hgen_float_aggr(&sym->type))) {
      �� fpn = assign_vfpreg(&avregs, align, size << 2);
      �� (fpn >= 0)
        addr = fpn * 4;
      ��
        ��ת from_stack;
    } ��
#����
    �� (pn < 4) {
#�綨�� TCC_ARM_EABI
        pn = (pn + (align-1)/4) & -(align/4);
#����
      addr = (nf + pn) * 4;
      pn += size;
      �� (!sn && pn > 4)
        sn = (pn - 4);
    } �� {
#�綨�� TCC_ARM_EABI
from_stack:
        sn = (sn + (align-1)/4) & -(align/4);
#����
      addr = (n + nf + sn) * 4;
      sn += size;
    }
    sym_push(sym->v & ~SYM_FIELD, type, VT_LOCAL | lvalue_type(type->t),
             addr + 12);
  }
  last_itod_magic=0;
  leaffunc = 1;
  loc = 0;
}

/* generate function epilog */
�� gfunc_epilog(��)
{
  uint32_t x;
  �� diff;
  /* Copy float return value to core register if base standard is used and
     float computation is made with VFP */
#�� �Ѷ���(TCC_ARM_EABI) && �Ѷ���(TCC_ARM_VFP)
  �� ((float_abi == ARM_SOFTFP_FLOAT || func_var) && is_float(func_vt.t)) {
    ��((func_vt.t & VT_BTYPE) == VT_FLOAT)
      o(0xEE100A10); /* fmrs r0, s0 */
    �� {
      o(0xEE100B10); /* fmrdl r0, d0 */
      o(0xEE301B10); /* fmrdh r1, d0 */
    }
  }
#����
  o(0xE89BA800); /* restore fp, sp, pc */
  diff = (-loc + 3) & -4;
#�綨�� TCC_ARM_EABI
  ��(!leaffunc)
    diff = ((diff + 11) & -8) - 4;
#����
  ��(diff > 0) {
    x=stuff_const(0xE24BD000, diff); /* sub sp,fp,# */
    ��(x)
      *(uint32_t *)(cur_text_section->data + func_sub_sp_offset) = x;
    �� {
      �� addr;
      addr=ind;
      o(0xE59FC004); /* ldr ip,[pc+4] */
      o(0xE04BD00C); /* sub sp,fp,ip  */
      o(0xE1A0F00E); /* mov pc,lr */
      o(diff);
      *(uint32_t *)(cur_text_section->data + func_sub_sp_offset) = 0xE1000000|encbranch(func_sub_sp_offset,addr,1);
    }
  }
}

/* generate a jump to a label */
�� gjmp(�� t)
{
  �� r;
  �� (nocode_wanted)
    ���� t;
  r=ind;
  o(0xE0000000|encbranch(r,t,1));
  ���� r;
}

/* generate a jump to a fixed address */
�� gjmp_addr(�� a)
{
  gjmp(a);
}

/* generate a test. set 'inv' to invert test. Stack entry is popped */
�� gtst(�� inv, �� t)
{
  �� v, r;
  uint32_t op;

  v = vtop->r & VT_VALMASK;
  r=ind;

  �� (nocode_wanted) {
    ;
  } �� �� (v == VT_CMP) {
    op=mapcc(inv?negcc(vtop->c.i):vtop->c.i);
    op|=encbranch(r,t,1);
    o(op);
    t=r;
  } �� �� (v == VT_JMP || v == VT_JMPI) {
    �� ((v & 1) == inv) {
      ��(!vtop->c.i)
        vtop->c.i=t;
      �� {
        uint32_t *x;
        �� p,lp;
        ��(t) {
          p = vtop->c.i;
          ���� {
            p = decbranch(lp=p);
          } ��(p);
          x = (uint32_t *)(cur_text_section->data + lp);
          *x &= 0xff000000;
          *x |= encbranch(lp,t,1);
        }
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
�� gen_opi(�� op)
{
  �� c, func = 0;
  uint32_t opc = 0, r, fr;
  �޷� �� retreg = REG_IRET;

  c=0;
  ת��(op) {
    ���� '+':
      opc = 0x8;
      c=1;
      ����;
    ���� TOK_ADDC1: /* add with carry generation */
      opc = 0x9;
      c=1;
      ����;
    ���� '-':
      opc = 0x4;
      c=1;
      ����;
    ���� TOK_SUBC1: /* sub with carry generation */
      opc = 0x5;
      c=1;
      ����;
    ���� TOK_ADDC2: /* add with carry use */
      opc = 0xA;
      c=1;
      ����;
    ���� TOK_SUBC2: /* sub with carry use */
      opc = 0xC;
      c=1;
      ����;
    ���� '&':
      opc = 0x0;
      c=1;
      ����;
    ���� '^':
      opc = 0x2;
      c=1;
      ����;
    ���� '|':
      opc = 0x18;
      c=1;
      ����;
    ���� '*':
      gv2(RC_INT, RC_INT);
      r = vtop[-1].r;
      fr = vtop[0].r;
      vtop--;
      o(0xE0000090|(intr(r)<<16)|(intr(r)<<8)|intr(fr));
      ����;
    ���� TOK_SHL:
      opc = 0;
      c=2;
      ����;
    ���� TOK_SHR:
      opc = 1;
      c=2;
      ����;
    ���� TOK_SAR:
      opc = 2;
      c=2;
      ����;
    ���� '/':
    ���� TOK_PDIV:
      func=TOK___divsi3;
      c=3;
      ����;
    ���� TOK_UDIV:
      func=TOK___udivsi3;
      c=3;
      ����;
    ���� '%':
#�綨�� TCC_ARM_EABI
      func=TOK___aeabi_idivmod;
      retreg=REG_LRET;
#��
      func=TOK___modsi3;
#����
      c=3;
      ����;
    ���� TOK_UMOD:
#�綨�� TCC_ARM_EABI
      func=TOK___aeabi_uidivmod;
      retreg=REG_LRET;
#��
      func=TOK___umodsi3;
#����
      c=3;
      ����;
    ���� TOK_UMULL:
      gv2(RC_INT, RC_INT);
      r=intr(vtop[-1].r2=get_reg(RC_INT));
      c=vtop[-1].r;
      vtop[-1].r=get_reg_ex(RC_INT,regmask(c));
      vtop--;
      o(0xE0800090|(r<<16)|(intr(vtop->r)<<12)|(intr(c)<<8)|intr(vtop[1].r));
      ����;
    ȱʡ:
      opc = 0x15;
      c=1;
      ����;
  }
  ת��(c) {
    ���� 1:
      ��((vtop[-1].r & (VT_VALMASK | VT_LVAL | VT_SYM)) == VT_CONST) {
        ��(opc == 4 || opc == 5 || opc == 0xc) {
          vswap();
          opc|=2; // sub -> rsb
        }
      }
      �� ((vtop->r & VT_VALMASK) == VT_CMP ||
          (vtop->r & (VT_VALMASK & ~1)) == VT_JMP)
        gv(RC_INT);
      vswap();
      c=intr(gv(RC_INT));
      vswap();
      opc=0xE0000000|(opc<<20)|(c<<16);
      ��((vtop->r & (VT_VALMASK | VT_LVAL | VT_SYM)) == VT_CONST) {
        uint32_t x;
        x=stuff_const(opc|0x2000000,vtop->c.i);
        ��(x) {
          r=intr(vtop[-1].r=get_reg_ex(RC_INT,regmask(vtop[-1].r)));
          o(x|(r<<12));
          ��ת done;
        }
      }
      fr=intr(gv(RC_INT));
      r=intr(vtop[-1].r=get_reg_ex(RC_INT,two2mask(vtop->r,vtop[-1].r)));
      o(opc|(r<<12)|fr);
done:
      vtop--;
      �� (op >= TOK_ULT && op <= TOK_GT) {
        vtop->r = VT_CMP;
        vtop->c.i = op;
      }
      ����;
    ���� 2:
      opc=0xE1A00000|(opc<<5);
      �� ((vtop->r & VT_VALMASK) == VT_CMP ||
          (vtop->r & (VT_VALMASK & ~1)) == VT_JMP)
        gv(RC_INT);
      vswap();
      r=intr(gv(RC_INT));
      vswap();
      opc|=r;
      �� ((vtop->r & (VT_VALMASK | VT_LVAL | VT_SYM)) == VT_CONST) {
        fr=intr(vtop[-1].r=get_reg_ex(RC_INT,regmask(vtop[-1].r)));
        c = vtop->c.i & 0x1f;
        o(opc|(c<<7)|(fr<<12));
      } �� {
        fr=intr(gv(RC_INT));
        c=intr(vtop[-1].r=get_reg_ex(RC_INT,two2mask(vtop->r,vtop[-1].r)));
        o(opc|(c<<12)|(fr<<8)|0x10);
      }
      vtop--;
      ����;
    ���� 3:
      vpush_global_sym(&func_old_type, func);
      vrott(3);
      gfunc_call(2);
      vpushi(0);
      vtop->r = retreg;
      ����;
    ȱʡ:
      tcc_error("gen_opi %i unimplemented!",op);
  }
}

#�綨�� TCC_ARM_VFP
��̬ �� is_zero(�� i)
{
  ��((vtop[i].r & (VT_VALMASK | VT_LVAL | VT_SYM)) != VT_CONST)
    ���� 0;
  �� (vtop[i].type.t == VT_FLOAT)
    ���� (vtop[i].c.f == 0.f);
  �� �� (vtop[i].type.t == VT_DOUBLE)
    ���� (vtop[i].c.d == 0.0);
  ���� (vtop[i].c.ld == 0.l);
}

/* generate a floating point operation 'v = t1 op t2' instruction. The
 *    two operands are guaranteed to have the same floating point type */
�� gen_opf(�� op)
{
  uint32_t x;
  �� fneg=0,r;
  x=0xEE000A00|T2CPR(vtop->type.t);
  ת��(op) {
    ���� '+':
      ��(is_zero(-1))
        vswap();
      ��(is_zero(0)) {
        vtop--;
        ����;
      }
      x|=0x300000;
      ����;
    ���� '-':
      x|=0x300040;
      ��(is_zero(0)) {
        vtop--;
        ����;
      }
      ��(is_zero(-1)) {
        x|=0x810000; /* fsubX -> fnegX */
        vswap();
        vtop--;
        fneg=1;
      }
      ����;
    ���� '*':
      x|=0x200000;
      ����;
    ���� '/':
      x|=0x800000;
      ����;
    ȱʡ:
      ��(op < TOK_ULT || op > TOK_GT) {
        tcc_error("unknown fp op %x!",op);
        ����;
      }
      ��(is_zero(-1)) {
        vswap();
        ת��(op) {
          ���� TOK_LT: op=TOK_GT; ����;
          ���� TOK_GE: op=TOK_ULE; ����;
          ���� TOK_LE: op=TOK_GE; ����;
          ���� TOK_GT: op=TOK_ULT; ����;
        }
      }
      x|=0xB40040; /* fcmpX */
      ��(op!=TOK_EQ && op!=TOK_NE)
        x|=0x80; /* fcmpX -> fcmpeX */
      ��(is_zero(0)) {
        vtop--;
        o(x|0x10000|(vfpr(gv(RC_FLOAT))<<12)); /* fcmp(e)X -> fcmp(e)zX */
      } �� {
        x|=vfpr(gv(RC_FLOAT));
        vswap();
        o(x|(vfpr(gv(RC_FLOAT))<<12));
        vtop--;
      }
      o(0xEEF1FA10); /* fmstat */

      ת��(op) {
        ���� TOK_LE: op=TOK_ULE; ����;
        ���� TOK_LT: op=TOK_ULT; ����;
        ���� TOK_UGE: op=TOK_GE; ����;
        ���� TOK_UGT: op=TOK_GT; ����;
      }

      vtop->r = VT_CMP;
      vtop->c.i = op;
      ����;
  }
  r=gv(RC_FLOAT);
  x|=vfpr(r);
  r=regmask(r);
  ��(!fneg) {
    �� r2;
    vswap();
    r2=gv(RC_FLOAT);
    x|=vfpr(r2)<<16;
    r|=regmask(r2);
  }
  vtop->r=get_reg_ex(RC_FLOAT,r);
  ��(!fneg)
    vtop--;
  o(x|(vfpr(vtop->r)<<12));
}

#��
��̬ uint32_t is_fconst()
{
  �� ˫�� f;
  uint32_t r;
  ��((vtop->r & (VT_VALMASK | VT_LVAL | VT_SYM)) != VT_CONST)
    ���� 0;
  �� (vtop->type.t == VT_FLOAT)
    f = vtop->c.f;
  �� �� (vtop->type.t == VT_DOUBLE)
    f = vtop->c.d;
  ��
    f = vtop->c.ld;
  ��(!ieee_finite(f))
    ���� 0;
  r=0x8;
  ��(f<0.0) {
    r=0x18;
    f=-f;
  }
  ��(f==0.0)
    ���� r;
  ��(f==1.0)
    ���� r|1;
  ��(f==2.0)
    ���� r|2;
  ��(f==3.0)
    ���� r|3;
  ��(f==4.0)
    ���� r|4;
  ��(f==5.0)
    ���� r|5;
  ��(f==0.5)
    ���� r|6;
  ��(f==10.0)
    ���� r|7;
  ���� 0;
}

/* generate a floating point operation 'v = t1 op t2' instruction. The
   two operands are guaranted to have the same floating point type */
�� gen_opf(�� op)
{
  uint32_t x, r, r2, c1, c2;
  //fputs("gen_opf\n",stderr);
  vswap();
  c1 = is_fconst();
  vswap();
  c2 = is_fconst();
  x=0xEE000100;
#�� LDOUBLE_SIZE == 8
  �� ((vtop->type.t & VT_BTYPE) != VT_FLOAT)
    x|=0x80;
#��
  �� ((vtop->type.t & VT_BTYPE) == VT_DOUBLE)
    x|=0x80;
  �� �� ((vtop->type.t & VT_BTYPE) == VT_LDOUBLE)
    x|=0x80000;
#����
  ת��(op)
  {
    ���� '+':
      ��(!c2) {
        vswap();
        c2=c1;
      }
      vswap();
      r=fpr(gv(RC_FLOAT));
      vswap();
      ��(c2) {
        ��(c2>0xf)
          x|=0x200000; // suf
        r2=c2&0xf;
      } �� {
        r2=fpr(gv(RC_FLOAT));
      }
      ����;
    ���� '-':
      ��(c2) {
        ��(c2<=0xf)
          x|=0x200000; // suf
        r2=c2&0xf;
        vswap();
        r=fpr(gv(RC_FLOAT));
        vswap();
      } �� ��(c1 && c1<=0xf) {
        x|=0x300000; // rsf
        r2=c1;
        r=fpr(gv(RC_FLOAT));
        vswap();
      } �� {
        x|=0x200000; // suf
        vswap();
        r=fpr(gv(RC_FLOAT));
        vswap();
        r2=fpr(gv(RC_FLOAT));
      }
      ����;
    ���� '*':
      ��(!c2 || c2>0xf) {
        vswap();
        c2=c1;
      }
      vswap();
      r=fpr(gv(RC_FLOAT));
      vswap();
      ��(c2 && c2<=0xf)
        r2=c2;
      ��
        r2=fpr(gv(RC_FLOAT));
      x|=0x100000; // muf
      ����;
    ���� '/':
      ��(c2 && c2<=0xf) {
        x|=0x400000; // dvf
        r2=c2;
        vswap();
        r=fpr(gv(RC_FLOAT));
        vswap();
      } �� ��(c1 && c1<=0xf) {
        x|=0x500000; // rdf
        r2=c1;
        r=fpr(gv(RC_FLOAT));
        vswap();
      } �� {
        x|=0x400000; // dvf
        vswap();
        r=fpr(gv(RC_FLOAT));
        vswap();
        r2=fpr(gv(RC_FLOAT));
      }
      ����;
    ȱʡ:
      ��(op >= TOK_ULT && op <= TOK_GT) {
        x|=0xd0f110; // cmfe
/* bug (intention?) in Linux FPU emulator
   doesn't set carry if equal */
        ת��(op) {
          ���� TOK_ULT:
          ���� TOK_UGE:
          ���� TOK_ULE:
          ���� TOK_UGT:
            tcc_error("unsigned comparison on floats?");
            ����;
          ���� TOK_LT:
            op=TOK_Nset;
            ����;
          ���� TOK_LE:
            op=TOK_ULE; /* correct in unordered case only if AC bit in FPSR set */
            ����;
          ���� TOK_EQ:
          ���� TOK_NE:
            x&=~0x400000; // cmfe -> cmf
            ����;
        }
        ��(c1 && !c2) {
          c2=c1;
          vswap();
          ת��(op) {
            ���� TOK_Nset:
              op=TOK_GT;
              ����;
            ���� TOK_GE:
              op=TOK_ULE;
              ����;
            ���� TOK_ULE:
              op=TOK_GE;
              ����;
            ���� TOK_GT:
              op=TOK_Nset;
              ����;
          }
        }
        vswap();
        r=fpr(gv(RC_FLOAT));
        vswap();
        ��(c2) {
          ��(c2>0xf)
            x|=0x200000;
          r2=c2&0xf;
        } �� {
          r2=fpr(gv(RC_FLOAT));
        }
        vtop[-1].r = VT_CMP;
        vtop[-1].c.i = op;
      } �� {
        tcc_error("unknown fp op %x!",op);
        ����;
      }
  }
  ��(vtop[-1].r == VT_CMP)
    c1=15;
  �� {
    c1=vtop->r;
    ��(r2&0x8)
      c1=vtop[-1].r;
    vtop[-1].r=get_reg_ex(RC_FLOAT,two2mask(vtop[-1].r,c1));
    c1=fpr(vtop[-1].r);
  }
  vtop--;
  o(x|(r<<16)|(c1<<12)|r2);
}
#����

/* convert integers to fp 't' type. Must handle 'int', 'unsigned int'
   and 'long long' cases. */
ST_FUNC �� gen_cvt_itof1(�� t)
{
  uint32_t r, r2;
  �� bt;
  bt=vtop->type.t & VT_BTYPE;
  ��(bt == VT_INT || bt == VT_SHORT || bt == VT_BYTE) {
#��δ���� TCC_ARM_VFP
    uint32_t dsize = 0;
#����
    r=intr(gv(RC_INT));
#�綨�� TCC_ARM_VFP
    r2=vfpr(vtop->r=get_reg(RC_FLOAT));
    o(0xEE000A10|(r<<12)|(r2<<16)); /* fmsr */
    r2|=r2<<12;
    ��(!(vtop->type.t & VT_UNSIGNED))
      r2|=0x80;                /* fuitoX -> fsituX */
    o(0xEEB80A40|r2|T2CPR(t)); /* fYitoX*/
#��
    r2=fpr(vtop->r=get_reg(RC_FLOAT));
    ��((t & VT_BTYPE) != VT_FLOAT)
      dsize=0x80;    /* flts -> fltd */
    o(0xEE000110|dsize|(r2<<16)|(r<<12)); /* flts */
    ��((vtop->type.t & (VT_UNSIGNED|VT_BTYPE)) == (VT_UNSIGNED|VT_INT)) {
      uint32_t off = 0;
      o(0xE3500000|(r<<12));        /* cmp */
      r=fpr(get_reg(RC_FLOAT));
      ��(last_itod_magic) {
        off=ind+8-last_itod_magic;
        off/=4;
        ��(off>255)
          off=0;
      }
      o(0xBD1F0100|(r<<12)|off);    /* ldflts */
      ��(!off) {
        o(0xEA000000);              /* b */
        last_itod_magic=ind;
        o(0x4F800000);              /* 4294967296.0f */
      }
      o(0xBE000100|dsize|(r2<<16)|(r2<<12)|r); /* adflt */
    }
#����
    ����;
  } �� ��(bt == VT_LLONG) {
    �� func;
    CType *func_type = 0;
    ��((t & VT_BTYPE) == VT_FLOAT) {
      func_type = &func_float_type;
      ��(vtop->type.t & VT_UNSIGNED)
        func=TOK___floatundisf;
      ��
        func=TOK___floatdisf;
#�� LDOUBLE_SIZE != 8
    } �� ��((t & VT_BTYPE) == VT_LDOUBLE) {
      func_type = &func_ldouble_type;
      ��(vtop->type.t & VT_UNSIGNED)
        func=TOK___floatundixf;
      ��
        func=TOK___floatdixf;
    } �� ��((t & VT_BTYPE) == VT_DOUBLE) {
#��
    } �� ��((t & VT_BTYPE) == VT_DOUBLE || (t & VT_BTYPE) == VT_LDOUBLE) {
#����
      func_type = &func_double_type;
      ��(vtop->type.t & VT_UNSIGNED)
        func=TOK___floatundidf;
      ��
        func=TOK___floatdidf;
    }
    ��(func_type) {
      vpush_global_sym(func_type, func);
      vswap();
      gfunc_call(1);
      vpushi(0);
      vtop->r=TREG_F0;
      ����;
    }
  }
  tcc_error("unimplemented gen_cvt_itof %x!",vtop->type.t);
}

/* convert fp to int 't' type */
�� gen_cvt_ftoi(�� t)
{
  uint32_t r, r2;
  �� u, func = 0;
  u=t&VT_UNSIGNED;
  t&=VT_BTYPE;
  r2=vtop->type.t & VT_BTYPE;
  ��(t==VT_INT) {
#�綨�� TCC_ARM_VFP
    r=vfpr(gv(RC_FLOAT));
    u=u?0:0x10000;
    o(0xEEBC0AC0|(r<<12)|r|T2CPR(r2)|u); /* ftoXizY */
    r2=intr(vtop->r=get_reg(RC_INT));
    o(0xEE100A10|(r<<16)|(r2<<12));
    ����;
#��
    ��(u) {
      ��(r2 == VT_FLOAT)
        func=TOK___fixunssfsi;
#�� LDOUBLE_SIZE != 8
      �� ��(r2 == VT_LDOUBLE)
        func=TOK___fixunsxfsi;
      �� ��(r2 == VT_DOUBLE)
#��
      �� ��(r2 == VT_LDOUBLE || r2 == VT_DOUBLE)
#����
        func=TOK___fixunsdfsi;
    } �� {
      r=fpr(gv(RC_FLOAT));
      r2=intr(vtop->r=get_reg(RC_INT));
      o(0xEE100170|(r2<<12)|r);
      ����;
    }
#����
  } �� ��(t == VT_LLONG) { // unsigned handled in gen_cvt_ftoi1
    ��(r2 == VT_FLOAT)
      func=TOK___fixsfdi;
#�� LDOUBLE_SIZE != 8
    �� ��(r2 == VT_LDOUBLE)
      func=TOK___fixxfdi;
    �� ��(r2 == VT_DOUBLE)
#��
    �� ��(r2 == VT_LDOUBLE || r2 == VT_DOUBLE)
#����
      func=TOK___fixdfdi;
  }
  ��(func) {
    vpush_global_sym(&func_old_type, func);
    vswap();
    gfunc_call(1);
    vpushi(0);
    ��(t == VT_LLONG)
      vtop->r2 = REG_LRET;
    vtop->r = REG_IRET;
    ����;
  }
  tcc_error("unimplemented gen_cvt_ftoi!");
}

/* convert from one floating point type to another */
�� gen_cvt_ftof(�� t)
{
#�綨�� TCC_ARM_VFP
  ��(((vtop->type.t & VT_BTYPE) == VT_FLOAT) != ((t & VT_BTYPE) == VT_FLOAT)) {
    uint32_t r = vfpr(gv(RC_FLOAT));
    o(0xEEB70AC0|(r<<12)|r|T2CPR(vtop->type.t));
  }
#��
  /* all we have to do on i386 and FPA ARM is to put the float in a register */
  gv(RC_FLOAT);
#����
}

/* computed goto support */
�� ggoto(��)
{
  gcall_or_jmp(1);
  vtop--;
}

/* Save the stack pointer onto the stack and return the location of its address */
ST_FUNC �� gen_vla_sp_save(�� addr) {
    SValue v;
    v.type.t = VT_PTR;
    v.r = VT_LOCAL | VT_LVAL;
    v.c.i = addr;
    store(TREG_SP, &v);
}

/* Restore the SP from a location on the stack */
ST_FUNC �� gen_vla_sp_restore(�� addr) {
    SValue v;
    v.type.t = VT_PTR;
    v.r = VT_LOCAL | VT_LVAL;
    v.c.i = addr;
    load(TREG_SP, &v);
}

/* Subtract from the stack pointer, and push the resulting value onto the stack */
ST_FUNC �� gen_vla_alloc(CType *type, �� align) {
    �� r = intr(gv(RC_INT));
    o(0xE04D0000|(r<<12)|r); /* sub r, sp, r */
#�綨�� TCC_ARM_EABI
    �� (align < 8)
        align = 8;
#��
    �� (align < 4)
        align = 4;
#����
    �� (align & (align - 1))
        tcc_error("alignment is not a power of 2: %i", align);
    o(stuff_const(0xE3C0D000|(r<<16), align - 1)); /* bic sp, r, #align-1 */
    vpop();
}

/* end of ARM code generator */
/*************************************************************/
#����
/*************************************************************/
