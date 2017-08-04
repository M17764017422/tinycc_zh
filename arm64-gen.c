/*
 *  A64 code generator for TCC
 *
 *  Copyright (c) 2014-2015 Edmund Grimley Evans
 *
 * Copying and distribution of this file, with or without modification,
 * are permitted in any medium without royalty provided the copyright
 * notice and this notice are preserved.  This file is offered as-is,
 * without any warranty.
 */

#�綨�� TARGET_DEFS_ONLY

// Number of registers available to allocator:
#���� NB_REGS 28 // x0-x18, x30, v0-v7

#���� TREG_R(x) (x) // x = 0..18
#���� TREG_R30  19
#���� TREG_F(x) (x + 20) // x = 0..7

// Register classes sorted from more general to more precise:
#���� RC_INT (1 << 0)
#���� RC_FLOAT (1 << 1)
#���� RC_R(x) (1 << (2 + (x))) // x = 0..18
#���� RC_R30  (1 << 21)
#���� RC_F(x) (1 << (22 + (x))) // x = 0..7

#���� RC_IRET (RC_R(0)) // int return register class
#���� RC_FRET (RC_F(0)) // float return register class

#���� REG_IRET (TREG_R(0)) // int return register number
#���� REG_FRET (TREG_F(0)) // float return register number

#���� PTR_SIZE 8

#���� LDOUBLE_SIZE 16
#���� LDOUBLE_ALIGN 16

#���� MAX_ALIGN 16

#���� CHAR_IS_UNSIGNED

/******************************************************/
#�� /* ! TARGET_DEFS_ONLY */
/******************************************************/
#���� "tcc.h"
#���� <assert.h>

ST_DATA ���� �� reg_classes[NB_REGS] = {
  RC_INT | RC_R(0),
  RC_INT | RC_R(1),
  RC_INT | RC_R(2),
  RC_INT | RC_R(3),
  RC_INT | RC_R(4),
  RC_INT | RC_R(5),
  RC_INT | RC_R(6),
  RC_INT | RC_R(7),
  RC_INT | RC_R(8),
  RC_INT | RC_R(9),
  RC_INT | RC_R(10),
  RC_INT | RC_R(11),
  RC_INT | RC_R(12),
  RC_INT | RC_R(13),
  RC_INT | RC_R(14),
  RC_INT | RC_R(15),
  RC_INT | RC_R(16),
  RC_INT | RC_R(17),
  RC_INT | RC_R(18),
  RC_R30, // not in RC_INT as we make special use of x30
  RC_FLOAT | RC_F(0),
  RC_FLOAT | RC_F(1),
  RC_FLOAT | RC_F(2),
  RC_FLOAT | RC_F(3),
  RC_FLOAT | RC_F(4),
  RC_FLOAT | RC_F(5),
  RC_FLOAT | RC_F(6),
  RC_FLOAT | RC_F(7)
};

#���� IS_FREG(x) ((x) >= TREG_F(0))

��̬ uint32_t intr(�� r)
{
    assert(TREG_R(0) <= r && r <= TREG_R30);
    ���� r < TREG_R30 ? r : 30;
}

��̬ uint32_t fltr(�� r)
{
    assert(TREG_F(0) <= r && r <= TREG_F(7));
    ���� r - TREG_F(0);
}

// Add an instruction to text section:
ST_FUNC �� o(�޷� �� c)
{
    �� ind1 = ind + 4;
    �� (nocode_wanted)
        ����;
    �� (ind1 > cur_text_section->data_allocated)
        section_realloc(cur_text_section, ind1);
    write32le(cur_text_section->data + ind, c);
    ind = ind1;
}

��̬ �� arm64_encode_bimm64(uint64_t x)
{
    �� neg = x & 1;
    �� rep, pos, len;

    �� (neg)
        x = ~x;
    �� (!x)
        ���� -1;

    �� (x >> 2 == (x & (((uint64_t)1 << (64 - 2)) - 1)))
        rep = 2, x &= ((uint64_t)1 << 2) - 1;
    �� �� (x >> 4 == (x & (((uint64_t)1 << (64 - 4)) - 1)))
        rep = 4, x &= ((uint64_t)1 <<  4) - 1;
    �� �� (x >> 8 == (x & (((uint64_t)1 << (64 - 8)) - 1)))
        rep = 8, x &= ((uint64_t)1 <<  8) - 1;
    �� �� (x >> 16 == (x & (((uint64_t)1 << (64 - 16)) - 1)))
        rep = 16, x &= ((uint64_t)1 << 16) - 1;
    �� �� (x >> 32 == (x & (((uint64_t)1 << (64 - 32)) - 1)))
        rep = 32, x &= ((uint64_t)1 << 32) - 1;
    ��
        rep = 64;

    pos = 0;
    �� (!(x & (((uint64_t)1 << 32) - 1))) x >>= 32, pos += 32;
    �� (!(x & (((uint64_t)1 << 16) - 1))) x >>= 16, pos += 16;
    �� (!(x & (((uint64_t)1 <<  8) - 1))) x >>= 8, pos += 8;
    �� (!(x & (((uint64_t)1 <<  4) - 1))) x >>= 4, pos += 4;
    �� (!(x & (((uint64_t)1 <<  2) - 1))) x >>= 2, pos += 2;
    �� (!(x & (((uint64_t)1 <<  1) - 1))) x >>= 1, pos += 1;

    len = 0;
    �� (!(~x & (((uint64_t)1 << 32) - 1))) x >>= 32, len += 32;
    �� (!(~x & (((uint64_t)1 << 16) - 1))) x >>= 16, len += 16;
    �� (!(~x & (((uint64_t)1 << 8) - 1))) x >>= 8, len += 8;
    �� (!(~x & (((uint64_t)1 << 4) - 1))) x >>= 4, len += 4;
    �� (!(~x & (((uint64_t)1 << 2) - 1))) x >>= 2, len += 2;
    �� (!(~x & (((uint64_t)1 << 1) - 1))) x >>= 1, len += 1;

    �� (x)
        ���� -1;
    �� (neg) {
        pos = (pos + len) & (rep - 1);
        len = rep - len;
    }
    ���� ((0x1000 & rep << 6) | (((rep - 1) ^ 31) << 1 & 63) |
            ((rep - pos) & (rep - 1)) << 6 | (len - 1));
}

��̬ uint32_t arm64_movi(�� r, uint64_t x)
{
    uint64_t m = 0xffff;
    �� e;
    �� (!(x & ~m))
        ���� 0x52800000 | r | x << 5; // movz w(r),#(x)
    �� (!(x & ~(m << 16)))
        ���� 0x52a00000 | r | x >> 11; // movz w(r),#(x >> 16),lsl #16
    �� (!(x & ~(m << 32)))
        ���� 0xd2c00000 | r | x >> 27; // movz x(r),#(x >> 32),lsl #32
    �� (!(x & ~(m << 48)))
        ���� 0xd2e00000 | r | x >> 43; // movz x(r),#(x >> 48),lsl #48
    �� ((x & ~m) == m << 16)
        ���� (0x12800000 | r |
                (~x << 5 & 0x1fffe0)); // movn w(r),#(~x)
    �� ((x & ~(m << 16)) == m)
        ���� (0x12a00000 | r |
                (~x >> 11 & 0x1fffe0)); // movn w(r),#(~x >> 16),lsl #16
    �� (!~(x | m))
        ���� (0x92800000 | r |
                (~x << 5 & 0x1fffe0)); // movn x(r),#(~x)
    �� (!~(x | m << 16))
        ���� (0x92a00000 | r |
                (~x >> 11 & 0x1fffe0)); // movn x(r),#(~x >> 16),lsl #16
    �� (!~(x | m << 32))
        ���� (0x92c00000 | r |
                (~x >> 27 & 0x1fffe0)); // movn x(r),#(~x >> 32),lsl #32
    �� (!~(x | m << 48))
        ���� (0x92e00000 | r |
                (~x >> 43 & 0x1fffe0)); // movn x(r),#(~x >> 32),lsl #32
    �� (!(x >> 32) && (e = arm64_encode_bimm64(x | x << 32)) >= 0)
        ���� 0x320003e0 | r | (uint32_t)e << 10; // movi w(r),#(x)
    �� ((e = arm64_encode_bimm64(x)) >= 0)
        ���� 0xb20003e0 | r | (uint32_t)e << 10; // movi x(r),#(x)
    ���� 0;
}

��̬ �� arm64_movimm(�� r, uint64_t x)
{
    uint32_t i;
    �� ((i = arm64_movi(r, x)))
        o(i); // a single MOV
    �� {
        // MOVZ/MOVN and 1-3 MOVKs
        �� z = 0, m = 0;
        uint32_t mov1 = 0xd2800000; // movz
        uint64_t x1 = x;
        ���� (i = 0; i < 64; i += 16) {
            z += !(x >> i & 0xffff);
            m += !(~x >> i & 0xffff);
        }
        �� (m > z) {
            x1 = ~x;
            mov1 = 0x92800000; // movn
        }
        ���� (i = 0; i < 64; i += 16)
            �� (x1 >> i & 0xffff) {
                o(mov1 | r | (x1 >> i & 0xffff) << 5 | i << 17);
                // movz/movn x(r),#(*),lsl #(i)
                ����;
            }
        ���� (i += 16; i < 64; i += 16)
            �� (x1 >> i & 0xffff)
                o(0xf2800000 | r | (x >> i & 0xffff) << 5 | i << 17);
                // movk x(r),#(*),lsl #(i)
    }
}

// Patch all branches in list pointed to by t to branch to a:
ST_FUNC �� gsym_addr(�� t_, �� a_)
{
    uint32_t t = t_;
    uint32_t a = a_;
    �� (t) {
        �޷� �� *ptr = cur_text_section->data + t;
        uint32_t next = read32le(ptr);
        �� (a - t + 0x8000000 >= 0x10000000)
            tcc_error("branch out of range");
        write32le(ptr, (a - t == 4 ? 0xd503201f : // nop
                        0x14000000 | ((a - t) >> 2 & 0x3ffffff))); // b
        t = next;
    }
}

// Patch all branches in list pointed to by t to branch to current location:
ST_FUNC �� gsym(�� t)
{
    gsym_addr(t, ind);
}

��̬ �� arm64_type_size(�� t)
{
    ת�� (t & VT_BTYPE) {
    ���� VT_INT: ���� 2;
    ���� VT_BYTE: ���� 0;
    ���� VT_SHORT: ���� 1;
    ���� VT_PTR: ���� 3;
    ���� VT_FUNC: ���� 3;
    ���� VT_FLOAT: ���� 2;
    ���� VT_DOUBLE: ���� 3;
    ���� VT_LDOUBLE: ���� 4;
    ���� VT_BOOL: ���� 0;
    ���� VT_LLONG: ���� 3;
    }
    assert(0);
    ���� 0;
}

��̬ �� arm64_spoff(�� reg, uint64_t off)
{
    uint32_t sub = off >> 63;
    �� (sub)
        off = -off;
    �� (off < 4096)
        o(0x910003e0 | sub << 30 | reg | off << 10);
        // (add|sub) x(reg),sp,#(off)
    �� {
        arm64_movimm(30, off); // use x30 for offset
        o(0x8b3e63e0 | sub << 30 | reg); // (add|sub) x(reg),sp,x30
    }
}

��̬ �� arm64_ldrx(�� sg, �� sz_, �� dst, �� bas, uint64_t off)
{
    uint32_t sz = sz_;
    �� (sz >= 2)
        sg = 0;
    �� (!(off & ~((uint32_t)0xfff << sz)))
        o(0x39400000 | dst | bas << 5 | off << (10 - sz) |
          (uint32_t)!!sg << 23 | sz << 30); // ldr(*) x(dst),[x(bas),#(off)]
    �� �� (off < 256 || -off <= 256)
        o(0x38400000 | dst | bas << 5 | (off & 511) << 12 |
          (uint32_t)!!sg << 23 | sz << 30); // ldur(*) x(dst),[x(bas),#(off)]
    �� {
        arm64_movimm(30, off); // use x30 for offset
        o(0x38206800 | dst | bas << 5 | (uint32_t)30 << 16 |
          (uint32_t)(!!sg + 1) << 22 | sz << 30); // ldr(*) x(dst),[x(bas),x30]
    }
}

��̬ �� arm64_ldrv(�� sz_, �� dst, �� bas, uint64_t off)
{
    uint32_t sz = sz_;
    �� (!(off & ~((uint32_t)0xfff << sz)))
        o(0x3d400000 | dst | bas << 5 | off << (10 - sz) |
          (sz & 4) << 21 | (sz & 3) << 30); // ldr (s|d|q)(dst),[x(bas),#(off)]
    �� �� (off < 256 || -off <= 256)
        o(0x3c400000 | dst | bas << 5 | (off & 511) << 12 |
          (sz & 4) << 21 | (sz & 3) << 30); // ldur (s|d|q)(dst),[x(bas),#(off)]
    �� {
        arm64_movimm(30, off); // use x30 for offset
        o(0x3c606800 | dst | bas << 5 | (uint32_t)30 << 16 |
          sz << 30 | (sz & 4) << 21); // ldr (s|d|q)(dst),[x(bas),x30]
    }
}

��̬ �� arm64_ldrs(�� reg_, �� size)
{
    uint32_t reg = reg_;
    // Use x30 for intermediate value in some cases.
    ת�� (size) {
    ȱʡ: assert(0); ����;
    ���� 1:
        arm64_ldrx(0, 0, reg, reg, 0);
        ����;
    ���� 2:
        arm64_ldrx(0, 1, reg, reg, 0);
        ����;
    ���� 3:
        arm64_ldrx(0, 1, 30, reg, 0);
        arm64_ldrx(0, 0, reg, reg, 2);
        o(0x2a0043c0 | reg | reg << 16); // orr x(reg),x30,x(reg),lsl #16
        ����;
    ���� 4:
        arm64_ldrx(0, 2, reg, reg, 0);
        ����;
    ���� 5:
        arm64_ldrx(0, 2, 30, reg, 0);
        arm64_ldrx(0, 0, reg, reg, 4);
        o(0xaa0083c0 | reg | reg << 16); // orr x(reg),x30,x(reg),lsl #32
        ����;
    ���� 6:
        arm64_ldrx(0, 2, 30, reg, 0);
        arm64_ldrx(0, 1, reg, reg, 4);
        o(0xaa0083c0 | reg | reg << 16); // orr x(reg),x30,x(reg),lsl #32
        ����;
    ���� 7:
        arm64_ldrx(0, 2, 30, reg, 0);
        arm64_ldrx(0, 2, reg, reg, 3);
        o(0x53087c00 | reg | reg << 5); // lsr w(reg), w(reg), #8
        o(0xaa0083c0 | reg | reg << 16); // orr x(reg),x30,x(reg),lsl #32
        ����;
    ���� 8:
        arm64_ldrx(0, 3, reg, reg, 0);
        ����;
    ���� 9:
        arm64_ldrx(0, 0, reg + 1, reg, 8);
        arm64_ldrx(0, 3, reg, reg, 0);
        ����;
    ���� 10:
        arm64_ldrx(0, 1, reg + 1, reg, 8);
        arm64_ldrx(0, 3, reg, reg, 0);
        ����;
    ���� 11:
        arm64_ldrx(0, 2, reg + 1, reg, 7);
        o(0x53087c00 | (reg+1) | (reg+1) << 5); // lsr w(reg+1), w(reg+1), #8
        arm64_ldrx(0, 3, reg, reg, 0);
        ����;
    ���� 12:
        arm64_ldrx(0, 2, reg + 1, reg, 8);
        arm64_ldrx(0, 3, reg, reg, 0);
        ����;
    ���� 13:
        arm64_ldrx(0, 3, reg + 1, reg, 5);
        o(0xd358fc00 | (reg+1) | (reg+1) << 5); // lsr x(reg+1), x(reg+1), #24
        arm64_ldrx(0, 3, reg, reg, 0);
        ����;
    ���� 14:
        arm64_ldrx(0, 3, reg + 1, reg, 6);
        o(0xd350fc00 | (reg+1) | (reg+1) << 5); // lsr x(reg+1), x(reg+1), #16
        arm64_ldrx(0, 3, reg, reg, 0);
        ����;
    ���� 15:
        arm64_ldrx(0, 3, reg + 1, reg, 7);
        o(0xd348fc00 | (reg+1) | (reg+1) << 5); // lsr x(reg+1), x(reg+1), #8
        arm64_ldrx(0, 3, reg, reg, 0);
        ����;
    ���� 16:
        o(0xa9400000 | reg | (reg+1) << 10 | reg << 5);
        // ldp x(reg),x(reg+1),[x(reg)]
        ����;
    }
}

��̬ �� arm64_strx(�� sz_, �� dst, �� bas, uint64_t off)
{
    uint32_t sz = sz_;
    �� (!(off & ~((uint32_t)0xfff << sz)))
        o(0x39000000 | dst | bas << 5 | off << (10 - sz) | sz << 30);
        // str(*) x(dst),[x(bas],#(off)]
    �� �� (off < 256 || -off <= 256)
        o(0x38000000 | dst | bas << 5 | (off & 511) << 12 | sz << 30);
        // stur(*) x(dst),[x(bas],#(off)]
    �� {
        arm64_movimm(30, off); // use x30 for offset
        o(0x38206800 | dst | bas << 5 | (uint32_t)30 << 16 | sz << 30);
        // str(*) x(dst),[x(bas),x30]
    }
}

��̬ �� arm64_strv(�� sz_, �� dst, �� bas, uint64_t off)
{
    uint32_t sz = sz_;
    �� (!(off & ~((uint32_t)0xfff << sz)))
        o(0x3d000000 | dst | bas << 5 | off << (10 - sz) |
          (sz & 4) << 21 | (sz & 3) << 30); // str (s|d|q)(dst),[x(bas),#(off)]
    �� �� (off < 256 || -off <= 256)
        o(0x3c000000 | dst | bas << 5 | (off & 511) << 12 |
          (sz & 4) << 21 | (sz & 3) << 30); // stur (s|d|q)(dst),[x(bas),#(off)]
    �� {
        arm64_movimm(30, off); // use x30 for offset
        o(0x3c206800 | dst | bas << 5 | (uint32_t)30 << 16 |
          sz << 30 | (sz & 4) << 21); // str (s|d|q)(dst),[x(bas),x30]
    }
}

��̬ �� arm64_sym(�� r, Sym *sym, �޷� �� addend)
{
    // Currently TCC's linker does not generate COPY relocations for
    // STT_OBJECTs when tcc is invoked with "-run". This typically
    // results in "R_AARCH64_ADR_PREL_PG_HI21 relocation failed" when
    // a program refers to stdin. A workaround is to avoid that
    // relocation and use only relocations with unlimited range.
    �� avoid_adrp = 1;

    �� (avoid_adrp || sym->a.weak) {
        // (GCC uses a R_AARCH64_ABS64 in this case.)
        greloca(cur_text_section, sym, ind, R_AARCH64_MOVW_UABS_G0_NC, addend);
        o(0xd2800000 | r); // mov x(rt),#0,lsl #0
        greloca(cur_text_section, sym, ind, R_AARCH64_MOVW_UABS_G1_NC, addend);
        o(0xf2a00000 | r); // movk x(rt),#0,lsl #16
        greloca(cur_text_section, sym, ind, R_AARCH64_MOVW_UABS_G2_NC, addend);
        o(0xf2c00000 | r); // movk x(rt),#0,lsl #32
        greloca(cur_text_section, sym, ind, R_AARCH64_MOVW_UABS_G3, addend);
        o(0xf2e00000 | r); // movk x(rt),#0,lsl #48
    }
    �� {
        greloca(cur_text_section, sym, ind, R_AARCH64_ADR_PREL_PG_HI21, addend);
        o(0x90000000 | r);
        greloca(cur_text_section, sym, ind, R_AARCH64_ADD_ABS_LO12_NC, addend);
        o(0x91000000 | r | r << 5);
    }
}

ST_FUNC �� load(�� r, SValue *sv)
{
    �� svtt = sv->type.t;
    �� svr = sv->r & ~VT_LVAL_TYPE;
    �� svrv = svr & VT_VALMASK;
    uint64_t svcul = (uint32_t)sv->c.i;
    svcul = svcul >> 31 & 1 ? svcul - ((uint64_t)1 << 32) : svcul;

    �� (svr == (VT_LOCAL | VT_LVAL)) {
        �� (IS_FREG(r))
            arm64_ldrv(arm64_type_size(svtt), fltr(r), 29, svcul);
        ��
            arm64_ldrx(!(svtt & VT_UNSIGNED), arm64_type_size(svtt),
                       intr(r), 29, svcul);
        ����;
    }

    �� ((svr & ~VT_VALMASK) == VT_LVAL && svrv < VT_CONST) {
        �� (IS_FREG(r))
            arm64_ldrv(arm64_type_size(svtt), fltr(r), intr(svrv), 0);
        ��
            arm64_ldrx(!(svtt & VT_UNSIGNED), arm64_type_size(svtt),
                       intr(r), intr(svrv), 0);
        ����;
    }

    �� (svr == (VT_CONST | VT_LVAL | VT_SYM)) {
        arm64_sym(30, sv->sym, svcul); // use x30 for address
        �� (IS_FREG(r))
            arm64_ldrv(arm64_type_size(svtt), fltr(r), 30, 0);
        ��
            arm64_ldrx(!(svtt & VT_UNSIGNED), arm64_type_size(svtt),
                       intr(r), 30, 0);
        ����;
    }

    �� (svr == (VT_CONST | VT_SYM)) {
        arm64_sym(intr(r), sv->sym, svcul);
        ����;
    }

    �� (svr == VT_CONST) {
        �� ((svtt & VT_BTYPE) != VT_VOID)
            arm64_movimm(intr(r), arm64_type_size(svtt) == 3 ?
                         sv->c.i : (uint32_t)svcul);
        ����;
    }

    �� (svr < VT_CONST) {
        �� (IS_FREG(r) && IS_FREG(svr))
            �� (svtt == VT_LDOUBLE)
                o(0x4ea01c00 | fltr(r) | fltr(svr) << 5);
                    // mov v(r).16b,v(svr).16b
            ��
                o(0x1e604000 | fltr(r) | fltr(svr) << 5); // fmov d(r),d(svr)
        �� �� (!IS_FREG(r) && !IS_FREG(svr))
            o(0xaa0003e0 | intr(r) | intr(svr) << 16); // mov x(r),x(svr)
        ��
            assert(0);
      ����;
    }

    �� (svr == VT_LOCAL) {
        �� (-svcul < 0x1000)
            o(0xd10003a0 | intr(r) | -svcul << 10); // sub x(r),x29,#...
        �� {
            arm64_movimm(30, -svcul); // use x30 for offset
            o(0xcb0003a0 | intr(r) | (uint32_t)30 << 16); // sub x(r),x29,x30
        }
        ����;
    }

    �� (svr == VT_JMP || svr == VT_JMPI) {
        �� t = (svr == VT_JMPI);
        arm64_movimm(intr(r), t);
        o(0x14000002); // b .+8
        gsym(svcul);
        arm64_movimm(intr(r), t ^ 1);
        ����;
    }

    �� (svr == (VT_LLOCAL | VT_LVAL)) {
        arm64_ldrx(0, 3, 30, 29, svcul); // use x30 for offset
        �� (IS_FREG(r))
            arm64_ldrv(arm64_type_size(svtt), fltr(r), 30, 0);
        ��
            arm64_ldrx(!(svtt & VT_UNSIGNED), arm64_type_size(svtt),
                       intr(r), 30, 0);
        ����;
    }

    printf("load(%x, (%x, %x, %llx))\n", r, svtt, sv->r, (�� ��)svcul);
    assert(0);
}

ST_FUNC �� store(�� r, SValue *sv)
{
    �� svtt = sv->type.t;
    �� svr = sv->r & ~VT_LVAL_TYPE;
    �� svrv = svr & VT_VALMASK;
    uint64_t svcul = (uint32_t)sv->c.i;
    svcul = svcul >> 31 & 1 ? svcul - ((uint64_t)1 << 32) : svcul;

    �� (svr == (VT_LOCAL | VT_LVAL)) {
        �� (IS_FREG(r))
            arm64_strv(arm64_type_size(svtt), fltr(r), 29, svcul);
        ��
            arm64_strx(arm64_type_size(svtt), intr(r), 29, svcul);
        ����;
    }

    �� ((svr & ~VT_VALMASK) == VT_LVAL && svrv < VT_CONST) {
        �� (IS_FREG(r))
            arm64_strv(arm64_type_size(svtt), fltr(r), intr(svrv), 0);
        ��
            arm64_strx(arm64_type_size(svtt), intr(r), intr(svrv), 0);
        ����;
    }

    �� (svr == (VT_CONST | VT_LVAL | VT_SYM)) {
        arm64_sym(30, sv->sym, svcul); // use x30 for address
        �� (IS_FREG(r))
            arm64_strv(arm64_type_size(svtt), fltr(r), 30, 0);
        ��
            arm64_strx(arm64_type_size(svtt), intr(r), 30, 0);
        ����;
    }

    printf("store(%x, (%x, %x, %llx))\n", r, svtt, sv->r, (�� ��)svcul);
    assert(0);
}

��̬ �� arm64_gen_bl_or_b(�� b)
{
    �� ((vtop->r & (VT_VALMASK | VT_LVAL)) == VT_CONST) {
        assert(!b && (vtop->r & VT_SYM));
        greloca(cur_text_section, vtop->sym, ind, R_AARCH64_CALL26, 0);
        o(0x94000000); // bl .
    }
    ��
        o(0xd61f0000 | (uint32_t)!b << 21 | intr(gv(RC_R30)) << 5); // br/blr
}

��̬ �� arm64_hfa_aux(CType *type, �� *fsize, �� num)
{
    �� (is_float(type->t)) {
        �� a, n = type_size(type, &a);
        �� (num >= 4 || (*fsize && *fsize != n))
            ���� -1;
        *fsize = n;
        ���� num + 1;
    }
    �� �� ((type->t & VT_BTYPE) == VT_STRUCT) {
        �� is_struct = 0; // rather than union
        Sym *field;
        ���� (field = type->ref->next; field; field = field->next)
            �� (field->c) {
                is_struct = 1;
                ����;
            }
        �� (is_struct) {
            �� num0 = num;
            ���� (field = type->ref->next; field; field = field->next) {
                �� (field->c != (num - num0) * *fsize)
                    ���� -1;
                num = arm64_hfa_aux(&field->type, fsize, num);
                �� (num == -1)
                    ���� -1;
            }
            �� (type->ref->c != (num - num0) * *fsize)
                ���� -1;
            ���� num;
        }
        �� { // union
            �� num0 = num;
            ���� (field = type->ref->next; field; field = field->next) {
                �� num1 = arm64_hfa_aux(&field->type, fsize, num0);
                �� (num1 == -1)
                    ���� -1;
                num = num1 < num ? num : num1;
            }
            �� (type->ref->c != (num - num0) * *fsize)
                ���� -1;
            ���� num;
        }
    }
    �� �� (type->t & VT_ARRAY) {
        �� num1;
        �� (!type->ref->c)
            ���� num;
        num1 = arm64_hfa_aux(&type->ref->type, fsize, num);
        �� (num1 == -1 || (num1 != num && type->ref->c > 4))
            ���� -1;
        num1 = num + type->ref->c * (num1 - num);
        �� (num1 > 4)
            ���� -1;
        ���� num1;
    }
    ���� -1;
}

��̬ �� arm64_hfa(CType *type, �� *fsize)
{
    �� ((type->t & VT_BTYPE) == VT_STRUCT || (type->t & VT_ARRAY)) {
        �� sz = 0;
        �� n = arm64_hfa_aux(type, &sz, 0);
        �� (0 < n && n <= 4) {
            �� (fsize)
                *fsize = sz;
            ���� n;
        }
    }
    ���� 0;
}

��̬ �޷� �� arm64_pcs_aux(�� n, CType **type, �޷� �� *a)
{
    �� nx = 0; // next integer register
    �� nv = 0; // next vector register
    �޷� �� ns = 32; // next stack offset
    �� i;

    ���� (i = 0; i < n; i++) {
        �� hfa = arm64_hfa(type[i], 0);
        �� size, align;

        �� ((type[i]->t & VT_ARRAY) ||
            (type[i]->t & VT_BTYPE) == VT_FUNC)
            size = align = 8;
        ��
            size = type_size(type[i], &align);

        �� (hfa)
            // B.2
            ;
        �� �� (size > 16) {
            // B.3: replace with pointer
            �� (nx < 8)
                a[i] = nx++ << 1 | 1;
            �� {
                ns = (ns + 7) & ~7;
                a[i] = ns | 1;
                ns += 8;
            }
            ����;
        }
        �� �� ((type[i]->t & VT_BTYPE) == VT_STRUCT)
            // B.4
            size = (size + 7) & ~7;

        // C.1
        �� (is_float(type[i]->t) && nv < 8) {
            a[i] = 16 + (nv++ << 1);
            ����;
        }

        // C.2
        �� (hfa && nv + hfa <= 8) {
            a[i] = 16 + (nv << 1);
            nv += hfa;
            ����;
        }

        // C.3
        �� (hfa) {
            nv = 8;
            size = (size + 7) & ~7;
        }

        // C.4
        �� (hfa || (type[i]->t & VT_BTYPE) == VT_LDOUBLE) {
            ns = (ns + 7) & ~7;
            ns = (ns + align - 1) & -align;
        }

        // C.5
        �� ((type[i]->t & VT_BTYPE) == VT_FLOAT)
            size = 8;

        // C.6
        �� (hfa || is_float(type[i]->t)) {
            a[i] = ns;
            ns += size;
            ����;
        }

        // C.7
        �� ((type[i]->t & VT_BTYPE) != VT_STRUCT && size <= 8 && nx < 8) {
            a[i] = nx++ << 1;
            ����;
        }

        // C.8
        �� (align == 16)
            nx = (nx + 1) & ~1;

        // C.9
        �� ((type[i]->t & VT_BTYPE) != VT_STRUCT && size == 16 && nx < 7) {
            a[i] = nx << 1;
            nx += 2;
            ����;
        }

        // C.10
        �� ((type[i]->t & VT_BTYPE) == VT_STRUCT && size <= (8 - nx) * 8) {
            a[i] = nx << 1;
            nx += (size + 7) >> 3;
            ����;
        }

        // C.11
        nx = 8;

        // C.12
        ns = (ns + 7) & ~7;
        ns = (ns + align - 1) & -align;

        // C.13
        �� ((type[i]->t & VT_BTYPE) == VT_STRUCT) {
            a[i] = ns;
            ns += size;
            ����;
        }

        // C.14
        �� (size < 8)
            size = 8;

        // C.15
        a[i] = ns;
        ns += size;
    }

    ���� ns - 32;
}

��̬ �޷� �� arm64_pcs(�� n, CType **type, �޷� �� *a)
{
    �޷� �� stack;

    // Return type:
    �� ((type[0]->t & VT_BTYPE) == VT_VOID)
        a[0] = -1;
    �� {
        arm64_pcs_aux(1, type, a);
        assert(a[0] == 0 || a[0] == 1 || a[0] == 16);
    }

    // Argument types:
    stack = arm64_pcs_aux(n, type + 1, a + 1);

    �� (0) {
        �� i;
        ���� (i = 0; i <= n; i++) {
            �� (!i)
                printf("arm64_pcs return: ");
            ��
                printf("arm64_pcs arg %d: ", i);
            �� (a[i] == (�޷� ��)-1)
                printf("void\n");
            �� �� (a[i] == 1 && !i)
                printf("X8 pointer\n");
            �� �� (a[i] < 16)
                printf("X%lu%s\n", a[i] / 2, a[i] & 1 ? " pointer" : "");
            �� �� (a[i] < 32)
                printf("V%lu\n", a[i] / 2 - 8);
            ��
                printf("stack %lu%s\n",
                       (a[i] - 32) & ~1, a[i] & 1 ? " pointer" : "");
        }
    }

    ���� stack;
}

ST_FUNC �� gfunc_call(�� nb_args)
{
    CType *return_type;
    CType **t;
    �޷� �� *a, *a1;
    �޷� �� stack;
    �� i;

    return_type = &vtop[-nb_args].type.ref->type;
    �� ((return_type->t & VT_BTYPE) == VT_STRUCT)
        --nb_args;

    t = tcc_malloc((nb_args + 1) * �󳤶�(*t));
    a = tcc_malloc((nb_args + 1) * �󳤶�(*a));
    a1 = tcc_malloc((nb_args + 1) * �󳤶�(*a1));

    t[0] = return_type;
    ���� (i = 0; i < nb_args; i++)
        t[nb_args - i] = &vtop[-i].type;

    stack = arm64_pcs(nb_args, t, a);

    // Allocate space for structs replaced by pointer:
    ���� (i = nb_args; i; i--)
        �� (a[i] & 1) {
            SValue *arg = &vtop[i - nb_args];
            �� align, size = type_size(&arg->type, &align);
            assert((arg->type.t & VT_BTYPE) == VT_STRUCT);
            stack = (stack + align - 1) & -align;
            a1[i] = stack;
            stack += size;
        }

    stack = (stack + 15) >> 4 << 4;

    assert(stack < 0x1000);
    �� (stack)
        o(0xd10003ff | stack << 10); // sub sp,sp,#(n)

    // First pass: set all values on stack
    ���� (i = nb_args; i; i--) {
        vpushv(vtop - nb_args + i);

        �� (a[i] & 1) {
            // struct replaced by pointer
            �� r = get_reg(RC_INT);
            arm64_spoff(intr(r), a1[i]);
            vset(&vtop->type, r | VT_LVAL, 0);
            vswap();
            vstore();
            �� (a[i] >= 32) {
                // pointer on stack
                r = get_reg(RC_INT);
                arm64_spoff(intr(r), a1[i]);
                arm64_strx(3, intr(r), 31, (a[i] - 32) >> 1 << 1);
            }
        }
        �� �� (a[i] >= 32) {
            // value on stack
            �� ((vtop->type.t & VT_BTYPE) == VT_STRUCT) {
                �� r = get_reg(RC_INT);
                arm64_spoff(intr(r), a[i] - 32);
                vset(&vtop->type, r | VT_LVAL, 0);
                vswap();
                vstore();
            }
            �� �� (is_float(vtop->type.t)) {
                gv(RC_FLOAT);
                arm64_strv(arm64_type_size(vtop[0].type.t),
                           fltr(vtop[0].r), 31, a[i] - 32);
            }
            �� {
                gv(RC_INT);
                arm64_strx(arm64_type_size(vtop[0].type.t),
                           intr(vtop[0].r), 31, a[i] - 32);
            }
        }

        --vtop;
    }

    // Second pass: assign values to registers
    ���� (i = nb_args; i; i--, vtop--) {
        �� (a[i] < 16 && !(a[i] & 1)) {
            // value in general-purpose registers
            �� ((vtop->type.t & VT_BTYPE) == VT_STRUCT) {
                �� align, size = type_size(&vtop->type, &align);
                vtop->type.t = VT_PTR;
                gaddrof();
                gv(RC_R(a[i] / 2));
                arm64_ldrs(a[i] / 2, size);
            }
            ��
                gv(RC_R(a[i] / 2));
        }
        �� �� (a[i] < 16)
            // struct replaced by pointer in register
            arm64_spoff(a[i] / 2, a1[i]);
        �� �� (a[i] < 32) {
            // value in floating-point registers
            �� ((vtop->type.t & VT_BTYPE) == VT_STRUCT) {
                uint32_t j, sz, n = arm64_hfa(&vtop->type, &sz);
                vtop->type.t = VT_PTR;
                gaddrof();
                gv(RC_R30);
                ���� (j = 0; j < n; j++)
                    o(0x3d4003c0 |
                      (sz & 16) << 19 | -(sz & 8) << 27 | (sz & 4) << 29 |
                      (a[i] / 2 - 8 + j) |
                      j << 10); // ldr ([sdq])(*),[x30,#(j * sz)]
            }
            ��
                gv(RC_F(a[i] / 2 - 8));
        }
    }

    �� ((return_type->t & VT_BTYPE) == VT_STRUCT) {
        �� (a[0] == 1) {
            // indirect return: set x8 and discard the stack value
            gv(RC_R(8));
            --vtop;
        }
        ��
            // return in registers: keep the address for after the call
            vswap();
    }

    save_regs(0);
    arm64_gen_bl_or_b(0);
    --vtop;
    �� (stack)
        o(0x910003ff | stack << 10); // add sp,sp,#(n)

    {
        �� rt = return_type->t;
        �� bt = rt & VT_BTYPE;
        �� (bt == VT_BYTE || bt == VT_SHORT)
            // Promote small integers:
            o(0x13001c00 | (bt == VT_SHORT) << 13 |
              (uint32_t)!!(rt & VT_UNSIGNED) << 30); // [su]xt[bh] w0,w0
        �� �� (bt == VT_STRUCT && !(a[0] & 1)) {
            // A struct was returned in registers, so write it out:
            gv(RC_R(8));
            --vtop;
            �� (a[0] == 0) {
                �� align, size = type_size(return_type, &align);
                assert(size <= 16);
                �� (size > 8)
                    o(0xa9000500); // stp x0,x1,[x8]
                �� �� (size)
                    arm64_strx(size > 4 ? 3 : size > 2 ? 2 : size > 1, 0, 8, 0);

            }
            �� �� (a[0] == 16) {
                uint32_t j, sz, n = arm64_hfa(return_type, &sz);
                ���� (j = 0; j < n; j++)
                    o(0x3d000100 |
                      (sz & 16) << 19 | -(sz & 8) << 27 | (sz & 4) << 29 |
                      (a[i] / 2 - 8 + j) |
                      j << 10); // str ([sdq])(*),[x8,#(j * sz)]
            }
        }
    }

    tcc_free(a1);
    tcc_free(a);
    tcc_free(t);
}

��̬ �޷� �� arm64_func_va_list_stack;
��̬ �� arm64_func_va_list_gr_offs;
��̬ �� arm64_func_va_list_vr_offs;
��̬ �� arm64_func_sub_sp_offset;

ST_FUNC �� gfunc_prolog(CType *func_type)
{
    �� n = 0;
    �� i = 0;
    Sym *sym;
    CType **t;
    �޷� �� *a;

    // Why doesn't the caller (gen_function) set func_vt?
    func_vt = func_type->ref->type;
    func_vc = 144; // offset of where x8 is stored

    ���� (sym = func_type->ref; sym; sym = sym->next)
        ++n;
    t = tcc_malloc(n * �󳤶�(*t));
    a = tcc_malloc(n * �󳤶�(*a));

    ���� (sym = func_type->ref; sym; sym = sym->next)
        t[i++] = &sym->type;

    arm64_func_va_list_stack = arm64_pcs(n - 1, t, a);

    o(0xa9b27bfd); // stp x29,x30,[sp,#-224]!
    o(0xad0087e0); // stp q0,q1,[sp,#16]
    o(0xad018fe2); // stp q2,q3,[sp,#48]
    o(0xad0297e4); // stp q4,q5,[sp,#80]
    o(0xad039fe6); // stp q6,q7,[sp,#112]
    o(0xa90923e8); // stp x8,x8,[sp,#144]
    o(0xa90a07e0); // stp x0,x1,[sp,#160]
    o(0xa90b0fe2); // stp x2,x3,[sp,#176]
    o(0xa90c17e4); // stp x4,x5,[sp,#192]
    o(0xa90d1fe6); // stp x6,x7,[sp,#208]

    arm64_func_va_list_gr_offs = -64;
    arm64_func_va_list_vr_offs = -128;

    ���� (i = 1, sym = func_type->ref->next; sym; i++, sym = sym->next) {
        �� off = (a[i] < 16 ? 160 + a[i] / 2 * 8 :
                   a[i] < 32 ? 16 + (a[i] - 16) / 2 * 16 :
                   224 + ((a[i] - 32) >> 1 << 1));
        sym_push(sym->v & ~SYM_FIELD, &sym->type,
                 (a[i] & 1 ? VT_LLOCAL : VT_LOCAL) | lvalue_type(sym->type.t),
                 off);

        �� (a[i] < 16) {
            �� align, size = type_size(&sym->type, &align);
            arm64_func_va_list_gr_offs = (a[i] / 2 - 7 +
                                          (!(a[i] & 1) && size > 8)) * 8;
        }
        �� �� (a[i] < 32) {
            uint32_t hfa = arm64_hfa(&sym->type, 0);
            arm64_func_va_list_vr_offs = (a[i] / 2 - 16 +
                                          (hfa ? hfa : 1)) * 16;
        }

        // HFAs of float and double need to be written differently:
        �� (16 <= a[i] && a[i] < 32 && (sym->type.t & VT_BTYPE) == VT_STRUCT) {
            uint32_t j, sz, k = arm64_hfa(&sym->type, &sz);
            �� (sz < 16)
                ���� (j = 0; j < k; j++) {
                    o(0x3d0003e0 | -(sz & 8) << 27 | (sz & 4) << 29 |
                      ((a[i] - 16) / 2 + j) | (off / sz + j) << 10);
                    // str ([sdq])(*),[sp,#(j * sz)]
                }
        }
    }

    tcc_free(a);
    tcc_free(t);

    o(0x910003fd); // mov x29,sp
    arm64_func_sub_sp_offset = ind;
    // In gfunc_epilog these will be replaced with code to decrement SP:
    o(0xd503201f); // nop
    o(0xd503201f); // nop
    loc = 0;
}

ST_FUNC �� gen_va_start(��)
{
    �� r;
    --vtop; // we don't need the "arg"
    gaddrof();
    r = intr(gv(RC_INT));

    �� (arm64_func_va_list_stack) {
        //xx could use add (immediate) here
        arm64_movimm(30, arm64_func_va_list_stack + 224);
        o(0x8b1e03be); // add x30,x29,x30
    }
    ��
        o(0x910383be); // add x30,x29,#224
    o(0xf900001e | r << 5); // str x30,[x(r)]

    �� (arm64_func_va_list_gr_offs) {
        �� (arm64_func_va_list_stack)
            o(0x910383be); // add x30,x29,#224
        o(0xf900041e | r << 5); // str x30,[x(r),#8]
    }

    �� (arm64_func_va_list_vr_offs) {
        o(0x910243be); // add x30,x29,#144
        o(0xf900081e | r << 5); // str x30,[x(r),#16]
    }

    arm64_movimm(30, arm64_func_va_list_gr_offs);
    o(0xb900181e | r << 5); // str w30,[x(r),#24]

    arm64_movimm(30, arm64_func_va_list_vr_offs);
    o(0xb9001c1e | r << 5); // str w30,[x(r),#28]

    --vtop;
}

ST_FUNC �� gen_va_arg(CType *t)
{
    �� align, size = type_size(t, &align);
    �� fsize, hfa = arm64_hfa(t, &fsize);
    uint32_t r0, r1;

    �� (is_float(t->t)) {
        hfa = 1;
        fsize = size;
    }

    gaddrof();
    r0 = intr(gv(RC_INT));
    r1 = get_reg(RC_INT);
    vtop[0].r = r1 | lvalue_type(t->t);
    r1 = intr(r1);

    �� (!hfa) {
        uint32_t n = size > 16 ? 8 : (size + 7) & -8;
        o(0xb940181e | r0 << 5); // ldr w30,[x(r0),#24] // __gr_offs
        �� (align == 16) {
            assert(0); // this path untested but needed for __uint128_t
            o(0x11003fde); // add w30,w30,#15
            o(0x121c6fde); // and w30,w30,#-16
        }
        o(0x310003c0 | r1 | n << 10); // adds w(r1),w30,#(n)
        o(0x540000ad); // b.le .+20
        o(0xf9400000 | r1 | r0 << 5); // ldr x(r1),[x(r0)] // __stack
        o(0x9100001e | r1 << 5 | n << 10); // add x30,x(r1),#(n)
        o(0xf900001e | r0 << 5); // str x30,[x(r0)] // __stack
        o(0x14000004); // b .+16
        o(0xb9001800 | r1 | r0 << 5); // str w(r1),[x(r0),#24] // __gr_offs
        o(0xf9400400 | r1 | r0 << 5); // ldr x(r1),[x(r0),#8] // __gr_top
        o(0x8b3ec000 | r1 | r1 << 5); // add x(r1),x(r1),w30,sxtw
        �� (size > 16)
            o(0xf9400000 | r1 | r1 << 5); // ldr x(r1),[x(r1)]
    }
    �� {
        uint32_t rsz = hfa << 4;
        uint32_t ssz = (size + 7) & -(uint32_t)8;
        uint32_t b1, b2;
        o(0xb9401c1e | r0 << 5); // ldr w30,[x(r0),#28] // __vr_offs
        o(0x310003c0 | r1 | rsz << 10); // adds w(r1),w30,#(rsz)
        b1 = ind; o(0x5400000d); // b.le lab1
        o(0xf9400000 | r1 | r0 << 5); // ldr x(r1),[x(r0)] // __stack
        �� (fsize == 16) {
            o(0x91003c00 | r1 | r1 << 5); // add x(r1),x(r1),#15
            o(0x927cec00 | r1 | r1 << 5); // and x(r1),x(r1),#-16
        }
        o(0x9100001e | r1 << 5 | ssz << 10); // add x30,x(r1),#(ssz)
        o(0xf900001e | r0 << 5); // str x30,[x(r0)] // __stack
        b2 = ind; o(0x14000000); // b lab2
        // lab1:
        write32le(cur_text_section->data + b1, 0x5400000d | (ind - b1) << 3);
        o(0xb9001c00 | r1 | r0 << 5); // str w(r1),[x(r0),#28] // __vr_offs
        o(0xf9400800 | r1 | r0 << 5); // ldr x(r1),[x(r0),#16] // __vr_top
        �� (hfa == 1 || fsize == 16)
            o(0x8b3ec000 | r1 | r1 << 5); // add x(r1),x(r1),w30,sxtw
        �� {
            // We need to change the layout of this HFA.
            // Get some space on the stack using global variable "loc":
            loc = (loc - size) & -(uint32_t)align;
            o(0x8b3ec000 | 30 | r1 << 5); // add x30,x(r1),w30,sxtw
            arm64_movimm(r1, loc);
            o(0x8b0003a0 | r1 | r1 << 16); // add x(r1),x29,x(r1)
            o(0x4c402bdc | (uint32_t)fsize << 7 |
              (uint32_t)(hfa == 2) << 15 |
              (uint32_t)(hfa == 3) << 14); // ld1 {v28.(4s|2d),...},[x30]
            o(0x0d00801c | r1 << 5 | (fsize == 8) << 10 |
              (uint32_t)(hfa != 2) << 13 |
              (uint32_t)(hfa != 3) << 21); // st(hfa) {v28.(s|d),...}[0],[x(r1)]
        }
        // lab2:
        write32le(cur_text_section->data + b2, 0x14000000 | (ind - b2) >> 2);
    }
}

ST_FUNC �� gfunc_sret(CType *vt, �� variadic, CType *ret,
                       �� *align, �� *regsize)
{
    ���� 0;
}

ST_FUNC �� gfunc_return(CType *func_type)
{
    CType *t = func_type;
    �޷� �� a;

    arm64_pcs(0, &t, &a);
    ת�� (a) {
    ���� -1:
        ����;
    ���� 0:
        �� ((func_type->t & VT_BTYPE) == VT_STRUCT) {
            �� align, size = type_size(func_type, &align);
            gaddrof();
            gv(RC_R(0));
            arm64_ldrs(0, size);
        }
        ��
            gv(RC_IRET);
        ����;
    ���� 1: {
        CType type = *func_type;
        mk_pointer(&type);
        vset(&type, VT_LOCAL | VT_LVAL, func_vc);
        indir();
        vswap();
        vstore();
        ����;
    }
    ���� 16:
        �� ((func_type->t & VT_BTYPE) == VT_STRUCT) {
          uint32_t j, sz, n = arm64_hfa(&vtop->type, &sz);
          gaddrof();
          gv(RC_R(0));
          ���� (j = 0; j < n; j++)
              o(0x3d400000 |
                (sz & 16) << 19 | -(sz & 8) << 27 | (sz & 4) << 29 |
                j | j << 10); // ldr ([sdq])(*),[x0,#(j * sz)]
        }
        ��
            gv(RC_FRET);
        ����;
    ȱʡ:
      assert(0);
    }
    vtop--;
}

ST_FUNC �� gfunc_epilog(��)
{
    �� (loc) {
        // Insert instructions to subtract size of stack frame from SP.
        �޷� �� *ptr = cur_text_section->data + arm64_func_sub_sp_offset;
        uint64_t diff = (-loc + 15) & ~15;
        �� (!(diff >> 24)) {
            �� (diff & 0xfff) // sub sp,sp,#(diff & 0xfff)
                write32le(ptr, 0xd10003ff | (diff & 0xfff) << 10);
            �� (diff >> 12) // sub sp,sp,#(diff >> 12),lsl #12
                write32le(ptr + 4, 0xd14003ff | (diff >> 12) << 10);
        }
        �� {
            // In this case we may subtract more than necessary,
            // but always less than 17/16 of what we were aiming for.
            �� i = 0;
            �� j = 0;
            �� (diff >> 20) {
                diff = (diff + 0xffff) >> 16;
                ++i;
            }
            �� (diff >> 16) {
                diff = (diff + 1) >> 1;
                ++j;
            }
            write32le(ptr, 0xd2800010 | diff << 5 | i << 21);
            // mov x16,#(diff),lsl #(16 * i)
            write32le(ptr + 4, 0xcb3063ff | j << 10);
            // sub sp,sp,x16,lsl #(j)
        }
    }
    o(0x910003bf); // mov sp,x29
    o(0xa8ce7bfd); // ldp x29,x30,[sp],#224

    o(0xd65f03c0); // ret
}

// Generate forward branch to label:
ST_FUNC �� gjmp(�� t)
{
    �� r = ind;
    �� (nocode_wanted)
        ���� t;
    o(t);
    ���� r;
}

// Generate branch to known address:
ST_FUNC �� gjmp_addr(�� a)
{
    assert(a - ind + 0x8000000 < 0x10000000);
    o(0x14000000 | ((a - ind) >> 2 & 0x3ffffff));
}

ST_FUNC �� gtst(�� inv, �� t)
{
    �� bt = vtop->type.t & VT_BTYPE;
    �� (bt == VT_LDOUBLE) {
        uint32_t a, b, f = fltr(gv(RC_FLOAT));
        a = get_reg(RC_INT);
        vpushi(0);
        vtop[0].r = a;
        b = get_reg(RC_INT);
        a = intr(a);
        b = intr(b);
        o(0x4e083c00 | a | f << 5); // mov x(a),v(f).d[0]
        o(0x4e183c00 | b | f << 5); // mov x(b),v(f).d[1]
        o(0xaa000400 | a | a << 5 | b << 16); // orr x(a),x(a),x(b),lsl #1
        o(0xb4000040 | a | !!inv << 24); // cbz/cbnz x(a),.+8
        --vtop;
    }
    �� �� (bt == VT_FLOAT || bt == VT_DOUBLE) {
        uint32_t a = fltr(gv(RC_FLOAT));
        o(0x1e202008 | a << 5 | (bt != VT_FLOAT) << 22); // fcmp
        o(0x54000040 | !!inv); // b.eq/b.ne .+8
    }
    �� {
        uint32_t ll = (bt == VT_PTR || bt == VT_LLONG);
        uint32_t a = intr(gv(RC_INT));
        o(0x34000040 | a | !!inv << 24 | ll << 31); // cbz/cbnz wA,.+8
    }
    --vtop;
    ���� gjmp(t);
}

��̬ �� arm64_iconst(uint64_t *val, SValue *sv)
{
    �� ((sv->r & (VT_VALMASK | VT_LVAL | VT_SYM)) != VT_CONST)
        ���� 0;
    �� (val) {
        �� t = sv->type.t;
        �� bt = t & VT_BTYPE;
        *val = ((bt == VT_LLONG || bt == VT_PTR) ? sv->c.i :
                (uint32_t)sv->c.i |
                (t & VT_UNSIGNED ? 0 : -(sv->c.i & 0x80000000)));
    }
    ���� 1;
}

��̬ �� arm64_gen_opic(�� op, uint32_t l, �� rev, uint64_t val,
                          uint32_t x, uint32_t a)
{
    �� (op == '-' && !rev) {
        val = -val;
        op = '+';
    }
    val = l ? val : (uint32_t)val;

    ת�� (op) {

    ���� '+': {
        uint32_t s = l ? val >> 63 : val >> 31;
        val = s ? -val : val;
        val = l ? val : (uint32_t)val;
        �� (!(val & ~(uint64_t)0xfff))
            o(0x11000000 | l << 31 | s << 30 | x | a << 5 | val << 10);
        �� �� (!(val & ~(uint64_t)0xfff000))
            o(0x11400000 | l << 31 | s << 30 | x | a << 5 | val >> 12 << 10);
        �� {
            arm64_movimm(30, val); // use x30
            o(0x0b1e0000 | l << 31 | s << 30 | x | a << 5);
        }
        ���� 1;
      }

    ���� '-':
        �� (!val)
            o(0x4b0003e0 | l << 31 | x | a << 16); // neg
        �� �� (val == (l ? (uint64_t)-1 : (uint32_t)-1))
            o(0x2a2003e0 | l << 31 | x | a << 16); // mvn
        �� {
            arm64_movimm(30, val); // use x30
            o(0x4b0003c0 | l << 31 | x | a << 16); // sub
        }
        ���� 1;

    ���� '^':
        �� (val == -1 || (val == 0xffffffff && !l)) {
            o(0x2a2003e0 | l << 31 | x | a << 16); // mvn
            ���� 1;
        }
        // fall through
    ���� '&':
    ���� '|': {
        �� e = arm64_encode_bimm64(l ? val : val | val << 32);
        �� (e < 0)
            ���� 0;
        o((op == '&' ? 0x12000000 :
           op == '|' ? 0x32000000 : 0x52000000) |
          l << 31 | x | a << 5 | (uint32_t)e << 10);
        ���� 1;
    }

    ���� TOK_SAR:
    ���� TOK_SHL:
    ���� TOK_SHR: {
        uint32_t n = 32 << l;
        val = val & (n - 1);
        �� (rev)
            ���� 0;
        �� (!val)
            assert(0);
        �� �� (op == TOK_SHL)
            o(0x53000000 | l << 31 | l << 22 | x | a << 5 |
              (n - val) << 16 | (n - 1 - val) << 10); // lsl
        ��
            o(0x13000000 | (op == TOK_SHR) << 30 | l << 31 | l << 22 |
              x | a << 5 | val << 16 | (n - 1) << 10); // lsr/asr
        ���� 1;
    }

    }
    ���� 0;
}

��̬ �� arm64_gen_opil(�� op, uint32_t l)
{
    uint32_t x, a, b;

    // Special treatment for operations with a constant operand:
    {
        uint64_t val;
        �� rev = 1;

        �� (arm64_iconst(0, &vtop[0])) {
            vswap();
            rev = 0;
        }
        �� (arm64_iconst(&val, &vtop[-1])) {
            gv(RC_INT);
            a = intr(vtop[0].r);
            --vtop;
            x = get_reg(RC_INT);
            ++vtop;
            �� (arm64_gen_opic(op, l, rev, val, intr(x), a)) {
                vtop[0].r = x;
                vswap();
                --vtop;
                ����;
            }
        }
        �� (!rev)
            vswap();
    }

    gv2(RC_INT, RC_INT);
    assert(vtop[-1].r < VT_CONST && vtop[0].r < VT_CONST);
    a = intr(vtop[-1].r);
    b = intr(vtop[0].r);
    vtop -= 2;
    x = get_reg(RC_INT);
    ++vtop;
    vtop[0].r = x;
    x = intr(x);

    ת�� (op) {
    ���� '%':
        // Use x30 for quotient:
        o(0x1ac00c00 | l << 31 | 30 | a << 5 | b << 16); // sdiv
        o(0x1b008000 | l << 31 | x | (uint32_t)30 << 5 |
          b << 16 | a << 10); // msub
        ����;
    ���� '&':
        o(0x0a000000 | l << 31 | x | a << 5 | b << 16); // and
        ����;
    ���� '*':
        o(0x1b007c00 | l << 31 | x | a << 5 | b << 16); // mul
        ����;
    ���� '+':
        o(0x0b000000 | l << 31 | x | a << 5 | b << 16); // add
        ����;
    ���� '-':
        o(0x4b000000 | l << 31 | x | a << 5 | b << 16); // sub
        ����;
    ���� '/':
        o(0x1ac00c00 | l << 31 | x | a << 5 | b << 16); // sdiv
        ����;
    ���� '^':
        o(0x4a000000 | l << 31 | x | a << 5 | b << 16); // eor
        ����;
    ���� '|':
        o(0x2a000000 | l << 31 | x | a << 5 | b << 16); // orr
        ����;
    ���� TOK_EQ:
        o(0x6b00001f | l << 31 | a << 5 | b << 16); // cmp
        o(0x1a9f17e0 | x); // cset wA,eq
        ����;
    ���� TOK_GE:
        o(0x6b00001f | l << 31 | a << 5 | b << 16); // cmp
        o(0x1a9fb7e0 | x); // cset wA,ge
        ����;
    ���� TOK_GT:
        o(0x6b00001f | l << 31 | a << 5 | b << 16); // cmp
        o(0x1a9fd7e0 | x); // cset wA,gt
        ����;
    ���� TOK_LE:
        o(0x6b00001f | l << 31 | a << 5 | b << 16); // cmp
        o(0x1a9fc7e0 | x); // cset wA,le
        ����;
    ���� TOK_LT:
        o(0x6b00001f | l << 31 | a << 5 | b << 16); // cmp
        o(0x1a9fa7e0 | x); // cset wA,lt
        ����;
    ���� TOK_NE:
        o(0x6b00001f | l << 31 | a << 5 | b << 16); // cmp
        o(0x1a9f07e0 | x); // cset wA,ne
        ����;
    ���� TOK_SAR:
        o(0x1ac02800 | l << 31 | x | a << 5 | b << 16); // asr
        ����;
    ���� TOK_SHL:
        o(0x1ac02000 | l << 31 | x | a << 5 | b << 16); // lsl
        ����;
    ���� TOK_SHR:
        o(0x1ac02400 | l << 31 | x | a << 5 | b << 16); // lsr
        ����;
    ���� TOK_UDIV:
    ���� TOK_PDIV:
        o(0x1ac00800 | l << 31 | x | a << 5 | b << 16); // udiv
        ����;
    ���� TOK_UGE:
        o(0x6b00001f | l << 31 | a << 5 | b << 16); // cmp
        o(0x1a9f37e0 | x); // cset wA,cs
        ����;
    ���� TOK_UGT:
        o(0x6b00001f | l << 31 | a << 5 | b << 16); // cmp
        o(0x1a9f97e0 | x); // cset wA,hi
        ����;
    ���� TOK_ULT:
        o(0x6b00001f | l << 31 | a << 5 | b << 16); // cmp
        o(0x1a9f27e0 | x); // cset wA,cc
        ����;
    ���� TOK_ULE:
        o(0x6b00001f | l << 31 | a << 5 | b << 16); // cmp
        o(0x1a9f87e0 | x); // cset wA,ls
        ����;
    ���� TOK_UMOD:
        // Use x30 for quotient:
        o(0x1ac00800 | l << 31 | 30 | a << 5 | b << 16); // udiv
        o(0x1b008000 | l << 31 | x | (uint32_t)30 << 5 |
          b << 16 | a << 10); // msub
        ����;
    ȱʡ:
        assert(0);
    }
}

ST_FUNC �� gen_opi(�� op)
{
    arm64_gen_opil(op, 0);
}

ST_FUNC �� gen_opl(�� op)
{
    arm64_gen_opil(op, 1);
}

ST_FUNC �� gen_opf(�� op)
{
    uint32_t x, a, b, dbl;

    �� (vtop[0].type.t == VT_LDOUBLE) {
        CType type = vtop[0].type;
        �� func = 0;
        �� cond = -1;
        ת�� (op) {
        ���� '*': func = TOK___multf3; ����;
        ���� '+': func = TOK___addtf3; ����;
        ���� '-': func = TOK___subtf3; ����;
        ���� '/': func = TOK___divtf3; ����;
        ���� TOK_EQ: func = TOK___eqtf2; cond = 1; ����;
        ���� TOK_NE: func = TOK___netf2; cond = 0; ����;
        ���� TOK_LT: func = TOK___lttf2; cond = 10; ����;
        ���� TOK_GE: func = TOK___getf2; cond = 11; ����;
        ���� TOK_LE: func = TOK___letf2; cond = 12; ����;
        ���� TOK_GT: func = TOK___gttf2; cond = 13; ����;
        ȱʡ: assert(0); ����;
        }
        vpush_global_sym(&func_old_type, func);
        vrott(3);
        gfunc_call(2);
        vpushi(0);
        vtop->r = cond < 0 ? REG_FRET : REG_IRET;
        �� (cond < 0)
            vtop->type = type;
        �� {
            o(0x7100001f); // cmp w0,#0
            o(0x1a9f07e0 | (uint32_t)cond << 12); // cset w0,(cond)
        }
        ����;
    }

    dbl = vtop[0].type.t != VT_FLOAT;
    gv2(RC_FLOAT, RC_FLOAT);
    assert(vtop[-1].r < VT_CONST && vtop[0].r < VT_CONST);
    a = fltr(vtop[-1].r);
    b = fltr(vtop[0].r);
    vtop -= 2;
    ת�� (op) {
    ���� TOK_EQ: ���� TOK_NE:
    ���� TOK_LT: ���� TOK_GE: ���� TOK_LE: ���� TOK_GT:
        x = get_reg(RC_INT);
        ++vtop;
        vtop[0].r = x;
        x = intr(x);
        ����;
    ȱʡ:
        x = get_reg(RC_FLOAT);
        ++vtop;
        vtop[0].r = x;
        x = fltr(x);
        ����;
    }

    ת�� (op) {
    ���� '*':
        o(0x1e200800 | dbl << 22 | x | a << 5 | b << 16); // fmul
        ����;
    ���� '+':
        o(0x1e202800 | dbl << 22 | x | a << 5 | b << 16); // fadd
        ����;
    ���� '-':
        o(0x1e203800 | dbl << 22 | x | a << 5 | b << 16); // fsub
        ����;
    ���� '/':
        o(0x1e201800 | dbl << 22 | x | a << 5 | b << 16); // fdiv
        ����;
    ���� TOK_EQ:
        o(0x1e202000 | dbl << 22 | a << 5 | b << 16); // fcmp
        o(0x1a9f17e0 | x); // cset w(x),eq
        ����;
    ���� TOK_GE:
        o(0x1e202000 | dbl << 22 | a << 5 | b << 16); // fcmp
        o(0x1a9fb7e0 | x); // cset w(x),ge
        ����;
    ���� TOK_GT:
        o(0x1e202000 | dbl << 22 | a << 5 | b << 16); // fcmp
        o(0x1a9fd7e0 | x); // cset w(x),gt
        ����;
    ���� TOK_LE:
        o(0x1e202000 | dbl << 22 | a << 5 | b << 16); // fcmp
        o(0x1a9f87e0 | x); // cset w(x),ls
        ����;
    ���� TOK_LT:
        o(0x1e202000 | dbl << 22 | a << 5 | b << 16); // fcmp
        o(0x1a9f57e0 | x); // cset w(x),mi
        ����;
    ���� TOK_NE:
        o(0x1e202000 | dbl << 22 | a << 5 | b << 16); // fcmp
        o(0x1a9f07e0 | x); // cset w(x),ne
        ����;
    ȱʡ:
        assert(0);
    }
}

// Generate sign extension from 32 to 64 bits:
ST_FUNC �� gen_cvt_sxtw(��)
{
    uint32_t r = intr(gv(RC_INT));
    o(0x93407c00 | r | r << 5); // sxtw x(r),w(r)
}

ST_FUNC �� gen_cvt_itof(�� t)
{
    �� (t == VT_LDOUBLE) {
        �� f = vtop->type.t;
        �� func = (f & VT_BTYPE) == VT_LLONG ?
          (f & VT_UNSIGNED ? TOK___floatunditf : TOK___floatditf) :
          (f & VT_UNSIGNED ? TOK___floatunsitf : TOK___floatsitf);
        vpush_global_sym(&func_old_type, func);
        vrott(2);
        gfunc_call(1);
        vpushi(0);
        vtop->type.t = t;
        vtop->r = REG_FRET;
        ����;
    }
    �� {
        �� d, n = intr(gv(RC_INT));
        �� s = !(vtop->type.t & VT_UNSIGNED);
        uint32_t l = ((vtop->type.t & VT_BTYPE) == VT_LLONG);
        --vtop;
        d = get_reg(RC_FLOAT);
        ++vtop;
        vtop[0].r = d;
        o(0x1e220000 | (uint32_t)!s << 16 |
          (uint32_t)(t != VT_FLOAT) << 22 | fltr(d) |
          l << 31 | n << 5); // [us]cvtf [sd](d),[wx](n)
    }
}

ST_FUNC �� gen_cvt_ftoi(�� t)
{
    �� ((vtop->type.t & VT_BTYPE) == VT_LDOUBLE) {
        �� func = (t & VT_BTYPE) == VT_LLONG ?
          (t & VT_UNSIGNED ? TOK___fixunstfdi : TOK___fixtfdi) :
          (t & VT_UNSIGNED ? TOK___fixunstfsi : TOK___fixtfsi);
        vpush_global_sym(&func_old_type, func);
        vrott(2);
        gfunc_call(1);
        vpushi(0);
        vtop->type.t = t;
        vtop->r = REG_IRET;
        ����;
    }
    �� {
        �� d, n = fltr(gv(RC_FLOAT));
        uint32_t l = ((vtop->type.t & VT_BTYPE) != VT_FLOAT);
        --vtop;
        d = get_reg(RC_INT);
        ++vtop;
        vtop[0].r = d;
        o(0x1e380000 |
          (uint32_t)!!(t & VT_UNSIGNED) << 16 |
          (uint32_t)((t & VT_BTYPE) == VT_LLONG) << 31 | intr(d) |
          l << 22 | n << 5); // fcvtz[su] [wx](d),[sd](n)
    }
}

ST_FUNC �� gen_cvt_ftof(�� t)
{
    �� f = vtop[0].type.t;
    assert(t == VT_FLOAT || t == VT_DOUBLE || t == VT_LDOUBLE);
    assert(f == VT_FLOAT || f == VT_DOUBLE || f == VT_LDOUBLE);
    �� (t == f)
        ����;

    �� (t == VT_LDOUBLE || f == VT_LDOUBLE) {
        �� func = (t == VT_LDOUBLE) ?
            (f == VT_FLOAT ? TOK___extendsftf2 : TOK___extenddftf2) :
            (t == VT_FLOAT ? TOK___trunctfsf2 : TOK___trunctfdf2);
        vpush_global_sym(&func_old_type, func);
        vrott(2);
        gfunc_call(1);
        vpushi(0);
        vtop->type.t = t;
        vtop->r = REG_FRET;
    }
    �� {
        �� x, a;
        gv(RC_FLOAT);
        assert(vtop[0].r < VT_CONST);
        a = fltr(vtop[0].r);
        --vtop;
        x = get_reg(RC_FLOAT);
        ++vtop;
        vtop[0].r = x;
        x = fltr(x);

        �� (f == VT_FLOAT)
            o(0x1e22c000 | x | a << 5); // fcvt d(x),s(a)
        ��
            o(0x1e624000 | x | a << 5); // fcvt s(x),d(a)
    }
}

ST_FUNC �� ggoto(��)
{
    arm64_gen_bl_or_b(1);
    --vtop;
}

ST_FUNC �� gen_clear_cache(��)
{
    uint32_t beg, end, dsz, isz, p, lab1, b1;
    gv2(RC_INT, RC_INT);
    vpushi(0);
    vtop->r = get_reg(RC_INT);
    vpushi(0);
    vtop->r = get_reg(RC_INT);
    vpushi(0);
    vtop->r = get_reg(RC_INT);
    beg = intr(vtop[-4].r); // x0
    end = intr(vtop[-3].r); // x1
    dsz = intr(vtop[-2].r); // x2
    isz = intr(vtop[-1].r); // x3
    p = intr(vtop[0].r);    // x4
    vtop -= 5;

    o(0xd53b0020 | isz); // mrs x(isz),ctr_el0
    o(0x52800080 | p); // mov w(p),#4
    o(0x53104c00 | dsz | isz << 5); // ubfx w(dsz),w(isz),#16,#4
    o(0x1ac02000 | dsz | p << 5 | dsz << 16); // lsl w(dsz),w(p),w(dsz)
    o(0x12000c00 | isz | isz << 5); // and w(isz),w(isz),#15
    o(0x1ac02000 | isz | p << 5 | isz << 16); // lsl w(isz),w(p),w(isz)
    o(0x51000400 | p | dsz << 5); // sub w(p),w(dsz),#1
    o(0x8a240004 | p | beg << 5 | p << 16); // bic x(p),x(beg),x(p)
    b1 = ind; o(0x14000000); // b
    lab1 = ind;
    o(0xd50b7b20 | p); // dc cvau,x(p)
    o(0x8b000000 | p | p << 5 | dsz << 16); // add x(p),x(p),x(dsz)
    write32le(cur_text_section->data + b1, 0x14000000 | (ind - b1) >> 2);
    o(0xeb00001f | p << 5 | end << 16); // cmp x(p),x(end)
    o(0x54ffffa3 | ((lab1 - ind) << 3 & 0xffffe0)); // b.cc lab1
    o(0xd5033b9f); // dsb ish
    o(0x51000400 | p | isz << 5); // sub w(p),w(isz),#1
    o(0x8a240004 | p | beg << 5 | p << 16); // bic x(p),x(beg),x(p)
    b1 = ind; o(0x14000000); // b
    lab1 = ind;
    o(0xd50b7520 | p); // ic ivau,x(p)
    o(0x8b000000 | p | p << 5 | isz << 16); // add x(p),x(p),x(isz)
    write32le(cur_text_section->data + b1, 0x14000000 | (ind - b1) >> 2);
    o(0xeb00001f | p << 5 | end << 16); // cmp x(p),x(end)
    o(0x54ffffa3 | ((lab1 - ind) << 3 & 0xffffe0)); // b.cc lab1
    o(0xd5033b9f); // dsb ish
    o(0xd5033fdf); // isb
}

ST_FUNC �� gen_vla_sp_save(�� addr) {
    uint32_t r = intr(get_reg(RC_INT));
    o(0x910003e0 | r); // mov x(r),sp
    arm64_strx(3, r, 29, addr);
}

ST_FUNC �� gen_vla_sp_restore(�� addr) {
    // Use x30 because this function can be called when there
    // is a live return value in x0 but there is nothing on
    // the value stack to prevent get_reg from returning x0.
    uint32_t r = 30;
    arm64_ldrx(0, 3, r, 29, addr);
    o(0x9100001f | r << 5); // mov sp,x(r)
}

ST_FUNC �� gen_vla_alloc(CType *type, �� align) {
    uint32_t r = intr(gv(RC_INT));
    o(0x91003c00 | r | r << 5); // add x(r),x(r),#15
    o(0x927cec00 | r | r << 5); // bic x(r),x(r),#15
    o(0xcb2063ff | r << 16); // sub sp,sp,x(r)
    vpop();
}

/* end of A64 code generator */
/*************************************************************/
#����
/*************************************************************/
