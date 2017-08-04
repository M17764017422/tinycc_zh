/*
 *  TMS320C67xx code generator for TCC
 * 
 *  Copyright (c) 2001, 2002 Fabrice Bellard
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

/* #���� ASSEMBLY_LISTING_C67 */

/* number of available registers */
#���� NB_REGS            24

/* a register can belong to several classes. The classes must be
   sorted from more general to more precise (see gv2() code which does
   assumptions on it). */
#���� RC_INT     0x0001   /* generic integer register */
#���� RC_FLOAT   0x0002   /* generic float register */
#���� RC_EAX     0x0004
#���� RC_ST0     0x0008
#���� RC_ECX     0x0010
#���� RC_EDX     0x0020
#���� RC_INT_BSIDE  0x00000040    /* generic integer register  on b side */
#���� RC_C67_A4     0x00000100
#���� RC_C67_A5     0x00000200
#���� RC_C67_B4     0x00000400
#���� RC_C67_B5     0x00000800
#���� RC_C67_A6     0x00001000
#���� RC_C67_A7     0x00002000
#���� RC_C67_B6     0x00004000
#���� RC_C67_B7     0x00008000
#���� RC_C67_A8     0x00010000
#���� RC_C67_A9     0x00020000
#���� RC_C67_B8     0x00040000
#���� RC_C67_B9     0x00080000
#���� RC_C67_A10    0x00100000
#���� RC_C67_A11    0x00200000
#���� RC_C67_B10    0x00400000
#���� RC_C67_B11    0x00800000
#���� RC_C67_A12    0x01000000
#���� RC_C67_A13    0x02000000
#���� RC_C67_B12    0x04000000
#���� RC_C67_B13    0x08000000
#���� RC_IRET    RC_C67_A4        /* function return: integer register */
#���� RC_LRET    RC_C67_A5        /* function return: second integer register */
#���� RC_FRET    RC_C67_A4        /* function return: float register */

/* pretty names for the registers */
ö�� {
    TREG_EAX = 0,               // really A2
    TREG_ECX,                   // really A3
    TREG_EDX,                   // really B0
    TREG_ST0,                   // really B1
    TREG_C67_A4,
    TREG_C67_A5,
    TREG_C67_B4,
    TREG_C67_B5,
    TREG_C67_A6,
    TREG_C67_A7,
    TREG_C67_B6,
    TREG_C67_B7,
    TREG_C67_A8,
    TREG_C67_A9,
    TREG_C67_B8,
    TREG_C67_B9,
    TREG_C67_A10,
    TREG_C67_A11,
    TREG_C67_B10,
    TREG_C67_B11,
    TREG_C67_A12,
    TREG_C67_A13,
    TREG_C67_B12,
    TREG_C67_B13,
};

/* return registers for function */
#���� REG_IRET TREG_C67_A4        /* single word int return register */
#���� REG_LRET TREG_C67_A5        /* second word return register (for long long) */
#���� REG_FRET TREG_C67_A4        /* float return register */

/* defined if function parameters must be evaluated in reverse order */
/* #���� INVERT_FUNC_PARAMS */

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

ST_DATA ���� �� reg_classes[NB_REGS] = {
    /* eax */ RC_INT | RC_FLOAT | RC_EAX, 
    // only allow even regs for floats (allow for doubles)
    /* ecx */ RC_INT | RC_ECX,
    /* edx */ RC_INT | RC_INT_BSIDE | RC_FLOAT | RC_EDX,
    // only allow even regs for floats (allow for doubles)
    /* st0 */ RC_INT | RC_INT_BSIDE | RC_ST0,
    /* A4  */ RC_C67_A4,
    /* A5  */ RC_C67_A5,
    /* B4  */ RC_C67_B4,
    /* B5  */ RC_C67_B5,
    /* A6  */ RC_C67_A6,
    /* A7  */ RC_C67_A7,
    /* B6  */ RC_C67_B6,
    /* B7  */ RC_C67_B7,
    /* A8  */ RC_C67_A8,
    /* A9  */ RC_C67_A9,
    /* B8  */ RC_C67_B8,
    /* B9  */ RC_C67_B9,
    /* A10  */ RC_C67_A10,
    /* A11  */ RC_C67_A11,
    /* B10  */ RC_C67_B10,
    /* B11  */ RC_C67_B11,
    /* A12  */ RC_C67_A10,
    /* A13  */ RC_C67_A11,
    /* B12  */ RC_C67_B10,
    /* B13  */ RC_C67_B11
};

// although tcc thinks it is passing parameters on the stack,
// the C67 really passes up to the first 10 params in special
// regs or regs pairs (for 64 bit params).  So keep track of
// the stack offsets so we can translate to the appropriate 
// reg (pair)

#���� NoCallArgsPassedOnStack 10
�� NoOfCurFuncArgs;
�� TranslateStackToReg[NoCallArgsPassedOnStack];
�� ParamLocOnStack[NoCallArgsPassedOnStack];
�� TotalBytesPushedOnStack;

#��δ���� FALSE
# ���� FALSE 0
# ���� TRUE 1
#����

#������ BOOL
#���� BOOL ��

#���� ALWAYS_ASSERT(x) \
���� {\
   �� (!(x))\
       tcc_error("internal compiler error file at %s:%d", __�ļ�__, __�к�__);\
} �� (0)

/******************************************************/
��̬ �޷� long func_sub_sp_offset;
��̬ �� func_ret_sub;

��̬ BOOL C67_invert_test;
��̬ �� C67_compare_reg;

#�綨�� ASSEMBLY_LISTING_C67
FILE *f = NULL;
#����

�� C67_g(�� c)
{
    �� ind1;
    �� (nocode_wanted)
        ����;
#�綨�� ASSEMBLY_LISTING_C67
    fprintf(f, " %08X", c);
#����
    ind1 = ind + 4;
    �� (ind1 > (��) cur_text_section->data_allocated)
        section_realloc(cur_text_section, ind1);
    cur_text_section->data[ind] = c & 0xff;
    cur_text_section->data[ind + 1] = (c >> 8) & 0xff;
    cur_text_section->data[ind + 2] = (c >> 16) & 0xff;
    cur_text_section->data[ind + 3] = (c >> 24) & 0xff;
    ind = ind1;
}


/* output a symbol and patch all calls to it */
�� gsym_addr(�� t, �� a)
{
    �� n, *ptr;
    �� (t) {
        ptr = (�� *) (cur_text_section->data + t);
        {
            Sym *sym;

            // extract 32 bit address from MVKH/MVKL
            n = ((*ptr >> 7) & 0xffff);
            n |= ((*(ptr + 1) >> 7) & 0xffff) << 16;

            // define a label that will be relocated

            sym = get_sym_ref(&char_pointer_type, cur_text_section, a, 0);
            greloc(cur_text_section, sym, t, R_C60LO16);
            greloc(cur_text_section, sym, t + 4, R_C60HI16);

            // clear out where the pointer was

            *ptr &= ~(0xffff << 7);
            *(ptr + 1) &= ~(0xffff << 7);
        }
        t = n;
    }
}

�� gsym(�� t)
{
    gsym_addr(t, ind);
}

// these are regs that tcc doesn't really know about, 
// but assign them unique values so the mapping routines
// can distinguish them

#���� C67_A0 105
#���� C67_SP 106
#���� C67_B3 107
#���� C67_FP 108
#���� C67_B2 109
#���� C67_CREG_ZERO -1    /* Special code for no condition reg test */


�� ConvertRegToRegClass(�� r)
{
    // only works for A4-B13

    ���� RC_C67_A4 << (r - TREG_C67_A4);
}


// map TCC reg to C67 reg number

�� C67_map_regn(�� r)
{
    �� (r == 0)                 // normal tcc regs
        ���� 0x2;             // A2
    �� �� (r == 1)            // normal tcc regs
        ���� 3;               // A3
    �� �� (r == 2)            // normal tcc regs
        ���� 0;               // B0
    �� �� (r == 3)            // normal tcc regs
        ���� 1;               // B1
    �� �� (r >= TREG_C67_A4 && r <= TREG_C67_B13)     // these form a pattern of alt pairs
        ���� (((r & 0xfffffffc) >> 1) | (r & 1)) + 2;
    �� �� (r == C67_A0)
        ���� 0;               // set to A0 (offset reg)
    �� �� (r == C67_B2)
        ���� 2;               // set to B2 (offset reg)
    �� �� (r == C67_B3)
        ���� 3;               // set to B3 (return address reg)
    �� �� (r == C67_SP)
        ���� 15;              // set to SP (B15) (offset reg)
    �� �� (r == C67_FP)
        ���� 15;              // set to FP (A15) (offset reg)
    �� �� (r == C67_CREG_ZERO)
        ���� 0;               // Special code for no condition reg test
    ��
        ALWAYS_ASSERT(FALSE);

    ���� 0;
}

// mapping from tcc reg number to 
// C67 register to condition code field
//
// valid condition code regs are:
//
// tcc reg 2 ->B0 -> 1
// tcc reg 3 ->B1 -> 2
// tcc reg 0 -> A2 -> 5
// tcc reg 1 -> A3 -> X
// tcc reg      B2 -> 3

�� C67_map_regc(�� r)
{
    �� (r == 0)                 // normal tcc regs
        ���� 0x5;
    �� �� (r == 2)            // normal tcc regs
        ���� 0x1;
    �� �� (r == 3)            // normal tcc regs
        ���� 0x2;
    �� �� (r == C67_B2)       // normal tcc regs
        ���� 0x3;
    �� �� (r == C67_CREG_ZERO)
        ���� 0;               // Special code for no condition reg test
    ��
        ALWAYS_ASSERT(FALSE);

    ���� 0;
}


// map TCC reg to C67 reg side A or B

�� C67_map_regs(�� r)
{
    �� (r == 0)                 // normal tcc regs
        ���� 0x0;
    �� �� (r == 1)            // normal tcc regs
        ���� 0x0;
    �� �� (r == 2)            // normal tcc regs
        ���� 0x1;
    �� �� (r == 3)            // normal tcc regs
        ���� 0x1;
    �� �� (r >= TREG_C67_A4 && r <= TREG_C67_B13)     // these form a pattern of alt pairs
        ���� (r & 2) >> 1;
    �� �� (r == C67_A0)
        ���� 0;               // set to A side 
    �� �� (r == C67_B2)
        ���� 1;               // set to B side 
    �� �� (r == C67_B3)
        ���� 1;               // set to B side
    �� �� (r == C67_SP)
        ���� 0x1;             // set to SP (B15) B side 
    �� �� (r == C67_FP)
        ���� 0x0;             // set to FP (A15) A side 
    ��
        ALWAYS_ASSERT(FALSE);

    ���� 0;
}

�� C67_map_S12(�� *s)
{
    �� (strstr(s, ".S1") != NULL)
        ���� 0;
    �� �� (strcmp(s, ".S2"))
        ���� 1;
    ��
        ALWAYS_ASSERT(FALSE);

    ���� 0;
}

�� C67_map_D12(�� *s)
{
    �� (strstr(s, ".D1") != NULL)
        ���� 0;
    �� �� (strcmp(s, ".D2"))
        ���� 1;
    ��
        ALWAYS_ASSERT(FALSE);

    ���� 0;
}



�� C67_asm(�� *s, �� a, �� b, �� c)
{
    BOOL xpath;

#�綨�� ASSEMBLY_LISTING_C67
    �� (!f) {
        f = fopen("TCC67_out.txt", "wt");
    }
    fprintf(f, "%04X ", ind);
#����

    �� (strstr(s, "MVKL") == s) {
        C67_g((C67_map_regn(b) << 23) |
              ((a & 0xffff) << 7) | (0x0a << 2) | (C67_map_regs(b) << 1));
    } �� �� (strstr(s, "MVKH") == s) {
        C67_g((C67_map_regn(b) << 23) |
              (((a >> 16) & 0xffff) << 7) |
              (0x1a << 2) | (C67_map_regs(b) << 1));
    } �� �� (strstr(s, "STW.D SP POST DEC") == s) {
        C67_g((C67_map_regn(a) << 23) | //src
              (15 << 18) |      //SP B15
              (2 << 13) |       //ucst5 (must keep 8 byte boundary !!)
              (0xa << 9) |      //mode a = post dec ucst
              (0 << 8) |        //r (LDDW bit 0)
              (1 << 7) |        //y D1/D2 use B side
              (7 << 4) |        //ldst 3=STB, 5=STH 5, 7=STW, 6=LDW 4=LDH 2=LDB 0=LDHU 1=LDBU
              (1 << 2) |        //opcode
              (C67_map_regs(a) << 1) |  //side of src
              (0 << 0));        //parallel
    } �� �� (strstr(s, "STB.D *+SP[A0]") == s) {
        C67_g((C67_map_regn(a) << 23) | //src
              (15 << 18) |      //base reg A15
              (0 << 13) |       //offset reg A0
              (5 << 9) |        //mode 5 = pos offset, base reg + off reg
              (0 << 8) |        //r (LDDW bit 0)
              (0 << 7) |        //y D1/D2 A side
              (3 << 4) |        //ldst 3=STB, 5=STH 5, 7=STW, 6=LDW 4=LDH 2=LDB 0=LDHU 1=LDBU 
              (1 << 2) |        //opcode
              (C67_map_regs(a) << 1) |  //side of src
              (0 << 0));        //parallel
    } �� �� (strstr(s, "STH.D *+SP[A0]") == s) {
        C67_g((C67_map_regn(a) << 23) | //src
              (15 << 18) |      //base reg A15
              (0 << 13) |       //offset reg A0
              (5 << 9) |        //mode 5 = pos offset, base reg + off reg
              (0 << 8) |        //r (LDDW bit 0)
              (0 << 7) |        //y D1/D2 A side
              (5 << 4) |        //ldst 3=STB, 5=STH 5, 7=STW, 6=LDW 4=LDH 2=LDB 0=LDHU 1=LDBU
              (1 << 2) |        //opcode
              (C67_map_regs(a) << 1) |  //side of src
              (0 << 0));        //parallel
    } �� �� (strstr(s, "STB.D *+SP[A0]") == s) {
        C67_g((C67_map_regn(a) << 23) | //src
              (15 << 18) |      //base reg A15
              (0 << 13) |       //offset reg A0
              (5 << 9) |        //mode 5 = pos offset, base reg + off reg
              (0 << 8) |        //r (LDDW bit 0)
              (0 << 7) |        //y D1/D2 A side
              (3 << 4) |        //ldst 3=STB, 5=STH 5, 7=STW, 6=LDW 4=LDH 2=LDB 0=LDHU 1=LDBU 
              (1 << 2) |        //opcode
              (C67_map_regs(a) << 1) |  //side of src
              (0 << 0));        //parallel
    } �� �� (strstr(s, "STH.D *+SP[A0]") == s) {
        C67_g((C67_map_regn(a) << 23) | //src
              (15 << 18) |      //base reg A15
              (0 << 13) |       //offset reg A0
              (5 << 9) |        //mode 5 = pos offset, base reg + off reg
              (0 << 8) |        //r (LDDW bit 0)
              (0 << 7) |        //y D1/D2 A side
              (5 << 4) |        //ldst 3=STB, 5=STH 5, 7=STW, 6=LDW 4=LDH 2=LDB 0=LDHU 1=LDBU
              (1 << 2) |        //opcode
              (C67_map_regs(a) << 1) |  //side of src
              (0 << 0));        //parallel
    } �� �� (strstr(s, "STW.D *+SP[A0]") == s) {
        C67_g((C67_map_regn(a) << 23) | //src
              (15 << 18) |      //base reg A15
              (0 << 13) |       //offset reg A0
              (5 << 9) |        //mode 5 = pos offset, base reg + off reg
              (0 << 8) |        //r (LDDW bit 0)
              (0 << 7) |        //y D1/D2 A side
              (7 << 4) |        //ldst 3=STB, 5=STH 5, 7=STW, 6=LDW 4=LDH 2=LDB 0=LDHU 1=LDBU 
              (1 << 2) |        //opcode
              (C67_map_regs(a) << 1) |  //side of src
              (0 << 0));        //parallel
    } �� �� (strstr(s, "STW.D *") == s) {
        C67_g((C67_map_regn(a) << 23) | //src
              (C67_map_regn(b) << 18) | //base reg A0
              (0 << 13) |       //cst5
              (1 << 9) |        //mode 1 = pos cst offset
              (0 << 8) |        //r (LDDW bit 0)
              (C67_map_regs(b) << 7) |  //y D1/D2 base reg side
              (7 << 4) |        //ldst 3=STB, 5=STH 5, 7=STW, 6=LDW 4=LDH 2=LDB 0=LDHU 1=LDBU 
              (1 << 2) |        //opcode
              (C67_map_regs(a) << 1) |  //side of src
              (0 << 0));        //parallel
    } �� �� (strstr(s, "STH.D *") == s) {
        C67_g((C67_map_regn(a) << 23) | //src
              (C67_map_regn(b) << 18) | //base reg A0
              (0 << 13) |       //cst5
              (1 << 9) |        //mode 1 = pos cst offset
              (0 << 8) |        //r (LDDW bit 0)
              (C67_map_regs(b) << 7) |  //y D1/D2 base reg side
              (5 << 4) |        //ldst 3=STB, 5=STH 5, 7=STW, 6=LDW 4=LDH 2=LDB 0=LDHU 1=LDBU 
              (1 << 2) |        //opcode
              (C67_map_regs(a) << 1) |  //side of src
              (0 << 0));        //parallel
    } �� �� (strstr(s, "STB.D *") == s) {
        C67_g((C67_map_regn(a) << 23) | //src
              (C67_map_regn(b) << 18) | //base reg A0
              (0 << 13) |       //cst5
              (1 << 9) |        //mode 1 = pos cst offset
              (0 << 8) |        //r (LDDW bit 0)
              (C67_map_regs(b) << 7) |  //y D1/D2 base reg side
              (3 << 4) |        //ldst 3=STB, 5=STH 5, 7=STW, 6=LDW 4=LDH 2=LDB 0=LDHU 1=LDBU 
              (1 << 2) |        //opcode
              (C67_map_regs(a) << 1) |  //side of src
              (0 << 0));        //parallel
    } �� �� (strstr(s, "STW.D +*") == s) {
        ALWAYS_ASSERT(c < 32);
        C67_g((C67_map_regn(a) << 23) | //src
              (C67_map_regn(b) << 18) | //base reg A0
              (c << 13) |       //cst5
              (1 << 9) |        //mode 1 = pos cst offset
              (0 << 8) |        //r (LDDW bit 0)
              (C67_map_regs(b) << 7) |  //y D1/D2 base reg side
              (7 << 4) |        //ldst 3=STB, 5=STH 5, 7=STW, 6=LDW 4=LDH 2=LDB 0=LDHU 1=LDBU 
              (1 << 2) |        //opcode
              (C67_map_regs(a) << 1) |  //side of src
              (0 << 0));        //parallel
    } �� �� (strstr(s, "LDW.D SP PRE INC") == s) {
        C67_g((C67_map_regn(a) << 23) | //dst
              (15 << 18) |      //base reg B15
              (2 << 13) |       //ucst5 (must keep 8 byte boundary)
              (9 << 9) |        //mode 9 = pre inc ucst5
              (0 << 8) |        //r (LDDW bit 0)
              (1 << 7) |        //y D1/D2  B side
              (6 << 4) |        //ldst 3=STB, 5=STH 5, 7=STW, 6=LDW 4=LDH 2=LDB 0=LDHU 1=LDBU
              (1 << 2) |        //opcode
              (C67_map_regs(a) << 1) |  //side of dst
              (0 << 0));        //parallel
    } �� �� (strstr(s, "LDDW.D SP PRE INC") == s) {
        C67_g((C67_map_regn(a) << 23) | //dst
              (15 << 18) |      //base reg B15
              (1 << 13) |       //ucst5 (must keep 8 byte boundary)
              (9 << 9) |        //mode 9 = pre inc ucst5
              (1 << 8) |        //r (LDDW bit 1)
              (1 << 7) |        //y D1/D2  B side
              (6 << 4) |        //ldst 3=STB, 5=STH 5, 7=STW, 6=LDW 4=LDH 2=LDB 0=LDHU 1=LDBU
              (1 << 2) |        //opcode
              (C67_map_regs(a) << 1) |  //side of dst
              (0 << 0));        //parallel
    } �� �� (strstr(s, "LDW.D *+SP[A0]") == s) {
        C67_g((C67_map_regn(a) << 23) | //dst
              (15 << 18) |      //base reg A15
              (0 << 13) |       //offset reg A0
              (5 << 9) |        //mode 5 = pos offset, base reg + off reg
              (0 << 8) |        //r (LDDW bit 0)
              (0 << 7) |        //y D1/D2  A side
              (6 << 4) |        //ldst 3=STB, 5=STH 5, 7=STW, 6=LDW 4=LDH 2=LDB 0=LDHU 1=LDBU
              (1 << 2) |        //opcode
              (C67_map_regs(a) << 1) |  //side of dst
              (0 << 0));        //parallel
    } �� �� (strstr(s, "LDDW.D *+SP[A0]") == s) {
        C67_g((C67_map_regn(a) << 23) | //dst
              (15 << 18) |      //base reg A15
              (0 << 13) |       //offset reg A0
              (5 << 9) |        //mode 5 = pos offset, base reg + off reg
              (1 << 8) |        //r (LDDW bit 1)
              (0 << 7) |        //y D1/D2  A side
              (6 << 4) |        //ldst 3=STB, 5=STH 5, 7=STW, 6=LDW 4=LDH 2=LDB 0=LDHU 1=LDBU
              (1 << 2) |        //opcode
              (C67_map_regs(a) << 1) |  //side of dst
              (0 << 0));        //parallel
    } �� �� (strstr(s, "LDH.D *+SP[A0]") == s) {
        C67_g((C67_map_regn(a) << 23) | //dst
              (15 << 18) |      //base reg A15
              (0 << 13) |       //offset reg A0
              (5 << 9) |        //mode 5 = pos offset, base reg + off reg
              (0 << 8) |        //r (LDDW bit 0)
              (0 << 7) |        //y D1/D2  A side
              (4 << 4) |        //ldst 3=STB, 5=STH 5, 7=STW, 6=LDW 4=LDH 2=LDB 0=LDHU 1=LDBU
              (1 << 2) |        //opcode
              (C67_map_regs(a) << 1) |  //side of dst
              (0 << 0));        //parallel
    } �� �� (strstr(s, "LDB.D *+SP[A0]") == s) {
        C67_g((C67_map_regn(a) << 23) | //dst
              (15 << 18) |      //base reg A15
              (0 << 13) |       //offset reg A0
              (5 << 9) |        //mode 5 = pos offset, base reg + off reg
              (0 << 8) |        //r (LDDW bit 0)
              (0 << 7) |        //y D1/D2  A side
              (2 << 4) |        //ldst 3=STB, 5=STH 5, 7=STW, 6=LDW 4=LDH 2=LDB 0=LDHU 1=LDBU
              (1 << 2) |        //opcode
              (C67_map_regs(a) << 1) |  //side of dst
              (0 << 0));        //parallel
    } �� �� (strstr(s, "LDHU.D *+SP[A0]") == s) {
        C67_g((C67_map_regn(a) << 23) | //dst
              (15 << 18) |      //base reg A15
              (0 << 13) |       //offset reg A0
              (5 << 9) |        //mode 5 = pos offset, base reg + off reg
              (0 << 8) |        //r (LDDW bit 0)
              (0 << 7) |        //y D1/D2  A side
              (0 << 4) |        //ldst 3=STB, 5=STH 5, 7=STW, 6=LDW 4=LDH 2=LDB 0=LDHU 1=LDBU
              (1 << 2) |        //opcode
              (C67_map_regs(a) << 1) |  //side of dst
              (0 << 0));        //parallel
    } �� �� (strstr(s, "LDBU.D *+SP[A0]") == s) {
        C67_g((C67_map_regn(a) << 23) | //dst
              (15 << 18) |      //base reg A15
              (0 << 13) |       //offset reg A0
              (5 << 9) |        //mode 5 = pos offset, base reg + off reg
              (0 << 8) |        //r (LDDW bit 0)
              (0 << 7) |        //y D1/D2  A side
              (1 << 4) |        //ldst 3=STB, 5=STH 5, 7=STW, 6=LDW 4=LDH 2=LDB 0=LDHU 1=LDBU
              (1 << 2) |        //opcode
              (C67_map_regs(a) << 1) |  //side of dst
              (0 << 0));        //parallel
    } �� �� (strstr(s, "LDW.D *") == s) {
        C67_g((C67_map_regn(b) << 23) | //dst
              (C67_map_regn(a) << 18) | //base reg A15
              (0 << 13) |       //cst5
              (1 << 9) |        //mode 1 = pos cst offset
              (0 << 8) |        //r (LDDW bit 0)
              (C67_map_regs(a) << 7) |  //y D1/D2  src side
              (6 << 4) |        //ldst 3=STB, 5=STH 5, 7=STW, 6=LDW 4=LDH 2=LDB 0=LDHU 1=LDBU
              (1 << 2) |        //opcode
              (C67_map_regs(b) << 1) |  //side of dst
              (0 << 0));        //parallel
    } �� �� (strstr(s, "LDDW.D *") == s) {
        C67_g((C67_map_regn(b) << 23) | //dst
              (C67_map_regn(a) << 18) | //base reg A15
              (0 << 13) |       //cst5
              (1 << 9) |        //mode 1 = pos cst offset
              (1 << 8) |        //r (LDDW bit 1)
              (C67_map_regs(a) << 7) |  //y D1/D2  src side
              (6 << 4) |        //ldst 3=STB, 5=STH 5, 7=STW, 6=LDW 4=LDH 2=LDB 0=LDHU 1=LDBU
              (1 << 2) |        //opcode
              (C67_map_regs(b) << 1) |  //side of dst
              (0 << 0));        //parallel
    } �� �� (strstr(s, "LDH.D *") == s) {
        C67_g((C67_map_regn(b) << 23) | //dst
              (C67_map_regn(a) << 18) | //base reg A15
              (0 << 13) |       //cst5
              (1 << 9) |        //mode 1 = pos cst offset
              (0 << 8) |        //r (LDDW bit 0)
              (C67_map_regs(a) << 7) |  //y D1/D2  src side
              (4 << 4) |        //ldst 3=STB, 5=STH 5, 7=STW, 6=LDW 4=LDH 2=LDB 0=LDHU 1=LDBU
              (1 << 2) |        //opcode
              (C67_map_regs(b) << 1) |  //side of dst
              (0 << 0));        //parallel
    } �� �� (strstr(s, "LDB.D *") == s) {
        C67_g((C67_map_regn(b) << 23) | //dst
              (C67_map_regn(a) << 18) | //base reg A15
              (0 << 13) |       //cst5
              (1 << 9) |        //mode 1 = pos cst offset
              (0 << 8) |        //r (LDDW bit 0)
              (C67_map_regs(a) << 7) |  //y D1/D2  src side
              (2 << 4) |        //ldst 3=STB, 5=STH 5, 7=STW, 6=LDW 4=LDH 2=LDB 0=LDHU 1=LDBU 
              (1 << 2) |        //opcode
              (C67_map_regs(b) << 1) |  //side of dst
              (0 << 0));        //parallel
    } �� �� (strstr(s, "LDHU.D *") == s) {
        C67_g((C67_map_regn(b) << 23) | //dst
              (C67_map_regn(a) << 18) | //base reg A15
              (0 << 13) |       //cst5
              (1 << 9) |        //mode 1 = pos cst offset
              (0 << 8) |        //r (LDDW bit 0)
              (C67_map_regs(a) << 7) |  //y D1/D2  src side
              (0 << 4) |        //ldst 3=STB, 5=STH 5, 7=STW, 6=LDW 4=LDH 2=LDB 0=LDHU 1=LDBU 
              (1 << 2) |        //opcode
              (C67_map_regs(b) << 1) |  //side of dst
              (0 << 0));        //parallel
    } �� �� (strstr(s, "LDBU.D *") == s) {
        C67_g((C67_map_regn(b) << 23) | //dst
              (C67_map_regn(a) << 18) | //base reg A15
              (0 << 13) |       //cst5
              (1 << 9) |        //mode 1 = pos cst offset
              (0 << 8) |        //r (LDDW bit 0)
              (C67_map_regs(a) << 7) |  //y D1/D2  src side
              (1 << 4) |        //ldst 3=STB, 5=STH 5, 7=STW, 6=LDW 4=LDH 2=LDB 0=LDHU 1=LDBU
              (1 << 2) |        //opcode
              (C67_map_regs(b) << 1) |  //side of dst
              (0 << 0));        //parallel
    } �� �� (strstr(s, "LDW.D +*") == s) {
        C67_g((C67_map_regn(b) << 23) | //dst
              (C67_map_regn(a) << 18) | //base reg A15
              (1 << 13) |       //cst5
              (1 << 9) |        //mode 1 = pos cst offset
              (0 << 8) |        //r (LDDW bit 0)
              (C67_map_regs(a) << 7) |  //y D1/D2  src side
              (6 << 4) |        //ldst 3=STB, 5=STH 5, 7=STW, 6=LDW 4=LDH 2=LDB 0=LDHU 1=LDBU 
              (1 << 2) |        //opcode
              (C67_map_regs(b) << 1) |  //side of dst
              (0 << 0));        //parallel
    } �� �� (strstr(s, "CMPLTSP") == s) {
        xpath = C67_map_regs(a) ^ C67_map_regs(b);
        ALWAYS_ASSERT(C67_map_regs(c) == C67_map_regs(a));

        C67_g((C67_map_regn(c) << 23) | //dst
              (C67_map_regn(b) << 18) | //src2
              (C67_map_regn(a) << 13) | //src1
              (xpath << 12) |   //x use cross path for src2
              (0x3a << 6) |     //opcode
              (0x8 << 2) |      //opcode fixed
              (C67_map_regs(c) << 1) |  //side for reg c
              (0 << 0));        //parallel
    } �� �� (strstr(s, "CMPGTSP") == s) {
        xpath = C67_map_regs(a) ^ C67_map_regs(b);
        ALWAYS_ASSERT(C67_map_regs(c) == C67_map_regs(a));

        C67_g((C67_map_regn(c) << 23) | //dst
              (C67_map_regn(b) << 18) | //src2
              (C67_map_regn(a) << 13) | //src1
              (xpath << 12) |   //x use cross path for src2
              (0x39 << 6) |     //opcode
              (0x8 << 2) |      //opcode fixed
              (C67_map_regs(c) << 1) |  //side for reg c
              (0 << 0));        //parallel
    } �� �� (strstr(s, "CMPEQSP") == s) {
        xpath = C67_map_regs(a) ^ C67_map_regs(b);
        ALWAYS_ASSERT(C67_map_regs(c) == C67_map_regs(a));

        C67_g((C67_map_regn(c) << 23) | //dst
              (C67_map_regn(b) << 18) | //src2
              (C67_map_regn(a) << 13) | //src1
              (xpath << 12) |   //x use cross path for src2
              (0x38 << 6) |     //opcode
              (0x8 << 2) |      //opcode fixed
              (C67_map_regs(c) << 1) |  //side for reg c
              (0 << 0));        //parallel
    }

    �� �� (strstr(s, "CMPLTDP") == s) {
        xpath = C67_map_regs(a) ^ C67_map_regs(b);
        ALWAYS_ASSERT(C67_map_regs(c) == C67_map_regs(a));

        C67_g((C67_map_regn(c) << 23) | //dst
              (C67_map_regn(b) << 18) | //src2
              (C67_map_regn(a) << 13) | //src1
              (xpath << 12) |   //x use cross path for src2
              (0x2a << 6) |     //opcode
              (0x8 << 2) |      //opcode fixed
              (C67_map_regs(c) << 1) |  //side for reg c
              (0 << 0));        //parallel
    } �� �� (strstr(s, "CMPGTDP") == s) {
        xpath = C67_map_regs(a) ^ C67_map_regs(b);
        ALWAYS_ASSERT(C67_map_regs(c) == C67_map_regs(a));

        C67_g((C67_map_regn(c) << 23) | //dst
              (C67_map_regn(b) << 18) | //src2
              (C67_map_regn(a) << 13) | //src1
              (xpath << 12) |   //x use cross path for src2
              (0x29 << 6) |     //opcode
              (0x8 << 2) |      //opcode fixed
              (C67_map_regs(c) << 1) |  //side for reg c
              (0 << 0));        //parallel
    } �� �� (strstr(s, "CMPEQDP") == s) {
        xpath = C67_map_regs(a) ^ C67_map_regs(b);
        ALWAYS_ASSERT(C67_map_regs(c) == C67_map_regs(a));

        C67_g((C67_map_regn(c) << 23) | //dst
              (C67_map_regn(b) << 18) | //src2
              (C67_map_regn(a) << 13) | //src1
              (xpath << 12) |   //x use cross path for src2
              (0x28 << 6) |     //opcode
              (0x8 << 2) |      //opcode fixed
              (C67_map_regs(c) << 1) |  //side for reg c
              (0 << 0));        //parallel
    } �� �� (strstr(s, "CMPLT") == s) {
        xpath = C67_map_regs(a) ^ C67_map_regs(b);
        ALWAYS_ASSERT(C67_map_regs(c) == C67_map_regs(a));

        C67_g((C67_map_regn(c) << 23) | //dst
              (C67_map_regn(b) << 18) | //src2
              (C67_map_regn(a) << 13) | //src1
              (xpath << 12) |   //x use cross path for src2
              (0x57 << 5) |     //opcode
              (0x6 << 2) |      //opcode fixed
              (C67_map_regs(c) << 1) |  //side for reg c
              (0 << 0));        //parallel
    } �� �� (strstr(s, "CMPGT") == s) {
        xpath = C67_map_regs(a) ^ C67_map_regs(b);
        ALWAYS_ASSERT(C67_map_regs(c) == C67_map_regs(a));

        C67_g((C67_map_regn(c) << 23) | //dst
              (C67_map_regn(b) << 18) | //src2
              (C67_map_regn(a) << 13) | //src1
              (xpath << 12) |   //x use cross path for src2
              (0x47 << 5) |     //opcode
              (0x6 << 2) |      //opcode fixed
              (C67_map_regs(c) << 1) |  //side for reg c
              (0 << 0));        //parallel
    } �� �� (strstr(s, "CMPEQ") == s) {
        xpath = C67_map_regs(a) ^ C67_map_regs(b);
        ALWAYS_ASSERT(C67_map_regs(c) == C67_map_regs(a));

        C67_g((C67_map_regn(c) << 23) | //dst
              (C67_map_regn(b) << 18) | //src2
              (C67_map_regn(a) << 13) | //src1
              (xpath << 12) |   //x use cross path for src2
              (0x53 << 5) |     //opcode
              (0x6 << 2) |      //opcode fixed
              (C67_map_regs(c) << 1) |  //side for reg c
              (0 << 0));        //parallel
    } �� �� (strstr(s, "CMPLTU") == s) {
        xpath = C67_map_regs(a) ^ C67_map_regs(b);
        ALWAYS_ASSERT(C67_map_regs(c) == C67_map_regs(a));

        C67_g((C67_map_regn(c) << 23) | //dst
              (C67_map_regn(b) << 18) | //src2
              (C67_map_regn(a) << 13) | //src1
              (xpath << 12) |   //x use cross path for src2
              (0x5f << 5) |     //opcode
              (0x6 << 2) |      //opcode fixed
              (C67_map_regs(c) << 1) |  //side for reg c
              (0 << 0));        //parallel
    } �� �� (strstr(s, "CMPGTU") == s) {
        xpath = C67_map_regs(a) ^ C67_map_regs(b);
        ALWAYS_ASSERT(C67_map_regs(c) == C67_map_regs(a));

        C67_g((C67_map_regn(c) << 23) | //dst
              (C67_map_regn(b) << 18) | //src2
              (C67_map_regn(a) << 13) | //src1
              (xpath << 12) |   //x use cross path for src2
              (0x4f << 5) |     //opcode
              (0x6 << 2) |      //opcode fixed
              (C67_map_regs(c) << 1) |  //side for reg c
              (0 << 0));        //parallel
    } �� �� (strstr(s, "B DISP") == s) {
        C67_g((0 << 29) |       //creg
              (0 << 28) |       //z
              (a << 7) |        //cnst
              (0x4 << 2) |      //opcode fixed
              (0 << 1) |        //S0/S1
              (0 << 0));        //parallel
    } �� �� (strstr(s, "B.") == s) {
        xpath = C67_map_regs(c) ^ 1;

        C67_g((C67_map_regc(b) << 29) | //creg
              (a << 28) |       //inv
              (0 << 23) |       //dst
              (C67_map_regn(c) << 18) | //src2
              (0 << 13) |       //
              (xpath << 12) |   //x cross path if !B side
              (0xd << 6) |      //opcode
              (0x8 << 2) |      //opcode fixed
              (1 << 1) |        //must be S2
              (0 << 0));        //parallel
    } �� �� (strstr(s, "MV.L") == s) {
        xpath = C67_map_regs(b) ^ C67_map_regs(c);

        C67_g((0 << 29) |       //creg
              (0 << 28) |       //inv
              (C67_map_regn(c) << 23) | //dst
              (C67_map_regn(b) << 18) | //src2
              (0 << 13) |       //src1 (cst5)
              (xpath << 12) |   //x cross path if opposite sides
              (0x2 << 5) |      //opcode
              (0x6 << 2) |      //opcode fixed
              (C67_map_regs(c) << 1) |  //side of dest
              (0 << 0));        //parallel
    } �� �� (strstr(s, "SPTRUNC.L") == s) {
        xpath = C67_map_regs(b) ^ C67_map_regs(c);

        C67_g((0 << 29) |       //creg
              (0 << 28) |       //inv
              (C67_map_regn(c) << 23) | //dst
              (C67_map_regn(b) << 18) | //src2
              (0 << 13) |       //src1 NA
              (xpath << 12) |   //x cross path if opposite sides
              (0xb << 5) |      //opcode
              (0x6 << 2) |      //opcode fixed
              (C67_map_regs(c) << 1) |  //side of dest
              (0 << 0));        //parallel
    } �� �� (strstr(s, "DPTRUNC.L") == s) {
        xpath = C67_map_regs(b) ^ C67_map_regs(c);

        C67_g((0 << 29) |       //creg
              (0 << 28) |       //inv
              (C67_map_regn(c) << 23) | //dst
              ((C67_map_regn(b) + 1) << 18) |   //src2   WEIRD CPU must specify odd reg for some reason
              (0 << 13) |       //src1 NA
              (xpath << 12) |   //x cross path if opposite sides
              (0x1 << 5) |      //opcode
              (0x6 << 2) |      //opcode fixed
              (C67_map_regs(c) << 1) |  //side of dest
              (0 << 0));        //parallel
    } �� �� (strstr(s, "INTSP.L") == s) {
        xpath = C67_map_regs(b) ^ C67_map_regs(c);

        C67_g((0 << 29) |       //creg
              (0 << 28) |       //inv
              (C67_map_regn(c) << 23) | //dst
              (C67_map_regn(b) << 18) | //src2   
              (0 << 13) |       //src1 NA
              (xpath << 12) |   //x cross path if opposite sides
              (0x4a << 5) |     //opcode
              (0x6 << 2) |      //opcode fixed
              (C67_map_regs(c) << 1) |  //side of dest
              (0 << 0));        //parallel
    } �� �� (strstr(s, "INTSPU.L") == s) {
        xpath = C67_map_regs(b) ^ C67_map_regs(c);

        C67_g((0 << 29) |       //creg
              (0 << 28) |       //inv
              (C67_map_regn(c) << 23) | //dst
              (C67_map_regn(b) << 18) | //src2  
              (0 << 13) |       //src1 NA
              (xpath << 12) |   //x cross path if opposite sides
              (0x49 << 5) |     //opcode
              (0x6 << 2) |      //opcode fixed
              (C67_map_regs(c) << 1) |  //side of dest
              (0 << 0));        //parallel
    } �� �� (strstr(s, "INTDP.L") == s) {
        xpath = C67_map_regs(b) ^ C67_map_regs(c);

        C67_g((0 << 29) |       //creg
              (0 << 28) |       //inv
              (C67_map_regn(c) << 23) | //dst
              (C67_map_regn(b) << 18) | //src2  
              (0 << 13) |       //src1 NA
              (xpath << 12) |   //x cross path if opposite sides
              (0x39 << 5) |     //opcode
              (0x6 << 2) |      //opcode fixed
              (C67_map_regs(c) << 1) |  //side of dest
              (0 << 0));        //parallel
    } �� �� (strstr(s, "INTDPU.L") == s) {
        xpath = C67_map_regs(b) ^ C67_map_regs(c);

        C67_g((0 << 29) |       //creg
              (0 << 28) |       //inv
              (C67_map_regn(c) << 23) | //dst
              ((C67_map_regn(b) + 1) << 18) |   //src2   WEIRD CPU must specify odd reg for some reason
              (0 << 13) |       //src1 NA
              (xpath << 12) |   //x cross path if opposite sides
              (0x3b << 5) |     //opcode
              (0x6 << 2) |      //opcode fixed
              (C67_map_regs(c) << 1) |  //side of dest
              (0 << 0));        //parallel
    } �� �� (strstr(s, "SPDP.L") == s) {
        xpath = C67_map_regs(b) ^ C67_map_regs(c);

        C67_g((0 << 29) |       //creg
              (0 << 28) |       //inv
              (C67_map_regn(c) << 23) | //dst
              (C67_map_regn(b) << 18) | //src2
              (0 << 13) |       //src1 NA
              (xpath << 12) |   //x cross path if opposite sides
              (0x2 << 6) |      //opcode
              (0x8 << 2) |      //opcode fixed
              (C67_map_regs(c) << 1) |  //side of dest
              (0 << 0));        //parallel
    } �� �� (strstr(s, "DPSP.L") == s) {
        ALWAYS_ASSERT(C67_map_regs(b) == C67_map_regs(c));

        C67_g((0 << 29) |       //creg
              (0 << 28) |       //inv
              (C67_map_regn(c) << 23) | //dst
              ((C67_map_regn(b) + 1) << 18) |   //src2 WEIRD CPU must specify odd reg for some reason
              (0 << 13) |       //src1 NA
              (0 << 12) |       //x cross path if opposite sides
              (0x9 << 5) |      //opcode
              (0x6 << 2) |      //opcode fixed
              (C67_map_regs(c) << 1) |  //side of dest
              (0 << 0));        //parallel
    } �� �� (strstr(s, "ADD.L") == s) {
        xpath = C67_map_regs(b) ^ C67_map_regs(c);

        ALWAYS_ASSERT(C67_map_regs(a) == C67_map_regs(c));

        C67_g((0 << 29) |       //creg
              (0 << 28) |       //inv
              (C67_map_regn(c) << 23) | //dst
              (C67_map_regn(b) << 18) | //src2 (possible x path)
              (C67_map_regn(a) << 13) | //src1 
              (xpath << 12) |   //x cross path if opposite sides
              (0x3 << 5) |      //opcode
              (0x6 << 2) |      //opcode fixed
              (C67_map_regs(c) << 1) |  //side of dest
              (0 << 0));        //parallel
    } �� �� (strstr(s, "SUB.L") == s) {
        xpath = C67_map_regs(b) ^ C67_map_regs(c);

        ALWAYS_ASSERT(C67_map_regs(a) == C67_map_regs(c));

        C67_g((0 << 29) |       //creg
              (0 << 28) |       //inv
              (C67_map_regn(c) << 23) | //dst
              (C67_map_regn(b) << 18) | //src2 (possible x path)
              (C67_map_regn(a) << 13) | //src1 
              (xpath << 12) |   //x cross path if opposite sides
              (0x7 << 5) |      //opcode
              (0x6 << 2) |      //opcode fixed
              (C67_map_regs(c) << 1) |  //side of dest
              (0 << 0));        //parallel
    } �� �� (strstr(s, "OR.L") == s) {
        xpath = C67_map_regs(b) ^ C67_map_regs(c);

        ALWAYS_ASSERT(C67_map_regs(a) == C67_map_regs(c));

        C67_g((0 << 29) |       //creg
              (0 << 28) |       //inv
              (C67_map_regn(c) << 23) | //dst
              (C67_map_regn(b) << 18) | //src2 (possible x path)
              (C67_map_regn(a) << 13) | //src1 
              (xpath << 12) |   //x cross path if opposite sides
              (0x7f << 5) |     //opcode
              (0x6 << 2) |      //opcode fixed
              (C67_map_regs(c) << 1) |  //side of dest
              (0 << 0));        //parallel
    } �� �� (strstr(s, "AND.L") == s) {
        xpath = C67_map_regs(b) ^ C67_map_regs(c);

        ALWAYS_ASSERT(C67_map_regs(a) == C67_map_regs(c));

        C67_g((0 << 29) |       //creg
              (0 << 28) |       //inv
              (C67_map_regn(c) << 23) | //dst
              (C67_map_regn(b) << 18) | //src2 (possible x path)
              (C67_map_regn(a) << 13) | //src1 
              (xpath << 12) |   //x cross path if opposite sides
              (0x7b << 5) |     //opcode
              (0x6 << 2) |      //opcode fixed
              (C67_map_regs(c) << 1) |  //side of dest
              (0 << 0));        //parallel
    } �� �� (strstr(s, "XOR.L") == s) {
        xpath = C67_map_regs(b) ^ C67_map_regs(c);

        ALWAYS_ASSERT(C67_map_regs(a) == C67_map_regs(c));

        C67_g((0 << 29) |       //creg
              (0 << 28) |       //inv
              (C67_map_regn(c) << 23) | //dst
              (C67_map_regn(b) << 18) | //src2 (possible x path)
              (C67_map_regn(a) << 13) | //src1 
              (xpath << 12) |   //x cross path if opposite sides
              (0x6f << 5) |     //opcode
              (0x6 << 2) |      //opcode fixed
              (C67_map_regs(c) << 1) |  //side of dest
              (0 << 0));        //parallel
    } �� �� (strstr(s, "ADDSP.L") == s) {
        xpath = C67_map_regs(b) ^ C67_map_regs(c);

        ALWAYS_ASSERT(C67_map_regs(a) == C67_map_regs(c));

        C67_g((0 << 29) |       //creg
              (0 << 28) |       //inv
              (C67_map_regn(c) << 23) | //dst
              (C67_map_regn(b) << 18) | //src2 (possible x path)
              (C67_map_regn(a) << 13) | //src1 
              (xpath << 12) |   //x cross path if opposite sides
              (0x10 << 5) |     //opcode
              (0x6 << 2) |      //opcode fixed
              (C67_map_regs(c) << 1) |  //side of dest
              (0 << 0));        //parallel
    } �� �� (strstr(s, "ADDDP.L") == s) {
        xpath = C67_map_regs(b) ^ C67_map_regs(c);

        ALWAYS_ASSERT(C67_map_regs(a) == C67_map_regs(c));

        C67_g((0 << 29) |       //creg
              (0 << 28) |       //inv
              (C67_map_regn(c) << 23) | //dst
              (C67_map_regn(b) << 18) | //src2 (possible x path)
              (C67_map_regn(a) << 13) | //src1 
              (xpath << 12) |   //x cross path if opposite sides
              (0x18 << 5) |     //opcode
              (0x6 << 2) |      //opcode fixed
              (C67_map_regs(c) << 1) |  //side of dest
              (0 << 0));        //parallel
    } �� �� (strstr(s, "SUBSP.L") == s) {
        xpath = C67_map_regs(b) ^ C67_map_regs(c);

        ALWAYS_ASSERT(C67_map_regs(a) == C67_map_regs(c));

        C67_g((0 << 29) |       //creg
              (0 << 28) |       //inv
              (C67_map_regn(c) << 23) | //dst
              (C67_map_regn(b) << 18) | //src2 (possible x path)
              (C67_map_regn(a) << 13) | //src1 
              (xpath << 12) |   //x cross path if opposite sides
              (0x11 << 5) |     //opcode
              (0x6 << 2) |      //opcode fixed
              (C67_map_regs(c) << 1) |  //side of dest
              (0 << 0));        //parallel
    } �� �� (strstr(s, "SUBDP.L") == s) {
        xpath = C67_map_regs(b) ^ C67_map_regs(c);

        ALWAYS_ASSERT(C67_map_regs(a) == C67_map_regs(c));

        C67_g((0 << 29) |       //creg
              (0 << 28) |       //inv
              (C67_map_regn(c) << 23) | //dst
              (C67_map_regn(b) << 18) | //src2 (possible x path)
              (C67_map_regn(a) << 13) | //src1 
              (xpath << 12) |   //x cross path if opposite sides
              (0x19 << 5) |     //opcode
              (0x6 << 2) |      //opcode fixed
              (C67_map_regs(c) << 1) |  //side of dest
              (0 << 0));        //parallel
    } �� �� (strstr(s, "MPYSP.M") == s) {
        xpath = C67_map_regs(b) ^ C67_map_regs(c);

        ALWAYS_ASSERT(C67_map_regs(a) == C67_map_regs(c));

        C67_g((0 << 29) |       //creg
              (0 << 28) |       //inv
              (C67_map_regn(c) << 23) | //dst
              (C67_map_regn(b) << 18) | //src2 (possible x path)
              (C67_map_regn(a) << 13) | //src1 
              (xpath << 12) |   //x cross path if opposite sides
              (0x1c << 7) |     //opcode
              (0x0 << 2) |      //opcode fixed
              (C67_map_regs(c) << 1) |  //side of dest
              (0 << 0));        //parallel
    } �� �� (strstr(s, "MPYDP.M") == s) {
        xpath = C67_map_regs(b) ^ C67_map_regs(c);

        ALWAYS_ASSERT(C67_map_regs(a) == C67_map_regs(c));

        C67_g((0 << 29) |       //creg
              (0 << 28) |       //inv
              (C67_map_regn(c) << 23) | //dst
              (C67_map_regn(b) << 18) | //src2 (possible x path)
              (C67_map_regn(a) << 13) | //src1 
              (xpath << 12) |   //x cross path if opposite sides
              (0x0e << 7) |     //opcode
              (0x0 << 2) |      //opcode fixed
              (C67_map_regs(c) << 1) |  //side of dest
              (0 << 0));        //parallel
    } �� �� (strstr(s, "MPYI.M") == s) {
        xpath = C67_map_regs(b) ^ C67_map_regs(c);

        ALWAYS_ASSERT(C67_map_regs(a) == C67_map_regs(c));

        C67_g((0 << 29) |       //creg
              (0 << 28) |       //inv
              (C67_map_regn(c) << 23) | //dst
              (C67_map_regn(b) << 18) | //src2
              (C67_map_regn(a) << 13) | //src1 (cst5)
              (xpath << 12) |   //x cross path if opposite sides
              (0x4 << 7) |      //opcode
              (0x0 << 2) |      //opcode fixed
              (C67_map_regs(c) << 1) |  //side of dest
              (0 << 0));        //parallel
    } �� �� (strstr(s, "SHR.S") == s) {
        xpath = C67_map_regs(b) ^ C67_map_regs(c);

        ALWAYS_ASSERT(C67_map_regs(c) == C67_map_regs(a));

        C67_g((0 << 29) |       //creg
              (0 << 28) |       //inv
              (C67_map_regn(c) << 23) | //dst
              (C67_map_regn(b) << 18) | //src2
              (C67_map_regn(a) << 13) | //src1 
              (xpath << 12) |   //x cross path if opposite sides
              (0x37 << 6) |     //opcode
              (0x8 << 2) |      //opcode fixed
              (C67_map_regs(c) << 1) |  //side of dest
              (0 << 0));        //parallel
    } �� �� (strstr(s, "SHRU.S") == s) {
        xpath = C67_map_regs(b) ^ C67_map_regs(c);

        ALWAYS_ASSERT(C67_map_regs(c) == C67_map_regs(a));

        C67_g((0 << 29) |       //creg
              (0 << 28) |       //inv
              (C67_map_regn(c) << 23) | //dst
              (C67_map_regn(b) << 18) | //src2
              (C67_map_regn(a) << 13) | //src1 
              (xpath << 12) |   //x cross path if opposite sides
              (0x27 << 6) |     //opcode
              (0x8 << 2) |      //opcode fixed
              (C67_map_regs(c) << 1) |  //side of dest
              (0 << 0));        //parallel
    } �� �� (strstr(s, "SHL.S") == s) {
        xpath = C67_map_regs(b) ^ C67_map_regs(c);

        ALWAYS_ASSERT(C67_map_regs(c) == C67_map_regs(a));

        C67_g((0 << 29) |       //creg
              (0 << 28) |       //inv
              (C67_map_regn(c) << 23) | //dst
              (C67_map_regn(b) << 18) | //src2
              (C67_map_regn(a) << 13) | //src1 
              (xpath << 12) |   //x cross path if opposite sides
              (0x33 << 6) |     //opcode
              (0x8 << 2) |      //opcode fixed
              (C67_map_regs(c) << 1) |  //side of dest
              (0 << 0));        //parallel
    } �� �� (strstr(s, "||ADDK") == s) {
        xpath = 0;              // no xpath required just use the side of the src/dst

        C67_g((0 << 29) |       //creg
              (0 << 28) |       //inv
              (C67_map_regn(b) << 23) | //dst
              (a << 07) |       //scst16
              (0x14 << 2) |     //opcode fixed
              (C67_map_regs(b) << 1) |  //side of dst
              (1 << 0));        //parallel
    } �� �� (strstr(s, "ADDK") == s) {
        xpath = 0;              // no xpath required just use the side of the src/dst

        C67_g((0 << 29) |       //creg
              (0 << 28) |       //inv
              (C67_map_regn(b) << 23) | //dst
              (a << 07) |       //scst16
              (0x14 << 2) |     //opcode fixed
              (C67_map_regs(b) << 1) |  //side of dst
              (0 << 0));        //parallel
    } �� �� (strstr(s, "NOP") == s) {
        C67_g(((a - 1) << 13) | //no of cycles
              (0 << 0));        //parallel
    } ��
        ALWAYS_ASSERT(FALSE);

#�綨�� ASSEMBLY_LISTING_C67
    fprintf(f, " %s %d %d %d\n", s, a, b, c);
#����

}

//r=reg to load, fr=from reg, symbol for relocation, constant

�� C67_MVKL(�� r, �� fc)
{
    C67_asm("MVKL.", fc, r, 0);
}

�� C67_MVKH(�� r, �� fc)
{
    C67_asm("MVKH.", fc, r, 0);
}

�� C67_STB_SP_A0(�� r)
{
    C67_asm("STB.D *+SP[A0]", r, 0, 0); // STB  r,*+SP[A0]
}

�� C67_STH_SP_A0(�� r)
{
    C67_asm("STH.D *+SP[A0]", r, 0, 0); // STH  r,*+SP[A0]
}

�� C67_STW_SP_A0(�� r)
{
    C67_asm("STW.D *+SP[A0]", r, 0, 0); // STW  r,*+SP[A0]
}

�� C67_STB_PTR(�� r, �� r2)
{
    C67_asm("STB.D *", r, r2, 0);       // STB  r, *r2
}

�� C67_STH_PTR(�� r, �� r2)
{
    C67_asm("STH.D *", r, r2, 0);       // STH  r, *r2
}

�� C67_STW_PTR(�� r, �� r2)
{
    C67_asm("STW.D *", r, r2, 0);       // STW  r, *r2
}

�� C67_STW_PTR_PRE_INC(�� r, �� r2, �� n)
{
    C67_asm("STW.D +*", r, r2, n);      // STW  r, *+r2
}

�� C67_PUSH(�� r)
{
    C67_asm("STW.D SP POST DEC", r, 0, 0);      // STW  r,*SP--
}

�� C67_LDW_SP_A0(�� r)
{
    C67_asm("LDW.D *+SP[A0]", r, 0, 0); // LDW  *+SP[A0],r
}

�� C67_LDDW_SP_A0(�� r)
{
    C67_asm("LDDW.D *+SP[A0]", r, 0, 0);        // LDDW  *+SP[A0],r
}

�� C67_LDH_SP_A0(�� r)
{
    C67_asm("LDH.D *+SP[A0]", r, 0, 0); // LDH  *+SP[A0],r
}

�� C67_LDB_SP_A0(�� r)
{
    C67_asm("LDB.D *+SP[A0]", r, 0, 0); // LDB  *+SP[A0],r
}

�� C67_LDHU_SP_A0(�� r)
{
    C67_asm("LDHU.D *+SP[A0]", r, 0, 0);        // LDHU  *+SP[A0],r
}

�� C67_LDBU_SP_A0(�� r)
{
    C67_asm("LDBU.D *+SP[A0]", r, 0, 0);        // LDBU  *+SP[A0],r
}

�� C67_LDW_PTR(�� r, �� r2)
{
    C67_asm("LDW.D *", r, r2, 0);       // LDW  *r,r2
}

�� C67_LDDW_PTR(�� r, �� r2)
{
    C67_asm("LDDW.D *", r, r2, 0);      // LDDW  *r,r2
}

�� C67_LDH_PTR(�� r, �� r2)
{
    C67_asm("LDH.D *", r, r2, 0);       // LDH  *r,r2
}

�� C67_LDB_PTR(�� r, �� r2)
{
    C67_asm("LDB.D *", r, r2, 0);       // LDB  *r,r2
}

�� C67_LDHU_PTR(�� r, �� r2)
{
    C67_asm("LDHU.D *", r, r2, 0);      // LDHU  *r,r2
}

�� C67_LDBU_PTR(�� r, �� r2)
{
    C67_asm("LDBU.D *", r, r2, 0);      // LDBU  *r,r2
}

�� C67_LDW_PTR_PRE_INC(�� r, �� r2)
{
    C67_asm("LDW.D +*", r, r2, 0);      // LDW  *+r,r2
}

�� C67_POP(�� r)
{
    C67_asm("LDW.D SP PRE INC", r, 0, 0);       // LDW  *++SP,r
}

�� C67_POP_DW(�� r)
{
    C67_asm("LDDW.D SP PRE INC", r, 0, 0);      // LDDW  *++SP,r
}

�� C67_CMPLT(�� s1, �� s2, �� dst)
{
    C67_asm("CMPLT.L1", s1, s2, dst);
}

�� C67_CMPGT(�� s1, �� s2, �� dst)
{
    C67_asm("CMPGT.L1", s1, s2, dst);
}

�� C67_CMPEQ(�� s1, �� s2, �� dst)
{
    C67_asm("CMPEQ.L1", s1, s2, dst);
}

�� C67_CMPLTU(�� s1, �� s2, �� dst)
{
    C67_asm("CMPLTU.L1", s1, s2, dst);
}

�� C67_CMPGTU(�� s1, �� s2, �� dst)
{
    C67_asm("CMPGTU.L1", s1, s2, dst);
}


�� C67_CMPLTSP(�� s1, �� s2, �� dst)
{
    C67_asm("CMPLTSP.S1", s1, s2, dst);
}

�� C67_CMPGTSP(�� s1, �� s2, �� dst)
{
    C67_asm("CMPGTSP.S1", s1, s2, dst);
}

�� C67_CMPEQSP(�� s1, �� s2, �� dst)
{
    C67_asm("CMPEQSP.S1", s1, s2, dst);
}

�� C67_CMPLTDP(�� s1, �� s2, �� dst)
{
    C67_asm("CMPLTDP.S1", s1, s2, dst);
}

�� C67_CMPGTDP(�� s1, �� s2, �� dst)
{
    C67_asm("CMPGTDP.S1", s1, s2, dst);
}

�� C67_CMPEQDP(�� s1, �� s2, �� dst)
{
    C67_asm("CMPEQDP.S1", s1, s2, dst);
}


�� C67_IREG_B_REG(�� inv, �� r1, �� r2)    // [!R] B  r2
{
    C67_asm("B.S2", inv, r1, r2);
}


// call with how many 32 bit words to skip
// (0 would branch to the branch instruction)

�� C67_B_DISP(�� disp)       //  B  +2  Branch with constant displacement
{
    // Branch point is relative to the 8 word fetch packet
    //
    // we will assume the text section always starts on an 8 word (32 byte boundary)
    //
    // so add in how many words into the fetch packet the branch is


    C67_asm("B DISP", disp + ((ind & 31) >> 2), 0, 0);
}

�� C67_NOP(�� n)
{
    C67_asm("NOP", n, 0, 0);
}

�� C67_ADDK(�� n, �� r)
{
    ALWAYS_ASSERT(abs(n) < 32767);

    C67_asm("ADDK", n, r, 0);
}

�� C67_ADDK_PARALLEL(�� n, �� r)
{
    ALWAYS_ASSERT(abs(n) < 32767);

    C67_asm("||ADDK", n, r, 0);
}

�� C67_Adjust_ADDK(�� *inst, �� n)
{
    ALWAYS_ASSERT(abs(n) < 32767);

    *inst = (*inst & (~(0xffff << 7))) | ((n & 0xffff) << 7);
}

�� C67_MV(�� r, �� v)
{
    C67_asm("MV.L", 0, r, v);
}


�� C67_DPTRUNC(�� r, �� v)
{
    C67_asm("DPTRUNC.L", 0, r, v);
}

�� C67_SPTRUNC(�� r, �� v)
{
    C67_asm("SPTRUNC.L", 0, r, v);
}

�� C67_INTSP(�� r, �� v)
{
    C67_asm("INTSP.L", 0, r, v);
}

�� C67_INTDP(�� r, �� v)
{
    C67_asm("INTDP.L", 0, r, v);
}

�� C67_INTSPU(�� r, �� v)
{
    C67_asm("INTSPU.L", 0, r, v);
}

�� C67_INTDPU(�� r, �� v)
{
    C67_asm("INTDPU.L", 0, r, v);
}

�� C67_SPDP(�� r, �� v)
{
    C67_asm("SPDP.L", 0, r, v);
}

�� C67_DPSP(�� r, �� v)     // note regs must be on the same side
{
    C67_asm("DPSP.L", 0, r, v);
}

�� C67_ADD(�� r, �� v)
{
    C67_asm("ADD.L", v, r, v);
}

�� C67_SUB(�� r, �� v)
{
    C67_asm("SUB.L", v, r, v);
}

�� C67_AND(�� r, �� v)
{
    C67_asm("AND.L", v, r, v);
}

�� C67_OR(�� r, �� v)
{
    C67_asm("OR.L", v, r, v);
}

�� C67_XOR(�� r, �� v)
{
    C67_asm("XOR.L", v, r, v);
}

�� C67_ADDSP(�� r, �� v)
{
    C67_asm("ADDSP.L", v, r, v);
}

�� C67_SUBSP(�� r, �� v)
{
    C67_asm("SUBSP.L", v, r, v);
}

�� C67_MPYSP(�� r, �� v)
{
    C67_asm("MPYSP.M", v, r, v);
}

�� C67_ADDDP(�� r, �� v)
{
    C67_asm("ADDDP.L", v, r, v);
}

�� C67_SUBDP(�� r, �� v)
{
    C67_asm("SUBDP.L", v, r, v);
}

�� C67_MPYDP(�� r, �� v)
{
    C67_asm("MPYDP.M", v, r, v);
}

�� C67_MPYI(�� r, �� v)
{
    C67_asm("MPYI.M", v, r, v);
}

�� C67_SHL(�� r, �� v)
{
    C67_asm("SHL.S", r, v, v);
}

�� C67_SHRU(�� r, �� v)
{
    C67_asm("SHRU.S", r, v, v);
}

�� C67_SHR(�� r, �� v)
{
    C67_asm("SHR.S", r, v, v);
}



/* load 'r' from value 'sv' */
�� load(�� r, SValue * sv)
{
    �� v, t, ft, fc, fr, size = 0, element;
    BOOL Unsigned = FALSE;
    SValue v1;

    fr = sv->r;
    ft = sv->type.t;
    fc = sv->c.i;

    v = fr & VT_VALMASK;
    �� (fr & VT_LVAL) {
        �� (v == VT_LLOCAL) {
            v1.type.t = VT_INT;
            v1.r = VT_LOCAL | VT_LVAL;
            v1.c.i = fc;
            load(r, &v1);
            fr = r;
        } �� �� ((ft & VT_BTYPE) == VT_LDOUBLE) {
            tcc_error("long double not supported");
        } �� �� ((ft & VT_TYPE) == VT_BYTE) {
            size = 1;
        } �� �� ((ft & VT_TYPE) == (VT_BYTE | VT_UNSIGNED)) {
            size = 1;
            Unsigned = TRUE;
        } �� �� ((ft & VT_TYPE) == VT_SHORT) {
            size = 2;
        } �� �� ((ft & VT_TYPE) == (VT_SHORT | VT_UNSIGNED)) {
            size = 2;
            Unsigned = TRUE;
        } �� �� ((ft & VT_BTYPE) == VT_DOUBLE) {
            size = 8;
        } �� {
            size = 4;
        }

        // check if fc is a positive reference on the stack, 
        // if it is tcc is referencing what it thinks is a parameter
        // on the stack, so check if it is really in a register.


        �� (v == VT_LOCAL && fc > 0) {
            �� stack_pos = 8;

            ���� (t = 0; t < NoCallArgsPassedOnStack; t++) {
                �� (fc == stack_pos)
                    ����;

                stack_pos += TranslateStackToReg[t];
            }

            // param has been pushed on stack, get it like a local var

            fc = ParamLocOnStack[t] - 8;
        }

        �� ((fr & VT_VALMASK) < VT_CONST)       // check for pure indirect
        {
            �� (size == 1) {
                �� (Unsigned)
                    C67_LDBU_PTR(v, r); // LDBU  *v,r
                ��
                    C67_LDB_PTR(v, r);  // LDB  *v,r
            } �� �� (size == 2) {
                �� (Unsigned)
                    C67_LDHU_PTR(v, r); // LDHU  *v,r
                ��
                    C67_LDH_PTR(v, r);  // LDH  *v,r
            } �� �� (size == 4) {
                C67_LDW_PTR(v, r);      // LDW  *v,r
            } �� �� (size == 8) {
                C67_LDDW_PTR(v, r);     // LDDW  *v,r
            }

            C67_NOP(4);         // NOP 4
            ����;
        } �� �� (fr & VT_SYM) {
            greloc(cur_text_section, sv->sym, ind, R_C60LO16);  // rem the inst need to be patched
            greloc(cur_text_section, sv->sym, ind + 4, R_C60HI16);


            C67_MVKL(C67_A0, fc);       //r=reg to load,  constant
            C67_MVKH(C67_A0, fc);       //r=reg to load,  constant


            �� (size == 1) {
                �� (Unsigned)
                    C67_LDBU_PTR(C67_A0, r);    // LDBU  *A0,r
                ��
                    C67_LDB_PTR(C67_A0, r);     // LDB  *A0,r
            } �� �� (size == 2) {
                �� (Unsigned)
                    C67_LDHU_PTR(C67_A0, r);    // LDHU  *A0,r
                ��
                    C67_LDH_PTR(C67_A0, r);     // LDH  *A0,r
            } �� �� (size == 4) {
                C67_LDW_PTR(C67_A0, r); // LDW  *A0,r
            } �� �� (size == 8) {
                C67_LDDW_PTR(C67_A0, r);        // LDDW  *A0,r
            }

            C67_NOP(4);         // NOP 4
            ����;
        } �� {
            element = size;

            // divide offset in bytes to create element index
            C67_MVKL(C67_A0, (fc / element) + 8 / element);     //r=reg to load,  constant
            C67_MVKH(C67_A0, (fc / element) + 8 / element);     //r=reg to load,  constant

            �� (size == 1) {
                �� (Unsigned)
                    C67_LDBU_SP_A0(r);  // LDBU  r, SP[A0]
                ��
                    C67_LDB_SP_A0(r);   // LDB  r, SP[A0]
            } �� �� (size == 2) {
                �� (Unsigned)
                    C67_LDHU_SP_A0(r);  // LDHU  r, SP[A0]
                ��
                    C67_LDH_SP_A0(r);   // LDH  r, SP[A0]
            } �� �� (size == 4) {
                C67_LDW_SP_A0(r);       // LDW  r, SP[A0]
            } �� �� (size == 8) {
                C67_LDDW_SP_A0(r);      // LDDW  r, SP[A0]
            }


            C67_NOP(4);         // NOP 4
            ����;
        }
    } �� {
        �� (v == VT_CONST) {
            �� (fr & VT_SYM) {
                greloc(cur_text_section, sv->sym, ind, R_C60LO16);      // rem the inst need to be patched
                greloc(cur_text_section, sv->sym, ind + 4, R_C60HI16);
            }
            C67_MVKL(r, fc);    //r=reg to load, constant
            C67_MVKH(r, fc);    //r=reg to load, constant
        } �� �� (v == VT_LOCAL) {
            C67_MVKL(r, fc + 8);        //r=reg to load, constant C67 stack points to next free
            C67_MVKH(r, fc + 8);        //r=reg to load, constant
            C67_ADD(C67_FP, r); // MV v,r   v -> r
        } �� �� (v == VT_CMP) {
            C67_MV(C67_compare_reg, r); // MV v,r   v -> r
        } �� �� (v == VT_JMP || v == VT_JMPI) {
            t = v & 1;
            C67_B_DISP(4);      //  Branch with constant displacement, skip over this branch, load, nop, load
            C67_MVKL(r, t);     //  r=reg to load, 0 or 1 (do this while branching)
            C67_NOP(4);         //  NOP 4
            gsym(fc);           //  modifies other branches to branch here
            C67_MVKL(r, t ^ 1); //  r=reg to load, 0 or 1
        } �� �� (v != r) {
            C67_MV(v, r);       // MV v,r   v -> r

            �� ((ft & VT_BTYPE) == VT_DOUBLE)
                C67_MV(v + 1, r + 1);   // MV v,r   v -> r
        }
    }
}


/* store register 'r' in lvalue 'v' */
�� store(�� r, SValue * v)
{
    �� fr, bt, ft, fc, size, t, element;

    ft = v->type.t;
    fc = v->c.i;
    fr = v->r & VT_VALMASK;
    bt = ft & VT_BTYPE;
    /* XXX: incorrect if float reg to reg */

    �� (bt == VT_LDOUBLE) {
        tcc_error("long double not supported");
    } �� {
        �� (bt == VT_SHORT)
            size = 2;
        �� �� (bt == VT_BYTE)
            size = 1;
        �� �� (bt == VT_DOUBLE)
            size = 8;
        ��
            size = 4;

        �� ((v->r & VT_VALMASK) == VT_CONST) {
            /* constant memory reference */

            �� (v->r & VT_SYM) {
                greloc(cur_text_section, v->sym, ind, R_C60LO16);       // rem the inst need to be patched
                greloc(cur_text_section, v->sym, ind + 4, R_C60HI16);
            }
            C67_MVKL(C67_A0, fc);       //r=reg to load,  constant
            C67_MVKH(C67_A0, fc);       //r=reg to load,  constant

            �� (size == 1)
                C67_STB_PTR(r, C67_A0); // STB  r, *A0
            �� �� (size == 2)
                C67_STH_PTR(r, C67_A0); // STH  r, *A0
            �� �� (size == 4 || size == 8)
                C67_STW_PTR(r, C67_A0); // STW  r, *A0

            �� (size == 8)
                C67_STW_PTR_PRE_INC(r + 1, C67_A0, 1);  // STW  r, *+A0[1]
        } �� �� ((v->r & VT_VALMASK) == VT_LOCAL) {
            // check case of storing to passed argument that
            // tcc thinks is on the stack but for C67 is
            // passed as a reg.  However it may have been
            // saved to the stack, if that reg was required
            // for a call to a child function

            �� (fc > 0)         // argument ??
            {
                // walk through sizes and figure which param

                �� stack_pos = 8;

                ���� (t = 0; t < NoCallArgsPassedOnStack; t++) {
                    �� (fc == stack_pos)
                        ����;

                    stack_pos += TranslateStackToReg[t];
                }

                // param has been pushed on stack, get it like a local var
                fc = ParamLocOnStack[t] - 8;
            }

            �� (size == 8)
                element = 4;
            ��
                element = size;

            // divide offset in bytes to create word index
            C67_MVKL(C67_A0, (fc / element) + 8 / element);     //r=reg to load,  constant
            C67_MVKH(C67_A0, (fc / element) + 8 / element);     //r=reg to load,  constant



            �� (size == 1)
                C67_STB_SP_A0(r);       // STB  r, SP[A0]
            �� �� (size == 2)
                C67_STH_SP_A0(r);       // STH  r, SP[A0]
            �� �� (size == 4 || size == 8)
                C67_STW_SP_A0(r);       // STW  r, SP[A0]

            �� (size == 8) {
                C67_ADDK(1, C67_A0);    //  ADDK 1,A0
                C67_STW_SP_A0(r + 1);   //  STW  r, SP[A0]
            }
        } �� {
            �� (size == 1)
                C67_STB_PTR(r, fr);     // STB  r, *fr
            �� �� (size == 2)
                C67_STH_PTR(r, fr);     // STH  r, *fr
            �� �� (size == 4 || size == 8)
                C67_STW_PTR(r, fr);     // STW  r, *fr

            �� (size == 8) {
                C67_STW_PTR_PRE_INC(r + 1, fr, 1);      // STW  r, *+fr[1]
            }
        }
    }
}

/* 'is_jmp' is '1' if it is a jump */
��̬ �� gcall_or_jmp(�� is_jmp)
{
    �� r;
    Sym *sym;

    �� ((vtop->r & (VT_VALMASK | VT_LVAL)) == VT_CONST) {
        /* constant case */
        �� (vtop->r & VT_SYM) {
            /* relocation case */

            // get add into A0, then start the jump B3

            greloc(cur_text_section, vtop->sym, ind, R_C60LO16);        // rem the inst need to be patched
            greloc(cur_text_section, vtop->sym, ind + 4, R_C60HI16);

            C67_MVKL(C67_A0, 0);        //r=reg to load, constant
            C67_MVKH(C67_A0, 0);        //r=reg to load, constant
            C67_IREG_B_REG(0, C67_CREG_ZERO, C67_A0);   //  B.S2x  A0

            �� (is_jmp) {
                C67_NOP(5);     // simple jump, just put NOP
            } �� {
                // Call, must load return address into B3 during delay slots

                sym = get_sym_ref(&char_pointer_type, cur_text_section, ind + 12, 0);   // symbol for return address
                greloc(cur_text_section, sym, ind, R_C60LO16);  // rem the inst need to be patched
                greloc(cur_text_section, sym, ind + 4, R_C60HI16);
                C67_MVKL(C67_B3, 0);    //r=reg to load, constant
                C67_MVKH(C67_B3, 0);    //r=reg to load, constant
                C67_NOP(3);     // put remaining NOPs
            }
        } �� {
            /* put an empty PC32 relocation */
            ALWAYS_ASSERT(FALSE);
        }
    } �� {
        /* otherwise, indirect call */
        r = gv(RC_INT);
        C67_IREG_B_REG(0, C67_CREG_ZERO, r);    //  B.S2x  r

        �� (is_jmp) {
            C67_NOP(5);         // simple jump, just put NOP
        } �� {
            // Call, must load return address into B3 during delay slots

            sym = get_sym_ref(&char_pointer_type, cur_text_section, ind + 12, 0);       // symbol for return address
            greloc(cur_text_section, sym, ind, R_C60LO16);      // rem the inst need to be patched
            greloc(cur_text_section, sym, ind + 4, R_C60HI16);
            C67_MVKL(C67_B3, 0);        //r=reg to load, constant
            C67_MVKH(C67_B3, 0);        //r=reg to load, constant
            C67_NOP(3);         // put remaining NOPs
        }
    }
}

/* Return the number of registers needed to return the struct, or 0 if
   returning via struct pointer. */
ST_FUNC �� gfunc_sret(CType *vt, �� variadic, CType *ret, �� *ret_align, �� *regsize) {
    *ret_align = 1; // Never have to re-align return values for x86-64
    ���� 0;
}

/* generate function call with address in (vtop->t, vtop->c) and free function
   context. Stack entry is popped */
�� gfunc_call(�� nb_args)
{
    �� i, r, size = 0;
    �� args_sizes[NoCallArgsPassedOnStack];

    �� (nb_args > NoCallArgsPassedOnStack) {
        tcc_error("more than 10 function params not currently supported");
        // handle more than 10, put some on the stack
    }

    ���� (i = 0; i < nb_args; i++) {
        �� ((vtop->type.t & VT_BTYPE) == VT_STRUCT) {
            ALWAYS_ASSERT(FALSE);
        } �� {
            /* simple type (currently always same size) */
            /* XXX: implicit cast ? */


            �� ((vtop->type.t & VT_BTYPE) == VT_LLONG) {
                tcc_error("long long not supported");
            } �� �� ((vtop->type.t & VT_BTYPE) == VT_LDOUBLE) {
                tcc_error("long double not supported");
            } �� �� ((vtop->type.t & VT_BTYPE) == VT_DOUBLE) {
                size = 8;
            } �� {
                size = 4;
            }

            // put the parameter into the corresponding reg (pair)

            r = gv(RC_C67_A4 << (2 * i));

            // must put on stack because with 1 pass compiler , no way to tell
            // if an up coming nested call might overwrite these regs

            C67_PUSH(r);

            �� (size == 8) {
                C67_STW_PTR_PRE_INC(r + 1, C67_SP, 3);  // STW  r, *+SP[3] (go back and put the other)
            }
            args_sizes[i] = size;
        }
        vtop--;
    }
    // POP all the params on the stack into registers for the
    // immediate call (in reverse order)

    ���� (i = nb_args - 1; i >= 0; i--) {

        �� (args_sizes[i] == 8)
            C67_POP_DW(TREG_C67_A4 + i * 2);
        ��
            C67_POP(TREG_C67_A4 + i * 2);
    }
    gcall_or_jmp(0);
    vtop--;
}


// to be compatible with Code Composer for the C67
// the first 10 parameters must be passed in registers
// (pairs for 64 bits) starting wit; A4:A5, then B4:B5 and
// ending with B12:B13.
//
// When a call is made, if the caller has its parameters
// in regs A4-B13 these must be saved before/as the call 
// parameters are loaded and restored upon return (or if/when needed).

/* generate function prolog of type 't' */
�� gfunc_prolog(CType * func_type)
{
    �� addr, align, size, func_call, i;
    Sym *sym;
    CType *type;

    sym = func_type->ref;
    func_call = sym->f.func_call;
    addr = 8;
    /* if the function returns a structure, then add an
       implicit pointer parameter */
    func_vt = sym->type;
    func_var = (sym->f.func_type == FUNC_ELLIPSIS);
    �� ((func_vt.t & VT_BTYPE) == VT_STRUCT) {
        func_vc = addr;
        addr += 4;
    }

    NoOfCurFuncArgs = 0;

    /* define parameters */
    �� ((sym = sym->next) != NULL) {
        type = &sym->type;
        sym_push(sym->v & ~SYM_FIELD, type, VT_LOCAL | lvalue_type(type->t), addr);
        size = type_size(type, &align);
        size = (size + 3) & ~3;

        // keep track of size of arguments so
        // we can translate where tcc thinks they
        // are on the stack into the appropriate reg

        TranslateStackToReg[NoOfCurFuncArgs] = size;
        NoOfCurFuncArgs++;

#�綨�� FUNC_STRUCT_PARAM_AS_PTR
        /* structs are passed as pointer */
        �� ((type->t & VT_BTYPE) == VT_STRUCT) {
            size = 4;
        }
#����
        addr += size;
    }
    func_ret_sub = 0;
    /* pascal type call ? */
    �� (func_call == FUNC_STDCALL)
        func_ret_sub = addr - 8;

    C67_MV(C67_FP, C67_A0);     //  move FP -> A0
    C67_MV(C67_SP, C67_FP);     //  move SP -> FP

    // place all the args passed in regs onto the stack

    loc = 0;
    ���� (i = 0; i < NoOfCurFuncArgs; i++) {

        ParamLocOnStack[i] = loc;       // remember where the param is
        loc += -8;

        C67_PUSH(TREG_C67_A4 + i * 2);

        �� (TranslateStackToReg[i] == 8) {
            C67_STW_PTR_PRE_INC(TREG_C67_A4 + i * 2 + 1, C67_SP, 3);    // STW  r, *+SP[1] (go back and put the other)
        }
    }

    TotalBytesPushedOnStack = -loc;

    func_sub_sp_offset = ind;   // remember where we put the stack instruction 
    C67_ADDK(0, C67_SP);        //  ADDK.L2 loc,SP  (just put zero temporarily)

    C67_PUSH(C67_A0);
    C67_PUSH(C67_B3);
}

/* generate function epilog */
�� gfunc_epilog(��)
{
    {
        �� local = (-loc + 7) & -8;    // stack must stay aligned to 8 bytes for LDDW instr
        C67_POP(C67_B3);
        C67_NOP(4);             // NOP wait for load
        C67_IREG_B_REG(0, C67_CREG_ZERO, C67_B3);       //  B.S2  B3
        C67_POP(C67_FP);
        C67_ADDK(local, C67_SP);        //  ADDK.L2 loc,SP  
        C67_Adjust_ADDK((�� *) (cur_text_section->data +
                                 func_sub_sp_offset),
                        -local + TotalBytesPushedOnStack);
        C67_NOP(3);             // NOP 
    }
}

/* generate a jump to a label */
�� gjmp(�� t)
{
    �� ind1 = ind;
    �� (nocode_wanted)
        ���� t;

    C67_MVKL(C67_A0, t);        //r=reg to load,  constant
    C67_MVKH(C67_A0, t);        //r=reg to load,  constant
    C67_IREG_B_REG(0, C67_CREG_ZERO, C67_A0);   // [!R] B.S2x  A0
    C67_NOP(5);
    ���� ind1;
}

/* generate a jump to a fixed address */
�� gjmp_addr(�� a)
{
    Sym *sym;
    // I guess this routine is used for relative short
    // local jumps, for now just handle it as the general
    // case

    // define a label that will be relocated

    sym = get_sym_ref(&char_pointer_type, cur_text_section, a, 0);
    greloc(cur_text_section, sym, ind, R_C60LO16);
    greloc(cur_text_section, sym, ind + 4, R_C60HI16);

    gjmp(0);                    // place a zero there later the symbol will be added to it
}

/* generate a test. set 'inv' to invert test. Stack entry is popped */
�� gtst(�� inv, �� t)
{
    �� ind1, n;
    �� v, *p;

    v = vtop->r & VT_VALMASK;
    �� (nocode_wanted) {
        ;
    } �� �� (v == VT_CMP) {
        /* fast case : can jump directly since flags are set */
        // C67 uses B2 sort of as flags register
        ind1 = ind;
        C67_MVKL(C67_A0, t);    //r=reg to load, constant
        C67_MVKH(C67_A0, t);    //r=reg to load, constant

        �� (C67_compare_reg != TREG_EAX &&      // check if not already in a conditional test reg
            C67_compare_reg != TREG_EDX &&
            C67_compare_reg != TREG_ST0 && C67_compare_reg != C67_B2) {
            C67_MV(C67_compare_reg, C67_B2);
            C67_compare_reg = C67_B2;
        }

        C67_IREG_B_REG(C67_invert_test ^ inv, C67_compare_reg, C67_A0); // [!R] B.S2x  A0
        C67_NOP(5);
        t = ind1;               //return where we need to patch

    } �� �� (v == VT_JMP || v == VT_JMPI) {
        /* && or || optimization */
        �� ((v & 1) == inv) {
            /* insert vtop->c jump list in t */

            // I guess the idea is to traverse to the
            // null at the end of the list and store t
            // there

            n = vtop->c.i;
            �� (n != 0) {
                p = (�� *) (cur_text_section->data + n);

                // extract 32 bit address from MVKH/MVKL
                n = ((*p >> 7) & 0xffff);
                n |= ((*(p + 1) >> 7) & 0xffff) << 16;
            }
            *p |= (t & 0xffff) << 7;
            *(p + 1) |= ((t >> 16) & 0xffff) << 7;
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
    �� r, fr, opc, t;

    ת�� (op) {
    ���� '+':
    ���� TOK_ADDC1:               /* add with carry generation */
        opc = 0;
      gen_op8:


// C67 can't do const compares, must load into a reg
// so just go to gv2 directly - tktk



        �� (op >= TOK_ULT && op <= TOK_GT)
            gv2(RC_INT_BSIDE, RC_INT);  // make sure r (src1) is on the B Side of CPU
        ��
            gv2(RC_INT, RC_INT);

        r = vtop[-1].r;
        fr = vtop[0].r;

        C67_compare_reg = C67_B2;


        �� (op == TOK_LT) {
            C67_CMPLT(r, fr, C67_B2);
            C67_invert_test = FALSE;
        } �� �� (op == TOK_GE) {
            C67_CMPLT(r, fr, C67_B2);
            C67_invert_test = TRUE;
        } �� �� (op == TOK_GT) {
            C67_CMPGT(r, fr, C67_B2);
            C67_invert_test = FALSE;
        } �� �� (op == TOK_LE) {
            C67_CMPGT(r, fr, C67_B2);
            C67_invert_test = TRUE;
        } �� �� (op == TOK_EQ) {
            C67_CMPEQ(r, fr, C67_B2);
            C67_invert_test = FALSE;
        } �� �� (op == TOK_NE) {
            C67_CMPEQ(r, fr, C67_B2);
            C67_invert_test = TRUE;
        } �� �� (op == TOK_ULT) {
            C67_CMPLTU(r, fr, C67_B2);
            C67_invert_test = FALSE;
        } �� �� (op == TOK_UGE) {
            C67_CMPLTU(r, fr, C67_B2);
            C67_invert_test = TRUE;
        } �� �� (op == TOK_UGT) {
            C67_CMPGTU(r, fr, C67_B2);
            C67_invert_test = FALSE;
        } �� �� (op == TOK_ULE) {
            C67_CMPGTU(r, fr, C67_B2);
            C67_invert_test = TRUE;
        } �� �� (op == '+')
            C67_ADD(fr, r);     // ADD  r,fr,r
        �� �� (op == '-')
            C67_SUB(fr, r);     // SUB  r,fr,r
        �� �� (op == '&')
            C67_AND(fr, r);     // AND  r,fr,r
        �� �� (op == '|')
            C67_OR(fr, r);      // OR  r,fr,r
        �� �� (op == '^')
            C67_XOR(fr, r);     // XOR  r,fr,r
        ��
            ALWAYS_ASSERT(FALSE);

        vtop--;
        �� (op >= TOK_ULT && op <= TOK_GT) {
            vtop->r = VT_CMP;
            vtop->c.i = op;
        }
        ����;
    ���� '-':
    ���� TOK_SUBC1:               /* sub with carry generation */
        opc = 5;
        ��ת gen_op8;
    ���� TOK_ADDC2:               /* add with carry use */
        opc = 2;
        ��ת gen_op8;
    ���� TOK_SUBC2:               /* sub with carry use */
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
    ���� TOK_UMULL:
        gv2(RC_INT, RC_INT);
        r = vtop[-1].r;
        fr = vtop[0].r;
        vtop--;
        C67_MPYI(fr, r);        // 32 bit multiply  fr,r,fr
        C67_NOP(8);             // NOP 8 for worst case
        ����;
    ���� TOK_SHL:
        gv2(RC_INT_BSIDE, RC_INT_BSIDE);        // shift amount must be on same side as dst
        r = vtop[-1].r;
        fr = vtop[0].r;
        vtop--;
        C67_SHL(fr, r);         // arithmetic/logical shift
        ����;

    ���� TOK_SHR:
        gv2(RC_INT_BSIDE, RC_INT_BSIDE);        // shift amount must be on same side as dst
        r = vtop[-1].r;
        fr = vtop[0].r;
        vtop--;
        C67_SHRU(fr, r);        // logical shift
        ����;

    ���� TOK_SAR:
        gv2(RC_INT_BSIDE, RC_INT_BSIDE);        // shift amount must be on same side as dst
        r = vtop[-1].r;
        fr = vtop[0].r;
        vtop--;
        C67_SHR(fr, r);         // arithmetic shift
        ����;

    ���� '/':
        t = TOK__divi;
      call_func:
        vswap();
        /* call generic idiv function */
        vpush_global_sym(&func_old_type, t);
        vrott(3);
        gfunc_call(2);
        vpushi(0);
        vtop->r = REG_IRET;
        vtop->r2 = VT_CONST;
        ����;
    ���� TOK_UDIV:
    ���� TOK_PDIV:
        t = TOK__divu;
        ��ת call_func;
    ���� '%':
        t = TOK__remi;
        ��ת call_func;
    ���� TOK_UMOD:
        t = TOK__remu;
        ��ת call_func;

    ȱʡ:
        opc = 7;
        ��ת gen_op8;
    }
}

/* generate a floating point operation 'v = t1 op t2' instruction. The
   two operands are guaranteed to have the same floating point type */
/* XXX: need to use ST1 too */
�� gen_opf(�� op)
{
    �� ft, fc, fr, r;

    �� (op >= TOK_ULT && op <= TOK_GT)
        gv2(RC_EDX, RC_EAX);    // make sure src2 is on b side
    ��
        gv2(RC_FLOAT, RC_FLOAT);        // make sure src2 is on b side

    ft = vtop->type.t;
    fc = vtop->c.i;
    r = vtop->r;
    fr = vtop[-1].r;


    �� ((ft & VT_BTYPE) == VT_LDOUBLE)
        tcc_error("long doubles not supported");

    �� (op >= TOK_ULT && op <= TOK_GT) {

        r = vtop[-1].r;
        fr = vtop[0].r;

        C67_compare_reg = C67_B2;

        �� (op == TOK_LT) {
            �� ((ft & VT_BTYPE) == VT_DOUBLE)
                C67_CMPLTDP(r, fr, C67_B2);
            ��
                C67_CMPLTSP(r, fr, C67_B2);

            C67_invert_test = FALSE;
        } �� �� (op == TOK_GE) {
            �� ((ft & VT_BTYPE) == VT_DOUBLE)
                C67_CMPLTDP(r, fr, C67_B2);
            ��
                C67_CMPLTSP(r, fr, C67_B2);

            C67_invert_test = TRUE;
        } �� �� (op == TOK_GT) {
            �� ((ft & VT_BTYPE) == VT_DOUBLE)
                C67_CMPGTDP(r, fr, C67_B2);
            ��
                C67_CMPGTSP(r, fr, C67_B2);

            C67_invert_test = FALSE;
        } �� �� (op == TOK_LE) {
            �� ((ft & VT_BTYPE) == VT_DOUBLE)
                C67_CMPGTDP(r, fr, C67_B2);
            ��
                C67_CMPGTSP(r, fr, C67_B2);

            C67_invert_test = TRUE;
        } �� �� (op == TOK_EQ) {
            �� ((ft & VT_BTYPE) == VT_DOUBLE)
                C67_CMPEQDP(r, fr, C67_B2);
            ��
                C67_CMPEQSP(r, fr, C67_B2);

            C67_invert_test = FALSE;
        } �� �� (op == TOK_NE) {
            �� ((ft & VT_BTYPE) == VT_DOUBLE)
                C67_CMPEQDP(r, fr, C67_B2);
            ��
                C67_CMPEQSP(r, fr, C67_B2);

            C67_invert_test = TRUE;
        } �� {
            ALWAYS_ASSERT(FALSE);
        }
        vtop->r = VT_CMP;       // tell TCC that result is in "flags" actually B2
    } �� {
        �� (op == '+') {
            �� ((ft & VT_BTYPE) == VT_DOUBLE) {
                C67_ADDDP(r, fr);       // ADD  fr,r,fr
                C67_NOP(6);
            } �� {
                C67_ADDSP(r, fr);       // ADD  fr,r,fr
                C67_NOP(3);
            }
            vtop--;
        } �� �� (op == '-') {
            �� ((ft & VT_BTYPE) == VT_DOUBLE) {
                C67_SUBDP(r, fr);       // SUB  fr,r,fr
                C67_NOP(6);
            } �� {
                C67_SUBSP(r, fr);       // SUB  fr,r,fr
                C67_NOP(3);
            }
            vtop--;
        } �� �� (op == '*') {
            �� ((ft & VT_BTYPE) == VT_DOUBLE) {
                C67_MPYDP(r, fr);       // MPY  fr,r,fr
                C67_NOP(9);
            } �� {
                C67_MPYSP(r, fr);       // MPY  fr,r,fr
                C67_NOP(3);
            }
            vtop--;
        } �� �� (op == '/') {
            �� ((ft & VT_BTYPE) == VT_DOUBLE) {
                // must call intrinsic DP floating point divide
                vswap();
                /* call generic idiv function */
                vpush_global_sym(&func_old_type, TOK__divd);
                vrott(3);
                gfunc_call(2);
                vpushi(0);
                vtop->r = REG_FRET;
                vtop->r2 = REG_LRET;

            } �� {
                // must call intrinsic SP floating point divide
                vswap();
                /* call generic idiv function */
                vpush_global_sym(&func_old_type, TOK__divf);
                vrott(3);
                gfunc_call(2);
                vpushi(0);
                vtop->r = REG_FRET;
                vtop->r2 = VT_CONST;
            }
        } ��
            ALWAYS_ASSERT(FALSE);


    }
}


/* convert integers to fp 't' type. Must handle 'int', 'unsigned int'
   and 'long long' cases. */
�� gen_cvt_itof(�� t)
{
    �� r;

    gv(RC_INT);
    r = vtop->r;

    �� ((t & VT_BTYPE) == VT_DOUBLE) {
        �� (t & VT_UNSIGNED)
            C67_INTDPU(r, r);
        ��
            C67_INTDP(r, r);

        C67_NOP(4);
        vtop->type.t = VT_DOUBLE;
    } �� {
        �� (t & VT_UNSIGNED)
            C67_INTSPU(r, r);
        ��
            C67_INTSP(r, r);
        C67_NOP(3);
        vtop->type.t = VT_FLOAT;
    }

}

/* convert fp to int 't' type */
/* XXX: handle long long case */
�� gen_cvt_ftoi(�� t)
{
    �� r;

    gv(RC_FLOAT);
    r = vtop->r;

    �� (t != VT_INT)
        tcc_error("long long not supported");
    �� {
        �� ((vtop->type.t & VT_BTYPE) == VT_DOUBLE) {
            C67_DPTRUNC(r, r);
            C67_NOP(3);
        } �� {
            C67_SPTRUNC(r, r);
            C67_NOP(3);
        }

        vtop->type.t = VT_INT;

    }
}

/* convert from one floating point type to another */
�� gen_cvt_ftof(�� t)
{
    �� r, r2;

    �� ((vtop->type.t & VT_BTYPE) == VT_DOUBLE &&
        (t & VT_BTYPE) == VT_FLOAT) {
        // convert double to float

        gv(RC_FLOAT);           // get it in a register pair

        r = vtop->r;

        C67_DPSP(r, r);         // convert it to SP same register
        C67_NOP(3);

        vtop->type.t = VT_FLOAT;
        vtop->r2 = VT_CONST;    // set this as unused
    } �� �� ((vtop->type.t & VT_BTYPE) == VT_FLOAT &&
               (t & VT_BTYPE) == VT_DOUBLE) {
        // convert float to double

        gv(RC_FLOAT);           // get it in a register

        r = vtop->r;

        �� (r == TREG_EAX) {    // make sure the paired reg is avail
            r2 = get_reg(RC_ECX);
        } �� �� (r == TREG_EDX) {
            r2 = get_reg(RC_ST0);
        } �� {
            ALWAYS_ASSERT(FALSE);
            r2 = 0; /* avoid warning */
        }

        C67_SPDP(r, r);         // convert it to DP same register
        C67_NOP(1);

        vtop->type.t = VT_DOUBLE;
        vtop->r2 = r2;          // set this as unused
    } �� {
        ALWAYS_ASSERT(FALSE);
    }
}

/* computed goto support */
�� ggoto(��)
{
    gcall_or_jmp(1);
    vtop--;
}

/* Save the stack pointer onto the stack and return the location of its address */
ST_FUNC �� gen_vla_sp_save(�� addr) {
    tcc_error("variable length arrays unsupported for this target");
}

/* Restore the SP from a location on the stack */
ST_FUNC �� gen_vla_sp_restore(�� addr) {
    tcc_error("variable length arrays unsupported for this target");
}

/* Subtract from the stack pointer, and push the resulting value onto the stack */
ST_FUNC �� gen_vla_alloc(CType *type, �� align) {
    tcc_error("variable length arrays unsupported for this target");
}

/* end of C67 code generator */
/*************************************************************/
#����
/*************************************************************/
