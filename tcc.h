/*
 *  TCC - Tiny C Compiler
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

#��δ���� _TCC_H
#���� _TCC_H

#���� _GNU_SOURCE
#���� "config.h"

#���� <stdlib.h>
#���� <stdio.h>
#���� <stdarg.h>
#���� <string.h>
#���� <errno.h>
#���� <math.h>
#���� <fcntl.h>
#���� <setjmp.h>
#���� <time.h>

#��δ���� _WIN32
# ���� <unistd.h>
# ���� <sys/time.h>
# ��δ���� CONFIG_TCC_STATIC
#  ���� <dlfcn.h>
# ����
/* XXX: need to define this to use them in non ISOC99 context */
�ⲿ ���� strtof (���� �� *__nptr, �� **__endptr);
�ⲿ �� ˫�� strtold (���� �� *__nptr, �� **__endptr);
#����

#�綨�� _WIN32
# ���� <windows.h>
# ���� <io.h> /* open, close etc. */
# ���� <direct.h> /* getcwd */
# �綨�� __GNUC__
#  ���� <stdint.h>
# ����
# ���� inline __inline
# ���� snprintf _snprintf
# ���� vsnprintf _vsnprintf
# ��δ���� __GNUC__
#  ���� strtold (�� ˫��)strtod
#  ���� strtof (����)strtod
#  ���� strtoll _strtoi64
#  ���� strtoull _strtoui64
# ����
# �綨�� LIBTCC_AS_DLL
#  ���� LIBTCCAPI __declspec(dllexport)
#  ���� PUB_FUNC LIBTCCAPI
# ����
# ���� inp next_inp /* inp is an intrinsic on msvc/mingw */
# �綨�� _MSC_VER
#  pragma warning (disable : 4244)  // conversion from 'uint64_t' to 'int', possible loss of data
#  pragma warning (disable : 4267)  // conversion from 'size_t' to 'int', possible loss of data
#  pragma warning (disable : 4996)  // The POSIX name for this item is deprecated. Instead, use the ISO C and C++ conformant name
#  pragma warning (disable : 4018)  // signed/unsigned mismatch
#  pragma warning (disable : 4146)  // unary minus operator applied to unsigned type, result still unsigned
#  ���� ssize_t intptr_t
# ����
# ������ CONFIG_TCC_STATIC
#����

#��δ���� O_BINARY
# ���� O_BINARY 0
#����

#��δ���� offsetof
#���� offsetof(type, field) ((size_t) &((type *)0)->field)
#����

#��δ���� countof
#���� countof(tab) (�󳤶�(tab) / �󳤶�((tab)[0]))
#����

#�綨�� _MSC_VER
# ���� NORETURN __declspec(noreturn)
# ���� ALIGNED(x) __declspec(align(x))
#��
# ���� NORETURN __attribute__((noreturn))
# ���� ALIGNED(x) __attribute__((aligned(x)))
#����

#�綨�� _WIN32
# ���� IS_DIRSEP(c) (c == '/' || c == '\\')
# ���� IS_ABSPATH(p) (IS_DIRSEP(p[0]) || (p[0] && p[1] == ':' && IS_DIRSEP(p[2])))
# ���� PATHCMP stricmp
#��
# ���� IS_DIRSEP(c) (c == '/')
# ���� IS_ABSPATH(p) IS_DIRSEP(p[0])
# ���� PATHCMP strcmp
#����

/* -------------------------------------------- */

/* parser debug */
/* #���� PARSE_DEBUG */
/* preprocessor debug */
/* #���� PP_DEBUG */
/* include file debug */
/* #���� INC_DEBUG */
/* memory leak debug */
/* #���� MEM_DEBUG */
/* assembler debug */
/* #���� ASM_DEBUG */

/* target selection */
/* #���� TCC_TARGET_I386   *//* i386 code generator */
/* #���� TCC_TARGET_X86_64 *//* x86-64 code generator */
/* #���� TCC_TARGET_ARM    *//* ARMv4 code generator */
/* #���� TCC_TARGET_ARM64  *//* ARMv8 code generator */
/* #���� TCC_TARGET_C67    *//* TMS320C67xx code generator */

/* default target is I386 */
#�� !�Ѷ���(TCC_TARGET_I386) && !�Ѷ���(TCC_TARGET_ARM) && \
    !�Ѷ���(TCC_TARGET_ARM64) && !�Ѷ���(TCC_TARGET_C67) && \
    !�Ѷ���(TCC_TARGET_X86_64)
# �� �Ѷ��� __x86_64__ || �Ѷ��� _AMD64_
#  ���� TCC_TARGET_X86_64
# ���� �Ѷ��� __arm__
#  ���� TCC_TARGET_ARM
#  ���� TCC_ARM_EABI
#  ���� TCC_ARM_HARDFLOAT
# ���� �Ѷ��� __aarch64__
#  ���� TCC_TARGET_ARM64
# ��
#  ���� TCC_TARGET_I386
# ����
# �綨�� _WIN32
#  ���� TCC_TARGET_PE 1
# ����
#����

/* only native compiler supports -run */
#�� �Ѷ��� _WIN32 == �Ѷ��� TCC_TARGET_PE
# �� (�Ѷ��� __i386__ || �Ѷ��� _X86_) && �Ѷ��� TCC_TARGET_I386
#  ���� TCC_IS_NATIVE
# ���� (�Ѷ��� __x86_64__ || �Ѷ��� _AMD64_) && �Ѷ��� TCC_TARGET_X86_64
#  ���� TCC_IS_NATIVE
# ���� �Ѷ��� __arm__ && �Ѷ��� TCC_TARGET_ARM
#  ���� TCC_IS_NATIVE
# ���� �Ѷ��� __aarch64__ && �Ѷ��� TCC_TARGET_ARM64
#  ���� TCC_IS_NATIVE
# ����
#����

#�� �Ѷ��� TCC_IS_NATIVE && !�Ѷ��� CONFIG_TCCBOOT
# ���� CONFIG_TCC_BACKTRACE
# �� (�Ѷ��� TCC_TARGET_I386 || �Ѷ��� TCC_TARGET_X86_64) \
  && !�Ѷ��� TCC_UCLIBC && !�Ѷ��� TCC_MUSL
# ���� CONFIG_TCC_BCHECK /* enable bound checking code */
# ����
#����

/* ------------ path configuration ------------ */

#��δ���� CONFIG_SYSROOT
# ���� CONFIG_SYSROOT ""
#����
#��δ���� CONFIG_TCCDIR
# ���� CONFIG_TCCDIR "/usr/local/lib/tcc"
#����
#��δ���� CONFIG_LDDIR
# ���� CONFIG_LDDIR "lib"
#����
#�綨�� CONFIG_TRIPLET
# ���� USE_TRIPLET(s) s "/" CONFIG_TRIPLET
# ���� ALSO_TRIPLET(s) USE_TRIPLET(s) ":" s
#��
# ���� USE_TRIPLET(s) s
# ���� ALSO_TRIPLET(s) s
#����

/* path to find crt1.o, crti.o and crtn.o */
#��δ���� CONFIG_TCC_CRTPREFIX
# ���� CONFIG_TCC_CRTPREFIX USE_TRIPLET(CONFIG_SYSROOT "/usr/" CONFIG_LDDIR)
#����

/* Below: {B} is substituted by CONFIG_TCCDIR (rsp. -B option) */

/* system include paths */
#��δ���� CONFIG_TCC_SYSINCLUDEPATHS
# �綨�� TCC_TARGET_PE
#  ���� CONFIG_TCC_SYSINCLUDEPATHS "{B}/include;{B}/include/winapi"
# ��
#  ���� CONFIG_TCC_SYSINCLUDEPATHS \
        "{B}/include" \
    ":" ALSO_TRIPLET(CONFIG_SYSROOT "/usr/local/include") \
    ":" ALSO_TRIPLET(CONFIG_SYSROOT "/usr/include")
# ����
#����

/* library search paths */
#��δ���� CONFIG_TCC_LIBPATHS
# �綨�� TCC_TARGET_PE
#  ���� CONFIG_TCC_LIBPATHS "{B}/lib"
# ��
#  ���� CONFIG_TCC_LIBPATHS \
        ALSO_TRIPLET(CONFIG_SYSROOT "/usr/" CONFIG_LDDIR) \
    ":" ALSO_TRIPLET(CONFIG_SYSROOT "/" CONFIG_LDDIR) \
    ":" ALSO_TRIPLET(CONFIG_SYSROOT "/usr/local/" CONFIG_LDDIR)
# ����
#����

/* name of ELF interpreter */
#��δ���� CONFIG_TCC_ELFINTERP
# �� �Ѷ��� __FreeBSD__
#  ���� CONFIG_TCC_ELFINTERP "/libexec/ld-elf.so.1"
# ���� �Ѷ��� __FreeBSD_kernel__
#  �� �Ѷ���(TCC_TARGET_X86_64)
#   ���� CONFIG_TCC_ELFINTERP "/lib/ld-kfreebsd-x86-64.so.1"
#  ��
#   ���� CONFIG_TCC_ELFINTERP "/lib/ld.so.1"
#  ����
# ���� �Ѷ��� __DragonFly__
#  ���� CONFIG_TCC_ELFINTERP "/usr/libexec/ld-elf.so.2"
# ���� �Ѷ��� __NetBSD__
#  ���� CONFIG_TCC_ELFINTERP "/usr/libexec/ld.elf_so"
# ���� �Ѷ��� __GNU__
#  ���� CONFIG_TCC_ELFINTERP "/lib/ld.so"
# ���� �Ѷ���(TCC_TARGET_PE)
#  ���� CONFIG_TCC_ELFINTERP "-"
# ���� �Ѷ���(TCC_UCLIBC)
#  ���� CONFIG_TCC_ELFINTERP "/lib/ld-uClibc.so.0" /* is there a uClibc for x86_64 ? */
# ���� �Ѷ��� TCC_TARGET_ARM64
#  �� �Ѷ���(TCC_MUSL)
#   ���� CONFIG_TCC_ELFINTERP "/lib/ld-musl-aarch64.so.1"
#  ��
#   ���� CONFIG_TCC_ELFINTERP "/lib/ld-linux-aarch64.so.1"
#  ����
# ���� �Ѷ���(TCC_TARGET_X86_64)
#  �� �Ѷ���(TCC_MUSL)
#   ���� CONFIG_TCC_ELFINTERP "/lib/ld-musl-x86_64.so.1"
#  ��
#   ���� CONFIG_TCC_ELFINTERP "/lib64/ld-linux-x86-64.so.2"
#  ����
# ���� !�Ѷ���(TCC_ARM_EABI)
#  �� �Ѷ���(TCC_MUSL)
#   ���� CONFIG_TCC_ELFINTERP "/lib/ld-musl-arm.so.1"
#  ��
#   ���� CONFIG_TCC_ELFINTERP "/lib/ld-linux.so.2"
#  ����
# ����
#����

/* var elf_interp dans *-gen.c */
#�綨�� CONFIG_TCC_ELFINTERP
# ���� DEFAULT_ELFINTERP(s) CONFIG_TCC_ELFINTERP
#��
# ���� DEFAULT_ELFINTERP(s) default_elfinterp(s)
#����

/* (target specific) libtcc1.a */
#��δ���� TCC_LIBTCC1
# ���� TCC_LIBTCC1 "libtcc1.a"
#����

/* library to use with CONFIG_USE_LIBGCC instead of libtcc1.a */
#�� �Ѷ��� CONFIG_USE_LIBGCC && !�Ѷ��� TCC_LIBGCC
#���� TCC_LIBGCC USE_TRIPLET(CONFIG_SYSROOT "/" CONFIG_LDDIR) "/libgcc_s.so.1"
#����

#�綨�� TCC_TARGET_PE
#���� PATHSEP ';'
#��
#���� PATHSEP ':'
#����

/* -------------------------------------------- */

#���� "libtcc.h"
#���� "elf.h"
#���� "stab.h"

/* -------------------------------------------- */

#��δ���� PUB_FUNC /* functions used by tcc.c but not in libtcc.h */
# ���� PUB_FUNC
#����

#��δ���� ONE_SOURCE
# ���� ONE_SOURCE 1
#����

#�� ONE_SOURCE
#���� ST_INLN ��̬ inline
#���� ST_FUNC ��̬
#���� ST_DATA ��̬
#��
#���� ST_INLN
#���� ST_FUNC
#���� ST_DATA �ⲿ
#����

#�綨�� TCC_PROFILE /* profile all functions */
# ���� ��̬
#����

/* -------------------------------------------- */
/* include the target specific definitions */

#���� TARGET_DEFS_ONLY
#�綨�� TCC_TARGET_I386
# ���� "i386-gen.c"
# ���� "i386-link.c"
#����
#�綨�� TCC_TARGET_X86_64
# ���� "x86_64-gen.c"
# ���� "x86_64-link.c"
#����
#�綨�� TCC_TARGET_ARM
# ���� "arm-gen.c"
# ���� "arm-link.c"
# ���� "arm-asm.c"
#����
#�綨�� TCC_TARGET_ARM64
# ���� "arm64-gen.c"
# ���� "arm64-link.c"
#����
#�綨�� TCC_TARGET_C67
# ���� TCC_TARGET_COFF
# ���� "coff.h"
# ���� "c67-gen.c"
# ���� "c67-link.c"
#����
#������ TARGET_DEFS_ONLY

/* -------------------------------------------- */

#�� PTR_SIZE == 8
# ���� ELFCLASSW ELFCLASS64
# ���� ElfW(type) Elf##64##_##type
# ���� ELFW(type) ELF##64##_##type
# ���� ElfW_Rel ElfW(Rela)
# ���� SHT_RELX SHT_RELA
# ���� REL_SECTION_FMT ".rela%s"
#��
# ���� ELFCLASSW ELFCLASS32
# ���� ElfW(type) Elf##32##_##type
# ���� ELFW(type) ELF##32##_##type
# ���� ElfW_Rel ElfW(Rel)
# ���� SHT_RELX SHT_REL
# ���� REL_SECTION_FMT ".rel%s"
#����
/* target address type */
#���� addr_t ElfW(Addr)

/* -------------------------------------------- */

#���� INCLUDE_STACK_SIZE  32
#���� IFDEF_STACK_SIZE    64
#���� VSTACK_SIZE         256
#���� STRING_MAX_SIZE     1024
#���� TOKSTR_MAX_SIZE     256
#���� PACK_STACK_SIZE     8

#���� TOK_HASH_SIZE       16384 /* must be a power of two */
#���� TOK_ALLOC_INCR      512  /* must be a power of two */
#���� TOK_MAX_SIZE        4 /* token max size in int unit when stored in string */

/* token symbol management */
���Ͷ��� �ṹ TokenSym {
    �ṹ TokenSym *hash_next;
    �ṹ Sym *sym_define; /* direct pointer to define */
    �ṹ Sym *sym_label; /* direct pointer to label */
    �ṹ Sym *sym_struct; /* direct pointer to structure */
    �ṹ Sym *sym_identifier; /* direct pointer to identifier */
    �� tok; /* token number */
    �� len;
    �� str[1];
} TokenSym;

#�綨�� TCC_TARGET_PE
���Ͷ��� �޷� �� nwchar_t;
#��
���Ͷ��� �� nwchar_t;
#����

���Ͷ��� �ṹ CString {
    �� size; /* size in bytes */
    �� *data; /* either 'char *' or 'nwchar_t *' */
    �� size_allocated;
} CString;

/* type definition */
���Ͷ��� �ṹ CType {
    �� t;
    �ṹ Sym *ref;
} CType;

/* constant value */
���Ͷ��� ���� CValue {
    �� ˫�� ld;
    ˫�� d;
    ���� f;
    uint64_t i;
    �ṹ {
        �� size;
        ���� �� *data;
    } str;
    �� tab[LDOUBLE_SIZE/4];
} CValue;

/* value on stack */
���Ͷ��� �ṹ SValue {
    CType type;      /* type */
    �޷� �� r;      /* register + flags */
    �޷� �� r2;     /* second register, used for 'long long'
                              type. If not used, set to VT_CONST */
    CValue c;              /* constant, if VT_CONST */
    �ṹ Sym *sym;       /* symbol, if (VT_SYM | VT_CONST), or if
    			      result of unary() for an identifier. */
} SValue;

/* symbol attributes */
�ṹ SymAttr {
    �޷� ��
    aligned     : 5, /* alignment as log2+1 (0 == unspecified) */
    packed      : 1,
    weak        : 1,
    visibility  : 2,
    dllexport   : 1,
    dllimport   : 1,
    unused      : 5;
};

/* function attributes or temporary attributes for parsing */
�ṹ FuncAttr {
    �޷�
    func_call   : 3, /* calling convention (0..5), see below */
    func_type   : 2, /* FUNC_OLD/NEW/ELLIPSIS */
    func_body   : 1, /* body was defined */
    func_args   : 8; /* PE __stdcall args */
};

/* GNUC attribute definition */
���Ͷ��� �ṹ AttributeDef {
    �ṹ SymAttr a;
    �ṹ FuncAttr f;
    �ṹ Section *section;
    �� alias_target; /* token */
    �� asm_label; /* associated asm label */
    �� attr_mode; /* __attribute__((__mode__(...))) */
} AttributeDef;

/* symbol management */
���Ͷ��� �ṹ Sym {
    �� v; /* symbol token */
    �޷� �� r; /* associated register or VT_CONST/VT_LOCAL and LVAL type */
    �ṹ SymAttr a; /* symbol attributes */
    ���� {
        �ṹ {
            �� c; /* associated number or Elf symbol index */
            ���� {
                �� sym_scope; /* scope level for locals */
                �� jnext; /* next jump label */
                �ṹ FuncAttr f; /* function attributes */
                �� auxtype; /* bitfield access type */
            };
        };
        �� �� enum_val; /* enum constant if IS_ENUM_VAL */
        �� *d; /* define token stream */
    };
    CType type; /* associated type */
    ���� {
        �ṹ Sym *next; /* next related symbol (for fields and anoms) */
        �� asm_label; /* associated asm label */
    };
    �ṹ Sym *prev; /* prev symbol in stack */
    �ṹ Sym *prev_tok; /* previous symbol for this token */
} Sym;

/* section definition */
/* XXX: use directly ELF structure for parameters ? */
/* special flag to indicate that the section should not be linked to
   the other ones */
#���� SHF_PRIVATE 0x80000000

/* special flag, too */
#���� SECTION_ABS ((�� *)1)

���Ͷ��� �ṹ Section {
    �޷� �� data_offset; /* current data offset */
    �޷� �� *data;       /* section data */
    �޷� �� data_allocated; /* used for realloc() handling */
    �� sh_name;             /* elf section name (only used during output) */
    �� sh_num;              /* elf section number */
    �� sh_type;             /* elf section type */
    �� sh_flags;            /* elf section flags */
    �� sh_info;             /* elf section info */
    �� sh_addralign;        /* elf section alignment */
    �� sh_entsize;          /* elf entry size */
    �޷� �� sh_size;   /* section size (only used during output) */
    addr_t sh_addr;          /* address at which the section is relocated */
    �޷� �� sh_offset; /* file offset */
    �� nb_hashed_syms;      /* used to resize the hash table */
    �ṹ Section *link;    /* link to another section */
    �ṹ Section *reloc;   /* corresponding section for relocation, if any */
    �ṹ Section *hash;    /* hash table for symbols */
    �ṹ Section *prev;    /* previous section on section stack */
    �� name[1];           /* section name */
} Section;

���Ͷ��� �ṹ DLLReference {
    �� level;
    �� *handle;
    �� name[1];
} DLLReference;

/* -------------------------------------------------- */

#���� SYM_STRUCT     0x40000000 /* struct/union/enum symbol space */
#���� SYM_FIELD      0x20000000 /* struct/union field symbol space */
#���� SYM_FIRST_ANOM 0x10000000 /* first anonymous sym */

/* stored in 'Sym->f.func_type' field */
#���� FUNC_NEW       1 /* ansi function prototype */
#���� FUNC_OLD       2 /* old function prototype */
#���� FUNC_ELLIPSIS  3 /* ansi function prototype with ... */

/* stored in 'Sym->f.func_call' field */
#���� FUNC_CDECL     0 /* standard c call */
#���� FUNC_STDCALL   1 /* pascal c call */
#���� FUNC_FASTCALL1 2 /* first param in %eax */
#���� FUNC_FASTCALL2 3 /* first parameters in %eax, %edx */
#���� FUNC_FASTCALL3 4 /* first parameter in %eax, %edx, %ecx */
#���� FUNC_FASTCALLW 5 /* first parameter in %ecx, %edx */

/* field 'Sym.t' for macros */
#���� MACRO_OBJ      0 /* object like macro */
#���� MACRO_FUNC     1 /* function like macro */

/* field 'Sym.r' for C labels */
#���� LABEL_DEFINED  0 /* label is defined */
#���� LABEL_FORWARD  1 /* label is forward defined */
#���� LABEL_DECLARED 2 /* label is declared but never used */

/* type_decl() types */
#���� TYPE_ABSTRACT  1 /* type without variable */
#���� TYPE_DIRECT    2 /* type with variable */

#���� IO_BUF_SIZE 8192

���Ͷ��� �ṹ BufferedFile {
    uint8_t *buf_ptr;
    uint8_t *buf_end;
    �� fd;
    �ṹ BufferedFile *prev;
    �� line_num;    /* current line number - here to simplify code */
    �� line_ref;    /* tcc -E: last printed line */
    �� ifndef_macro;  /* #ifndef macro / #endif search */
    �� ifndef_macro_saved; /* saved ifndef_macro */
    �� *ifdef_stack_ptr; /* ifdef_stack value at the start of the file */
    �� include_next_index; /* next search path */
    �� filename[1024];    /* filename */
    �� *true_filename; /* filename not modified by # line directive */
    �޷� �� unget[4];
    �޷� �� buffer[1]; /* extra size for CH_EOB char */
} BufferedFile;

#���� CH_EOB   '\\'       /* end of buffer or '\0' char in file */
#���� CH_EOF   (-1)   /* end of file */

/* used to record tokens */
���Ͷ��� �ṹ TokenString {
    �� *str;
    �� len;
    �� lastlen;
    �� allocated_len;
    �� last_line_num;
    �� save_line_num;
    /* used to chain token-strings with begin/end_macro() */
    �ṹ TokenString *prev;
    ���� �� *prev_ptr;
    �� alloc;
} TokenString;

/* inline functions */
���Ͷ��� �ṹ InlineFunc {
    TokenString *func_str;
    Sym *sym;
    �� filename[1];
} InlineFunc;

/* include file cache, used to find files faster and also to eliminate
   inclusion if the include file is protected by #ifndef ... #endif */
���Ͷ��� �ṹ CachedInclude {
    �� ifndef_macro;
    �� once;
    �� hash_next; /* -1 if none */
    �� filename[1]; /* path specified in #include */
} CachedInclude;

#���� CACHED_INCLUDES_HASH_SIZE 32

#�綨�� CONFIG_TCC_ASM
���Ͷ��� �ṹ ExprValue {
    uint64_t v;
    Sym *sym;
    �� pcrel;
} ExprValue;

#���� MAX_ASM_OPERANDS 30
���Ͷ��� �ṹ ASMOperand {
    �� id; /* GCC 3 optional identifier (0 if number only supported */
    �� *constraint;
    �� asm_str[16]; /* computed asm string for operand */
    SValue *vt; /* C value of the expression */
    �� ref_index; /* if >= 0, gives reference to a output constraint */
    �� input_index; /* if >= 0, gives reference to an input constraint */
    �� priority; /* priority, used to assign registers */
    �� reg; /* if >= 0, register number used for this operand */
    �� is_llong; /* true if double register value */
    �� is_memory; /* true if memory operand */
    �� is_rw;     /* for '+' modifier */
} ASMOperand;
#����

/* extra symbol attributes (not in symbol table) */
�ṹ sym_attr {
    �޷� got_offset;
    �޷� plt_offset;
    �� plt_sym;
    �� dyn_index;
#�綨�� TCC_TARGET_ARM
    �޷� �� plt_thumb_stub:1;
#����
};

�ṹ TCCState {

    �� verbose; /* if true, display some information during compilation */
    �� nostdinc; /* if true, no standard headers are added */
    �� nostdlib; /* if true, no standard libraries are added */
    �� nocommon; /* if true, do not use common symbols for .bss data */
    �� static_link; /* if true, static linking is performed */
    �� rdynamic; /* if true, all symbols are exported */
    �� symbolic; /* if true, resolve symbols in the current module first */
    �� alacarte_link; /* if true, only link in referenced objects from archive */

    �� *tcc_lib_path; /* CONFIG_TCCDIR or -B option */
    �� *soname; /* as specified on the command line (-soname) */
    �� *rpath; /* as specified on the command line (-Wl,-rpath=) */
    �� enable_new_dtags; /* ditto, (-Wl,--enable-new-dtags) */

    /* output type, see TCC_OUTPUT_XXX */
    �� output_type;
    /* output format, see TCC_OUTPUT_FORMAT_xxx */
    �� output_format;

    /* C language options */
    �� char_is_unsigned;
    �� leading_underscore;
    �� ms_extensions;	/* allow nested named struct w/o identifier behave like unnamed */
    �� dollars_in_identifiers;	/* allows '$' char in identifiers */
    �� ms_bitfields; /* if true, emulate MS algorithm for aligning bitfields */

    /* warning switches */
    �� warn_write_strings;
    �� warn_unsupported;
    �� warn_error;
    �� warn_none;
    �� warn_implicit_function_declaration;
    �� warn_gcc_compat;

    /* compile with debug symbol (and use them if error during execution) */
    �� do_debug;
#�綨�� CONFIG_TCC_BCHECK
    /* compile with built-in memory and bounds checker */
    �� do_bounds_check;
#����
#�綨�� TCC_TARGET_ARM
    ö�� float_abi float_abi; /* float ABI of the generated code*/
#����
    �� run_test; /* nth test to run with -dt -run */

    addr_t text_addr; /* address of text section */
    �� has_text_addr;

    �޷� section_align; /* section alignment */

    �� *init_symbol; /* symbols to call at load-time (not used currently) */
    �� *fini_symbol; /* symbols to call at unload-time (not used currently) */
    
#�綨�� TCC_TARGET_I386
    �� seg_size; /* 32. Can be 16 with i386 assembler (.code16) */
#����
#�綨�� TCC_TARGET_X86_64
    �� nosse; /* For -mno-sse support. */
#����

    /* array of all loaded dlls (including those referenced by loaded dlls) */
    DLLReference **loaded_dlls;
    �� nb_loaded_dlls;

    /* include paths */
    �� **include_paths;
    �� nb_include_paths;

    �� **sysinclude_paths;
    �� nb_sysinclude_paths;

    /* library paths */
    �� **library_paths;
    �� nb_library_paths;

    /* crt?.o object path */
    �� **crt_paths;
    �� nb_crt_paths;

    /* -include files */
    �� **cmd_include_files;
    �� nb_cmd_include_files;

    /* error handling */
    �� *error_opaque;
    �� (*error_func)(�� *opaque, ���� �� *msg);
    �� error_set_jmp_enabled;
    jmp_buf error_jmp_buf;
    �� nb_errors;

    /* output file for preprocessing (-E) */
    FILE *ppfp;
    ö�� {
	LINE_MACRO_OUTPUT_FORMAT_GCC,
	LINE_MACRO_OUTPUT_FORMAT_NONE,
	LINE_MACRO_OUTPUT_FORMAT_STD,
    LINE_MACRO_OUTPUT_FORMAT_P10 = 11
    } Pflag; /* -P switch */
    �� dflag; /* -dX value */

    /* for -MD/-MF: collected dependencies for this compilation */
    �� **target_deps;
    �� nb_target_deps;

    /* compilation */
    BufferedFile *include_stack[INCLUDE_STACK_SIZE];
    BufferedFile **include_stack_ptr;

    �� ifdef_stack[IFDEF_STACK_SIZE];
    �� *ifdef_stack_ptr;

    /* included files enclosed with #ifndef MACRO */
    �� cached_includes_hash[CACHED_INCLUDES_HASH_SIZE];
    CachedInclude **cached_includes;
    �� nb_cached_includes;

    /* #pragma pack stack */
    �� pack_stack[PACK_STACK_SIZE];
    �� *pack_stack_ptr;
    �� **pragma_libs;
    �� nb_pragma_libs;

    /* inline functions are stored as token lists and compiled last
       only if referenced */
    �ṹ InlineFunc **inline_fns;
    �� nb_inline_fns;

    /* sections */
    Section **sections;
    �� nb_sections; /* number of sections, including first dummy section */

    Section **priv_sections;
    �� nb_priv_sections; /* number of private sections */

    /* got & plt handling */
    Section *got;
    Section *plt;

    /* temporary dynamic symbol sections (for dll loading) */
    Section *dynsymtab_section;
    /* exported dynamic symbol section */
    Section *dynsym;
    /* copy of the global symtab_section variable */
    Section *symtab;
    /* extra attributes (eg. GOT/PLT value) for symtab symbols */
    �ṹ sym_attr *sym_attrs;
    �� nb_sym_attrs;
    /* tiny assembler state */
    Sym *asm_labels;

#�綨�� TCC_TARGET_PE
    /* PE info */
    �� pe_subsystem;
    �޷� pe_characteristics;
    �޷� pe_file_align;
    �޷� pe_stack_size;
# �綨�� TCC_TARGET_X86_64
    Section *uw_pdata;
    �� uw_sym;
    �޷� uw_offs;
# ����
#����

#�綨�� TCC_IS_NATIVE
    ���� �� *runtime_main;
    �� **runtime_mem;
    �� nb_runtime_mem;
#����

    /* used by main and tcc_parse_args only */
    �ṹ filespec **files; /* files seen on command line */
    �� nb_files; /* number thereof */
    �� nb_libraries; /* number of libs thereof */
    �� filetype;
    �� *outfile; /* output filename */
    �� option_r; /* option -r */
    �� do_bench; /* option -bench */
    �� gen_deps; /* option -MD  */
    �� *deps_outfile; /* option -MF */
    �� option_pthread; /* -pthread option */
    �� argc;
    �� **argv;
};

�ṹ filespec {
    �� type;
    �� alacarte;
    �� name[1];
};

/* The current value can be: */
#���� VT_VALMASK   0x003f  /* mask for value location, register or: */
#���� VT_CONST     0x0030  /* constant in vc (must be first non register value) */
#���� VT_LLOCAL    0x0031  /* lvalue, offset on stack */
#���� VT_LOCAL     0x0032  /* offset on stack */
#���� VT_CMP       0x0033  /* the value is stored in processor flags (in vc) */
#���� VT_JMP       0x0034  /* value is the consequence of jmp true (even) */
#���� VT_JMPI      0x0035  /* value is the consequence of jmp false (odd) */
#���� VT_LVAL      0x0100  /* var is an lvalue */
#���� VT_SYM       0x0200  /* a symbol value is added */
#���� VT_MUSTCAST  0x0400  /* value must be casted to be correct (used for
                                char/short stored in integer registers) */
#���� VT_MUSTBOUND 0x0800  /* bound checking must be done before
                                dereferencing value */
#���� VT_BOUNDED   0x8000  /* value is bounded. The address of the
                                bounding function call point is in vc */
#���� VT_LVAL_BYTE     0x1000  /* lvalue is a byte */
#���� VT_LVAL_SHORT    0x2000  /* lvalue is a short */
#���� VT_LVAL_UNSIGNED 0x4000  /* lvalue is unsigned */
#���� VT_LVAL_TYPE     (VT_LVAL_BYTE | VT_LVAL_SHORT | VT_LVAL_UNSIGNED)

/* types */
#���� VT_BTYPE       0x000f  /* mask for basic type */
#���� VT_VOID             0  /* void type */
#���� VT_BYTE             1  /* signed byte type */
#���� VT_SHORT            2  /* short type */
#���� VT_INT              3  /* integer type */
#���� VT_LLONG            4  /* 64 bit integer */
#���� VT_PTR              5  /* pointer */
#���� VT_FUNC             6  /* function type */
#���� VT_STRUCT           7  /* struct/union definition */
#���� VT_FLOAT            8  /* IEEE float */
#���� VT_DOUBLE           9  /* IEEE double */
#���� VT_LDOUBLE         10  /* IEEE long double */
#���� VT_BOOL            11  /* ISOC99 boolean type */
#���� VT_QLONG           13  /* 128-bit integer. Only used for x86-64 ABI */
#���� VT_QFLOAT          14  /* 128-bit float. Only used for x86-64 ABI */

#���� VT_UNSIGNED    0x0010  /* unsigned type */
#���� VT_DEFSIGN     0x0020  /* explicitly signed or unsigned */
#���� VT_ARRAY       0x0040  /* array type (also has VT_PTR) */
#���� VT_BITFIELD    0x0080  /* bitfield modifier */
#���� VT_CONSTANT    0x0100  /* const modifier */
#���� VT_VOLATILE    0x0200  /* volatile modifier */
#���� VT_VLA         0x0400  /* VLA type (also has VT_PTR and VT_ARRAY) */
#���� VT_LONG	       0x0800

/* storage */
#���� VT_EXTERN  0x00001000  /* extern definition */
#���� VT_STATIC  0x00002000  /* static variable */
#���� VT_TYPEDEF 0x00004000  /* typedef definition */
#���� VT_INLINE  0x00008000  /* inline definition */
/* currently unused: 0x0800, 0x000[1248]0000  */

#���� VT_STRUCT_SHIFT 20     /* shift for bitfield shift values (32 - 2*6) */
#���� VT_STRUCT_MASK (((1 << (6+6)) - 1) << VT_STRUCT_SHIFT | VT_BITFIELD)
#���� BIT_POS(t) (((t) >> VT_STRUCT_SHIFT) & 0x3f)
#���� BIT_SIZE(t) (((t) >> (VT_STRUCT_SHIFT + 6)) & 0x3f)

#���� VT_UNION    (1 << VT_STRUCT_SHIFT | VT_STRUCT)
#���� VT_ENUM     (2 << VT_STRUCT_SHIFT) /* integral type is an enum really */
#���� VT_ENUM_VAL (3 << VT_STRUCT_SHIFT) /* integral type is an enum constant really */

#���� IS_ENUM(t) ((t & VT_STRUCT_MASK) == VT_ENUM)
#���� IS_ENUM_VAL(t) ((t & VT_STRUCT_MASK) == VT_ENUM_VAL)
#���� IS_UNION(t) ((t & (VT_STRUCT_MASK|VT_BTYPE)) == VT_UNION)

/* type mask (except storage) */
#���� VT_STORAGE (VT_EXTERN | VT_STATIC | VT_TYPEDEF | VT_INLINE)
#���� VT_TYPE (~(VT_STORAGE|VT_STRUCT_MASK))


/* token values */

/* warning: the following compare tokens depend on i386 asm code */
#���� TOK_ULT 0x92
#���� TOK_UGE 0x93
#���� TOK_EQ  0x94
#���� TOK_NE  0x95
#���� TOK_ULE 0x96
#���� TOK_UGT 0x97
#���� TOK_Nset 0x98
#���� TOK_Nclear 0x99
#���� TOK_LT  0x9c
#���� TOK_GE  0x9d
#���� TOK_LE  0x9e
#���� TOK_GT  0x9f

#���� TOK_LAND  0xa0
#���� TOK_LOR   0xa1
#���� TOK_DEC   0xa2
#���� TOK_MID   0xa3 /* inc/dec, to void constant */
#���� TOK_INC   0xa4
#���� TOK_UDIV  0xb0 /* unsigned division */
#���� TOK_UMOD  0xb1 /* unsigned modulo */
#���� TOK_PDIV  0xb2 /* fast division with undefined rounding for pointers */

/* tokens that carry values (in additional token string space / tokc) --> */
#���� TOK_CCHAR   0xb3 /* char constant in tokc */
#���� TOK_LCHAR   0xb4
#���� TOK_CINT    0xb5 /* number in tokc */
#���� TOK_CUINT   0xb6 /* unsigned int constant */
#���� TOK_CLLONG  0xb7 /* long long constant */
#���� TOK_CULLONG 0xb8 /* unsigned long long constant */
#���� TOK_STR     0xb9 /* pointer to string in tokc */
#���� TOK_LSTR    0xba
#���� TOK_CFLOAT  0xbb /* float constant */
#���� TOK_CDOUBLE 0xbc /* double constant */
#���� TOK_CLDOUBLE 0xbd /* long double constant */
#���� TOK_PPNUM   0xbe /* preprocessor number */
#���� TOK_PPSTR   0xbf /* preprocessor string */
#���� TOK_LINENUM 0xc0 /* line number info */
/* <-- */

#���� TOK_UMULL    0xc2 /* unsigned 32x32 -> 64 mul */
#���� TOK_ADDC1    0xc3 /* add with carry generation */
#���� TOK_ADDC2    0xc4 /* add with carry use */
#���� TOK_SUBC1    0xc5 /* add with carry generation */
#���� TOK_SUBC2    0xc6 /* add with carry use */
#���� TOK_ARROW    0xc7
#���� TOK_DOTS     0xc8 /* three dots */
#���� TOK_SHR      0xc9 /* unsigned shift right */
#���� TOK_TWOSHARPS 0xca /* ## preprocessing token */
#���� TOK_PLCHLDR  0xcb /* placeholder token as �Ѷ��� in C99 */
#���� TOK_NOSUBST  0xcc /* means following token has already been pp'd */
#���� TOK_PPJOIN   0xcd /* A '##' in the right position to mean pasting */

#���� TOK_CLONG   0xce /* long constant */
#���� TOK_CULONG  0xcf /* unsigned long constant */


#�� �Ѷ��� TCC_TARGET_X86_64 && !�Ѷ��� TCC_TARGET_PE
    #���� TCC_LONG_ARE_64_BIT
#����


#���� TOK_SHL   0x01 /* shift left */
#���� TOK_SAR   0x02 /* signed shift right */
  
/* assignment operators : normal operator or 0x80 */
#���� TOK_A_MOD 0xa5
#���� TOK_A_AND 0xa6
#���� TOK_A_MUL 0xaa
#���� TOK_A_ADD 0xab
#���� TOK_A_SUB 0xad
#���� TOK_A_DIV 0xaf
#���� TOK_A_XOR 0xde
#���� TOK_A_OR  0xfc
#���� TOK_A_SHL 0x81
#���� TOK_A_SAR 0x82

#���� TOK_EOF       (-1)  /* end of file */
#���� TOK_LINEFEED  10    /* line feed */

/* all identifiers and strings have token above that */
#���� TOK_IDENT 256

#���� DEF_ASM(x) DEF(TOK_ASM_ ## x, #x)
#���� TOK_ASM_int TOK_INT
#���� DEF_ASMDIR(x) DEF(TOK_ASMDIR_ ## x, "." #x)
#���� TOK_ASMDIR_FIRST TOK_ASMDIR_byte
#���� TOK_ASMDIR_LAST TOK_ASMDIR_section

#�� �Ѷ��� TCC_TARGET_I386 || �Ѷ��� TCC_TARGET_X86_64
/* only used for i386 asm opcodes definitions */
#���� DEF_BWL(x) \
 DEF(TOK_ASM_ ## x ## b, #x "b") \
 DEF(TOK_ASM_ ## x ## w, #x "w") \
 DEF(TOK_ASM_ ## x ## l, #x "l") \
 DEF(TOK_ASM_ ## x, #x)
#���� DEF_WL(x) \
 DEF(TOK_ASM_ ## x ## w, #x "w") \
 DEF(TOK_ASM_ ## x ## l, #x "l") \
 DEF(TOK_ASM_ ## x, #x)
#�綨�� TCC_TARGET_X86_64
# ���� DEF_BWLQ(x) \
 DEF(TOK_ASM_ ## x ## b, #x "b") \
 DEF(TOK_ASM_ ## x ## w, #x "w") \
 DEF(TOK_ASM_ ## x ## l, #x "l") \
 DEF(TOK_ASM_ ## x ## q, #x "q") \
 DEF(TOK_ASM_ ## x, #x)
# ���� DEF_WLQ(x) \
 DEF(TOK_ASM_ ## x ## w, #x "w") \
 DEF(TOK_ASM_ ## x ## l, #x "l") \
 DEF(TOK_ASM_ ## x ## q, #x "q") \
 DEF(TOK_ASM_ ## x, #x)
# ���� DEF_BWLX DEF_BWLQ
# ���� DEF_WLX DEF_WLQ
/* number of sizes + 1 */
# ���� NBWLX 5
#��
# ���� DEF_BWLX DEF_BWL
# ���� DEF_WLX DEF_WL
/* number of sizes + 1 */
# ���� NBWLX 4
#����

#���� DEF_FP1(x) \
 DEF(TOK_ASM_ ## f ## x ## s, "f" #x "s") \
 DEF(TOK_ASM_ ## fi ## x ## l, "fi" #x "l") \
 DEF(TOK_ASM_ ## f ## x ## l, "f" #x "l") \
 DEF(TOK_ASM_ ## fi ## x ## s, "fi" #x "s")

#���� DEF_FP(x) \
 DEF(TOK_ASM_ ## f ## x, "f" #x ) \
 DEF(TOK_ASM_ ## f ## x ## p, "f" #x "p") \
 DEF_FP1(x)

#���� DEF_ASMTEST(x,suffix) \
 DEF_ASM(x ## o ## suffix) \
 DEF_ASM(x ## no ## suffix) \
 DEF_ASM(x ## b ## suffix) \
 DEF_ASM(x ## c ## suffix) \
 DEF_ASM(x ## nae ## suffix) \
 DEF_ASM(x ## nb ## suffix) \
 DEF_ASM(x ## nc ## suffix) \
 DEF_ASM(x ## ae ## suffix) \
 DEF_ASM(x ## e ## suffix) \
 DEF_ASM(x ## z ## suffix) \
 DEF_ASM(x ## ne ## suffix) \
 DEF_ASM(x ## nz ## suffix) \
 DEF_ASM(x ## be ## suffix) \
 DEF_ASM(x ## na ## suffix) \
 DEF_ASM(x ## nbe ## suffix) \
 DEF_ASM(x ## a ## suffix) \
 DEF_ASM(x ## s ## suffix) \
 DEF_ASM(x ## ns ## suffix) \
 DEF_ASM(x ## p ## suffix) \
 DEF_ASM(x ## pe ## suffix) \
 DEF_ASM(x ## np ## suffix) \
 DEF_ASM(x ## po ## suffix) \
 DEF_ASM(x ## l ## suffix) \
 DEF_ASM(x ## nge ## suffix) \
 DEF_ASM(x ## nl ## suffix) \
 DEF_ASM(x ## ge ## suffix) \
 DEF_ASM(x ## le ## suffix) \
 DEF_ASM(x ## ng ## suffix) \
 DEF_ASM(x ## nle ## suffix) \
 DEF_ASM(x ## g ## suffix)

#���� /* �Ѷ��� TCC_TARGET_I386 || �Ѷ��� TCC_TARGET_X86_64 */

ö�� tcc_token {
    TOK_LAST = TOK_IDENT - 1
#���� DEF(id, str) ,id
#���� "tcctok.h"
#������ DEF
};

/* keywords: tok >= TOK_IDENT && tok < TOK_UIDENT */
#���� TOK_UIDENT TOK_DEFINE

/* ------------ libtcc.c ------------ */

/* use GNU C extensions */
ST_DATA �� gnu_ext;
/* use Tiny C extensions */
ST_DATA �� tcc_ext;
/* XXX: get rid of this ASAP */
ST_DATA �ṹ TCCState *tcc_state;

/* public functions currently used by the tcc main function */
ST_FUNC �� *pstrcpy(�� *buf, �� buf_size, ���� �� *s);
ST_FUNC �� *pstrcat(�� *buf, �� buf_size, ���� �� *s);
ST_FUNC �� *pstrncpy(�� *out, ���� �� *in, size_t num);
PUB_FUNC �� *tcc_basename(���� �� *name);
PUB_FUNC �� *tcc_fileextension (���� �� *name);

#��δ���� MEM_DEBUG
PUB_FUNC �� tcc_free(�� *ptr);
PUB_FUNC �� *tcc_malloc(�޷� �� size);
PUB_FUNC �� *tcc_mallocz(�޷� �� size);
PUB_FUNC �� *tcc_realloc(�� *ptr, �޷� �� size);
PUB_FUNC �� *tcc_strdup(���� �� *str);
#��
#���� tcc_free(ptr)           tcc_free_debug(ptr)
#���� tcc_malloc(size)        tcc_malloc_debug(size, __�ļ�__, __�к�__)
#���� tcc_mallocz(size)       tcc_mallocz_debug(size, __�ļ�__, __�к�__)
#���� tcc_realloc(ptr,size)   tcc_realloc_debug(ptr, size, __�ļ�__, __�к�__)
#���� tcc_strdup(str)         tcc_strdup_debug(str, __�ļ�__, __�к�__)
PUB_FUNC �� tcc_free_debug(�� *ptr);
PUB_FUNC �� *tcc_malloc_debug(�޷� �� size, ���� �� *file, �� line);
PUB_FUNC �� *tcc_mallocz_debug(�޷� �� size, ���� �� *file, �� line);
PUB_FUNC �� *tcc_realloc_debug(�� *ptr, �޷� �� size, ���� �� *file, �� line);
PUB_FUNC �� *tcc_strdup_debug(���� �� *str, ���� �� *file, �� line);
#����

#���� free(p) use_tcc_free(p)
#���� malloc(s) use_tcc_malloc(s)
#���� realloc(p, s) use_tcc_realloc(p, s)
#������ strdup
#���� strdup(s) use_tcc_strdup(s)
PUB_FUNC �� tcc_memcheck(��);
PUB_FUNC �� tcc_error_noabort(���� �� *fmt, ...);
PUB_FUNC NORETURN �� tcc_error(���� �� *fmt, ...);
PUB_FUNC �� tcc_warning(���� �� *fmt, ...);

/* other utilities */
ST_FUNC �� dynarray_add(�� *ptab, �� *nb_ptr, �� *data);
ST_FUNC �� dynarray_reset(�� *pp, �� *n);
ST_INLN �� cstr_ccat(CString *cstr, �� ch);
ST_FUNC �� cstr_cat(CString *cstr, ���� �� *str, �� len);
ST_FUNC �� cstr_wccat(CString *cstr, �� ch);
ST_FUNC �� cstr_new(CString *cstr);
ST_FUNC �� cstr_free(CString *cstr);
ST_FUNC �� cstr_reset(CString *cstr);

ST_INLN �� sym_free(Sym *sym);
ST_FUNC Sym *sym_push2(Sym **ps, �� v, �� t, �� c);
ST_FUNC Sym *sym_find2(Sym *s, �� v);
ST_FUNC Sym *sym_push(�� v, CType *type, �� r, �� c);
ST_FUNC �� sym_pop(Sym **ptop, Sym *b, �� keep);
ST_INLN Sym *struct_find(�� v);
ST_INLN Sym *sym_find(�� v);
ST_FUNC Sym *global_identifier_push(�� v, �� t, �� c);

ST_FUNC �� tcc_open_bf(TCCState *s1, ���� �� *filename, �� initlen);
ST_FUNC �� tcc_open(TCCState *s1, ���� �� *filename);
ST_FUNC �� tcc_close(��);

ST_FUNC �� tcc_add_file_internal(TCCState *s1, ���� �� *filename, �� flags);
/* flags: */
#���� AFF_PRINT_ERROR     0x10 /* print error if file not found */
#���� AFF_REFERENCED_DLL  0x20 /* load a referenced dll from another dll */
#���� AFF_TYPE_BIN        0x40 /* file to add is binary */
/* s->filetype: */
#���� AFF_TYPE_NONE   0
#���� AFF_TYPE_C      1
#���� AFF_TYPE_ASM    2
#���� AFF_TYPE_ASMPP  3
#���� AFF_TYPE_LIB    4
/* values from tcc_object_type(...) */
#���� AFF_BINTYPE_REL 1
#���� AFF_BINTYPE_DYN 2
#���� AFF_BINTYPE_AR  3
#���� AFF_BINTYPE_C67 4


ST_FUNC �� tcc_add_crt(TCCState *s, ���� �� *filename);
ST_FUNC �� tcc_add_dll(TCCState *s, ���� �� *filename, �� flags);
ST_FUNC �� tcc_add_pragma_libs(TCCState *s1);
PUB_FUNC �� tcc_add_library_err(TCCState *s, ���� �� *f);
PUB_FUNC �� tcc_print_stats(TCCState *s, �޷� total_time);
PUB_FUNC �� tcc_parse_args(TCCState *s, �� *argc, �� ***argv, �� optind);
#�綨�� _WIN32
ST_FUNC �� *normalize_slashes(�� *path);
#����

/* tcc_parse_args return codes: */
#���� OPT_HELP 1
#���� OPT_HELP2 2
#���� OPT_V 3
#���� OPT_PRINT_DIRS 4
#���� OPT_AR 5
#���� OPT_IMPDEF 6
#���� OPT_M32 32
#���� OPT_M64 64

/* ------------ tccpp.c ------------ */

ST_DATA �ṹ BufferedFile *file;
ST_DATA �� ch, tok;
ST_DATA CValue tokc;
ST_DATA ���� �� *macro_ptr;
ST_DATA �� parse_flags;
ST_DATA �� tok_flags;
ST_DATA CString tokcstr; /* current parsed string, if any */

/* display benchmark infos */
ST_DATA �� total_lines;
ST_DATA �� total_bytes;
ST_DATA �� tok_ident;
ST_DATA TokenSym **table_ident;

#���� TOK_FLAG_BOL   0x0001 /* beginning of line before */
#���� TOK_FLAG_BOF   0x0002 /* beginning of file before */
#���� TOK_FLAG_ENDIF 0x0004 /* a endif was found matching starting #ifdef */
#���� TOK_FLAG_EOF   0x0008 /* end of file */

#���� PARSE_FLAG_PREPROCESS 0x0001 /* activate preprocessing */
#���� PARSE_FLAG_TOK_NUM    0x0002 /* return numbers instead of TOK_PPNUM */
#���� PARSE_FLAG_LINEFEED   0x0004 /* line feed is returned as a
                                        token. line feed is also
                                        returned at eof */
#���� PARSE_FLAG_ASM_FILE 0x0008 /* we processing an asm file: '#' can be used for line comment, etc. */
#���� PARSE_FLAG_SPACES     0x0010 /* next() returns space tokens (for -E) */
#���� PARSE_FLAG_ACCEPT_STRAYS 0x0020 /* next() returns '\\' token */
#���� PARSE_FLAG_TOK_STR    0x0040 /* return parsed strings instead of TOK_PPSTR */

/* isidnum_table flags: */
#���� IS_SPC 1
#���� IS_ID  2
#���� IS_NUM 4

ST_FUNC TokenSym *tok_alloc(���� �� *str, �� len);
ST_FUNC ���� �� *get_tok_str(�� v, CValue *cv);
ST_FUNC �� begin_macro(TokenString *str, �� alloc);
ST_FUNC �� end_macro(��);
ST_FUNC �� set_idnum(�� c, �� val);
ST_INLN �� tok_str_new(TokenString *s);
ST_FUNC TokenString *tok_str_alloc(��);
ST_FUNC �� tok_str_free(TokenString *s);
ST_FUNC �� tok_str_free_str(�� *str);
ST_FUNC �� tok_str_add(TokenString *s, �� t);
ST_FUNC �� tok_str_add_tok(TokenString *s);
ST_INLN �� define_push(�� v, �� macro_type, �� *str, Sym *first_arg);
ST_FUNC �� define_undef(Sym *s);
ST_INLN Sym *define_find(�� v);
ST_FUNC �� free_defines(Sym *b);
ST_FUNC Sym *label_find(�� v);
ST_FUNC Sym *label_push(Sym **ptop, �� v, �� flags);
ST_FUNC �� label_pop(Sym **ptop, Sym *slast, �� keep);
ST_FUNC �� parse_define(��);
ST_FUNC �� preprocess(�� is_bof);
ST_FUNC �� next_nomacro(��);
ST_FUNC �� next(��);
ST_INLN �� unget_tok(�� last_tok);
ST_FUNC �� preprocess_start(TCCState *s1, �� is_asm);
ST_FUNC �� preprocess_end(TCCState *s1);
ST_FUNC �� tccpp_new(TCCState *s);
ST_FUNC �� tccpp_delete(TCCState *s);
ST_FUNC �� tcc_preprocess(TCCState *s1);
ST_FUNC �� skip(�� c);
ST_FUNC NORETURN �� expect(���� �� *msg);

/* space excluding newline */
��̬ ���� �� is_space(�� ch) {
    ���� ch == ' ' || ch == '\t' || ch == '\v' || ch == '\f' || ch == '\r';
}
��̬ ���� �� isid(�� c) {
    ���� (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}
��̬ ���� �� isnum(�� c) {
    ���� c >= '0' && c <= '9';
}
��̬ ���� �� isoct(�� c) {
    ���� c >= '0' && c <= '7';
}
��̬ ���� �� toup(�� c) {
    ���� (c >= 'a' && c <= 'z') ? c - 'a' + 'A' : c;
}

/* ------------ tccgen.c ------------ */

#���� SYM_POOL_NB (8192 / �󳤶�(Sym))
ST_DATA Sym *sym_free_first;
ST_DATA �� **sym_pools;
ST_DATA �� nb_sym_pools;

ST_DATA Sym *global_stack;
ST_DATA Sym *local_stack;
ST_DATA Sym *local_label_stack;
ST_DATA Sym *global_label_stack;
ST_DATA Sym *define_stack;
ST_DATA CType char_pointer_type, func_old_type, int_type, size_type;
ST_DATA SValue __vstack[1+/*to make bcheck happy*/ VSTACK_SIZE], *vtop, *pvtop;
#���� vstack  (__vstack + 1)
ST_DATA �� rsym, anon_sym, ind, loc;

ST_DATA �� const_wanted; /* true if constant wanted */
ST_DATA �� nocode_wanted; /* true if no code generation wanted for an expression */
ST_DATA �� global_expr;  /* true if compound literals must be allocated globally (used during initializers parsing */
ST_DATA CType func_vt; /* current function return type (used by return instruction) */
ST_DATA �� func_var; /* true if current function is variadic */
ST_DATA �� func_vc;
ST_DATA �� last_line_num, last_ind, func_ind; /* debug last line number and pc */
ST_DATA ���� �� *funcname;
ST_DATA �� g_debug;

ST_FUNC �� tcc_debug_start(TCCState *s1);
ST_FUNC �� tcc_debug_end(TCCState *s1);
ST_FUNC �� tcc_debug_funcstart(TCCState *s1, Sym *sym);
ST_FUNC �� tcc_debug_funcend(TCCState *s1, �� size);
ST_FUNC �� tcc_debug_line(TCCState *s1);

ST_FUNC �� tccgen_compile(TCCState *s1);
ST_FUNC �� free_inline_functions(TCCState *s);
ST_FUNC �� check_vstack(��);

ST_INLN �� is_float(�� t);
ST_FUNC �� ieee_finite(˫�� d);
ST_FUNC �� test_lvalue(��);
ST_FUNC �� vpushi(�� v);
ST_FUNC Sym *external_global_sym(�� v, CType *type, �� r);
ST_FUNC �� vset(CType *type, �� r, �� v);
ST_FUNC �� vswap(��);
ST_FUNC �� vpush_global_sym(CType *type, �� v);
ST_FUNC �� vrote(SValue *e, �� n);
ST_FUNC �� vrott(�� n);
ST_FUNC �� vrotb(�� n);
#�綨�� TCC_TARGET_ARM
ST_FUNC �� get_reg_ex(�� rc, �� rc2);
ST_FUNC �� lexpand_nr(��);
#����
ST_FUNC �� vpushv(SValue *v);
ST_FUNC �� save_reg(�� r);
ST_FUNC �� save_reg_upstack(�� r, �� n);
ST_FUNC �� get_reg(�� rc);
ST_FUNC �� save_regs(�� n);
ST_FUNC �� gaddrof(��);
ST_FUNC �� gv(�� rc);
ST_FUNC �� gv2(�� rc1, �� rc2);
ST_FUNC �� vpop(��);
ST_FUNC �� gen_op(�� op);
ST_FUNC �� type_size(CType *type, �� *a);
ST_FUNC �� mk_pointer(CType *type);
ST_FUNC �� vstore(��);
ST_FUNC �� inc(�� post, �� c);
ST_FUNC �� parse_mult_str (CString *astr, ���� �� *msg);
ST_FUNC �� parse_asm_str(CString *astr);
ST_FUNC �� lvalue_type(�� t);
ST_FUNC �� indir(��);
ST_FUNC �� unary(��);
ST_FUNC �� expr_prod(��);
ST_FUNC �� expr_sum(��);
ST_FUNC �� gexpr(��);
ST_FUNC �� expr_const(��);
#�� �Ѷ��� CONFIG_TCC_BCHECK || �Ѷ��� TCC_TARGET_C67
ST_FUNC Sym *get_sym_ref(CType *type, Section *sec, �޷� �� offset, �޷� �� size);
#����
#�� �Ѷ��� TCC_TARGET_X86_64 && !�Ѷ��� TCC_TARGET_PE
ST_FUNC �� classify_x86_64_va_arg(CType *ty);
#����

/* ------------ tccelf.c ------------ */

#���� TCC_OUTPUT_FORMAT_ELF    0 /* default output format: ELF */
#���� TCC_OUTPUT_FORMAT_BINARY 1 /* binary image output */
#���� TCC_OUTPUT_FORMAT_COFF   2 /* COFF */

#���� ARMAG  "!<arch>\012"    /* For COFF and a.out archives */

���Ͷ��� �ṹ {
    �޷� �� n_strx;         /* index into string table of name */
    �޷� �� n_type;         /* type of symbol */
    �޷� �� n_other;        /* misc info (usually empty) */
    �޷� �� n_desc;        /* description field */
    �޷� �� n_value;        /* value of symbol */
} Stab_Sym;

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
ST_FUNC �� tccelf_bounds_new(TCCState *s);
#����
/* symbol sections */
ST_DATA Section *symtab_section, *strtab_section;
/* debug sections */
ST_DATA Section *stab_section, *stabstr_section;

ST_FUNC �� tccelf_new(TCCState *s);
ST_FUNC �� tccelf_delete(TCCState *s);
ST_FUNC �� tccelf_stab_new(TCCState *s);

ST_FUNC Section *new_section(TCCState *s1, ���� �� *name, �� sh_type, �� sh_flags);
ST_FUNC �� section_realloc(Section *sec, �޷� �� new_size);
ST_FUNC size_t section_add(Section *sec, addr_t size, �� align);
ST_FUNC �� *section_ptr_add(Section *sec, addr_t size);
ST_FUNC �� section_reserve(Section *sec, �޷� �� size);
ST_FUNC Section *find_section(TCCState *s1, ���� �� *name);
ST_FUNC Section *new_symtab(TCCState *s1, ���� �� *symtab_name, �� sh_type, �� sh_flags, ���� �� *strtab_name, ���� �� *hash_name, �� hash_sh_flags);

ST_FUNC �� put_extern_sym2(Sym *sym, Section *section, addr_t value, �޷� �� size, �� can_add_underscore);
ST_FUNC �� put_extern_sym(Sym *sym, Section *section, addr_t value, �޷� �� size);
#�� PTR_SIZE == 4
ST_FUNC �� greloc(Section *s, Sym *sym, �޷� �� offset, �� type);
#����
ST_FUNC �� greloca(Section *s, Sym *sym, �޷� �� offset, �� type, addr_t addend);

ST_FUNC �� put_elf_str(Section *s, ���� �� *sym);
ST_FUNC �� put_elf_sym(Section *s, addr_t value, �޷� �� size, �� info, �� other, �� shndx, ���� �� *name);
ST_FUNC �� set_elf_sym(Section *s, addr_t value, �޷� �� size, �� info, �� other, �� shndx, ���� �� *name);
ST_FUNC �� find_elf_sym(Section *s, ���� �� *name);
ST_FUNC �� put_elf_reloc(Section *symtab, Section *s, �޷� �� offset, �� type, �� symbol);
ST_FUNC �� put_elf_reloca(Section *symtab, Section *s, �޷� �� offset, �� type, �� symbol, addr_t addend);

ST_FUNC �� put_stabs(���� �� *str, �� type, �� other, �� desc, �޷� �� value);
ST_FUNC �� put_stabs_r(���� �� *str, �� type, �� other, �� desc, �޷� �� value, Section *sec, �� sym_index);
ST_FUNC �� put_stabn(�� type, �� other, �� desc, �� value);
ST_FUNC �� put_stabd(�� type, �� other, �� desc);

ST_FUNC �� relocate_common_syms(��);
ST_FUNC �� relocate_syms(TCCState *s1, Section *symtab, �� do_resolve);
ST_FUNC �� relocate_section(TCCState *s1, Section *s);

ST_FUNC �� tcc_add_linker_symbols(TCCState *s1);
ST_FUNC �� tcc_object_type(�� fd, ElfW(Ehdr) *h);
ST_FUNC �� tcc_load_object_file(TCCState *s1, �� fd, �޷� �� file_offset);
ST_FUNC �� tcc_load_archive(TCCState *s1, �� fd);
ST_FUNC �� tcc_add_bcheck(TCCState *s1);
ST_FUNC �� tcc_add_runtime(TCCState *s1);

ST_FUNC �� build_got_entries(TCCState *s1);
ST_FUNC �ṹ sym_attr *get_sym_attr(TCCState *s1, �� index, �� alloc);
ST_FUNC �� squeeze_multi_relocs(Section *sec, size_t oldrelocoffset);

ST_FUNC addr_t get_elf_sym_addr(TCCState *s, ���� �� *name, �� err);
#�� �Ѷ��� TCC_IS_NATIVE || �Ѷ��� TCC_TARGET_PE
ST_FUNC �� *tcc_get_symbol_err(TCCState *s, ���� �� *name);
#����

#��δ���� TCC_TARGET_PE
ST_FUNC �� tcc_load_dll(TCCState *s1, �� fd, ���� �� *filename, �� level);
ST_FUNC �� tcc_load_ldscript(TCCState *s1);
ST_FUNC uint8_t *parse_comment(uint8_t *p);
ST_FUNC �� minp(��);
ST_INLN �� inp(��);
ST_FUNC �� handle_eob(��);
#����

/* ------------ xxx-link.c ------------ */

/* Wether to generate a GOT/PLT entry and when. NO_GOTPLT_ENTRY is first so
   that unknown relocation don't create a GOT or PLT entry */
ö�� gotplt_entry {
    NO_GOTPLT_ENTRY,	/* never generate (eg. GLOB_DAT & JMP_SLOT relocs) */
    BUILD_GOT_ONLY,	/* only build GOT (eg. TPOFF relocs) */
    AUTO_GOTPLT_ENTRY,	/* generate if sym is UNDEF */
    ALWAYS_GOTPLT_ENTRY	/* always generate (eg. PLTOFF relocs) */
};

ST_FUNC �� code_reloc (�� reloc_type);
ST_FUNC �� gotplt_entry_type (�� reloc_type);
ST_FUNC �޷� create_plt_entry(TCCState *s1, �޷� got_offset, �ṹ sym_attr *attr);
ST_FUNC �� relocate_init(Section *sr);
ST_FUNC �� relocate(TCCState *s1, ElfW_Rel *rel, �� type, �޷� �� *ptr, addr_t addr, addr_t val);
ST_FUNC �� relocate_plt(TCCState *s1);

/* ------------ xxx-gen.c ------------ */

ST_DATA ���� �� reg_classes[NB_REGS];

ST_FUNC �� gsym_addr(�� t, �� a);
ST_FUNC �� gsym(�� t);
ST_FUNC �� load(�� r, SValue *sv);
ST_FUNC �� store(�� r, SValue *v);
ST_FUNC �� gfunc_sret(CType *vt, �� variadic, CType *ret, �� *align, �� *regsize);
ST_FUNC �� gfunc_call(�� nb_args);
ST_FUNC �� gfunc_prolog(CType *func_type);
ST_FUNC �� gfunc_epilog(��);
ST_FUNC �� gjmp(�� t);
ST_FUNC �� gjmp_addr(�� a);
ST_FUNC �� gtst(�� inv, �� t);
#�� �Ѷ��� TCC_TARGET_I386 || �Ѷ��� TCC_TARGET_X86_64
ST_FUNC �� gtst_addr(�� inv, �� a);
#��
#���� gtst_addr(inv, a) gsym_addr(gtst(inv, 0), a)
#����
ST_FUNC �� gen_opi(�� op);
ST_FUNC �� gen_opf(�� op);
ST_FUNC �� gen_cvt_ftoi(�� t);
ST_FUNC �� gen_cvt_ftof(�� t);
ST_FUNC �� ggoto(��);
#��δ���� TCC_TARGET_C67
ST_FUNC �� o(�޷� �� c);
#����
#��δ���� TCC_TARGET_ARM
ST_FUNC �� gen_cvt_itof(�� t);
#����
ST_FUNC �� gen_vla_sp_save(�� addr);
ST_FUNC �� gen_vla_sp_restore(�� addr);
ST_FUNC �� gen_vla_alloc(CType *type, �� align);

��̬ ���� uint16_t read16le(�޷� �� *p) {
    ���� p[0] | (uint16_t)p[1] << 8;
}
��̬ ���� �� write16le(�޷� �� *p, uint16_t x) {
    p[0] = x & 255, p[1] = x >> 8 & 255;
}
��̬ ���� uint32_t read32le(�޷� �� *p) {
  ���� read16le(p) | (uint32_t)read16le(p + 2) << 16;
}
��̬ ���� �� write32le(�޷� �� *p, uint32_t x) {
    write16le(p, x), write16le(p + 2, x >> 16);
}
��̬ ���� �� add32le(�޷� �� *p, int32_t x) {
    write32le(p, read32le(p) + x);
}
��̬ ���� uint64_t read64le(�޷� �� *p) {
  ���� read32le(p) | (uint64_t)read32le(p + 4) << 32;
}
��̬ ���� �� write64le(�޷� �� *p, uint64_t x) {
    write32le(p, x), write32le(p + 4, x >> 32);
}
��̬ ���� �� add64le(�޷� �� *p, int64_t x) {
    write64le(p, read64le(p) + x);
}

/* ------------ i386-gen.c ------------ */
#�� �Ѷ��� TCC_TARGET_I386 || �Ѷ��� TCC_TARGET_X86_64
ST_FUNC �� g(�� c);
ST_FUNC �� gen_le16(�� c);
ST_FUNC �� gen_le32(�� c);
ST_FUNC �� gen_addr32(�� r, Sym *sym, �� c);
ST_FUNC �� gen_addrpc32(�� r, Sym *sym, �� c);
#����

#�綨�� CONFIG_TCC_BCHECK
ST_FUNC �� gen_bounded_ptr_add(��);
ST_FUNC �� gen_bounded_ptr_deref(��);
#����

/* ------------ x86_64-gen.c ------------ */
#�綨�� TCC_TARGET_X86_64
ST_FUNC �� gen_addr64(�� r, Sym *sym, int64_t c);
ST_FUNC �� gen_opl(�� op);
#����

/* ------------ arm-gen.c ------------ */
#�綨�� TCC_TARGET_ARM
#�� �Ѷ���(TCC_ARM_EABI) && !�Ѷ���(CONFIG_TCC_ELFINTERP)
PUB_FUNC ���� �� *default_elfinterp(�ṹ TCCState *s);
#����
ST_FUNC �� arm_init(�ṹ TCCState *s);
ST_FUNC �� gen_cvt_itof1(�� t);
#����

/* ------------ arm64-gen.c ------------ */
#�綨�� TCC_TARGET_ARM64
ST_FUNC �� gen_cvt_sxtw(��);
ST_FUNC �� gen_opl(�� op);
ST_FUNC �� gfunc_return(CType *func_type);
ST_FUNC �� gen_va_start(��);
ST_FUNC �� gen_va_arg(CType *t);
ST_FUNC �� gen_clear_cache(��);
#����

/* ------------ c67-gen.c ------------ */
#�綨�� TCC_TARGET_C67
#����

/* ------------ tcccoff.c ------------ */

#�綨�� TCC_TARGET_COFF
ST_FUNC �� tcc_output_coff(TCCState *s1, FILE *f);
ST_FUNC �� tcc_load_coff(TCCState * s1, �� fd);
#����

/* ------------ tccasm.c ------------ */
ST_FUNC �� asm_instr(��);
ST_FUNC �� asm_global_instr(��);
#�綨�� CONFIG_TCC_ASM
ST_FUNC �� find_constraint(ASMOperand *operands, �� nb_operands, ���� �� *name, ���� �� **pp);
ST_FUNC Sym* get_asm_sym(�� name, Sym *csym);
ST_FUNC �� asm_expr(TCCState *s1, ExprValue *pe);
ST_FUNC �� asm_int_expr(TCCState *s1);
ST_FUNC �� tcc_assemble(TCCState *s1, �� do_preprocess);
/* ------------ i386-asm.c ------------ */
ST_FUNC �� gen_expr32(ExprValue *pe);
#�綨�� TCC_TARGET_X86_64
ST_FUNC �� gen_expr64(ExprValue *pe);
#����
ST_FUNC �� asm_opcode(TCCState *s1, �� opcode);
ST_FUNC �� asm_parse_regvar(�� t);
ST_FUNC �� asm_compute_constraints(ASMOperand *operands, �� nb_operands, �� nb_outputs, ���� uint8_t *clobber_regs, �� *pout_reg);
ST_FUNC �� subst_asm_operand(CString *add_str, SValue *sv, �� modifier);
ST_FUNC �� asm_gen_code(ASMOperand *operands, �� nb_operands, �� nb_outputs, �� is_output, uint8_t *clobber_regs, �� out_reg);
ST_FUNC �� asm_clobber(uint8_t *clobber_regs, ���� �� *str);
#����

/* ------------ tccpe.c -------------- */
#�綨�� TCC_TARGET_PE
ST_FUNC �� pe_load_file(�ṹ TCCState *s1, ���� �� *filename, �� fd);
ST_FUNC �� pe_output_file(TCCState * s1, ���� �� *filename);
ST_FUNC �� pe_putimport(TCCState *s1, �� dllindex, ���� �� *name, addr_t value);
#��δ���� TCC_TARGET_ARM
ST_FUNC SValue *pe_getimport(SValue *sv, SValue *v2);
#����
#�綨�� TCC_TARGET_X86_64
ST_FUNC �� pe_add_unwind_data(�޷� start, �޷� end, �޷� stack);
#����
PUB_FUNC �� tcc_get_dllexports(���� �� *filename, �� **pp);
/* symbol properties stored in Elf32_Sym->st_other */
# ���� ST_PE_EXPORT 0x10
# ���� ST_PE_IMPORT 0x20
# ���� ST_PE_STDCALL 0x40
#����

/* ------------ tccrun.c ----------------- */
#�綨�� TCC_IS_NATIVE
#�綨�� CONFIG_TCC_STATIC
#���� RTLD_LAZY       0x001
#���� RTLD_NOW        0x002
#���� RTLD_GLOBAL     0x100
#���� RTLD_DEFAULT    NULL
/* dummy function for profiling */
ST_FUNC �� *dlopen(���� �� *filename, �� flag);
ST_FUNC �� dlclose(�� *p);
ST_FUNC ���� �� *dlerror(��);
ST_FUNC �� *dlsym(�� *handle, ���� �� *symbol);
#����
#�綨�� CONFIG_TCC_BACKTRACE
ST_DATA �� rt_num_callers;
ST_DATA ���� �� **rt_bound_error_msg;
ST_DATA �� *rt_prog_main;
ST_FUNC �� tcc_set_num_callers(�� n);
#����
ST_FUNC �� tcc_run_free(TCCState *s1);
#����

/* ------------ tcctools.c ----------------- */
#�� 0 /* included in tcc.c */
ST_FUNC �� tcc_tool_ar(TCCState *s, �� argc, �� **argv);
#�綨�� TCC_TARGET_PE
ST_FUNC �� tcc_tool_impdef(TCCState *s, �� argc, �� **argv);
#����
ST_FUNC �� tcc_tool_cross(TCCState *s, �� **argv, �� option);
ST_FUNC �� gen_makedeps(TCCState *s, ���� �� *target, ���� �� *filename);
#����

/********************************************************/
#������ ST_DATA
#�� ONE_SOURCE
#���� ST_DATA ��̬
#��
#���� ST_DATA
#����
/********************************************************/
#���� /* _TCC_H */
