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

/********************************************************/
/* global variables */

/* use GNU C extensions */
ST_DATA �� gnu_ext = 1;

/* use TinyCC extensions */
ST_DATA �� tcc_ext = 1;

/* XXX: get rid of this ASAP */
ST_DATA �ṹ TCCState *tcc_state;

��̬ �� nb_states;

/********************************************************/

#�� ONE_SOURCE
#���� "tccpp.c"
#���� "tccgen.c"
#���� "tccelf.c"
#���� "tccrun.c"
#�綨�� TCC_TARGET_I386
#���� "i386-gen.c"
#���� "i386-link.c"
#���� "i386-asm.c"
#����
#�綨�� TCC_TARGET_ARM
#���� "arm-gen.c"
#���� "arm-link.c"
#���� "arm-asm.c"
#����
#�綨�� TCC_TARGET_ARM64
#���� "arm64-gen.c"
#���� "arm64-link.c"
#����
#�綨�� TCC_TARGET_C67
#���� "c67-gen.c"
#���� "c67-link.c"
#���� "tcccoff.c"
#����
#�綨�� TCC_TARGET_X86_64
#���� "x86_64-gen.c"
#���� "x86_64-link.c"
#���� "i386-asm.c"
#����
#�綨�� CONFIG_TCC_ASM
#���� "tccasm.c"
#����
#�綨�� TCC_TARGET_PE
#���� "tccpe.c"
#����
#���� /* ONE_SOURCE */

/********************************************************/
#��δ���� CONFIG_TCC_ASM
ST_FUNC �� asm_instr(��)
{
    tcc_error("inline asm() not supported");
}
ST_FUNC �� asm_global_instr(��)
{
    tcc_error("inline asm() not supported");
}
#����

/********************************************************/
#�綨�� _WIN32
ST_FUNC �� *normalize_slashes(�� *path)
{
    �� *p;
    ���� (p = path; *p; ++p)
        �� (*p == '\\')
            *p = '/';
    ���� path;
}

��̬ HMODULE tcc_module;

/* on win32, we suppose the lib and includes are at the location of 'tcc.exe' */
��̬ �� tcc_set_lib_path_w32(TCCState *s)
{
    �� path[1024], *p;
    GetModuleFileNameA(tcc_module, path, �󳤶� path);
    p = tcc_basename(normalize_slashes(strlwr(path)));
    �� (p - 5 > path && 0 == strncmp(p - 5, "/bin/", 5))
        p -= 5;
    �� �� (p > path)
        p--;
    *p = 0;
    tcc_set_lib_path(s, path);
}

#�綨�� TCC_TARGET_PE
��̬ �� tcc_add_systemdir(TCCState *s)
{
    �� buf[1000];
    GetSystemDirectory(buf, �󳤶� buf);
    tcc_add_library_path(s, normalize_slashes(buf));
}
#����

#�綨�� LIBTCC_AS_DLL
BOOL WINAPI DllMain (HINSTANCE hDll, DWORD dwReason, LPVOID lpReserved)
{
    �� (DLL_PROCESS_ATTACH == dwReason)
        tcc_module = hDll;
    ���� TRUE;
}
#����
#����

/********************************************************/
/* copy a string and truncate it. */
ST_FUNC �� *pstrcpy(�� *buf, �� buf_size, ���� �� *s)
{
    �� *q, *q_end;
    �� c;

    �� (buf_size > 0) {
        q = buf;
        q_end = buf + buf_size - 1;
        �� (q < q_end) {
            c = *s++;
            �� (c == '\0')
                ����;
            *q++ = c;
        }
        *q = '\0';
    }
    ���� buf;
}

/* strcat and truncate. */
ST_FUNC �� *pstrcat(�� *buf, �� buf_size, ���� �� *s)
{
    �� len;
    len = strlen(buf);
    �� (len < buf_size)
        pstrcpy(buf + len, buf_size - len, s);
    ���� buf;
}

ST_FUNC �� *pstrncpy(�� *out, ���� �� *in, size_t num)
{
    memcpy(out, in, num);
    out[num] = '\0';
    ���� out;
}

/* extract the basename of a file */
PUB_FUNC �� *tcc_basename(���� �� *name)
{
    �� *p = strchr(name, 0);
    �� (p > name && !IS_DIRSEP(p[-1]))
        --p;
    ���� p;
}

/* extract extension part of a file
 *
 * (if no extension, return pointer to end-of-string)
 */
PUB_FUNC �� *tcc_fileextension (���� �� *name)
{
    �� *b = tcc_basename(name);
    �� *e = strrchr(b, '.');
    ���� e ? e : strchr(b, 0);
}

/********************************************************/
/* memory management */

#������ free
#������ malloc
#������ realloc

#��δ���� MEM_DEBUG

PUB_FUNC �� tcc_free(�� *ptr)
{
    free(ptr);
}

PUB_FUNC �� *tcc_malloc(�޷� �� size)
{
    �� *ptr;
    ptr = malloc(size);
    �� (!ptr && size)
        tcc_error("memory full (malloc)");
    ���� ptr;
}

PUB_FUNC �� *tcc_mallocz(�޷� �� size)
{
    �� *ptr;
    ptr = tcc_malloc(size);
    memset(ptr, 0, size);
    ���� ptr;
}

PUB_FUNC �� *tcc_realloc(�� *ptr, �޷� �� size)
{
    �� *ptr1;
    ptr1 = realloc(ptr, size);
    �� (!ptr1 && size)
        tcc_error("memory full (realloc)");
    ���� ptr1;
}

PUB_FUNC �� *tcc_strdup(���� �� *str)
{
    �� *ptr;
    ptr = tcc_malloc(strlen(str) + 1);
    strcpy(ptr, str);
    ���� ptr;
}

PUB_FUNC �� tcc_memcheck(��)
{
}

#��

#���� MEM_DEBUG_MAGIC1 0xFEEDDEB1
#���� MEM_DEBUG_MAGIC2 0xFEEDDEB2
#���� MEM_DEBUG_MAGIC3 0xFEEDDEB3
#���� MEM_DEBUG_FILE_LEN 40
#���� MEM_DEBUG_CHECK3(header) \
    ((mem_debug_header_t*)((��*)header + header->size))->magic3
#���� MEM_USER_PTR(header) \
    ((�� *)header + offsetof(mem_debug_header_t, magic3))
#���� MEM_HEADER_PTR(ptr) \
    (mem_debug_header_t *)((��*)ptr - offsetof(mem_debug_header_t, magic3))

�ṹ mem_debug_header {
    �޷� magic1;
    �޷� size;
    �ṹ mem_debug_header *prev;
    �ṹ mem_debug_header *next;
    �� line_num;
    �� file_name[MEM_DEBUG_FILE_LEN + 1];
    �޷� magic2;
    ALIGNED(16) �޷� magic3;
};

���Ͷ��� �ṹ mem_debug_header mem_debug_header_t;

��̬ mem_debug_header_t *mem_debug_chain;
��̬ �޷� mem_cur_size;
��̬ �޷� mem_max_size;

��̬ mem_debug_header_t *malloc_check(�� *ptr, ���� �� *msg)
{
    mem_debug_header_t * header = MEM_HEADER_PTR(ptr);
    �� (header->magic1 != MEM_DEBUG_MAGIC1 ||
        header->magic2 != MEM_DEBUG_MAGIC2 ||
        MEM_DEBUG_CHECK3(header) != MEM_DEBUG_MAGIC3 ||
        header->size == (�޷�)-1) {
        fprintf(stderr, "%s check failed\n", msg);
        �� (header->magic1 == MEM_DEBUG_MAGIC1)
            fprintf(stderr, "%s:%u: block allocated here.\n",
                header->file_name, header->line_num);
        exit(1);
    }
    ���� header;
}

PUB_FUNC �� *tcc_malloc_debug(�޷� �� size, ���� �� *file, �� line)
{
    �� ofs;
    mem_debug_header_t *header;

    header = malloc(�󳤶�(mem_debug_header_t) + size);
    �� (!header)
        tcc_error("memory full (malloc)");

    header->magic1 = MEM_DEBUG_MAGIC1;
    header->magic2 = MEM_DEBUG_MAGIC2;
    header->size = size;
    MEM_DEBUG_CHECK3(header) = MEM_DEBUG_MAGIC3;
    header->line_num = line;
    ofs = strlen(file) - MEM_DEBUG_FILE_LEN;
    strncpy(header->file_name, file + (ofs > 0 ? ofs : 0), MEM_DEBUG_FILE_LEN);
    header->file_name[MEM_DEBUG_FILE_LEN] = 0;

    header->next = mem_debug_chain;
    header->prev = NULL;
    �� (header->next)
        header->next->prev = header;
    mem_debug_chain = header;

    mem_cur_size += size;
    �� (mem_cur_size > mem_max_size)
        mem_max_size = mem_cur_size;

    ���� MEM_USER_PTR(header);
}

PUB_FUNC �� tcc_free_debug(�� *ptr)
{
    mem_debug_header_t *header;
    �� (!ptr)
        ����;
    header = malloc_check(ptr, "tcc_free");
    mem_cur_size -= header->size;
    header->size = (�޷�)-1;
    �� (header->next)
        header->next->prev = header->prev;
    �� (header->prev)
        header->prev->next = header->next;
    �� (header == mem_debug_chain)
        mem_debug_chain = header->next;
    free(header);
}

PUB_FUNC �� *tcc_mallocz_debug(�޷� �� size, ���� �� *file, �� line)
{
    �� *ptr;
    ptr = tcc_malloc_debug(size,file,line);
    memset(ptr, 0, size);
    ���� ptr;
}

PUB_FUNC �� *tcc_realloc_debug(�� *ptr, �޷� �� size, ���� �� *file, �� line)
{
    mem_debug_header_t *header;
    �� mem_debug_chain_update = 0;
    �� (!ptr)
        ���� tcc_malloc_debug(size, file, line);
    header = malloc_check(ptr, "tcc_realloc");
    mem_cur_size -= header->size;
    mem_debug_chain_update = (header == mem_debug_chain);
    header = realloc(header, �󳤶�(mem_debug_header_t) + size);
    �� (!header)
        tcc_error("memory full (realloc)");
    header->size = size;
    MEM_DEBUG_CHECK3(header) = MEM_DEBUG_MAGIC3;
    �� (header->next)
        header->next->prev = header;
    �� (header->prev)
        header->prev->next = header;
    �� (mem_debug_chain_update)
        mem_debug_chain = header;
    mem_cur_size += size;
    �� (mem_cur_size > mem_max_size)
        mem_max_size = mem_cur_size;
    ���� MEM_USER_PTR(header);
}

PUB_FUNC �� *tcc_strdup_debug(���� �� *str, ���� �� *file, �� line)
{
    �� *ptr;
    ptr = tcc_malloc_debug(strlen(str) + 1, file, line);
    strcpy(ptr, str);
    ���� ptr;
}

PUB_FUNC �� tcc_memcheck(��)
{
    �� (mem_cur_size) {
        mem_debug_header_t *header = mem_debug_chain;
        fprintf(stderr, "MEM_DEBUG: mem_leak= %d bytes, mem_max_size= %d bytes\n",
            mem_cur_size, mem_max_size);
        �� (header) {
            fprintf(stderr, "%s:%u: error: %u bytes leaked\n",
                header->file_name, header->line_num, header->size);
            header = header->next;
        }
#�� MEM_DEBUG-0 == 2
        exit(2);
#����
    }
}
#���� /* MEM_DEBUG */

#���� free(p) use_tcc_free(p)
#���� malloc(s) use_tcc_malloc(s)
#���� realloc(p, s) use_tcc_realloc(p, s)

/********************************************************/
/* dynarrays */

ST_FUNC �� dynarray_add(�� *ptab, �� *nb_ptr, �� *data)
{
    �� nb, nb_alloc;
    �� **pp;

    nb = *nb_ptr;
    pp = *(�� ***)ptab;
    /* every power of two we double array size */
    �� ((nb & (nb - 1)) == 0) {
        �� (!nb)
            nb_alloc = 1;
        ��
            nb_alloc = nb * 2;
        pp = tcc_realloc(pp, nb_alloc * �󳤶�(�� *));
        *(��***)ptab = pp;
    }
    pp[nb++] = data;
    *nb_ptr = nb;
}

ST_FUNC �� dynarray_reset(�� *pp, �� *n)
{
    �� **p;
    ���� (p = *(��***)pp; *n; ++p, --*n)
        �� (*p)
            tcc_free(*p);
    tcc_free(*(��**)pp);
    *(��**)pp = NULL;
}

��̬ �� tcc_split_path(TCCState *s, �� *p_ary, �� *p_nb_ary, ���� �� *in)
{
    ���� �� *p;
    ���� {
        �� c;
        CString str;

        cstr_new(&str);
        ���� (p = in; c = *p, c != '\0' && c != PATHSEP; ++p) {
            �� (c == '{' && p[1] && p[2] == '}') {
                c = p[1], p += 2;
                �� (c == 'B')
                    cstr_cat(&str, s->tcc_lib_path, -1);
            } �� {
                cstr_ccat(&str, c);
            }
        }
        �� (str.size) {
            cstr_ccat(&str, '\0');
            dynarray_add(p_ary, p_nb_ary, tcc_strdup(str.data));
        }
        cstr_free(&str);
        in = p+1;
    } �� (*p);
}

/********************************************************/

��̬ �� strcat_vprintf(�� *buf, �� buf_size, ���� �� *fmt, va_list ap)
{
    �� len;
    len = strlen(buf);
    vsnprintf(buf + len, buf_size - len, fmt, ap);
}

��̬ �� strcat_printf(�� *buf, �� buf_size, ���� �� *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    strcat_vprintf(buf, buf_size, fmt, ap);
    va_end(ap);
}

��̬ �� error1(TCCState *s1, �� is_warning, ���� �� *fmt, va_list ap)
{
    �� buf[2048];
    BufferedFile **pf, *f;

    buf[0] = '\0';
    /* use upper file if inline ":asm:" or token ":paste:" */
    ���� (f = file; f && f->filename[0] == ':'; f = f->prev)
     ;
    �� (f) {
        ����(pf = s1->include_stack; pf < s1->include_stack_ptr; pf++)
            strcat_printf(buf, �󳤶�(buf), "In file included from %s:%d:\n",
                (*pf)->filename, (*pf)->line_num);
        �� (f->line_num > 0) {
            strcat_printf(buf, �󳤶�(buf), "%s:%d: ",
                f->filename, f->line_num - !!(tok_flags & TOK_FLAG_BOL));
        } �� {
            strcat_printf(buf, �󳤶�(buf), "%s: ",
                f->filename);
        }
    } �� {
        strcat_printf(buf, �󳤶�(buf), "tcc: ");
    }
    �� (is_warning)
        strcat_printf(buf, �󳤶�(buf), "warning: ");
    ��
        strcat_printf(buf, �󳤶�(buf), "error: ");
    strcat_vprintf(buf, �󳤶�(buf), fmt, ap);

    �� (!s1->error_func) {
        /* default case: stderr */
        �� (s1->output_type == TCC_OUTPUT_PREPROCESS && s1->ppfp == stdout)
            /* print a newline during tcc -E */
            printf("\n"), fflush(stdout);
        fflush(stdout); /* flush -v output */
        fprintf(stderr, "%s\n", buf);
        fflush(stderr); /* print error/warning now (win32) */
    } �� {
        s1->error_func(s1->error_opaque, buf);
    }
    �� (!is_warning || s1->warn_error)
        s1->nb_errors++;
}

LIBTCCAPI �� tcc_set_error_func(TCCState *s, �� *error_opaque,
                        �� (*error_func)(�� *opaque, ���� �� *msg))
{
    s->error_opaque = error_opaque;
    s->error_func = error_func;
}

/* error without aborting current compilation */
PUB_FUNC �� tcc_error_noabort(���� �� *fmt, ...)
{
    TCCState *s1 = tcc_state;
    va_list ap;

    va_start(ap, fmt);
    error1(s1, 0, fmt, ap);
    va_end(ap);
}

PUB_FUNC �� tcc_error(���� �� *fmt, ...)
{
    TCCState *s1 = tcc_state;
    va_list ap;

    va_start(ap, fmt);
    error1(s1, 0, fmt, ap);
    va_end(ap);
    /* better than nothing: in some cases, we accept to handle errors */
    �� (s1->error_set_jmp_enabled) {
        longjmp(s1->error_jmp_buf, 1);
    } �� {
        /* XXX: eliminate this someday */
        exit(1);
    }
}

PUB_FUNC �� tcc_warning(���� �� *fmt, ...)
{
    TCCState *s1 = tcc_state;
    va_list ap;

    �� (s1->warn_none)
        ����;

    va_start(ap, fmt);
    error1(s1, 1, fmt, ap);
    va_end(ap);
}

/********************************************************/
/* I/O layer */

ST_FUNC �� tcc_open_bf(TCCState *s1, ���� �� *filename, �� initlen)
{
    BufferedFile *bf;
    �� buflen = initlen ? initlen : IO_BUF_SIZE;

    bf = tcc_mallocz(�󳤶�(BufferedFile) + buflen);
    bf->buf_ptr = bf->buffer;
    bf->buf_end = bf->buffer + initlen;
    bf->buf_end[0] = CH_EOB; /* put eob symbol */
    pstrcpy(bf->filename, �󳤶�(bf->filename), filename);
    bf->true_filename = bf->filename;
    bf->line_num = 1;
    bf->ifdef_stack_ptr = s1->ifdef_stack_ptr;
    bf->fd = -1;
    bf->prev = file;
    file = bf;
    tok_flags = TOK_FLAG_BOL | TOK_FLAG_BOF;
}

ST_FUNC �� tcc_close(��)
{
    BufferedFile *bf = file;
    �� (bf->fd > 0) {
        close(bf->fd);
        total_lines += bf->line_num;
    }
    �� (bf->true_filename != bf->filename)
        tcc_free(bf->true_filename);
    file = bf->prev;
    tcc_free(bf);
}

ST_FUNC �� tcc_open(TCCState *s1, ���� �� *filename)
{
    �� fd;
    �� (strcmp(filename, "-") == 0)
        fd = 0, filename = "<stdin>";
    ��
        fd = open(filename, O_RDONLY | O_BINARY);
    �� ((s1->verbose == 2 && fd >= 0) || s1->verbose == 3)
        printf("%s %*s%s\n", fd < 0 ? "nf":"->",
               (��)(s1->include_stack_ptr - s1->include_stack), "", filename);
    �� (fd < 0)
        ���� -1;
    tcc_open_bf(s1, filename, 0);
#�綨�� _WIN32
    normalize_slashes(file->filename);
#����
    file->fd = fd;
    ���� fd;
}

/* compile the file opened in 'file'. Return non zero if errors. */
��̬ �� tcc_compile(TCCState *s1)
{
    Sym *define_start;
    �� filetype, is_asm;

    define_start = define_stack;
    filetype = s1->filetype;
    is_asm = filetype == AFF_TYPE_ASM || filetype == AFF_TYPE_ASMPP;

    �� (setjmp(s1->error_jmp_buf) == 0) {
        s1->nb_errors = 0;
        s1->error_set_jmp_enabled = 1;

        preprocess_start(s1, is_asm);
        �� (s1->output_type == TCC_OUTPUT_PREPROCESS) {
            tcc_preprocess(s1);
        } �� �� (is_asm) {
#�綨�� CONFIG_TCC_ASM
            tcc_assemble(s1, filetype == AFF_TYPE_ASMPP);
#��
            tcc_error_noabort("asm not supported");
#����
        } �� {
            tccgen_compile(s1);
        }
    }
    s1->error_set_jmp_enabled = 0;

    preprocess_end(s1);
    free_inline_functions(s1);
    /* reset define stack, but keep -D and built-ins */
    free_defines(define_start);
    sym_pop(&global_stack, NULL, 0);
    sym_pop(&local_stack, NULL, 0);
    ���� s1->nb_errors != 0 ? -1 : 0;
}

LIBTCCAPI �� tcc_compile_string(TCCState *s, ���� �� *str)
{
    �� len, ret;

    len = strlen(str);
    tcc_open_bf(s, "<string>", len);
    memcpy(file->buffer, str, len);
    ret = tcc_compile(s);
    tcc_close();
    ���� ret;
}

/* define a preprocessor symbol. A value can also be provided with the '=' operator */
LIBTCCAPI �� tcc_define_symbol(TCCState *s1, ���� �� *sym, ���� �� *value)
{
    �� len1, len2;
    /* default value */
    �� (!value)
        value = "1";
    len1 = strlen(sym);
    len2 = strlen(value);

    /* init file structure */
    tcc_open_bf(s1, "<define>", len1 + len2 + 1);
    memcpy(file->buffer, sym, len1);
    file->buffer[len1] = ' ';
    memcpy(file->buffer + len1 + 1, value, len2);

    /* parse with define parser */
    next_nomacro();
    parse_define();
    tcc_close();
}

/* undefine a preprocessor symbol */
LIBTCCAPI �� tcc_undefine_symbol(TCCState *s1, ���� �� *sym)
{
    TokenSym *ts;
    Sym *s;
    ts = tok_alloc(sym, strlen(sym));
    s = define_find(ts->tok);
    /* undefine symbol by putting an invalid name */
    �� (s)
        define_undef(s);
}

/* cleanup all static data used during compilation */
��̬ �� tcc_cleanup(��)
{
    �� (NULL == tcc_state)
        ����;
    �� (file)
        tcc_close();
    tccpp_delete(tcc_state);
    tcc_state = NULL;
    /* free sym_pools */
    dynarray_reset(&sym_pools, &nb_sym_pools);
    /* reset symbol stack */
    sym_free_first = NULL;
}

LIBTCCAPI TCCState *tcc_new(��)
{
    TCCState *s;

    tcc_cleanup();

    s = tcc_mallocz(�󳤶�(TCCState));
    �� (!s)
        ���� NULL;
    tcc_state = s;
    ++nb_states;

    s->alacarte_link = 1;
    s->nocommon = 1;
    s->warn_implicit_function_declaration = 1;
    s->ms_extensions = 1;

#�綨�� CHAR_IS_UNSIGNED
    s->char_is_unsigned = 1;
#����
#�綨�� TCC_TARGET_I386
    s->seg_size = 32;
#����
    /* enable this if you want symbols with leading underscore on windows: */
#�� 0 /* def TCC_TARGET_PE */
    s->leading_underscore = 1;
#����
#�綨�� _WIN32
    tcc_set_lib_path_w32(s);
#��
    tcc_set_lib_path(s, CONFIG_TCCDIR);
#����
    tccelf_new(s);
    tccpp_new(s);

    /* we add dummy defines for some special macros to speed up tests
       and to have working defined() */
    define_push(TOK___LINE__, MACRO_OBJ, NULL, NULL);
    define_push(TOK___FILE__, MACRO_OBJ, NULL, NULL);
    define_push(TOK___DATE__, MACRO_OBJ, NULL, NULL);
    define_push(TOK___TIME__, MACRO_OBJ, NULL, NULL);
    define_push(TOK___COUNTER__, MACRO_OBJ, NULL, NULL);
    {
        /* define __TINYC__ 92X  */
        �� buffer[32]; �� a,b,c;
        sscanf(TCC_VERSION, "%d.%d.%d", &a, &b, &c);
        sprintf(buffer, "%d", a*10000 + b*100 + c);
        tcc_define_symbol(s, "__TINYC__", buffer);
    }

    /* standard defines */
    tcc_define_symbol(s, "__STDC__", NULL);
    tcc_define_symbol(s, "__STDC_VERSION__", "199901L");
    tcc_define_symbol(s, "__STDC_HOSTED__", NULL);

    /* target defines */
#�� �Ѷ���(TCC_TARGET_I386)
    tcc_define_symbol(s, "__i386__", NULL);
    tcc_define_symbol(s, "__i386", NULL);
    tcc_define_symbol(s, "i386", NULL);
#���� �Ѷ���(TCC_TARGET_X86_64)
    tcc_define_symbol(s, "__x86_64__", NULL);
#���� �Ѷ���(TCC_TARGET_ARM)
    tcc_define_symbol(s, "__ARM_ARCH_4__", NULL);
    tcc_define_symbol(s, "__arm_elf__", NULL);
    tcc_define_symbol(s, "__arm_elf", NULL);
    tcc_define_symbol(s, "arm_elf", NULL);
    tcc_define_symbol(s, "__arm__", NULL);
    tcc_define_symbol(s, "__arm", NULL);
    tcc_define_symbol(s, "arm", NULL);
    tcc_define_symbol(s, "__APCS_32__", NULL);
    tcc_define_symbol(s, "__ARMEL__", NULL);
#�� �Ѷ���(TCC_ARM_EABI)
    tcc_define_symbol(s, "__ARM_EABI__", NULL);
#����
#�� �Ѷ���(TCC_ARM_HARDFLOAT)
    s->float_abi = ARM_HARD_FLOAT;
    tcc_define_symbol(s, "__ARM_PCS_VFP", NULL);
#��
    s->float_abi = ARM_SOFTFP_FLOAT;
#����
#���� �Ѷ���(TCC_TARGET_ARM64)
    tcc_define_symbol(s, "__aarch64__", NULL);
#����

#�綨�� TCC_TARGET_PE
    tcc_define_symbol(s, "_WIN32", NULL);
# �綨�� TCC_TARGET_X86_64
    tcc_define_symbol(s, "_WIN64", NULL);
# ����
#��
    tcc_define_symbol(s, "__unix__", NULL);
    tcc_define_symbol(s, "__unix", NULL);
    tcc_define_symbol(s, "unix", NULL);
# �� �Ѷ���(__linux__)
    tcc_define_symbol(s, "__linux__", NULL);
    tcc_define_symbol(s, "__linux", NULL);
# ����
# �� �Ѷ���(__FreeBSD__)
    tcc_define_symbol(s, "__FreeBSD__", "__FreeBSD__");
    /* No 'Thread Storage Local' on FreeBSD with tcc */
    tcc_define_symbol(s, "__NO_TLS", NULL);
# ����
# �� �Ѷ���(__FreeBSD_kernel__)
    tcc_define_symbol(s, "__FreeBSD_kernel__", NULL);
# ����
#����
# �� �Ѷ���(__NetBSD__)
    tcc_define_symbol(s, "__NetBSD__", "__NetBSD__");
# ����
# �� �Ѷ���(__OpenBSD__)
    tcc_define_symbol(s, "__OpenBSD__", "__OpenBSD__");
# ����

    /* TinyCC & gcc defines */
#�� �Ѷ���(TCC_TARGET_PE) && PTR_SIZE == 8
    /* 64bit Windows. */
    tcc_define_symbol(s, "__SIZE_TYPE__", "unsigned long long");
    tcc_define_symbol(s, "__PTRDIFF_TYPE__", "long long");
    tcc_define_symbol(s, "__LLP64__", NULL);
#���� PTR_SIZE == 8
    /* Other 64bit systems. */
    tcc_define_symbol(s, "__SIZE_TYPE__", "unsigned long");
    tcc_define_symbol(s, "__PTRDIFF_TYPE__", "long");
    tcc_define_symbol(s, "__LP64__", NULL);
#��
    /* Other 32bit systems. */
    tcc_define_symbol(s, "__SIZE_TYPE__", "unsigned int");
    tcc_define_symbol(s, "__PTRDIFF_TYPE__", "int");
    tcc_define_symbol(s, "__ILP32__", NULL);
#����

#�� �Ѷ���(TCC_MUSL)
    tcc_define_symbol(s, "__builtin_va_list", "void *");
#���� /* TCC_MUSL */

#�綨�� TCC_TARGET_PE
    tcc_define_symbol(s, "__WCHAR_TYPE__", "unsigned short");
    tcc_define_symbol(s, "__WINT_TYPE__", "unsigned short");
#��
    tcc_define_symbol(s, "__WCHAR_TYPE__", "int");
    /* wint_t is unsigned int by default, but (signed) int on BSDs
       and unsigned short on windows.  Other OSes might have still
       other conventions, sigh.  */
# �� �Ѷ���(__FreeBSD__) || �Ѷ��� (__FreeBSD_kernel__) \
  || �Ѷ���(__NetBSD__) || �Ѷ���(__OpenBSD__)
    tcc_define_symbol(s, "__WINT_TYPE__", "int");
#  �綨�� __FreeBSD__
    /* define __GNUC__ to have some useful stuff from sys/cdefs.h
       that are unconditionally used in FreeBSDs other system headers :/ */
    tcc_define_symbol(s, "__GNUC__", "2");
    tcc_define_symbol(s, "__GNUC_MINOR__", "7");
    tcc_define_symbol(s, "__builtin_alloca", "alloca");
#  ����
# ��
    tcc_define_symbol(s, "__WINT_TYPE__", "unsigned int");
    /* glibc defines */
    tcc_define_symbol(s, "__REDIRECT(name, proto, alias)",
        "name proto __asm__ (#alias)");
    tcc_define_symbol(s, "__REDIRECT_NTH(name, proto, alias)",
        "name proto __asm__ (#alias) __THROW");
# ����
    /* Some GCC builtins that are simple to express as macros.  */
    tcc_define_symbol(s, "__builtin_extract_return_addr(x)", "x");
#���� /* ndef TCC_TARGET_PE */
    ���� s;
}

LIBTCCAPI �� tcc_delete(TCCState *s1)
{
    tcc_cleanup();

    /* free sections */
    tccelf_delete(s1);

    /* free library paths */
    dynarray_reset(&s1->library_paths, &s1->nb_library_paths);
    dynarray_reset(&s1->crt_paths, &s1->nb_crt_paths);

    /* free include paths */
    dynarray_reset(&s1->cached_includes, &s1->nb_cached_includes);
    dynarray_reset(&s1->include_paths, &s1->nb_include_paths);
    dynarray_reset(&s1->sysinclude_paths, &s1->nb_sysinclude_paths);
    dynarray_reset(&s1->cmd_include_files, &s1->nb_cmd_include_files);

    tcc_free(s1->tcc_lib_path);
    tcc_free(s1->soname);
    tcc_free(s1->rpath);
    tcc_free(s1->init_symbol);
    tcc_free(s1->fini_symbol);
    tcc_free(s1->outfile);
    tcc_free(s1->deps_outfile);
    dynarray_reset(&s1->files, &s1->nb_files);
    dynarray_reset(&s1->target_deps, &s1->nb_target_deps);
    dynarray_reset(&s1->pragma_libs, &s1->nb_pragma_libs);
    dynarray_reset(&s1->argv, &s1->argc);

#�綨�� TCC_IS_NATIVE
    /* free runtime memory */
    tcc_run_free(s1);
#����

    tcc_free(s1);
    �� (0 == --nb_states)
        tcc_memcheck();
}

LIBTCCAPI �� tcc_set_output_type(TCCState *s, �� output_type)
{
    s->output_type = output_type;

    /* always elf for objects */
    �� (output_type == TCC_OUTPUT_OBJ)
        s->output_format = TCC_OUTPUT_FORMAT_ELF;

    �� (s->char_is_unsigned)
        tcc_define_symbol(s, "__CHAR_UNSIGNED__", NULL);

    �� (!s->nostdinc) {
        /* default include paths */
        /* -isystem paths have already been handled */
        tcc_add_sysinclude_path(s, CONFIG_TCC_SYSINCLUDEPATHS);
    }

#�綨�� CONFIG_TCC_BCHECK
    �� (s->do_bounds_check) {
        /* if bound checking, then add corresponding sections */
        tccelf_bounds_new(s);
        /* define symbol */
        tcc_define_symbol(s, "__BOUNDS_CHECKING_ON", NULL);
    }
#����
    �� (s->do_debug) {
        /* add debug sections */
        tccelf_stab_new(s);
    }

    tcc_add_library_path(s, CONFIG_TCC_LIBPATHS);

#�綨�� TCC_TARGET_PE
# �綨�� _WIN32
    �� (!s->nostdlib && output_type != TCC_OUTPUT_OBJ)
        tcc_add_systemdir(s);
# ����
#��
    /* paths for crt objects */
    tcc_split_path(s, &s->crt_paths, &s->nb_crt_paths, CONFIG_TCC_CRTPREFIX);
    /* add libc crt1/crti objects */
    �� ((output_type == TCC_OUTPUT_EXE || output_type == TCC_OUTPUT_DLL) &&
        !s->nostdlib) {
        �� (output_type != TCC_OUTPUT_DLL)
            tcc_add_crt(s, "crt1.o");
        tcc_add_crt(s, "crti.o");
    }
#����
    ���� 0;
}

LIBTCCAPI �� tcc_add_include_path(TCCState *s, ���� �� *pathname)
{
    tcc_split_path(s, &s->include_paths, &s->nb_include_paths, pathname);
    ���� 0;
}

LIBTCCAPI �� tcc_add_sysinclude_path(TCCState *s, ���� �� *pathname)
{
    tcc_split_path(s, &s->sysinclude_paths, &s->nb_sysinclude_paths, pathname);
    ���� 0;
}

ST_FUNC �� tcc_add_file_internal(TCCState *s1, ���� �� *filename, �� flags)
{
    �� ret;

    /* open the file */
    ret = tcc_open(s1, filename);
    �� (ret < 0) {
        �� (flags & AFF_PRINT_ERROR)
            tcc_error_noabort("file '%s' not found", filename);
        ���� ret;
    }

    /* update target deps */
    dynarray_add(&s1->target_deps, &s1->nb_target_deps,
            tcc_strdup(filename));

    �� (flags & AFF_TYPE_BIN) {
        ElfW(Ehdr) ehdr;
        �� fd, obj_type;

        fd = file->fd;
        obj_type = tcc_object_type(fd, &ehdr);
        lseek(fd, 0, SEEK_SET);

        /* do not display line number if error */
        file->line_num = 0;

#�綨�� TCC_TARGET_MACHO
        �� (0 == obj_type && 0 == strcmp(tcc_fileextension(filename), "dylib"))
            obj_type = AFF_BINTYPE_DYN;
#����

        ת�� (obj_type) {
        ���� AFF_BINTYPE_REL:
            ret = tcc_load_object_file(s1, fd, 0);
            ����;
#��δ���� TCC_TARGET_PE
        ���� AFF_BINTYPE_DYN:
            �� (s1->output_type == TCC_OUTPUT_MEMORY) {
                ret = 0;
#�綨�� TCC_IS_NATIVE
                �� (NULL == dlopen(filename, RTLD_GLOBAL | RTLD_LAZY))
                    ret = -1;
#����
            } �� {
                ret = tcc_load_dll(s1, fd, filename,
                                   (flags & AFF_REFERENCED_DLL) != 0);
            }
            ����;
#����
        ���� AFF_BINTYPE_AR:
            ret = tcc_load_archive(s1, fd);
            ����;
#�綨�� TCC_TARGET_COFF
        ���� AFF_BINTYPE_C67:
            ret = tcc_load_coff(s1, fd);
            ����;
#����
        ȱʡ:
#�綨�� TCC_TARGET_PE
            ret = pe_load_file(s1, filename, fd);
#��
            /* as GNU ld, consider it is an ld script if not recognized */
            ret = tcc_load_ldscript(s1);
#����
            �� (ret < 0)
                tcc_error_noabort("unrecognized file type");
            ����;
        }
    } �� {
        ret = tcc_compile(s1);
    }
    tcc_close();
    ���� ret;
}

LIBTCCAPI �� tcc_add_file(TCCState *s, ���� �� *filename)
{
    �� filetype = s->filetype;
    �� flags = AFF_PRINT_ERROR;
    �� (filetype == 0) {
        /* use a file extension to detect a filetype */
        ���� �� *ext = tcc_fileextension(filename);
        �� (ext[0]) {
            ext++;
            �� (!strcmp(ext, "S"))
                filetype = AFF_TYPE_ASMPP;
            �� �� (!strcmp(ext, "s"))
                filetype = AFF_TYPE_ASM;
            �� �� (!PATHCMP(ext, "c") || !PATHCMP(ext, "i"))
                filetype = AFF_TYPE_C;
            ��
                flags |= AFF_TYPE_BIN;
        } �� {
            filetype = AFF_TYPE_C;
        }
        s->filetype = filetype;
    }
    ���� tcc_add_file_internal(s, filename, flags);
}

LIBTCCAPI �� tcc_add_library_path(TCCState *s, ���� �� *pathname)
{
    tcc_split_path(s, &s->library_paths, &s->nb_library_paths, pathname);
    ���� 0;
}

��̬ �� tcc_add_library_internal(TCCState *s, ���� �� *fmt,
    ���� �� *filename, �� flags, �� **paths, �� nb_paths)
{
    �� buf[1024];
    �� i;

    ����(i = 0; i < nb_paths; i++) {
        snprintf(buf, �󳤶�(buf), fmt, paths[i], filename);
        �� (tcc_add_file_internal(s, buf, flags | AFF_TYPE_BIN) == 0)
            ���� 0;
    }
    ���� -1;
}

/* find and load a dll. Return non zero if not found */
/* XXX: add '-rpath' option support ? */
ST_FUNC �� tcc_add_dll(TCCState *s, ���� �� *filename, �� flags)
{
    ���� tcc_add_library_internal(s, "%s/%s", filename, flags,
        s->library_paths, s->nb_library_paths);
}

ST_FUNC �� tcc_add_crt(TCCState *s, ���� �� *filename)
{
    �� (-1 == tcc_add_library_internal(s, "%s/%s",
        filename, 0, s->crt_paths, s->nb_crt_paths))
        tcc_error_noabort("file '%s' not found", filename);
    ���� 0;
}

/* the library name is the same as the argument of the '-l' option */
LIBTCCAPI �� tcc_add_library(TCCState *s, ���� �� *libraryname)
{
#�� �Ѷ��� TCC_TARGET_PE
    ���� �� *libs[] = { "%s/%s.def", "%s/lib%s.def", "%s/%s.dll", "%s/lib%s.dll", "%s/lib%s.a", NULL };
    ���� �� **pp = s->static_link ? libs + 4 : libs;
#���� �Ѷ��� TCC_TARGET_MACHO
    ���� �� *libs[] = { "%s/lib%s.dylib", "%s/lib%s.a", NULL };
    ���� �� **pp = s->static_link ? libs + 1 : libs;
#��
    ���� �� *libs[] = { "%s/lib%s.so", "%s/lib%s.a", NULL };
    ���� �� **pp = s->static_link ? libs + 1 : libs;
#����
    �� (*pp) {
        �� (0 == tcc_add_library_internal(s, *pp,
            libraryname, 0, s->library_paths, s->nb_library_paths))
            ���� 0;
        ++pp;
    }
    ���� -1;
}

PUB_FUNC �� tcc_add_library_err(TCCState *s, ���� �� *libname)
{
    �� ret = tcc_add_library(s, libname);
    �� (ret < 0)
        tcc_error_noabort("library '%s' not found", libname);
    ���� ret;
}

/* handle #pragma comment(lib,) */
ST_FUNC �� tcc_add_pragma_libs(TCCState *s1)
{
    �� i;
    ���� (i = 0; i < s1->nb_pragma_libs; i++)
        tcc_add_library_err(s1, s1->pragma_libs[i]);
}

LIBTCCAPI �� tcc_add_symbol(TCCState *s, ���� �� *name, ���� �� *val)
{
#�綨�� TCC_TARGET_PE
    /* On x86_64 'val' might not be reachable with a 32bit offset.
       So it is handled here as if it were in a DLL. */
    pe_putimport(s, 0, name, (uintptr_t)val);
#��
    set_elf_sym(symtab_section, (uintptr_t)val, 0,
        ELFW(ST_INFO)(STB_GLOBAL, STT_NOTYPE), 0,
        SHN_ABS, name);
#����
    ���� 0;
}

LIBTCCAPI �� tcc_set_lib_path(TCCState *s, ���� �� *path)
{
    tcc_free(s->tcc_lib_path);
    s->tcc_lib_path = tcc_strdup(path);
}

#���� WD_ALL    0x0001 /* warning is activated when using -Wall */
#���� FD_INVERT 0x0002 /* invert value before storing */

���Ͷ��� �ṹ FlagDef {
    uint16_t offset;
    uint16_t flags;
    ���� �� *name;
} FlagDef;

��̬ �� no_flag(���� �� **pp)
{
    ���� �� *p = *pp;
    �� (*p != 'n' || *++p != 'o' || *++p != '-')
        ���� 0;
    *pp = p + 1;
    ���� 1;
}

ST_FUNC �� set_flag(TCCState *s, ���� FlagDef *flags, ���� �� *name)
{
    �� value, ret;
    ���� FlagDef *p;
    ���� �� *r;

    value = 1;
    r = name;
    �� (no_flag(&r))
        value = 0;

    ���� (ret = -1, p = flags; p->name; ++p) {
        �� (ret) {
            �� (strcmp(r, p->name))
                ����;
        } �� {
            �� (0 == (p->flags & WD_ALL))
                ����;
        }
        �� (p->offset) {
            *(��*)((�� *)s + p->offset) =
                p->flags & FD_INVERT ? !value : value;
            �� (ret)
                ���� 0;
        } �� {
            ret = 0;
        }
    }
    ���� ret;
}

��̬ �� strstart(���� �� *val, ���� �� **str)
{
    ���� �� *p, *q;
    p = *str;
    q = val;
    �� (*q) {
        �� (*p != *q)
            ���� 0;
        p++;
        q++;
    }
    *str = p;
    ���� 1;
}

/* Like strstart, but automatically takes into account that ld options can
 *
 * - start with double or single dash (e.g. '--soname' or '-soname')
 * - arguments can be given as separate or after '=' (e.g. '-Wl,-soname,x.so'
 *   or '-Wl,-soname=x.so')
 *
 * you provide `val` always in 'option[=]' form (no leading -)
 */
��̬ �� link_option(���� �� *str, ���� �� *val, ���� �� **ptr)
{
    ���� �� *p, *q;
    �� ret;

    /* there should be 1 or 2 dashes */
    �� (*str++ != '-')
        ���� 0;
    �� (*str == '-')
        str++;

    /* then str & val should match (potentially up to '=') */
    p = str;
    q = val;

    ret = 1;
    �� (q[0] == '?') {
        ++q;
        �� (no_flag(&p))
            ret = -1;
    }

    �� (*q != '\0' && *q != '=') {
        �� (*p != *q)
            ���� 0;
        p++;
        q++;
    }

    /* '=' near eos means ',' or '=' is ok */
    �� (*q == '=') {
        �� (*p == 0)
            *ptr = p;
        �� (*p != ',' && *p != '=')
            ���� 0;
        p++;
    } �� �� (*p) {
        ���� 0;
    }
    *ptr = p;
    ���� ret;
}

��̬ ���� �� *skip_linker_arg(���� �� **str)
{
    ���� �� *s1 = *str;
    ���� �� *s2 = strchr(s1, ',');
    *str = s2 ? s2++ : (s2 = s1 + strlen(s1));
    ���� s2;
}

��̬ �� copy_linker_arg(�� **pp, ���� �� *s, �� sep)
{
    ���� �� *q = s;
    �� *p = *pp;
    �� l = 0;
    �� (p && sep)
        p[l = strlen(p)] = sep, ++l;
    skip_linker_arg(&q);
    pstrncpy(l + (*pp = tcc_realloc(p, q - s + l + 1)), s, q - s);
}

/* set linker options */
��̬ �� tcc_set_linker(TCCState *s, ���� �� *option)
{
    �� (*option) {

        ���� �� *p = NULL;
        �� *end = NULL;
        �� ignoring = 0;
        �� ret;

        �� (link_option(option, "Bsymbolic", &p)) {
            s->symbolic = 1;
        } �� �� (link_option(option, "nostdlib", &p)) {
            s->nostdlib = 1;
        } �� �� (link_option(option, "fini=", &p)) {
            copy_linker_arg(&s->fini_symbol, p, 0);
            ignoring = 1;
        } �� �� (link_option(option, "image-base=", &p)
                || link_option(option, "Ttext=", &p)) {
            s->text_addr = strtoull(p, &end, 16);
            s->has_text_addr = 1;
        } �� �� (link_option(option, "init=", &p)) {
            copy_linker_arg(&s->init_symbol, p, 0);
            ignoring = 1;
        } �� �� (link_option(option, "oformat=", &p)) {
#�� �Ѷ���(TCC_TARGET_PE)
            �� (strstart("pe-", &p)) {
#���� PTR_SIZE == 8
            �� (strstart("elf64-", &p)) {
#��
            �� (strstart("elf32-", &p)) {
#����
                s->output_format = TCC_OUTPUT_FORMAT_ELF;
            } �� �� (!strcmp(p, "binary")) {
                s->output_format = TCC_OUTPUT_FORMAT_BINARY;
#�綨�� TCC_TARGET_COFF
            } �� �� (!strcmp(p, "coff")) {
                s->output_format = TCC_OUTPUT_FORMAT_COFF;
#����
            } ��
                ��ת err;

        } �� �� (link_option(option, "as-needed", &p)) {
            ignoring = 1;
        } �� �� (link_option(option, "O", &p)) {
            ignoring = 1;
        } �� �� (link_option(option, "export-all-symbols", &p)) {
            s->rdynamic = 1;
        } �� �� (link_option(option, "rpath=", &p)) {
            copy_linker_arg(&s->rpath, p, ':');
        } �� �� (link_option(option, "enable-new-dtags", &p)) {
            s->enable_new_dtags = 1;
        } �� �� (link_option(option, "section-alignment=", &p)) {
            s->section_align = strtoul(p, &end, 16);
        } �� �� (link_option(option, "soname=", &p)) {
            copy_linker_arg(&s->soname, p, 0);
#�綨�� TCC_TARGET_PE
        } �� �� (link_option(option, "large-address-aware", &p)) {
            s->pe_characteristics |= 0x20;
        } �� �� (link_option(option, "file-alignment=", &p)) {
            s->pe_file_align = strtoul(p, &end, 16);
        } �� �� (link_option(option, "stack=", &p)) {
            s->pe_stack_size = strtoul(p, &end, 10);
        } �� �� (link_option(option, "subsystem=", &p)) {
#�� �Ѷ���(TCC_TARGET_I386) || �Ѷ���(TCC_TARGET_X86_64)
            �� (!strcmp(p, "native")) {
                s->pe_subsystem = 1;
            } �� �� (!strcmp(p, "console")) {
                s->pe_subsystem = 3;
            } �� �� (!strcmp(p, "gui") || !strcmp(p, "windows")) {
                s->pe_subsystem = 2;
            } �� �� (!strcmp(p, "posix")) {
                s->pe_subsystem = 7;
            } �� �� (!strcmp(p, "efiapp")) {
                s->pe_subsystem = 10;
            } �� �� (!strcmp(p, "efiboot")) {
                s->pe_subsystem = 11;
            } �� �� (!strcmp(p, "efiruntime")) {
                s->pe_subsystem = 12;
            } �� �� (!strcmp(p, "efirom")) {
                s->pe_subsystem = 13;
#���� �Ѷ���(TCC_TARGET_ARM)
            �� (!strcmp(p, "wince")) {
                s->pe_subsystem = 9;
#����
            } ��
                ��ת err;
#����
        } �� �� (ret = link_option(option, "?whole-archive", &p), ret) {
            s->alacarte_link = ret < 0;
        } �� �� (p) {
            ���� 0;
        } �� {
    err:
            tcc_error("unsupported linker option '%s'", option);
        }

        �� (ignoring && s->warn_unsupported)
            tcc_warning("unsupported linker option '%s'", option);

        option = skip_linker_arg(&p);
    }
    ���� 1;
}

���Ͷ��� �ṹ TCCOption {
    ���� �� *name;
    uint16_t index;
    uint16_t flags;
} TCCOption;

ö�� {
    TCC_OPTION_HELP,
    TCC_OPTION_HELP2,
    TCC_OPTION_v,
    TCC_OPTION_I,
    TCC_OPTION_D,
    TCC_OPTION_U,
    TCC_OPTION_P,
    TCC_OPTION_L,
    TCC_OPTION_B,
    TCC_OPTION_l,
    TCC_OPTION_bench,
    TCC_OPTION_bt,
    TCC_OPTION_b,
    TCC_OPTION_g,
    TCC_OPTION_c,
    TCC_OPTION_dumpversion,
    TCC_OPTION_d,
    TCC_OPTION_static,
    TCC_OPTION_std,
    TCC_OPTION_shared,
    TCC_OPTION_soname,
    TCC_OPTION_o,
    TCC_OPTION_r,
    TCC_OPTION_s,
    TCC_OPTION_traditional,
    TCC_OPTION_Wl,
    TCC_OPTION_Wp,
    TCC_OPTION_W,
    TCC_OPTION_O,
    TCC_OPTION_mfloat_abi,
    TCC_OPTION_m,
    TCC_OPTION_f,
    TCC_OPTION_isystem,
    TCC_OPTION_iwithprefix,
    TCC_OPTION_include,
    TCC_OPTION_nostdinc,
    TCC_OPTION_nostdlib,
    TCC_OPTION_print_search_dirs,
    TCC_OPTION_rdynamic,
    TCC_OPTION_param,
    TCC_OPTION_pedantic,
    TCC_OPTION_pthread,
    TCC_OPTION_run,
    TCC_OPTION_w,
    TCC_OPTION_pipe,
    TCC_OPTION_E,
    TCC_OPTION_MD,
    TCC_OPTION_MF,
    TCC_OPTION_x,
    TCC_OPTION_ar,
    TCC_OPTION_impdef
};

#���� TCC_OPTION_HAS_ARG 0x0001
#���� TCC_OPTION_NOSEP   0x0002 /* cannot have space before option and arg */

��̬ ���� TCCOption tcc_options[] = {
    { "h", TCC_OPTION_HELP, 0 },
    { "-help", TCC_OPTION_HELP, 0 },
    { "?", TCC_OPTION_HELP, 0 },
    { "hh", TCC_OPTION_HELP2, 0 },
    { "v", TCC_OPTION_v, TCC_OPTION_HAS_ARG | TCC_OPTION_NOSEP },
    { "I", TCC_OPTION_I, TCC_OPTION_HAS_ARG },
    { "D", TCC_OPTION_D, TCC_OPTION_HAS_ARG },
    { "U", TCC_OPTION_U, TCC_OPTION_HAS_ARG },
    { "P", TCC_OPTION_P, TCC_OPTION_HAS_ARG | TCC_OPTION_NOSEP },
    { "L", TCC_OPTION_L, TCC_OPTION_HAS_ARG },
    { "B", TCC_OPTION_B, TCC_OPTION_HAS_ARG },
    { "l", TCC_OPTION_l, TCC_OPTION_HAS_ARG | TCC_OPTION_NOSEP },
    { "bench", TCC_OPTION_bench, 0 },
#�綨�� CONFIG_TCC_BACKTRACE
    { "bt", TCC_OPTION_bt, TCC_OPTION_HAS_ARG },
#����
#�綨�� CONFIG_TCC_BCHECK
    { "b", TCC_OPTION_b, 0 },
#����
    { "g", TCC_OPTION_g, TCC_OPTION_HAS_ARG | TCC_OPTION_NOSEP },
    { "c", TCC_OPTION_c, 0 },
    { "dumpversion", TCC_OPTION_dumpversion, 0},
    { "d", TCC_OPTION_d, TCC_OPTION_HAS_ARG | TCC_OPTION_NOSEP },
    { "static", TCC_OPTION_static, 0 },
    { "std", TCC_OPTION_std, TCC_OPTION_HAS_ARG | TCC_OPTION_NOSEP },
    { "shared", TCC_OPTION_shared, 0 },
    { "soname", TCC_OPTION_soname, TCC_OPTION_HAS_ARG },
    { "o", TCC_OPTION_o, TCC_OPTION_HAS_ARG },
    { "-param", TCC_OPTION_param, TCC_OPTION_HAS_ARG },
    { "pedantic", TCC_OPTION_pedantic, 0},
    { "pthread", TCC_OPTION_pthread, 0},
    { "run", TCC_OPTION_run, TCC_OPTION_HAS_ARG | TCC_OPTION_NOSEP },
    { "rdynamic", TCC_OPTION_rdynamic, 0 },
    { "r", TCC_OPTION_r, 0 },
    { "s", TCC_OPTION_s, 0 },
    { "traditional", TCC_OPTION_traditional, 0 },
    { "Wl,", TCC_OPTION_Wl, TCC_OPTION_HAS_ARG | TCC_OPTION_NOSEP },
    { "Wp,", TCC_OPTION_Wp, TCC_OPTION_HAS_ARG | TCC_OPTION_NOSEP },
    { "W", TCC_OPTION_W, TCC_OPTION_HAS_ARG | TCC_OPTION_NOSEP },
    { "O", TCC_OPTION_O, TCC_OPTION_HAS_ARG | TCC_OPTION_NOSEP },
#�綨�� TCC_TARGET_ARM
    { "mfloat-abi", TCC_OPTION_mfloat_abi, TCC_OPTION_HAS_ARG },
#����
    { "m", TCC_OPTION_m, TCC_OPTION_HAS_ARG | TCC_OPTION_NOSEP },
    { "f", TCC_OPTION_f, TCC_OPTION_HAS_ARG | TCC_OPTION_NOSEP },
    { "isystem", TCC_OPTION_isystem, TCC_OPTION_HAS_ARG },
    { "iwithprefix", TCC_OPTION_iwithprefix, TCC_OPTION_HAS_ARG },
    { "include", TCC_OPTION_include, TCC_OPTION_HAS_ARG },
    { "nostdinc", TCC_OPTION_nostdinc, 0 },
    { "nostdlib", TCC_OPTION_nostdlib, 0 },
    { "print-search-dirs", TCC_OPTION_print_search_dirs, 0 },
    { "w", TCC_OPTION_w, 0 },
    { "pipe", TCC_OPTION_pipe, 0},
    { "E", TCC_OPTION_E, 0},
    { "MD", TCC_OPTION_MD, 0},
    { "MF", TCC_OPTION_MF, TCC_OPTION_HAS_ARG },
    { "x", TCC_OPTION_x, TCC_OPTION_HAS_ARG },
    { "ar", TCC_OPTION_ar, 0},
#�綨�� TCC_TARGET_PE
    { "impdef", TCC_OPTION_impdef, 0},
#����
    { NULL, 0, 0 },
};

��̬ ���� FlagDef options_W[] = {
    { 0, 0, "all" },
    { offsetof(TCCState, warn_unsupported), 0, "unsupported" },
    { offsetof(TCCState, warn_write_strings), 0, "write-strings" },
    { offsetof(TCCState, warn_error), 0, "error" },
    { offsetof(TCCState, warn_gcc_compat), 0, "gcc-compat" },
    { offsetof(TCCState, warn_implicit_function_declaration), WD_ALL,
      "implicit-function-declaration" },
    { 0, 0, NULL }
};

��̬ ���� FlagDef options_f[] = {
    { offsetof(TCCState, char_is_unsigned), 0, "unsigned-char" },
    { offsetof(TCCState, char_is_unsigned), FD_INVERT, "signed-char" },
    { offsetof(TCCState, nocommon), FD_INVERT, "common" },
    { offsetof(TCCState, leading_underscore), 0, "leading-underscore" },
    { offsetof(TCCState, ms_extensions), 0, "ms-extensions" },
    { offsetof(TCCState, dollars_in_identifiers), 0, "dollars-in-identifiers" },
    { 0, 0, NULL }
};

��̬ ���� FlagDef options_m[] = {
    { offsetof(TCCState, ms_bitfields), 0, "ms-bitfields" },
#�綨�� TCC_TARGET_X86_64
    { offsetof(TCCState, nosse), FD_INVERT, "sse" },
#����
    { 0, 0, NULL }
};

��̬ �� parse_option_D(TCCState *s1, ���� �� *optarg)
{
    �� *sym = tcc_strdup(optarg);
    �� *value = strchr(sym, '=');
    �� (value)
        *value++ = '\0';
    tcc_define_symbol(s1, sym, value);
    tcc_free(sym);
}

��̬ �� args_parser_add_file(TCCState *s, ���� ��* filename, �� filetype)
{
    �ṹ filespec *f = tcc_malloc(�󳤶� *f + strlen(filename));
    f->type = filetype;
    f->alacarte = s->alacarte_link;
    strcpy(f->name, filename);
    dynarray_add(&s->files, &s->nb_files, f);
}

��̬ �� args_parser_make_argv(���� �� *r, �� *argc, �� ***argv)
{
    �� ret = 0, q, c;
    CString str;
    ����(;;) {
        �� (c = (�޷� ��)*r, c && c <= ' ')
            ++r;
        �� (c == 0)
            ����;
        q = 0;
        cstr_new(&str);
        �� (c = (�޷� ��)*r, c) {
            ++r;
            �� (c == '\\' && (*r == '"' || *r == '\\')) {
                c = *r++;
            } �� �� (c == '"') {
                q = !q;
                ����;
            } �� �� (q == 0 && c <= ' ') {
                ����;
            }
            cstr_ccat(&str, c);
        }
        cstr_ccat(&str, 0);
        //printf("<%s>\n", str.data), fflush(stdout);
        dynarray_add(argv, argc, tcc_strdup(str.data));
        cstr_free(&str);
        ++ret;
    }
    ���� ret;
}

/* read list file */
��̬ �� args_parser_listfile(TCCState *s,
    ���� �� *filename, �� optind, �� *pargc, �� ***pargv)
{
    �� fd, i;
    size_t len;
    �� *p;
    �� argc = 0;
    �� **argv = NULL;

    fd = open(filename, O_RDONLY | O_BINARY);
    �� (fd < 0)
        tcc_error("listfile '%s' not found", filename);

    len = lseek(fd, 0, SEEK_END);
    p = tcc_malloc(len + 1), p[len] = 0;
    lseek(fd, 0, SEEK_SET), read(fd, p, len), close(fd);

    ���� (i = 0; i < *pargc; ++i)
        �� (i == optind)
            args_parser_make_argv(p, &argc, &argv);
        ��
            dynarray_add(&argv, &argc, tcc_strdup((*pargv)[i]));

    tcc_free(p);
    dynarray_reset(&s->argv, &s->argc);
    *pargc = s->argc = argc, *pargv = s->argv = argv;
}

PUB_FUNC �� tcc_parse_args(TCCState *s, �� *pargc, �� ***pargv, �� optind)
{
    ���� TCCOption *popt;
    ���� �� *optarg, *r;
    ���� �� *run = NULL;
    �� last_o = -1;
    �� x;
    CString linker_arg; /* collect -Wl options */
    �� buf[1024];
    �� tool = 0, arg_start = 0, noaction = optind;
    �� **argv = *pargv;
    �� argc = *pargc;

    cstr_new(&linker_arg);

    �� (optind < argc) {
        r = argv[optind];
        �� (r[0] == '@' && r[1] != '\0') {
            args_parser_listfile(s, r + 1, optind, &argc, &argv);
            ����;
        }
        optind++;
        �� (tool) {
            �� (r[0] == '-' && r[1] == 'v' && r[2] == 0)
                ++s->verbose;
            ����;
        }
reparse:
        �� (r[0] != '-' || r[1] == '\0') {
            �� (r[0] != '@') /* allow "tcc file(s) -run @ args ..." */
                args_parser_add_file(s, r, s->filetype);
            �� (run) {
                tcc_set_options(s, run);
                arg_start = optind - 1;
                ����;
            }
            ����;
        }

        /* find option in table */
        ����(popt = tcc_options; ; ++popt) {
            ���� �� *p1 = popt->name;
            ���� �� *r1 = r + 1;
            �� (p1 == NULL)
                tcc_error("invalid option -- '%s'", r);
            �� (!strstart(p1, &r1))
                ����;
            optarg = r1;
            �� (popt->flags & TCC_OPTION_HAS_ARG) {
                �� (*r1 == '\0' && !(popt->flags & TCC_OPTION_NOSEP)) {
                    �� (optind >= argc)
                arg_err:
                        tcc_error("argument to '%s' is missing", r);
                    optarg = argv[optind++];
                }
            } �� �� (*r1 != '\0')
                ����;
            ����;
        }

        ת��(popt->index) {
        ���� TCC_OPTION_HELP:
            ���� OPT_HELP;
        ���� TCC_OPTION_HELP2:
            ���� OPT_HELP2;
        ���� TCC_OPTION_I:
            tcc_add_include_path(s, optarg);
            ����;
        ���� TCC_OPTION_D:
            parse_option_D(s, optarg);
            ����;
        ���� TCC_OPTION_U:
            tcc_undefine_symbol(s, optarg);
            ����;
        ���� TCC_OPTION_L:
            tcc_add_library_path(s, optarg);
            ����;
        ���� TCC_OPTION_B:
            /* set tcc utilities path (mainly for tcc development) */
            tcc_set_lib_path(s, optarg);
            ����;
        ���� TCC_OPTION_l:
            args_parser_add_file(s, optarg, AFF_TYPE_LIB);
            s->nb_libraries++;
            ����;
        ���� TCC_OPTION_pthread:
            parse_option_D(s, "_REENTRANT");
            s->option_pthread = 1;
            ����;
        ���� TCC_OPTION_bench:
            s->do_bench = 1;
            ����;
#�綨�� CONFIG_TCC_BACKTRACE
        ���� TCC_OPTION_bt:
            tcc_set_num_callers(atoi(optarg));
            ����;
#����
#�綨�� CONFIG_TCC_BCHECK
        ���� TCC_OPTION_b:
            s->do_bounds_check = 1;
            s->do_debug = 1;
            ����;
#����
        ���� TCC_OPTION_g:
            s->do_debug = 1;
            ����;
        ���� TCC_OPTION_c:
            x = TCC_OUTPUT_OBJ;
        set_output_type:
            �� (s->output_type)
                tcc_warning("-%s: overriding compiler action already specified", popt->name);
            s->output_type = x;
            ����;
        ���� TCC_OPTION_d:
            �� (*optarg == 'D')
                s->dflag = 3;
            �� �� (*optarg == 'M')
                s->dflag = 7;
            �� �� (*optarg == 't')
                s->dflag = 16;
            �� �� (isnum(*optarg))
                g_debug = atoi(optarg);
            ��
                ��ת unsupported_option;
            ����;
        ���� TCC_OPTION_static:
            s->static_link = 1;
            ����;
        ���� TCC_OPTION_std:
            /* silently ignore, a current purpose:
               allow to use a tcc as a reference compiler for "make test" */
            ����;
        ���� TCC_OPTION_shared:
            x = TCC_OUTPUT_DLL;
            ��ת set_output_type;
        ���� TCC_OPTION_soname:
            s->soname = tcc_strdup(optarg);
            ����;
        ���� TCC_OPTION_o:
            �� (s->outfile) {
                tcc_warning("multiple -o option");
                tcc_free(s->outfile);
            }
            s->outfile = tcc_strdup(optarg);
            ����;
        ���� TCC_OPTION_r:
            /* generate a .o merging several output files */
            s->option_r = 1;
            x = TCC_OUTPUT_OBJ;
            ��ת set_output_type;
        ���� TCC_OPTION_isystem:
            tcc_add_sysinclude_path(s, optarg);
            ����;
        ���� TCC_OPTION_iwithprefix:
            snprintf(buf, �󳤶� buf, "{B}/%s", optarg);
            tcc_add_sysinclude_path(s, buf);
            ����;
        ���� TCC_OPTION_include:
            dynarray_add(&s->cmd_include_files,
                         &s->nb_cmd_include_files, tcc_strdup(optarg));
            ����;
        ���� TCC_OPTION_nostdinc:
            s->nostdinc = 1;
            ����;
        ���� TCC_OPTION_nostdlib:
            s->nostdlib = 1;
            ����;
        ���� TCC_OPTION_run:
#��δ���� TCC_IS_NATIVE
            tcc_error("-run is not available in a cross compiler");
#����
            run = optarg;
            x = TCC_OUTPUT_MEMORY;
            ��ת set_output_type;
        ���� TCC_OPTION_v:
            ���� ++s->verbose; �� (*optarg++ == 'v');
            ++noaction;
            ����;
        ���� TCC_OPTION_f:
            �� (set_flag(s, options_f, optarg) < 0)
                ��ת unsupported_option;
            ����;
#�綨�� TCC_TARGET_ARM
        ���� TCC_OPTION_mfloat_abi:
            /* tcc doesn't support soft float yet */
            �� (!strcmp(optarg, "softfp")) {
                s->float_abi = ARM_SOFTFP_FLOAT;
                tcc_undefine_symbol(s, "__ARM_PCS_VFP");
            } �� �� (!strcmp(optarg, "hard"))
                s->float_abi = ARM_HARD_FLOAT;
            ��
                tcc_error("unsupported float abi '%s'", optarg);
            ����;
#����
        ���� TCC_OPTION_m:
            �� (set_flag(s, options_m, optarg) < 0) {
                �� (x = atoi(optarg), x != 32 && x != 64)
                    ��ת unsupported_option;
                �� (PTR_SIZE != x/8)
                    ���� x;
                ++noaction;
            }
            ����;
        ���� TCC_OPTION_W:
            �� (set_flag(s, options_W, optarg) < 0)
                ��ת unsupported_option;
            ����;
        ���� TCC_OPTION_w:
            s->warn_none = 1;
            ����;
        ���� TCC_OPTION_rdynamic:
            s->rdynamic = 1;
            ����;
        ���� TCC_OPTION_Wl:
            �� (linker_arg.size)
                --linker_arg.size, cstr_ccat(&linker_arg, ',');
            cstr_cat(&linker_arg, optarg, 0);
            �� (tcc_set_linker(s, linker_arg.data))
                cstr_free(&linker_arg);
            ����;
        ���� TCC_OPTION_Wp:
            r = optarg;
            ��ת reparse;
        ���� TCC_OPTION_E:
            x = TCC_OUTPUT_PREPROCESS;
            ��ת set_output_type;
        ���� TCC_OPTION_P:
            s->Pflag = atoi(optarg) + 1;
            ����;
        ���� TCC_OPTION_MD:
            s->gen_deps = 1;
            ����;
        ���� TCC_OPTION_MF:
            s->deps_outfile = tcc_strdup(optarg);
            ����;
        ���� TCC_OPTION_dumpversion:
            printf ("%s\n", TCC_VERSION);
            exit(0);
            ����;
        ���� TCC_OPTION_x:
            �� (*optarg == 'c')
                s->filetype = AFF_TYPE_C;
            �� �� (*optarg == 'a')
                s->filetype = AFF_TYPE_ASMPP;
            �� �� (*optarg == 'n')
                s->filetype = AFF_TYPE_NONE;
            ��
                tcc_warning("unsupported language '%s'", optarg);
            ����;
        ���� TCC_OPTION_O:
            last_o = atoi(optarg);
            ����;
        ���� TCC_OPTION_print_search_dirs:
            x = OPT_PRINT_DIRS;
            ��ת extra_action;
        ���� TCC_OPTION_impdef:
            x = OPT_IMPDEF;
            ��ת extra_action;
        ���� TCC_OPTION_ar:
            x = OPT_AR;
        extra_action:
            arg_start = optind - 1;
            �� (arg_start != noaction)
                tcc_error("cannot parse %s here", r);
            tool = x;
            ����;
        ���� TCC_OPTION_traditional:
        ���� TCC_OPTION_pedantic:
        ���� TCC_OPTION_pipe:
        ���� TCC_OPTION_s:
            /* ignored */
            ����;
        ȱʡ:
unsupported_option:
            �� (s->warn_unsupported)
                tcc_warning("unsupported option '%s'", r);
            ����;
        }
    }
    �� (last_o > 0)
        tcc_define_symbol(s, "__OPTIMIZE__", NULL);
    �� (linker_arg.size) {
        r = linker_arg.data;
        ��ת arg_err;
    }
    *pargc = argc - arg_start;
    *pargv = argv + arg_start;
    �� (tool)
        ���� tool;
    �� (optind != noaction)
        ���� 0;
    �� (s->verbose == 2)
        ���� OPT_PRINT_DIRS;
    �� (s->verbose)
        ���� OPT_V;
    ���� OPT_HELP;
}

LIBTCCAPI �� tcc_set_options(TCCState *s, ���� �� *r)
{
    �� **argv = NULL;
    �� argc = 0;
    args_parser_make_argv(r, &argc, &argv);
    tcc_parse_args(s, &argc, &argv, 0);
    dynarray_reset(&argv, &argc);
}

PUB_FUNC �� tcc_print_stats(TCCState *s, �޷� total_time)
{
    �� (total_time < 1)
        total_time = 1;
    �� (total_bytes < 1)
        total_bytes = 1;
    fprintf(stderr, "* %d idents, %d lines, %d bytes\n"
                    "* %0.3f s, %u lines/s, %0.1f MB/s\n",
           tok_ident - TOK_IDENT, total_lines, total_bytes,
           (˫��)total_time/1000,
           (�޷�)total_lines*1000/total_time,
           (˫��)total_bytes/1000/total_time);
#�綨�� MEM_DEBUG
    fprintf(stderr, "* %d bytes memory used\n", mem_max_size);
#����
}
