/*
 *  TCC - Tiny C Compiler - Support for -run switch
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

/* only native compiler supports -run */
#�綨�� TCC_IS_NATIVE

#��δ���� _WIN32
# ���� <sys/mman.h>
#����

#�綨�� CONFIG_TCC_BACKTRACE
# ��δ���� _WIN32
#  ���� <signal.h>
#  ��δ���� __OpenBSD__
#   ���� <sys/ucontext.h>
#  ����
# ��
#  ���� ucontext_t CONTEXT
# ����
ST_DATA �� rt_num_callers = 6;
ST_DATA ���� �� **rt_bound_error_msg;
ST_DATA �� *rt_prog_main;
��̬ �� rt_get_caller_pc(addr_t *paddr, ucontext_t *uc, �� level);
��̬ �� rt_error(ucontext_t *uc, ���� �� *fmt, ...);
��̬ �� set_exception_handler(��);
#����

��̬ �� set_pages_executable(�� *ptr, �޷� �� length);
��̬ �� tcc_relocate_ex(TCCState *s1, �� *ptr);

#�綨�� _WIN64
��̬ �� *win64_add_function_table(TCCState *s1);
��̬ �� win64_del_function_table(�� *);
#����

// #���� HAVE_SELINUX

/* ------------------------------------------------------------- */
/* Do all relocations (needed before using tcc_get_symbol())
   Returns -1 on error. */

LIBTCCAPI �� tcc_relocate(TCCState *s1, �� *ptr)
{
    �� size;

    �� (TCC_RELOCATE_AUTO != ptr)
        ���� tcc_relocate_ex(s1, ptr);

    size = tcc_relocate_ex(s1, NULL);
    �� (size < 0)
        ���� -1;

#�綨�� HAVE_SELINUX
    /* Use mmap instead of malloc for Selinux. */
    ptr = mmap (NULL, size, PROT_READ|PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    �� (ptr == MAP_FAILED)
        tcc_error("tccrun: could not map memory");
    dynarray_add(&s1->runtime_mem, &s1->nb_runtime_mem, (��*)(addr_t)size);
#��
    ptr = tcc_malloc(size);
#����
    tcc_relocate_ex(s1, ptr); /* no more errors expected */
    dynarray_add(&s1->runtime_mem, &s1->nb_runtime_mem, ptr);
    ���� 0;
}

ST_FUNC �� tcc_run_free(TCCState *s1)
{
    �� i;

    ���� (i = 0; i < s1->nb_runtime_mem; ++i) {
#�綨�� HAVE_SELINUX
        �޷� size = (�޷�)(addr_t)s1->runtime_mem[i++];
        munmap(s1->runtime_mem[i], size);
#��
#�綨�� _WIN64
        win64_del_function_table(*(��**)s1->runtime_mem[i]);
#����
        tcc_free(s1->runtime_mem[i]);
#����
    }
    tcc_free(s1->runtime_mem);
}

/* launch the compiled program with the given arguments */
LIBTCCAPI �� tcc_run(TCCState *s1, �� argc, �� **argv)
{
    �� (*prog_main)(��, �� **);

    s1->runtime_main = "main";
    �� ((s1->dflag & 16) && !find_elf_sym(s1->symtab, s1->runtime_main))
        ���� 0;
    �� (tcc_relocate(s1, TCC_RELOCATE_AUTO) < 0)
        ���� -1;
    prog_main = tcc_get_symbol_err(s1, s1->runtime_main);

#�綨�� CONFIG_TCC_BACKTRACE
    �� (s1->do_debug) {
        set_exception_handler();
        rt_prog_main = prog_main;
    }
#����

    errno = 0; /* clean errno value */

#�綨�� CONFIG_TCC_BCHECK
    �� (s1->do_bounds_check) {
        �� (*bound_init)(��);
        �� (*bound_exit)(��);
        �� (*bound_new_region)(�� *p, addr_t size);
        ��  (*bound_delete_region)(�� *p);
        �� i, ret;

        /* set error function */
        rt_bound_error_msg = tcc_get_symbol_err(s1, "__bound_error_msg");
        /* XXX: use .init section so that it also work in binary ? */
        bound_init = tcc_get_symbol_err(s1, "__bound_init");
        bound_exit = tcc_get_symbol_err(s1, "__bound_exit");
        bound_new_region = tcc_get_symbol_err(s1, "__bound_new_region");
        bound_delete_region = tcc_get_symbol_err(s1, "__bound_delete_region");

        bound_init();
        /* mark argv area as valid */
        bound_new_region(argv, argc*�󳤶�(argv[0]));
        ���� (i=0; i<argc; ++i)
            bound_new_region(argv[i], strlen(argv[i]) + 1);

        ret = (*prog_main)(argc, argv);

        /* unmark argv area */
        ���� (i=0; i<argc; ++i)
            bound_delete_region(argv[i]);
        bound_delete_region(argv);
        bound_exit();
        ���� ret;
    }
#����
    ���� (*prog_main)(argc, argv);
}

#�� �Ѷ��� TCC_TARGET_I386 || �Ѷ��� TCC_TARGET_X86_64
 #���� RUN_SECTION_ALIGNMENT 63
#��
 #���� RUN_SECTION_ALIGNMENT 15
#����

/* relocate code. Return -1 on error, required size if ptr is NULL,
   otherwise copy code into buffer passed by the caller */
��̬ �� tcc_relocate_ex(TCCState *s1, �� *ptr)
{
    Section *s;
    �޷� offset, length, fill, i, k;
    addr_t mem;

    �� (NULL == ptr) {
        s1->nb_errors = 0;
#�綨�� TCC_TARGET_PE
        pe_output_file(s1, NULL);
#��
        tcc_add_runtime(s1);
        relocate_common_syms();
        tcc_add_linker_symbols(s1);
        build_got_entries(s1);
#����
        �� (s1->nb_errors)
            ���� -1;
    }

    offset = 0, mem = (addr_t)ptr;
    fill = -mem & RUN_SECTION_ALIGNMENT;
#�綨�� _WIN64
    offset += �󳤶� (��*);
#����
    ���� (k = 0; k < 2; ++k) {
        ����(i = 1; i < s1->nb_sections; i++) {
            s = s1->sections[i];
            �� (0 == (s->sh_flags & SHF_ALLOC))
                ����;
            �� (k != !(s->sh_flags & SHF_EXECINSTR))
                ����;
            offset += fill;
            s->sh_addr = mem ? mem + offset : 0;
#�� 0
            �� (mem)
                printf("%-16s +%02lx %p %04x\n",
                    s->name, fill, (��*)s->sh_addr, (�޷�)s->data_offset);
#����
            offset += s->data_offset;
            fill = -(mem + offset) & 15;
        }
#�� RUN_SECTION_ALIGNMENT > 15
        /* To avoid that x86 processors would reload cached instructions each time
           when data is written in the near, we need to make sure that code and data
           do not share the same 64 byte unit */
        fill = -(mem + offset) & RUN_SECTION_ALIGNMENT;
#����
    }

    /* relocate symbols */
    relocate_syms(s1, s1->symtab, 1);
    �� (s1->nb_errors)
        ���� -1;

    �� (0 == mem)
        ���� offset + RUN_SECTION_ALIGNMENT;

    /* relocate each section */
    ����(i = 1; i < s1->nb_sections; i++) {
        s = s1->sections[i];
        �� (s->reloc)
            relocate_section(s1, s);
    }
    relocate_plt(s1);

#�綨�� _WIN64
    *(��**)ptr = win64_add_function_table(s1);
#����

    ����(i = 1; i < s1->nb_sections; i++) {
        s = s1->sections[i];
        �� (0 == (s->sh_flags & SHF_ALLOC))
            ����;
        length = s->data_offset;
        ptr = (��*)s->sh_addr;
        �� (NULL == s->data || s->sh_type == SHT_NOBITS)
            memset(ptr, 0, length);
        ��
            memcpy(ptr, s->data, length);
        /* mark executable sections as executable in memory */
        �� (s->sh_flags & SHF_EXECINSTR)
            set_pages_executable(ptr, length);
    }
    ���� 0;
}

/* ------------------------------------------------------------- */
/* allow to run code in memory */

��̬ �� set_pages_executable(�� *ptr, �޷� �� length)
{
#�綨�� _WIN32
    �޷� �� old_protect;
    VirtualProtect(ptr, length, PAGE_EXECUTE_READWRITE, &old_protect);
#��
    �� __clear_cache(�� *beginning, �� *end);
    addr_t start, end;
#��δ���� PAGESIZE
# ���� PAGESIZE 4096
#����
    start = (addr_t)ptr & ~(PAGESIZE - 1);
    end = (addr_t)ptr + length;
    end = (end + PAGESIZE - 1) & ~(PAGESIZE - 1);
    �� (mprotect((�� *)start, end - start, PROT_READ | PROT_WRITE | PROT_EXEC))
        tcc_error("mprotect failed: did you mean to configure --with-selinux?");
# �� �Ѷ��� TCC_TARGET_ARM || �Ѷ��� TCC_TARGET_ARM64
    __clear_cache(ptr, (�� *)ptr + length);
# ����
#����
}

#�綨�� _WIN64
��̬ �� *win64_add_function_table(TCCState *s1)
{
    �� *p = NULL;
    �� (s1->uw_pdata) {
        p = (��*)s1->uw_pdata->sh_addr;
        RtlAddFunctionTable(
            (RUNTIME_FUNCTION*)p,
            s1->uw_pdata->data_offset / �󳤶� (RUNTIME_FUNCTION),
            text_section->sh_addr
            );
        s1->uw_pdata = NULL;
    }
    ���� p;;
}

��̬ �� win64_del_function_table(�� *p)
{
    �� (p) {
        RtlDeleteFunctionTable((RUNTIME_FUNCTION*)p);
    }
}
#����

/* ------------------------------------------------------------- */
#�綨�� CONFIG_TCC_BACKTRACE

ST_FUNC �� tcc_set_num_callers(�� n)
{
    rt_num_callers = n;
}

/* print the position in the source file of PC value 'pc' by reading
   the stabs debug information */
��̬ addr_t rt_printline(addr_t wanted_pc, ���� �� *msg)
{
    �� func_name[128], last_func_name[128];
    addr_t func_addr, last_pc, pc;
    ���� �� *incl_files[INCLUDE_STACK_SIZE];
    �� incl_index, len, last_line_num, i;
    ���� �� *str, *p;

    Stab_Sym *stab_sym = NULL, *stab_sym_end, *sym;
    �� stab_len = 0;
    �� *stab_str = NULL;

    �� (stab_section) {
        stab_len = stab_section->data_offset;
        stab_sym = (Stab_Sym *)stab_section->data;
        stab_str = (�� *) stabstr_section->data;
    }

    func_name[0] = '\0';
    func_addr = 0;
    incl_index = 0;
    last_func_name[0] = '\0';
    last_pc = (addr_t)-1;
    last_line_num = 1;

    �� (!stab_sym)
        ��ת no_stabs;

    stab_sym_end = (Stab_Sym*)((��*)stab_sym + stab_len);
    ���� (sym = stab_sym + 1; sym < stab_sym_end; ++sym) {
        ת��(sym->n_type) {
            /* function start or end */
        ���� N_FUN:
            �� (sym->n_strx == 0) {
                /* we test if between last line and end of function */
                pc = sym->n_value + func_addr;
                �� (wanted_pc >= last_pc && wanted_pc < pc)
                    ��ת found;
                func_name[0] = '\0';
                func_addr = 0;
            } �� {
                str = stab_str + sym->n_strx;
                p = strchr(str, ':');
                �� (!p) {
                    pstrcpy(func_name, �󳤶�(func_name), str);
                } �� {
                    len = p - str;
                    �� (len > �󳤶�(func_name) - 1)
                        len = �󳤶�(func_name) - 1;
                    memcpy(func_name, str, len);
                    func_name[len] = '\0';
                }
                func_addr = sym->n_value;
            }
            ����;
            /* line number info */
        ���� N_SLINE:
            pc = sym->n_value + func_addr;
            �� (wanted_pc >= last_pc && wanted_pc < pc)
                ��ת found;
            last_pc = pc;
            last_line_num = sym->n_desc;
            /* XXX: slow! */
            strcpy(last_func_name, func_name);
            ����;
            /* include files */
        ���� N_BINCL:
            str = stab_str + sym->n_strx;
        add_incl:
            �� (incl_index < INCLUDE_STACK_SIZE) {
                incl_files[incl_index++] = str;
            }
            ����;
        ���� N_EINCL:
            �� (incl_index > 1)
                incl_index--;
            ����;
        ���� N_SO:
            �� (sym->n_strx == 0) {
                incl_index = 0; /* end of translation unit */
            } �� {
                str = stab_str + sym->n_strx;
                /* do not add path */
                len = strlen(str);
                �� (len > 0 && str[len - 1] != '/')
                    ��ת add_incl;
            }
            ����;
        }
    }

no_stabs:
    /* second pass: we try symtab symbols (no line number info) */
    incl_index = 0;
    �� (symtab_section)
    {
        ElfW(Sym) *sym, *sym_end;
        �� type;

        sym_end = (ElfW(Sym) *)(symtab_section->data + symtab_section->data_offset);
        ����(sym = (ElfW(Sym) *)symtab_section->data + 1;
            sym < sym_end;
            sym++) {
            type = ELFW(ST_TYPE)(sym->st_info);
            �� (type == STT_FUNC || type == STT_GNU_IFUNC) {
                �� (wanted_pc >= sym->st_value &&
                    wanted_pc < sym->st_value + sym->st_size) {
                    pstrcpy(last_func_name, �󳤶�(last_func_name),
                            (�� *) strtab_section->data + sym->st_name);
                    func_addr = sym->st_value;
                    ��ת found;
                }
            }
        }
    }
    /* did not find any info: */
    fprintf(stderr, "%s %p ???\n", msg, (��*)wanted_pc);
    fflush(stderr);
    ���� 0;
 found:
    i = incl_index;
    �� (i > 0)
        fprintf(stderr, "%s:%d: ", incl_files[--i], last_line_num);
    fprintf(stderr, "%s %p", msg, (��*)wanted_pc);
    �� (last_func_name[0] != '\0')
        fprintf(stderr, " %s()", last_func_name);
    �� (--i >= 0) {
        fprintf(stderr, " (included from ");
        ���� (;;) {
            fprintf(stderr, "%s", incl_files[i]);
            �� (--i < 0)
                ����;
            fprintf(stderr, ", ");
        }
        fprintf(stderr, ")");
    }
    fprintf(stderr, "\n");
    fflush(stderr);
    ���� func_addr;
}

/* emit a run time error at position 'pc' */
��̬ �� rt_error(ucontext_t *uc, ���� �� *fmt, ...)
{
    va_list ap;
    addr_t pc;
    �� i;

    fprintf(stderr, "Runtime error: ");
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    va_end(ap);
    fprintf(stderr, "\n");

    ����(i=0;i<rt_num_callers;i++) {
        �� (rt_get_caller_pc(&pc, uc, i) < 0)
            ����;
        pc = rt_printline(pc, i ? "by" : "at");
        �� (pc == (addr_t)rt_prog_main && pc)
            ����;
    }
}

/* ------------------------------------------------------------- */
#��δ���� _WIN32

/* signal handler for fatal errors */
��̬ �� sig_error(�� signum, siginfo_t *siginf, �� *puc)
{
    ucontext_t *uc = puc;

    ת��(signum) {
    ���� SIGFPE:
        ת��(siginf->si_code) {
        ���� FPE_INTDIV:
        ���� FPE_FLTDIV:
            rt_error(uc, "division by zero");
            ����;
        ȱʡ:
            rt_error(uc, "floating point exception");
            ����;
        }
        ����;
    ���� SIGBUS:
    ���� SIGSEGV:
        �� (rt_bound_error_msg && *rt_bound_error_msg)
            rt_error(uc, *rt_bound_error_msg);
        ��
            rt_error(uc, "dereferencing invalid pointer");
        ����;
    ���� SIGILL:
        rt_error(uc, "illegal instruction");
        ����;
    ���� SIGABRT:
        rt_error(uc, "abort() called");
        ����;
    ȱʡ:
        rt_error(uc, "caught signal %d", signum);
        ����;
    }
    exit(255);
}

#��δ���� SA_SIGINFO
# ���� SA_SIGINFO 0x00000004u
#����

/* Generate a stack backtrace when a CPU exception occurs. */
��̬ �� set_exception_handler(��)
{
    �ṹ sigaction sigact;
    /* install TCC signal handlers to print debug info on fatal
       runtime errors */
    sigact.sa_flags = SA_SIGINFO | SA_RESETHAND;
    sigact.sa_sigaction = sig_error;
    sigemptyset(&sigact.sa_mask);
    sigaction(SIGFPE, &sigact, NULL);
    sigaction(SIGILL, &sigact, NULL);
    sigaction(SIGSEGV, &sigact, NULL);
    sigaction(SIGBUS, &sigact, NULL);
    sigaction(SIGABRT, &sigact, NULL);
}

/* ------------------------------------------------------------- */
#�綨�� __i386__

/* fix for glibc 2.1 */
#��δ���� REG_EIP
#���� REG_EIP EIP
#���� REG_EBP EBP
#����

/* return the PC at frame level 'level'. Return negative if not found */
��̬ �� rt_get_caller_pc(addr_t *paddr, ucontext_t *uc, �� level)
{
    addr_t fp;
    �� i;

    �� (level == 0) {
#�� �Ѷ���(__APPLE__)
        *paddr = uc->uc_mcontext->__ss.__eip;
#���� �Ѷ���(__FreeBSD__) || �Ѷ���(__FreeBSD_kernel__) || �Ѷ���(__DragonFly__)
        *paddr = uc->uc_mcontext.mc_eip;
#���� �Ѷ���(__dietlibc__)
        *paddr = uc->uc_mcontext.eip;
#���� �Ѷ���(__NetBSD__)
        *paddr = uc->uc_mcontext.__gregs[_REG_EIP];
#���� �Ѷ���(__OpenBSD__)
        *paddr = uc->sc_eip;
#��
        *paddr = uc->uc_mcontext.gregs[REG_EIP];
#����
        ���� 0;
    } �� {
#�� �Ѷ���(__APPLE__)
        fp = uc->uc_mcontext->__ss.__ebp;
#���� �Ѷ���(__FreeBSD__) || �Ѷ���(__FreeBSD_kernel__) || �Ѷ���(__DragonFly__)
        fp = uc->uc_mcontext.mc_ebp;
#���� �Ѷ���(__dietlibc__)
        fp = uc->uc_mcontext.ebp;
#���� �Ѷ���(__NetBSD__)
        fp = uc->uc_mcontext.__gregs[_REG_EBP];
#���� �Ѷ���(__OpenBSD__)
        *paddr = uc->sc_ebp;
#��
        fp = uc->uc_mcontext.gregs[REG_EBP];
#����
        ����(i=1;i<level;i++) {
            /* XXX: check address validity with program info */
            �� (fp <= 0x1000 || fp >= 0xc0000000)
                ���� -1;
            fp = ((addr_t *)fp)[0];
        }
        *paddr = ((addr_t *)fp)[1];
        ���� 0;
    }
}

/* ------------------------------------------------------------- */
#���� �Ѷ���(__x86_64__)

/* return the PC at frame level 'level'. Return negative if not found */
��̬ �� rt_get_caller_pc(addr_t *paddr, ucontext_t *uc, �� level)
{
    addr_t fp;
    �� i;

    �� (level == 0) {
        /* XXX: only support linux */
#�� �Ѷ���(__APPLE__)
        *paddr = uc->uc_mcontext->__ss.__rip;
#���� �Ѷ���(__FreeBSD__) || �Ѷ���(__FreeBSD_kernel__) || �Ѷ���(__DragonFly__)
        *paddr = uc->uc_mcontext.mc_rip;
#���� �Ѷ���(__NetBSD__)
        *paddr = uc->uc_mcontext.__gregs[_REG_RIP];
#��
        *paddr = uc->uc_mcontext.gregs[REG_RIP];
#����
        ���� 0;
    } �� {
#�� �Ѷ���(__APPLE__)
        fp = uc->uc_mcontext->__ss.__rbp;
#���� �Ѷ���(__FreeBSD__) || �Ѷ���(__FreeBSD_kernel__) || �Ѷ���(__DragonFly__)
        fp = uc->uc_mcontext.mc_rbp;
#���� �Ѷ���(__NetBSD__)
        fp = uc->uc_mcontext.__gregs[_REG_RBP];
#��
        fp = uc->uc_mcontext.gregs[REG_RBP];
#����
        ����(i=1;i<level;i++) {
            /* XXX: check address validity with program info */
            �� (fp <= 0x1000)
                ���� -1;
            fp = ((addr_t *)fp)[0];
        }
        *paddr = ((addr_t *)fp)[1];
        ���� 0;
    }
}

/* ------------------------------------------------------------- */
#���� �Ѷ���(__arm__)

/* return the PC at frame level 'level'. Return negative if not found */
��̬ �� rt_get_caller_pc(addr_t *paddr, ucontext_t *uc, �� level)
{
    addr_t fp, sp;
    �� i;

    �� (level == 0) {
        /* XXX: only supports linux */
#�� �Ѷ���(__linux__)
        *paddr = uc->uc_mcontext.arm_pc;
#��
        ���� -1;
#����
        ���� 0;
    } �� {
#�� �Ѷ���(__linux__)
        fp = uc->uc_mcontext.arm_fp;
        sp = uc->uc_mcontext.arm_sp;
        �� (sp < 0x1000)
            sp = 0x1000;
#��
        ���� -1;
#����
        /* XXX: specific to tinycc stack frames */
        �� (fp < sp + 12 || fp & 3)
            ���� -1;
        ����(i = 1; i < level; i++) {
            sp = ((addr_t *)fp)[-2];
            �� (sp < fp || sp - fp > 16 || sp & 3)
                ���� -1;
            fp = ((addr_t *)fp)[-3];
            �� (fp <= sp || fp - sp < 12 || fp & 3)
                ���� -1;
        }
        /* XXX: check address validity with program info */
        *paddr = ((addr_t *)fp)[-1];
        ���� 0;
    }
}

/* ------------------------------------------------------------- */
#���� �Ѷ���(__aarch64__)

��̬ �� rt_get_caller_pc(addr_t *paddr, ucontext_t *uc, �� level)
{
    �� (level < 0)
        ���� -1;
    �� �� (level == 0) {
        *paddr = uc->uc_mcontext.pc;
        ���� 0;
    }
    �� {
        addr_t *fp = (addr_t *)uc->uc_mcontext.regs[29];
        �� i;
        ���� (i = 1; i < level; i++)
            fp = (addr_t *)fp[0];
        *paddr = fp[1];
        ���� 0;
    }
}

/* ------------------------------------------------------------- */
#��

#�澯 add arch specific rt_get_caller_pc()
��̬ �� rt_get_caller_pc(addr_t *paddr, ucontext_t *uc, �� level)
{
    ���� -1;
}

#���� /* !__i386__ */

/* ------------------------------------------------------------- */
#�� /* WIN32 */

��̬ �� __stdcall cpu_exception_handler(EXCEPTION_POINTERS *ex_info)
{
    EXCEPTION_RECORD *er = ex_info->ExceptionRecord;
    CONTEXT *uc = ex_info->ContextRecord;
    ת�� (er->ExceptionCode) {
    ���� EXCEPTION_ACCESS_VIOLATION:
        �� (rt_bound_error_msg && *rt_bound_error_msg)
            rt_error(uc, *rt_bound_error_msg);
        ��
            rt_error(uc, "access violation");
        ����;
    ���� EXCEPTION_STACK_OVERFLOW:
        rt_error(uc, "stack overflow");
        ����;
    ���� EXCEPTION_INT_DIVIDE_BY_ZERO:
        rt_error(uc, "division by zero");
        ����;
    ȱʡ:
        rt_error(uc, "exception caught");
        ����;
    }
    ���� EXCEPTION_EXECUTE_HANDLER;
}

/* Generate a stack backtrace when a CPU exception occurs. */
��̬ �� set_exception_handler(��)
{
    SetUnhandledExceptionFilter(cpu_exception_handler);
}

/* return the PC at frame level 'level'. Return non zero if not found */
��̬ �� rt_get_caller_pc(addr_t *paddr, CONTEXT *uc, �� level)
{
    addr_t fp, pc;
    �� i;
#�綨�� _WIN64
    pc = uc->Rip;
    fp = uc->Rbp;
#��
    pc = uc->Eip;
    fp = uc->Ebp;
#����
    �� (level > 0) {
        ����(i=1;i<level;i++) {
            /* XXX: check address validity with program info */
            �� (fp <= 0x1000 || fp >= 0xc0000000)
                ���� -1;
            fp = ((addr_t*)fp)[0];
        }
        pc = ((addr_t*)fp)[1];
    }
    *paddr = pc;
    ���� 0;
}

#���� /* _WIN32 */
#���� /* CONFIG_TCC_BACKTRACE */
/* ------------------------------------------------------------- */
#�綨�� CONFIG_TCC_STATIC

/* dummy function for profiling */
ST_FUNC �� *dlopen(���� �� *filename, �� flag)
{
    ���� NULL;
}

ST_FUNC �� dlclose(�� *p)
{
}

ST_FUNC ���� �� *dlerror(��)
{
    ���� "error";
}

���Ͷ��� �ṹ TCCSyms {
    �� *str;
    �� *ptr;
} TCCSyms;


/* add the symbol you want here if no dynamic linking is done */
��̬ TCCSyms tcc_syms[] = {
#�� !�Ѷ���(CONFIG_TCCBOOT)
#���� TCCSYM(a) { #a, &a, },
    TCCSYM(printf)
    TCCSYM(fprintf)
    TCCSYM(fopen)
    TCCSYM(fclose)
#������ TCCSYM
#����
    { NULL, NULL },
};

ST_FUNC �� *dlsym(�� *handle, ���� �� *symbol)
{
    TCCSyms *p;
    p = tcc_syms;
    �� (p->str != NULL) {
        �� (!strcmp(p->str, symbol))
            ���� p->ptr;
        p++;
    }
    ���� NULL;
}

#���� /* CONFIG_TCC_STATIC */
#���� /* TCC_IS_NATIVE */
/* ------------------------------------------------------------- */
