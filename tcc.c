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

#���� "tcc.h"
#�� ONE_SOURCE
# ���� "libtcc.c"
#����
#���� "tcctools.c"

��̬ ���� �� help[] =
    "Tiny C Compiler "TCC_VERSION" - Copyright (C) 2001-2006 Fabrice Bellard\n"
    "Usage: tcc [options...] [-o outfile] [-c] infile(s)...\n"
    "       tcc [options...] -run infile [arguments...]\n"
    "General options:\n"
    "  -c          compile only - generate an object file\n"
    "  -o outfile  set output filename\n"
    "  -run        run compiled source\n"
    "  -fflag      set or reset (with 'no-' prefix) 'flag' (see tcc -hh)\n"
    "  -Wwarning   set or reset (with 'no-' prefix) 'warning' (see tcc -hh)\n"
    "  -w          disable all warnings\n"
    "  -v -vv      show version, show search paths or loaded files\n"
    "  -h -hh      show this, show more help\n"
    "  -bench      show compilation statistics\n"
    "  -           use stdin pipe as infile\n"
    "  @listfile   read arguments from listfile\n"
    "Preprocessor options:\n"
    "  -Idir       add include path 'dir'\n"
    "  -Dsym[=val] define 'sym' with value 'val'\n"
    "  -Usym       undefine 'sym'\n"
    "  -E          preprocess only\n"
    "Linker options:\n"
    "  -Ldir       add library path 'dir'\n"
    "  -llib       link with dynamic or static library 'lib'\n"
    "  -r          generate (relocatable) object file\n"
    "  -shared     generate a shared library/dll\n"
    "  -rdynamic   export all global symbols to dynamic linker\n"
    "  -soname     set name for shared library to be used at runtime\n"
    "  -Wl,-opt[=val]  set linker option (see tcc -hh)\n"
    "Debugger options:\n"
    "  -g          generate runtime debug info\n"
#�綨�� CONFIG_TCC_BCHECK
    "  -b          compile with built-in memory and bounds checker (implies -g)\n"
#����
#�綨�� CONFIG_TCC_BACKTRACE
    "  -bt N       show N callers in stack traces\n"
#����
    "Misc. options:\n"
    "  -x[c|a|n]   specify type of the next infile\n"
    "  -nostdinc   do not use standard system include paths\n"
    "  -nostdlib   do not link with standard crt and libraries\n"
    "  -Bdir       set tcc's private include/library dir\n"
    "  -MD         generate dependency file for make\n"
    "  -MF file    specify dependency file name\n"
    "  -m32/64     defer to i386/x86_64 cross compiler\n"
    "Tools:\n"
    "  create library  : tcc -ar [rcsv] lib.a files\n"
#�綨�� TCC_TARGET_PE
    "  create def file : tcc -impdef lib.dll [-v] [-o lib.def]\n"
#����
    ;

��̬ ���� �� help2[] =
    "Tiny C Compiler "TCC_VERSION" - More Options\n"
    "Special options:\n"
    "  -P -P1                        with -E: no/alternative #line output\n"
    "  -dD -dM                       with -E: output #define directives\n"
    "  -pthread                      same as -D_REENTRANT and -lpthread\n"
    "  -On                           same as -D__OPTIMIZE__ for n > 0\n"
    "  -Wp,-opt                      same as -opt\n"
    "  -include file                 include 'file' above each input file\n"
    "  -isystem dir                  add 'dir' to system include path\n"
    "  -iwithprefix dir              set tcc's private include/library subdir\n"
    "  -static                       link to static libraries (not recommended)\n"
    "  -dumpversion                  print version\n"
    "  -print-search-dirs            print search paths\n"
    "  -dt                           with -run/-E: auto-define 'test_...' macros\n"
    "Ignored options:\n"
    "  --param  -pedantic  -pipe  -s  -std  -traditional\n"
    "-W... warnings:\n"
    "  all                           turn on some (*) warnings\n"
    "  error                         stop after first warning\n"
    "  unsupported                   warn about ignored options, pragmas, etc.\n"
    "  write-strings                 strings are const\n"
    "  implicit-function-declaration warn for missing prototype (*)\n"
    "-f[no-]... flags:\n"
    "  unsigned-char                 default char is unsigned\n"
    "  signed-char                   default char is signed\n"
    "  common                        use common section instead of bss\n"
    "  leading-underscore            decorate extern symbols\n"
    "  ms-extensions                 allow anonymous struct in struct\n"
    "  dollars-in-identifiers        allow '$' in C symbols\n"
    "-m... target specific options:\n"
    "  ms-bitfields                  use MSVC bitfield layout\n"
#�綨�� TCC_TARGET_ARM
    "  float-abi                     hard/softfp on arm\n"
#����
#�綨�� TCC_TARGET_X86_64
    "  no-sse                        disable floats on x86_64\n"
#����
    "-Wl,... linker options:\n"
    "  -nostdlib                     do not link with standard crt/libs\n"
    "  -[no-]whole-archive           load lib(s) fully/only as needed\n"
    "  -export-all-symbols           same as -rdynamic\n"
    "  -image-base= -Ttext=          set base address of executable\n"
    "  -section-alignment=           set section alignment in executable\n"
#�綨�� TCC_TARGET_PE
    "  -file-alignment=              set PE file alignment\n"
    "  -stack=                       set PE stack reserve\n"
    "  -large-address-aware          set related PE option\n"
    "  -subsystem=[console/windows]  set PE subsystem\n"
    "  -oformat=[pe-* binary]        set executable output format\n"
    "Predefined macros:\n"
    "  tcc -E -dM - < nul\n"
#��
    "  -rpath=                       set dynamic library search path\n"
    "  -enable-new-dtags             set DT_RUNPATH instead of DT_RPATH\n"
    "  -soname=                      set DT_SONAME elf tag\n"
    "  -Bsymbolic                    set DT_SYMBOLIC elf tag\n"
    "  -oformat=[elf32/64-* binary]  set executable output format\n"
    "  -init= -fini= -as-needed -O   (ignored)\n"
    "Predefined macros:\n"
    "  tcc -E -dM - < /dev/null\n"
#����
    "See also the manual for more details.\n"
    ;

��̬ ���� �� version[] =
    "tcc version "TCC_VERSION" ("
#�綨�� TCC_TARGET_I386
        "i386"
#���� �Ѷ��� TCC_TARGET_X86_64
        "x86_64"
#���� �Ѷ��� TCC_TARGET_C67
        "C67"
#���� �Ѷ��� TCC_TARGET_ARM
        "ARM"
#���� �Ѷ��� TCC_TARGET_ARM64
        "AArch64"
#����
#�綨�� TCC_ARM_HARDFLOAT
        " Hard Float"
#����
#�綨�� TCC_TARGET_PE
        " Windows"
#���� �Ѷ���(TCC_TARGET_MACHO)
        " Darwin"
#���� �Ѷ���(__FreeBSD__) || �Ѷ���(__FreeBSD_kernel__)
        " FreeBSD"
#��
        " Linux"
#����
    ")\n"
    ;

��̬ �� print_dirs(���� �� *msg, �� **paths, �� nb_paths)
{
    �� i;
    printf("%s:\n%s", msg, nb_paths ? "" : "  -\n");
    ����(i = 0; i < nb_paths; i++)
        printf("  %s\n", paths[i]);
}

��̬ �� print_search_dirs(TCCState *s)
{
    printf("install: %s\n", s->tcc_lib_path);
    /* print_dirs("programs", NULL, 0); */
    print_dirs("include", s->sysinclude_paths, s->nb_sysinclude_paths);
    print_dirs("libraries", s->library_paths, s->nb_library_paths);
#��δ���� TCC_TARGET_PE
    print_dirs("crt", s->crt_paths, s->nb_crt_paths);
    printf("libtcc1:\n  %s/"TCC_LIBTCC1"\n", s->tcc_lib_path);
    printf("elfinterp:\n  %s\n",  DEFAULT_ELFINTERP(s));
#����
}

��̬ �� set_environment(TCCState *s)
{
    �� * path;

    path = getenv("C_INCLUDE_PATH");
    ��(path != NULL) {
        tcc_add_include_path(s, path);
    }
    path = getenv("CPATH");
    ��(path != NULL) {
        tcc_add_include_path(s, path);
    }
    path = getenv("LIBRARY_PATH");
    ��(path != NULL) {
        tcc_add_library_path(s, path);
    }
}

��̬ �� *default_outputfile(TCCState *s, ���� �� *first_file)
{
    �� buf[1024];
    �� *ext;
    ���� �� *name = "a";

    �� (first_file && strcmp(first_file, "-"))
        name = tcc_basename(first_file);
    snprintf(buf, �󳤶�(buf), "%s", name);
    ext = tcc_fileextension(buf);
#�綨�� TCC_TARGET_PE
    �� (s->output_type == TCC_OUTPUT_DLL)
        strcpy(ext, ".dll");
    ��
    �� (s->output_type == TCC_OUTPUT_EXE)
        strcpy(ext, ".exe");
    ��
#����
    �� (s->output_type == TCC_OUTPUT_OBJ && !s->option_r && *ext)
        strcpy(ext, ".o");
    ��
        strcpy(buf, "a.out");
    ���� tcc_strdup(buf);
}

��̬ �޷� getclock_ms(��)
{
#�綨�� _WIN32
    ���� GetTickCount();
#��
    �ṹ timeval tv;
    gettimeofday(&tv, NULL);
    ���� tv.tv_sec*1000 + (tv.tv_usec+500)/1000;
#����
}

�� main(�� argc0, �� **argv0)
{
    TCCState *s;
    �� ret, opt, n = 0, t = 0;
    �޷� start_time = 0;
    ���� �� *first_file;
    �� argc; �� **argv;
    FILE *ppfp = stdout;

redo:
    argc = argc0, argv = argv0;
    s = tcc_new();
    opt = tcc_parse_args(s, &argc, &argv, 1);

    �� ((n | t) == 0) {
        �� (opt == OPT_HELP)
            ���� printf(help), 1;
        �� (opt == OPT_HELP2)
            ���� printf(help2), 1;
        �� (opt == OPT_M32 || opt == OPT_M64)
            tcc_tool_cross(s, argv, opt); /* never returns */
        �� (s->verbose)
            printf(version);
        �� (opt == OPT_AR)
            ���� tcc_tool_ar(s, argc, argv);
#�綨�� TCC_TARGET_PE
        �� (opt == OPT_IMPDEF)
            ���� tcc_tool_impdef(s, argc, argv);
#����
        �� (opt == OPT_V)
            ���� 0;
        �� (opt == OPT_PRINT_DIRS) {
            /* initialize search dirs */
            tcc_set_output_type(s, TCC_OUTPUT_MEMORY);
            print_search_dirs(s);
            ���� 0;
        }

        n = s->nb_files;
        �� (n == 0)
            tcc_error("no input files\n");

        �� (s->output_type == TCC_OUTPUT_PREPROCESS) {
            �� (s->outfile) {
                ppfp = fopen(s->outfile, "w");
                �� (!ppfp)
                    tcc_error("could not write '%s'", s->outfile);
            }
        } �� �� (s->output_type == TCC_OUTPUT_OBJ && !s->option_r) {
            �� (s->nb_libraries)
                tcc_error("cannot specify libraries with -c");
            �� (n > 1 && s->outfile)
                tcc_error("cannot specify output file with -c many files");
        } �� {
            �� (s->option_pthread)
                tcc_set_options(s, "-lpthread");
        }

        �� (s->do_bench)
            start_time = getclock_ms();
    }

    set_environment(s);
    �� (s->output_type == 0)
        s->output_type = TCC_OUTPUT_EXE;
    tcc_set_output_type(s, s->output_type);
    s->ppfp = ppfp;

    �� ((s->output_type == TCC_OUTPUT_MEMORY
      || s->output_type == TCC_OUTPUT_PREPROCESS) && (s->dflag & 16))
        s->dflag |= t ? 32 : 0, s->run_test = ++t, n = s->nb_files;

    /* compile or add each files or library */
    ���� (first_file = NULL, ret = 0;;) {
        �ṹ filespec *f = s->files[s->nb_files - n];
        s->filetype = f->type;
        s->alacarte_link = f->alacarte;
        �� (f->type == AFF_TYPE_LIB) {
            �� (tcc_add_library_err(s, f->name) < 0)
                ret = 1;
        } �� {
            �� (1 == s->verbose)
                printf("-> %s\n", f->name);
            �� (!first_file)
                first_file = f->name;
            �� (tcc_add_file(s, f->name) < 0)
                ret = 1;
        }
        s->filetype = 0;
        s->alacarte_link = 1;
        �� (--n == 0 || ret
            || (s->output_type == TCC_OUTPUT_OBJ && !s->option_r))
            ����;
    }

    �� (s->run_test) {
        t = 0;
    } �� �� (s->output_type == TCC_OUTPUT_PREPROCESS) {
        ;
    } �� �� (0 == ret) {
        �� (s->output_type == TCC_OUTPUT_MEMORY) {
#�綨�� TCC_IS_NATIVE
            ret = tcc_run(s, argc, argv);
#����
        } �� {
            �� (!s->outfile)
                s->outfile = default_outputfile(s, first_file);
            �� (tcc_output_file(s, s->outfile))
                ret = 1;
            �� �� (s->gen_deps)
                gen_makedeps(s, s->outfile, s->deps_outfile);
        }
    }

    �� (s->do_bench && (n | t | ret) == 0)
        tcc_print_stats(s, getclock_ms() - start_time);
    tcc_delete(s);
    �� (ret == 0 && n)
        ��ת redo; /* compile more files with -c */
    �� (t)
        ��ת redo; /* run more tests with -dt -run */
    �� (ppfp && ppfp != stdout)
        fclose(ppfp);
    ���� ret;
}
