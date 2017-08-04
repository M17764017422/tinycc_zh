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

ST_DATA �� tok_flags;
ST_DATA �� parse_flags;

ST_DATA �ṹ BufferedFile *file;
ST_DATA �� ch, tok;
ST_DATA CValue tokc;
ST_DATA ���� �� *macro_ptr;
ST_DATA CString tokcstr; /* current parsed string, if any */

/* display benchmark infos */
ST_DATA �� total_lines;
ST_DATA �� total_bytes;
ST_DATA �� tok_ident;
ST_DATA TokenSym **table_ident;

/* ------------------------------------------------------------------------- */

��̬ TokenSym *hash_ident[TOK_HASH_SIZE];
��̬ �� token_buf[STRING_MAX_SIZE + 1];
��̬ CString cstr_buf;
��̬ CString macro_equal_buf;
��̬ TokenString tokstr_buf;
��̬ �޷� �� isidnum_table[256 - CH_EOF];
��̬ �� pp_debug_tok, pp_debug_symv;
��̬ �� pp_once;
��̬ �� pp_expr;
��̬ �� pp_counter;
��̬ �� tok_print(���� �� *msg, ���� �� *str);

��̬ �ṹ TinyAlloc *toksym_alloc;
��̬ �ṹ TinyAlloc *tokstr_alloc;
��̬ �ṹ TinyAlloc *cstr_alloc;

��̬ TokenString *macro_stack;

��̬ ���� �� tcc_keywords[] =
#���� DEF(id, str) str "\0"
#���� "tcctok.h"
#������ DEF
;

/* WARNING: the content of this string encodes token numbers */
��̬ ���� �޷� �� tok_two_chars[] =
/* outdated -- gr
        "<=\236>=\235!=\225&&\240||\241++\244--\242==\224<<\1>>\2+=\253"
        "-=\255*=\252/=\257%=\245&=\246^=\336|=\374->\313..\250##\266";
*/{
        '<','=', TOK_LE,
        '>','=', TOK_GE,
        '!','=', TOK_NE,
        '&','&', TOK_LAND,
        '|','|', TOK_LOR,
        '+','+', TOK_INC,
        '-','-', TOK_DEC,
        '=','=', TOK_EQ,
        '<','<', TOK_SHL,
        '>','>', TOK_SAR,
        '+','=', TOK_A_ADD,
        '-','=', TOK_A_SUB,
        '*','=', TOK_A_MUL,
        '/','=', TOK_A_DIV,
        '%','=', TOK_A_MOD,
        '&','=', TOK_A_AND,
        '^','=', TOK_A_XOR,
        '|','=', TOK_A_OR,
        '-','>', TOK_ARROW,
        '.','.', 0xa8, // C++ token ?
        '#','#', TOK_TWOSHARPS,
        0
};

��̬ �� next_nomacro_spc(��);

ST_FUNC �� skip(�� c)
{
        �� (tok != c)
                tcc_error("'%c' expected (got \"%s\")", c, get_tok_str(tok, &tokc));
        next();
}

ST_FUNC �� expect(���� �� *msg)
{
        tcc_error("%s expected", msg);
}

/* ------------------------------------------------------------------------- */
/* Custom allocator for tiny objects */

#���� USE_TAL

#��δ���� USE_TAL
#���� tal_free(al, p) tcc_free(p)
#���� tal_realloc(al, p, size) tcc_realloc(p, size)
#���� tal_new(a,b,c)
#���� tal_delete(a)
#��
#�� !�Ѷ���(MEM_DEBUG)
#���� tal_free(al, p) tal_free_impl(al, p)
#���� tal_realloc(al, p, size) tal_realloc_impl(&al, p, size)
#���� TAL_DEBUG_PARAMS
#��
#���� TAL_DEBUG 1
//#���� TAL_INFO 1 /* collect and dump allocators stats */
#���� tal_free(al, p) tal_free_impl(al, p, __�ļ�__, __�к�__)
#���� tal_realloc(al, p, size) tal_realloc_impl(&al, p, size, __�ļ�__, __�к�__)
#���� TAL_DEBUG_PARAMS , ���� �� *file, �� line
#���� TAL_DEBUG_FILE_LEN 40
#����

#���� TOKSYM_TAL_SIZE     (768 * 1024) /* allocator for tiny TokenSym in table_ident */
#���� TOKSTR_TAL_SIZE     (768 * 1024) /* allocator for tiny TokenString instances */
#���� CSTR_TAL_SIZE       (256 * 1024) /* allocator for tiny CString instances */
#���� TOKSYM_TAL_LIMIT    256 /* prefer unique limits to distinguish allocators debug msgs */
#���� TOKSTR_TAL_LIMIT    128 /* 32 * sizeof(int) */
#���� CSTR_TAL_LIMIT      1024

���Ͷ��� �ṹ TinyAlloc {
        �޷�  limit;
        �޷�  size;
        uint8_t *buffer;
        uint8_t *p;
        �޷�  nb_allocs;
        �ṹ TinyAlloc *next, *top;
#�綨�� TAL_INFO
        �޷�  nb_peak;
        �޷�  nb_total;
        �޷�  nb_missed;
        uint8_t *peak_p;
#����
} TinyAlloc;

���Ͷ��� �ṹ tal_header_t {
        �޷�  size;
#�綨�� TAL_DEBUG
        ��     line_num; /* negative line_num used for double free check */
        ��    file_name[TAL_DEBUG_FILE_LEN + 1];
#����
} tal_header_t;

/* ------------------------------------------------------------------------- */

��̬ TinyAlloc *tal_new(TinyAlloc **pal, �޷� limit, �޷� size)
{
        TinyAlloc *al = tcc_mallocz(�󳤶�(TinyAlloc));
        al->p = al->buffer = tcc_malloc(size);
        al->limit = limit;
        al->size = size;
        �� (pal) *pal = al;
        ���� al;
}

��̬ �� tal_delete(TinyAlloc *al)
{
        TinyAlloc *next;

tail_call:
        �� (!al)
                ����;
#�綨�� TAL_INFO
        fprintf(stderr, "limit=%5d, size=%5g MB, nb_peak=%6d, nb_total=%8d, nb_missed=%6d, usage=%5.1f%%\n",
                        al->limit, al->size / 1024.0 / 1024.0, al->nb_peak, al->nb_total, al->nb_missed,
                        (al->peak_p - al->buffer) * 100.0 / al->size);
#����
#�綨�� TAL_DEBUG
        �� (al->nb_allocs > 0) {
                uint8_t *p;
                fprintf(stderr, "TAL_DEBUG: memory leak %d chunk(s) (limit= %d)\n",
                                al->nb_allocs, al->limit);
                p = al->buffer;
                �� (p < al->p) {
                        tal_header_t *header = (tal_header_t *)p;
                        �� (header->line_num > 0) {
                                fprintf(stderr, "%s:%d: chunk of %d bytes leaked\n",
                                                header->file_name, header->line_num, header->size);
                        }
                        p += header->size + �󳤶�(tal_header_t);
                }
#�� MEM_DEBUG-0 == 2
                exit(2);
#����
        }
#����
        next = al->next;
        tcc_free(al->buffer);
        tcc_free(al);
        al = next;
        ��ת tail_call;
}

��̬ �� tal_free_impl(TinyAlloc *al, �� *p TAL_DEBUG_PARAMS)
{
        �� (!p)
                ����;
tail_call:
        �� (al->buffer <= (uint8_t *)p && (uint8_t *)p < al->buffer + al->size) {
#�綨�� TAL_DEBUG
                tal_header_t *header = (((tal_header_t *)p) - 1);
                �� (header->line_num < 0) {
                        fprintf(stderr, "%s:%d: TAL_DEBUG: double frees chunk from\n",
                                        file, line);
                        fprintf(stderr, "%s:%d: %d bytes\n",
                                        header->file_name, (��)-header->line_num, (��)header->size);
                } ��
                        header->line_num = -header->line_num;
#����
                al->nb_allocs--;
                �� (!al->nb_allocs)
                        al->p = al->buffer;
        } �� �� (al->next) {
                al = al->next;
                ��ת tail_call;
        }
        ��
                tcc_free(p);
}

��̬ �� *tal_realloc_impl(TinyAlloc **pal, �� *p, �޷� size TAL_DEBUG_PARAMS)
{
        tal_header_t *header;
        �� *ret;
        �� is_own;
        �޷� adj_size = (size + 3) & -4;
        TinyAlloc *al = *pal;

tail_call:
        is_own = (al->buffer <= (uint8_t *)p && (uint8_t *)p < al->buffer + al->size);
        �� ((!p || is_own) && size <= al->limit) {
                �� (al->p + adj_size + �󳤶�(tal_header_t) < al->buffer + al->size) {
                        header = (tal_header_t *)al->p;
                        header->size = adj_size;
#�綨�� TAL_DEBUG
                        { �� ofs = strlen(file) - TAL_DEBUG_FILE_LEN;
                        strncpy(header->file_name, file + (ofs > 0 ? ofs : 0), TAL_DEBUG_FILE_LEN);
                        header->file_name[TAL_DEBUG_FILE_LEN] = 0;
                        header->line_num = line; }
#����
                        ret = al->p + �󳤶�(tal_header_t);
                        al->p += adj_size + �󳤶�(tal_header_t);
                        �� (is_own) {
                                header = (((tal_header_t *)p) - 1);
                                memcpy(ret, p, header->size);
#�綨�� TAL_DEBUG
                                header->line_num = -header->line_num;
#����
                        } �� {
                                al->nb_allocs++;
                        }
#�綨�� TAL_INFO
                        �� (al->nb_peak < al->nb_allocs)
                                al->nb_peak = al->nb_allocs;
                        �� (al->peak_p < al->p)
                                al->peak_p = al->p;
                        al->nb_total++;
#����
                        ���� ret;
                } �� �� (is_own) {
                        al->nb_allocs--;
                        ret = tal_realloc(*pal, 0, size);
                        header = (((tal_header_t *)p) - 1);
                        memcpy(ret, p, header->size);
#�綨�� TAL_DEBUG
                        header->line_num = -header->line_num;
#����
                        ���� ret;
                }
                �� (al->next) {
                        al = al->next;
                } �� {
                        TinyAlloc *bottom = al, *next = al->top ? al->top : al;

                        al = tal_new(pal, next->limit, next->size * 2);
                        al->next = next;
                        bottom->top = al;
                }
                ��ת tail_call;
        }
        �� (is_own) {
                al->nb_allocs--;
                ret = tcc_malloc(size);
                header = (((tal_header_t *)p) - 1);
                memcpy(ret, p, header->size);
#�綨�� TAL_DEBUG
                header->line_num = -header->line_num;
#����
        } �� �� (al->next) {
                al = al->next;
                ��ת tail_call;
        } ��
                ret = tcc_realloc(p, size);
#�綨�� TAL_INFO
        al->nb_missed++;
#����
        ���� ret;
}

#���� /* USE_TAL */

/* ------------------------------------------------------------------------- */
/* CString handling */
��̬ �� cstr_realloc(CString *cstr, �� new_size)
{
        �� size;

        size = cstr->size_allocated;
        �� (size < 8)
                size = 8; /* no need to allocate a too small first string */
        �� (size < new_size)
                size = size * 2;
        cstr->data = tal_realloc(cstr_alloc, cstr->data, size);
        cstr->size_allocated = size;
}

/* add a byte */
ST_INLN �� cstr_ccat(CString *cstr, �� ch)
{
        �� size;
        size = cstr->size + 1;
        �� (size > cstr->size_allocated)
                cstr_realloc(cstr, size);
        ((�޷� �� *)cstr->data)[size - 1] = ch;
        cstr->size = size;
}

ST_FUNC �� cstr_cat(CString *cstr, ���� �� *str, �� len)
{
        �� size;
        �� (len <= 0)
                len = strlen(str) + 1 + len;
        size = cstr->size + len;
        �� (size > cstr->size_allocated)
                cstr_realloc(cstr, size);
        memmove(((�޷� �� *)cstr->data) + cstr->size, str, len);
        cstr->size = size;
}

/* add a wide char */
ST_FUNC �� cstr_wccat(CString *cstr, �� ch)
{
        �� size;
        size = cstr->size + �󳤶�(nwchar_t);
        �� (size > cstr->size_allocated)
                cstr_realloc(cstr, size);
        *(nwchar_t *)(((�޷� �� *)cstr->data) + size - �󳤶�(nwchar_t)) = ch;
        cstr->size = size;
}

ST_FUNC �� cstr_new(CString *cstr)
{
        memset(cstr, 0, �󳤶�(CString));
}

/* free string and reset it to NULL */
ST_FUNC �� cstr_free(CString *cstr)
{
        tal_free(cstr_alloc, cstr->data);
        cstr_new(cstr);
}

/* reset string to empty */
ST_FUNC �� cstr_reset(CString *cstr)
{
        cstr->size = 0;
}

/* XXX: unicode ? */
��̬ �� add_char(CString *cstr, �� c)
{
        �� (c == '\'' || c == '\"' || c == '\\') {
                /* XXX: could be more precise if char or string */
                cstr_ccat(cstr, '\\');
        }
        �� (c >= 32 && c <= 126) {
                cstr_ccat(cstr, c);
        } �� {
                cstr_ccat(cstr, '\\');
                �� (c == '\n') {
                        cstr_ccat(cstr, 'n');
                } �� {
                        cstr_ccat(cstr, '0' + ((c >> 6) & 7));
                        cstr_ccat(cstr, '0' + ((c >> 3) & 7));
                        cstr_ccat(cstr, '0' + (c & 7));
                }
        }
}

/* ------------------------------------------------------------------------- */
/* allocate a new token */
��̬ TokenSym *tok_alloc_new(TokenSym **pts, ���� �� *str, �� len)
{
        TokenSym *ts, **ptable;
        �� i;

        �� (tok_ident >= SYM_FIRST_ANOM)
                tcc_error("memory full (symbols)");

        /* expand token table if needed */
        i = tok_ident - TOK_IDENT;
        �� ((i % TOK_ALLOC_INCR) == 0) {
                ptable = tcc_realloc(table_ident, (i + TOK_ALLOC_INCR) * �󳤶�(TokenSym *));
                table_ident = ptable;
        }

        ts = tal_realloc(toksym_alloc, 0, �󳤶�(TokenSym) + len);
        table_ident[i] = ts;
        ts->tok = tok_ident++;
        ts->sym_define = NULL;
        ts->sym_label = NULL;
        ts->sym_struct = NULL;
        ts->sym_identifier = NULL;
        ts->len = len;
        ts->hash_next = NULL;
        memcpy(ts->str, str, len);
        ts->str[len] = '\0';
        *pts = ts;
        ���� ts;
}

#���� TOK_HASH_INIT 1
#���� TOK_HASH_FUNC(h, c) ((h) + ((h) << 5) + ((h) >> 27) + (c))


/* find a token and add it if not found */
ST_FUNC TokenSym *tok_alloc(���� �� *str, �� len)
{
        TokenSym *ts, **pts;
        �� i;
        �޷� �� h;

        h = TOK_HASH_INIT;
        ����(i=0;i<len;i++)
                h = TOK_HASH_FUNC(h, ((�޷� �� *)str)[i]);
        h &= (TOK_HASH_SIZE - 1);

        pts = &hash_ident[h];
        ����(;;) {
                ts = *pts;
                �� (!ts)
                        ����;
                �� (ts->len == len && !memcmp(ts->str, str, len))
                        ���� ts;
                pts = &(ts->hash_next);
        }
        ���� tok_alloc_new(pts, str, len);
}

/* XXX: buffer overflow */
/* XXX: float tokens */
ST_FUNC ���� �� *get_tok_str(�� v, CValue *cv)
{
        �� *p;
        �� i, len;

        cstr_reset(&cstr_buf);
        p = cstr_buf.data;

        ת��(v) {
        ���� TOK_CINT:
        ���� TOK_CUINT:
        ���� TOK_CLONG:
        ���� TOK_CULONG:
        ���� TOK_CLLONG:
        ���� TOK_CULLONG:
                /* XXX: not quite exact, but only useful for testing  */
#�綨�� _WIN32
                sprintf(p, "%u", (�޷�)cv->i);
#��
                sprintf(p, "%llu", (�޷� �� ��)cv->i);
#����
                ����;
        ���� TOK_LCHAR:
                cstr_ccat(&cstr_buf, 'L');
        ���� TOK_CCHAR:
                cstr_ccat(&cstr_buf, '\'');
                add_char(&cstr_buf, cv->i);
                cstr_ccat(&cstr_buf, '\'');
                cstr_ccat(&cstr_buf, '\0');
                ����;
        ���� TOK_PPNUM:
        ���� TOK_PPSTR:
                ���� (��*)cv->str.data;
        ���� TOK_LSTR:
                cstr_ccat(&cstr_buf, 'L');
        ���� TOK_STR:
                cstr_ccat(&cstr_buf, '\"');
                �� (v == TOK_STR) {
                        len = cv->str.size - 1;
                        ����(i=0;i<len;i++)
                                add_char(&cstr_buf, ((�޷� �� *)cv->str.data)[i]);
                } �� {
                        len = (cv->str.size / �󳤶�(nwchar_t)) - 1;
                        ����(i=0;i<len;i++)
                                add_char(&cstr_buf, ((nwchar_t *)cv->str.data)[i]);
                }
                cstr_ccat(&cstr_buf, '\"');
                cstr_ccat(&cstr_buf, '\0');
                ����;

        ���� TOK_CFLOAT:
                cstr_cat(&cstr_buf, "<float>", 0);
                ����;
        ���� TOK_CDOUBLE:
                cstr_cat(&cstr_buf, "<double>", 0);
                ����;
        ���� TOK_CLDOUBLE:
                cstr_cat(&cstr_buf, "<long double>", 0);
                ����;
        ���� TOK_LINENUM:
                cstr_cat(&cstr_buf, "<linenumber>", 0);
                ����;

        /* above tokens have value, the ones below don't */
        ���� TOK_LT:
                v = '<';
                ��ת addv;
        ���� TOK_GT:
                v = '>';
                ��ת addv;
        ���� TOK_DOTS:
                ���� strcpy(p, "...");
        ���� TOK_A_SHL:
                ���� strcpy(p, "<<=");
        ���� TOK_A_SAR:
                ���� strcpy(p, ">>=");
        ���� TOK_EOF:
                ���� strcpy(p, "<eof>");
        ȱʡ:
                �� (v < TOK_IDENT) {
                        /* search in two bytes table */
                        ���� �޷� �� *q = tok_two_chars;
                        �� (*q) {
                                �� (q[2] == v) {
                                        *p++ = q[0];
                                        *p++ = q[1];
                                        *p = '\0';
                                        ���� cstr_buf.data;
                                }
                                q += 3;
                        }
                �� (v >= 127) {
                        sprintf(cstr_buf.data, "<%02x>", v);
                        ���� cstr_buf.data;
                }
                addv:
                        *p++ = v;
                        *p = '\0';
                } �� �� (v < tok_ident) {
                        ���� table_ident[v - TOK_IDENT]->str;
                } �� �� (v >= SYM_FIRST_ANOM) {
                        /* special name for anonymous symbol */
                        sprintf(p, "L.%u", v - SYM_FIRST_ANOM);
                } �� {
                        /* should never happen */
                        ���� NULL;
                }
                ����;
        }
        ���� cstr_buf.data;
}

/* return the current character, handling end of block if necessary
   (but not stray) */
ST_FUNC �� handle_eob(��)
{
        BufferedFile *bf = file;
        �� len;

        /* only tries to read if really end of buffer */
        �� (bf->buf_ptr >= bf->buf_end) {
                �� (bf->fd != -1) {
#�� �Ѷ���(PARSE_DEBUG)
                        len = 1;
#��
                        len = IO_BUF_SIZE;
#����
                        len = read(bf->fd, bf->buffer, len);
                        �� (len < 0)
                                len = 0;
                } �� {
                        len = 0;
                }
                total_bytes += len;
                bf->buf_ptr = bf->buffer;
                bf->buf_end = bf->buffer + len;
                *bf->buf_end = CH_EOB;
        }
        �� (bf->buf_ptr < bf->buf_end) {
                ���� bf->buf_ptr[0];
        } �� {
                bf->buf_ptr = bf->buf_end;
                ���� CH_EOF;
        }
}

/* read next char from current input file and handle end of input buffer */
ST_INLN �� inp(��)
{
        ch = *(++(file->buf_ptr));
        /* end of buffer/file handling */
        �� (ch == CH_EOB)
                ch = handle_eob();
}

/* handle '\[\r]\n' */
��̬ �� handle_stray_noerror(��)
{
        �� (ch == '\\') {
                inp();
                �� (ch == '\n') {
                        file->line_num++;
                        inp();
                } �� �� (ch == '\r') {
                        inp();
                        �� (ch != '\n')
                                ��ת fail;
                        file->line_num++;
                        inp();
                } �� {
                fail:
                        ���� 1;
                }
        }
        ���� 0;
}

��̬ �� handle_stray(��)
{
        �� (handle_stray_noerror())
                tcc_error("stray '\\' in program");
}

/* skip the stray and handle the \\n case. Output an error if
   incorrect char after the stray */
��̬ �� handle_stray1(uint8_t *p)
{
        �� c;

        file->buf_ptr = p;
        �� (p >= file->buf_end) {
                c = handle_eob();
                �� (c != '\\')
                        ���� c;
                p = file->buf_ptr;
        }
        ch = *p;
        �� (handle_stray_noerror()) {
                �� (!(parse_flags & PARSE_FLAG_ACCEPT_STRAYS))
                        tcc_error("stray '\\' in program");
                *--file->buf_ptr = '\\';
        }
        p = file->buf_ptr;
        c = *p;
        ���� c;
}

/* handle just the EOB case, but not stray */
#���� PEEKC_EOB(c, p)\
{\
        p++;\
        c = *p;\
        �� (c == '\\') {\
                file->buf_ptr = p;\
                c = handle_eob();\
                p = file->buf_ptr;\
        }\
}

/* handle the complicated stray case */
#���� PEEKC(c, p)\
{\
        p++;\
        c = *p;\
        �� (c == '\\') {\
                c = handle_stray1(p);\
                p = file->buf_ptr;\
        }\
}

/* input with '\[\r]\n' handling. Note that this function cannot
   handle other characters after '\', so you cannot call it inside
   strings or comments */
ST_FUNC �� minp(��)
{
        inp();
        �� (ch == '\\')
                handle_stray();
}

/* single line C++ comments */
��̬ uint8_t *parse_line_comment(uint8_t *p)
{
        �� c;

        p++;
        ����(;;) {
                c = *p;
        redo:
                �� (c == '\n' || c == CH_EOF) {
                        ����;
                } �� �� (c == '\\') {
                        file->buf_ptr = p;
                        c = handle_eob();
                        p = file->buf_ptr;
                        �� (c == '\\') {
                                PEEKC_EOB(c, p);
                                �� (c == '\n') {
                                        file->line_num++;
                                        PEEKC_EOB(c, p);
                                } �� �� (c == '\r') {
                                        PEEKC_EOB(c, p);
                                        �� (c == '\n') {
                                                file->line_num++;
                                                PEEKC_EOB(c, p);
                                        }
                                }
                        } �� {
                                ��ת redo;
                        }
                } �� {
                        p++;
                }
        }
        ���� p;
}

/* C comments */
ST_FUNC uint8_t *parse_comment(uint8_t *p)
{
        �� c;

        p++;
        ����(;;) {
                /* fast skip loop */
                ����(;;) {
                        c = *p;
                        �� (c == '\n' || c == '*' || c == '\\')
                                ����;
                        p++;
                        c = *p;
                        �� (c == '\n' || c == '*' || c == '\\')
                                ����;
                        p++;
                }
                /* now we can handle all the cases */
                �� (c == '\n') {
                        file->line_num++;
                        p++;
                } �� �� (c == '*') {
                        p++;
                        ����(;;) {
                                c = *p;
                                �� (c == '*') {
                                        p++;
                                } �� �� (c == '/') {
                                        ��ת end_of_comment;
                                } �� �� (c == '\\') {
                                        file->buf_ptr = p;
                                        c = handle_eob();
                                        p = file->buf_ptr;
                                        �� (c == CH_EOF)
                                                tcc_error("unexpected end of file in comment");
                                        �� (c == '\\') {
                                                /* skip '\[\r]\n', otherwise just skip the stray */
                                                �� (c == '\\') {
                                                        PEEKC_EOB(c, p);
                                                        �� (c == '\n') {
                                                                file->line_num++;
                                                                PEEKC_EOB(c, p);
                                                        } �� �� (c == '\r') {
                                                                PEEKC_EOB(c, p);
                                                                �� (c == '\n') {
                                                                        file->line_num++;
                                                                        PEEKC_EOB(c, p);
                                                                }
                                                        } �� {
                                                                ��ת after_star;
                                                        }
                                                }
                                        }
                                } �� {
                                        ����;
                                }
                        }
                after_star: ;
                } �� {
                        /* stray, eob or eof */
                        file->buf_ptr = p;
                        c = handle_eob();
                        p = file->buf_ptr;
                        �� (c == CH_EOF) {
                                tcc_error("unexpected end of file in comment");
                        } �� �� (c == '\\') {
                                p++;
                        }
                }
        }
 end_of_comment:
        p++;
        ���� p;
}

ST_FUNC �� set_idnum(�� c, �� val)
{
        �� prev = isidnum_table[c - CH_EOF];
        isidnum_table[c - CH_EOF] = val;
        ���� prev;
}

#���� cinp minp

��̬ ���� �� skip_spaces(��)
{
        �� (isidnum_table[ch - CH_EOF] & IS_SPC)
                cinp();
}

��̬ ���� �� check_space(�� t, �� *spc)
{
        �� (t < 256 && (isidnum_table[t - CH_EOF] & IS_SPC)) {
                �� (*spc)
                        ���� 1;
                *spc = 1;
        } ��
                *spc = 0;
        ���� 0;
}

/* parse a string without interpreting escapes */
��̬ uint8_t *parse_pp_string(uint8_t *p,
                                                                �� sep, CString *str)
{
        �� c;
        p++;
        ����(;;) {
                c = *p;
                �� (c == sep) {
                        ����;
                } �� �� (c == '\\') {
                        file->buf_ptr = p;
                        c = handle_eob();
                        p = file->buf_ptr;
                        �� (c == CH_EOF) {
                        unterminated_string:
                                /* XXX: indicate line number of start of string */
                                tcc_error("missing terminating %c character", sep);
                        } �� �� (c == '\\') {
                                /* escape : just skip \[\r]\n */
                                PEEKC_EOB(c, p);
                                �� (c == '\n') {
                                        file->line_num++;
                                        p++;
                                } �� �� (c == '\r') {
                                        PEEKC_EOB(c, p);
                                        �� (c != '\n')
                                                expect("'\n' after '\r'");
                                        file->line_num++;
                                        p++;
                                } �� �� (c == CH_EOF) {
                                        ��ת unterminated_string;
                                } �� {
                                        �� (str) {
                                                cstr_ccat(str, '\\');
                                                cstr_ccat(str, c);
                                        }
                                        p++;
                                }
                        }
                } �� �� (c == '\n') {
                        file->line_num++;
                        ��ת add_char;
                } �� �� (c == '\r') {
                        PEEKC_EOB(c, p);
                        �� (c != '\n') {
                                �� (str)
                                        cstr_ccat(str, '\r');
                        } �� {
                                file->line_num++;
                                ��ת add_char;
                        }
                } �� {
                add_char:
                        �� (str)
                                cstr_ccat(str, c);
                        p++;
                }
        }
        p++;
        ���� p;
}

/* skip block of text until #else, #elif or #endif. skip also pairs of
   #if/#endif */
��̬ �� preprocess_skip(��)
{
        �� a, start_of_line, c, in_warn_or_error;
        uint8_t *p;

        p = file->buf_ptr;
        a = 0;
redo_start:
        start_of_line = 1;
        in_warn_or_error = 0;
        ����(;;) {
        redo_no_start:
                c = *p;
                ת��(c) {
                ���� ' ':
                ���� '\t':
                ���� '\f':
                ���� '\v':
                ���� '\r':
                        p++;
                        ��ת redo_no_start;
                ���� '\n':
                        file->line_num++;
                        p++;
                        ��ת redo_start;
                ���� '\\':
                        file->buf_ptr = p;
                        c = handle_eob();
                        �� (c == CH_EOF) {
                                expect("#endif");
                        } �� �� (c == '\\') {
                                ch = file->buf_ptr[0];
                                handle_stray_noerror();
                        }
                        p = file->buf_ptr;
                        ��ת redo_no_start;
                /* skip strings */
                ���� '\"':
                ���� '\'':
                        �� (in_warn_or_error)
                                ��ת _default;
                        p = parse_pp_string(p, c, NULL);
                        ����;
                /* skip comments */
                ���� '/':
                        �� (in_warn_or_error)
                                ��ת _default;
                        file->buf_ptr = p;
                        ch = *p;
                        minp();
                        p = file->buf_ptr;
                        �� (ch == '*') {
                                p = parse_comment(p);
                        } �� �� (ch == '/') {
                                p = parse_line_comment(p);
                        }
                        ����;
                ���� '#':
                        p++;
                        �� (start_of_line) {
                                file->buf_ptr = p;
                                next_nomacro();
                                p = file->buf_ptr;
                                �� (a == 0 &&
                                        (tok == TOK_ELSE || tok == TOK_ELSE_CN || tok == TOK_ELIF || tok == TOK_ELIF_CN || tok == TOK_ENDIF || tok == TOK_ENDIF_CN))
                                        ��ת the_end;
                                �� (tok == TOK_IF || tok == TOK_IF_CN || tok == TOK_IFDEF || tok == TOK_IFDEF_CN || tok == TOK_IFNDEF || tok == TOK_IFNDEF_CN)
                                        a++;
                                �� �� (tok == TOK_ENDIF || tok == TOK_ENDIF_CN)
                                        a--;
                                �� ��(tok == TOK_ERROR || tok == TOK_ERROR_CN || tok == TOK_WARNING || tok == TOK_WARNING_CN)
                                        in_warn_or_error = 1;
                                �� �� (tok == TOK_LINEFEED)
                                        ��ת redo_start;
                                �� �� (parse_flags & PARSE_FLAG_ASM_FILE)
                                        p = parse_line_comment(p - 1);
                        } �� �� (parse_flags & PARSE_FLAG_ASM_FILE)
                                p = parse_line_comment(p - 1);
                        ����;
_default:
                ȱʡ:
                        p++;
                        ����;
                }
                start_of_line = 0;
        }
 the_end: ;
        file->buf_ptr = p;
}

#�� 0
/* return the number of additional 'ints' necessary to store the
   token */
��̬ ���� �� tok_size(���� �� *p)
{
        ת��(*p) {
                /* 4 bytes */
        ���� TOK_CINT:
        ���� TOK_CUINT:
        ���� TOK_CCHAR:
        ���� TOK_LCHAR:
        ���� TOK_CFLOAT:
        ���� TOK_LINENUM:
#��δ���� TCC_LONG_ARE_64_BIT
        ���� TOK_CLONG;
        ���� TOK_CULONG;
#����
                ���� 1 + 1;
        ���� TOK_STR:
        ���� TOK_LSTR:
        ���� TOK_PPNUM:
        ���� TOK_PPSTR:
                ���� 1 + ((�󳤶�(CString) + ((CString *)(p+1))->size + 3) >> 2);
        ���� TOK_CDOUBLE:
        ���� TOK_CLLONG:
        ���� TOK_CULLONG:
#�綨�� TCC_LONG_ARE_64_BIT
        ���� TOK_CLONG;
        ���� TOK_CULONG;
#����
                ���� 1 + 2;
        ���� TOK_CLDOUBLE:
                ���� 1 + LDOUBLE_SIZE / 4;
        ȱʡ:
                ���� 1 + 0;
        }
}
#����

/* token string handling */
ST_INLN �� tok_str_new(TokenString *s)
{
        s->str = NULL;
        s->len = s->lastlen = 0;
        s->allocated_len = 0;
        s->last_line_num = -1;
}

ST_FUNC TokenString *tok_str_alloc(��)
{
        TokenString *str = tal_realloc(tokstr_alloc, 0, �󳤶� *str);
        tok_str_new(str);
        ���� str;
}

ST_FUNC �� *tok_str_dup(TokenString *s)
{
        �� *str;

        str = tal_realloc(tokstr_alloc, 0, s->len * �󳤶�(��));
        memcpy(str, s->str, s->len * �󳤶�(��));
        ���� str;
}

ST_FUNC �� tok_str_free_str(�� *str)
{
        tal_free(tokstr_alloc, str);
}

ST_FUNC �� tok_str_free(TokenString *str)
{
        tok_str_free_str(str->str);
        tal_free(tokstr_alloc, str);
}

ST_FUNC �� *tok_str_realloc(TokenString *s, �� new_size)
{
        �� *str, size;

        size = s->allocated_len;
        �� (size < 16)
                size = 16;
        �� (size < new_size)
                size = size * 2;
        �� (size > s->allocated_len) {
                str = tal_realloc(tokstr_alloc, s->str, size * �󳤶�(��));
                s->allocated_len = size;
                s->str = str;
        }
        ���� s->str;
}

ST_FUNC �� tok_str_add(TokenString *s, �� t)
{
        �� len, *str;

        len = s->len;
        str = s->str;
        �� (len >= s->allocated_len)
                str = tok_str_realloc(s, len + 1);
        str[len++] = t;
        s->len = len;
}

ST_FUNC �� begin_macro(TokenString *str, �� alloc)
{
        str->alloc = alloc;
        str->prev = macro_stack;
        str->prev_ptr = macro_ptr;
        str->save_line_num = file->line_num;
        macro_ptr = str->str;
        macro_stack = str;
}

ST_FUNC �� end_macro(��)
{
        TokenString *str = macro_stack;
        macro_stack = str->prev;
        macro_ptr = str->prev_ptr;
        file->line_num = str->save_line_num;
        �� (str->alloc == 2) {
                str->alloc = 3; /* just mark as finished */
        } �� {
                tok_str_free(str);
        }
}

��̬ �� tok_str_add2(TokenString *s, �� t, CValue *cv)
{
        �� len, *str;

        len = s->lastlen = s->len;
        str = s->str;

        /* allocate space for worst case */
        �� (len + TOK_MAX_SIZE >= s->allocated_len)
                str = tok_str_realloc(s, len + TOK_MAX_SIZE + 1);
        str[len++] = t;
        ת��(t) {
        ���� TOK_CINT:
        ���� TOK_CUINT:
        ���� TOK_CCHAR:
        ���� TOK_LCHAR:
        ���� TOK_CFLOAT:
        ���� TOK_LINENUM:
#��δ���� TCC_LONG_ARE_64_BIT
        ���� TOK_CLONG:
        ���� TOK_CULONG:
#����
                str[len++] = cv->tab[0];
                ����;
        ���� TOK_PPNUM:
        ���� TOK_PPSTR:
        ���� TOK_STR:
        ���� TOK_LSTR:
                {
                        /* Insert the string into the int array. */
                        size_t nb_words =
                                1 + (cv->str.size + �󳤶�(��) - 1) / �󳤶�(��);
                        �� (len + nb_words >= s->allocated_len)
                                str = tok_str_realloc(s, len + nb_words + 1);
                        str[len] = cv->str.size;
                        memcpy(&str[len + 1], cv->str.data, cv->str.size);
                        len += nb_words;
                }
                ����;
        ���� TOK_CDOUBLE:
        ���� TOK_CLLONG:
        ���� TOK_CULLONG:
#�綨�� TCC_LONG_ARE_64_BIT
        ���� TOK_CLONG:
        ���� TOK_CULONG:
#����
#�� LDOUBLE_SIZE == 8
        ���� TOK_CLDOUBLE:
#����
                str[len++] = cv->tab[0];
                str[len++] = cv->tab[1];
                ����;
#�� LDOUBLE_SIZE == 12
        ���� TOK_CLDOUBLE:
                str[len++] = cv->tab[0];
                str[len++] = cv->tab[1];
                str[len++] = cv->tab[2];
#���� LDOUBLE_SIZE == 16
        ���� TOK_CLDOUBLE:
                str[len++] = cv->tab[0];
                str[len++] = cv->tab[1];
                str[len++] = cv->tab[2];
                str[len++] = cv->tab[3];
#���� LDOUBLE_SIZE != 8
#���� add long double size support
#����
                ����;
        ȱʡ:
                ����;
        }
        s->len = len;
}

/* add the current parse token in token string 's' */
ST_FUNC �� tok_str_add_tok(TokenString *s)
{
        CValue cval;

        /* save line number info */
        �� (file->line_num != s->last_line_num) {
                s->last_line_num = file->line_num;
                cval.i = s->last_line_num;
                tok_str_add2(s, TOK_LINENUM, &cval);
        }
        tok_str_add2(s, tok, &tokc);
}

/* get a token from an integer array and increment pointer
   accordingly. we code it as a macro to avoid pointer aliasing. */
��̬ ���� �� TOK_GET(�� *t, ���� �� **pp, CValue *cv)
{
        ���� �� *p = *pp;
        �� n, *tab;

        tab = cv->tab;
        ת��(*t = *p++) {
        ���� TOK_CINT:
        ���� TOK_CUINT:
        ���� TOK_CCHAR:
        ���� TOK_LCHAR:
        ���� TOK_LINENUM:
#��δ���� TCC_LONG_ARE_64_BIT
        ���� TOK_CLONG:
        ���� TOK_CULONG:
#����
                tab[0] = *p++;
                cv->i = (*t == TOK_CUINT) ? (�޷�)cv->i : (��)cv->i;
                ����;
        ���� TOK_CFLOAT:
                tab[0] = *p++;
                ����;
        ���� TOK_STR:
        ���� TOK_LSTR:
        ���� TOK_PPNUM:
        ���� TOK_PPSTR:
                cv->str.size = *p++;
                cv->str.data = p;
                p += (cv->str.size + �󳤶�(��) - 1) / �󳤶�(��);
                ����;
        ���� TOK_CDOUBLE:
        ���� TOK_CLLONG:
        ���� TOK_CULLONG:
#�綨�� TCC_LONG_ARE_64_BIT
        ���� TOK_CLONG:
        ���� TOK_CULONG:
#����
                n = 2;
                ��ת copy;
        ���� TOK_CLDOUBLE:
#�� LDOUBLE_SIZE == 16
                n = 4;
#���� LDOUBLE_SIZE == 12
                n = 3;
#���� LDOUBLE_SIZE == 8
                n = 2;
#��
# ���� add long double size support
#����
        copy:
                ����
                        *tab++ = *p++;
                �� (--n);
                ����;
        ȱʡ:
                ����;
        }
        *pp = p;
}

��̬ �� macro_is_equal(���� �� *a, ���� �� *b)
{
        CValue cv;
        �� t;

        �� (!a || !b)
                ���� 1;

        �� (*a && *b) {
                /* first time preallocate macro_equal_buf, next time only reset position to start */
                cstr_reset(&macro_equal_buf);
                TOK_GET(&t, &a, &cv);
                cstr_cat(&macro_equal_buf, get_tok_str(t, &cv), 0);
                TOK_GET(&t, &b, &cv);
                �� (strcmp(macro_equal_buf.data, get_tok_str(t, &cv)))
                        ���� 0;
        }
        ���� !(*a || *b);
}

/* defines handling */
ST_INLN �� define_push(�� v, �� macro_type, �� *str, Sym *first_arg)
{
        Sym *s, *o;

        o = define_find(v);
        s = sym_push2(&define_stack, v, macro_type, 0);
        s->d = str;
        s->next = first_arg;
        table_ident[v - TOK_IDENT]->sym_define = s;

        �� (o && !macro_is_equal(o->d, s->d))
                tcc_warning("%s redefined", get_tok_str(v, NULL));
}

/* undefined a define symbol. Its name is just set to zero */
ST_FUNC �� define_undef(Sym *s)
{
        �� v = s->v;
        �� (v >= TOK_IDENT && v < tok_ident)
                table_ident[v - TOK_IDENT]->sym_define = NULL;
}

ST_INLN Sym *define_find(�� v)
{
        v -= TOK_IDENT;
        �� ((�޷�)v >= (�޷�)(tok_ident - TOK_IDENT))
                ���� NULL;
        ���� table_ident[v]->sym_define;
}

/* free define stack until top reaches 'b' */
ST_FUNC �� free_defines(Sym *b)
{
        �� (define_stack != b) {
                Sym *top = define_stack;
                define_stack = top->prev;
                tok_str_free_str(top->d);
                define_undef(top);
                sym_free(top);
        }

        /* restore remaining (-D or predefined) symbols if they were
           #undef'd in the file */
        �� (b) {
                �� v = b->v;
                �� (v >= TOK_IDENT && v < tok_ident) {
                        Sym **d = &table_ident[v - TOK_IDENT]->sym_define;
                        �� (!*d)
                                *d = b;
                }
                b = b->prev;
        }
}

/* label lookup */
ST_FUNC Sym *label_find(�� v)
{
        v -= TOK_IDENT;
        �� ((�޷�)v >= (�޷�)(tok_ident - TOK_IDENT))
                ���� NULL;
        ���� table_ident[v]->sym_label;
}

ST_FUNC Sym *label_push(Sym **ptop, �� v, �� flags)
{
        Sym *s, **ps;
        s = sym_push2(ptop, v, 0, 0);
        s->r = flags;
        ps = &table_ident[v - TOK_IDENT]->sym_label;
        �� (ptop == &global_label_stack) {
                /* modify the top most local identifier, so that
                   sym_identifier will point to 's' when popped */
                �� (*ps != NULL)
                        ps = &(*ps)->prev_tok;
        }
        s->prev_tok = *ps;
        *ps = s;
        ���� s;
}

/* pop labels until element last is reached. Look if any labels are
   undefined. Define symbols if '&&label' was used. */
ST_FUNC �� label_pop(Sym **ptop, Sym *slast, �� keep)
{
        Sym *s, *s1;
        ����(s = *ptop; s != slast; s = s1) {
                s1 = s->prev;
                �� (s->r == LABEL_DECLARED) {
                        tcc_warning("label '%s' declared but not used", get_tok_str(s->v, NULL));
                } �� �� (s->r == LABEL_FORWARD) {
                                tcc_error("label '%s' used but not defined",
                                          get_tok_str(s->v, NULL));
                } �� {
                        �� (s->c) {
                                /* define corresponding symbol. A size of
                                   1 is put. */
                                put_extern_sym(s, cur_text_section, s->jnext, 1);
                        }
                }
                /* remove label */
                table_ident[s->v - TOK_IDENT]->sym_label = s->prev_tok;
                �� (!keep)
                        sym_free(s);
        }
        �� (!keep)
                *ptop = slast;
}

/* fake the nth "#if defined test_..." for tcc -dt -run */
��̬ �� maybe_run_test(TCCState *s)
{
        ���� �� *p;
        �� (s->include_stack_ptr != s->include_stack)
                ����;
        p = get_tok_str(tok, NULL);
        �� (0 != memcmp(p, "test_", 5))
                ����;
        �� (0 != --s->run_test)
                ����;
        fprintf(s->ppfp, "\n[%s]\n" + !(s->dflag & 32), p), fflush(s->ppfp);
        define_push(tok, MACRO_OBJ, NULL, NULL);
}

/* eval an expression for #if/#elif */
��̬ �� expr_preprocess(��)
{
        �� c, t;
        TokenString *str;

        str = tok_str_alloc();
        pp_expr = 1;
        �� (tok != TOK_LINEFEED && tok != TOK_EOF) {
                next(); /* do macro subst */
                �� (tok == TOK_DEFINED || tok == TOK_DEFINED_CN) {
                        next_nomacro();
                        t = tok;
                        �� (t == '(')
                                next_nomacro();
                        �� (tok < TOK_IDENT)
                                expect("identifier");
                        �� (tcc_state->run_test)
                                maybe_run_test(tcc_state);
                        c = define_find(tok) != 0;
                        �� (t == '(') {
                                next_nomacro();
                                �� (tok != ')')
                                        expect("')'");
                        }
                        tok = TOK_CINT;
                        tokc.i = c;
                } �� �� (tok >= TOK_IDENT) {
                        /* if undefined macro */
                        tok = TOK_CINT;
                        tokc.i = 0;
                }
                tok_str_add_tok(str);
        }
        pp_expr = 0;
        tok_str_add(str, -1); /* simulate end of file */
        tok_str_add(str, 0);
        /* now evaluate C constant expression */
        begin_macro(str, 1);
        next();
        c = expr_const();
        end_macro();
        ���� c != 0;
}


/* parse after #define */
ST_FUNC �� parse_define(��)
{
        Sym *s, *first, **ps;
        �� v, t, varg, is_vaargs, spc;
        �� saved_parse_flags = parse_flags;

        v = tok;
        �� (v < TOK_IDENT || v == TOK_DEFINED || v == TOK_DEFINED_CN)
                tcc_error("invalid macro name '%s'", get_tok_str(tok, &tokc));
        /* XXX: should check if same macro (ANSI) */
        first = NULL;
        t = MACRO_OBJ;
        /* We have to parse the whole define as if not in asm mode, in particular
           no line comment with '#' must be ignored.  Also for function
           macros the argument list must be parsed without '.' being an ID
           character.  */
        parse_flags = ((parse_flags & ~PARSE_FLAG_ASM_FILE) | PARSE_FLAG_SPACES);
        /* '(' must be just after macro definition for MACRO_FUNC */
        next_nomacro_spc();
        �� (tok == '(') {
                �� dotid = set_idnum('.', 0);
                next_nomacro();
                ps = &first;
                �� (tok != ')') ���� (;;) {
                        varg = tok;
                        next_nomacro();
                        is_vaargs = 0;
                        �� (varg == TOK_DOTS) {
                                varg = TOK___VA_ARGS__;
                                is_vaargs = 1;
                        } �� �� (tok == TOK_DOTS && gnu_ext) {
                                is_vaargs = 1;
                                next_nomacro();
                        }
                        �� (varg < TOK_IDENT)
                bad_list:
                                tcc_error("bad macro parameter list");
                        s = sym_push2(&define_stack, varg | SYM_FIELD, is_vaargs, 0);
                        *ps = s;
                        ps = &s->next;
                        �� (tok == ')')
                                ����;
                        �� (tok != ',' || is_vaargs)
                                ��ת bad_list;
                        next_nomacro();
                }
                next_nomacro_spc();
                t = MACRO_FUNC;
                set_idnum('.', dotid);
        }

        tokstr_buf.len = 0;
        spc = 2;
        parse_flags |= PARSE_FLAG_ACCEPT_STRAYS | PARSE_FLAG_SPACES | PARSE_FLAG_LINEFEED;
        /* The body of a macro definition should be parsed such that identifiers
           are parsed like the file mode determines (i.e. with '.' being an
           ID character in asm mode).  But '#' should be retained instead of
           regarded as line comment leader, so still don't set ASM_FILE
           in parse_flags. */
        �� (tok != TOK_LINEFEED && tok != TOK_EOF) {
                /* remove spaces around ## and after '#' */
                �� (TOK_TWOSHARPS == tok) {
                        �� (2 == spc)
                                ��ת bad_twosharp;
                        �� (1 == spc)
                                --tokstr_buf.len;
                        spc = 3;
                        tok = TOK_PPJOIN;
                } �� �� ('#' == tok) {
                        spc = 4;
                } �� �� (check_space(tok, &spc)) {
                        ��ת skip;
                }
                tok_str_add2(&tokstr_buf, tok, &tokc);
        skip:
                next_nomacro_spc();
        }

        parse_flags = saved_parse_flags;
        �� (spc == 1)
                --tokstr_buf.len; /* remove trailing space */
        tok_str_add(&tokstr_buf, 0);
        �� (3 == spc)
bad_twosharp:
                tcc_error("'##' cannot appear at either end of macro");
        define_push(v, t, tok_str_dup(&tokstr_buf), first);
}

��̬ CachedInclude *search_cached_include(TCCState *s1, ���� �� *filename, �� add)
{
        ���� �޷� �� *s;
        �޷� �� h;
        CachedInclude *e;
        �� i;

        h = TOK_HASH_INIT;
        s = (�޷� �� *) filename;
        �� (*s) {
#�綨�� _WIN32
                h = TOK_HASH_FUNC(h, toup(*s));
#��
                h = TOK_HASH_FUNC(h, *s);
#����
                s++;
        }
        h &= (CACHED_INCLUDES_HASH_SIZE - 1);

        i = s1->cached_includes_hash[h];
        ����(;;) {
                �� (i == 0)
                        ����;
                e = s1->cached_includes[i - 1];
                �� (0 == PATHCMP(e->filename, filename))
                        ���� e;
                i = e->hash_next;
        }
        �� (!add)
                ���� NULL;

        e = tcc_malloc(�󳤶�(CachedInclude) + strlen(filename));
        strcpy(e->filename, filename);
        e->ifndef_macro = e->once = 0;
        dynarray_add(&s1->cached_includes, &s1->nb_cached_includes, e);
        /* add in hash table */
        e->hash_next = s1->cached_includes_hash[h];
        s1->cached_includes_hash[h] = s1->nb_cached_includes;
#�綨�� INC_DEBUG
        printf("adding cached '%s'\n", filename);
#����
        ���� e;
}

��̬ �� pragma_parse(TCCState *s1)
{
        next_nomacro();
        �� (tok == TOK_push_macro || tok == TOK_pop_macro) {
                �� t = tok, v;
                Sym *s;

                �� (next(), tok != '(')
                        ��ת pragma_err;
                �� (next(), tok != TOK_STR)
                        ��ת pragma_err;
                v = tok_alloc(tokc.str.data, tokc.str.size - 1)->tok;
                �� (next(), tok != ')')
                        ��ת pragma_err;
                �� (t == TOK_push_macro) {
                        �� (NULL == (s = define_find(v)))
                                define_push(v, 0, NULL, NULL);
                        s->type.ref = s; /* set push boundary */
                } �� {
                        ���� (s = define_stack; s; s = s->prev)
                                �� (s->v == v && s->type.ref == s) {
                                        s->type.ref = NULL;
                                        ����;
                                }
                }
                �� (s)
                        table_ident[v - TOK_IDENT]->sym_define = s->d ? s : NULL;
                ��
                        tcc_warning("unbalanced #pragma pop_macro");
                pp_debug_tok = t, pp_debug_symv = v;

        } �� �� (tok == TOK_once) {
                search_cached_include(s1, file->filename, 1)->once = pp_once;

        } �� �� (s1->output_type == TCC_OUTPUT_PREPROCESS) {
                /* tcc -E: keep pragmas below unchanged */
                unget_tok(' ');
                unget_tok(TOK_PRAGMA);
                unget_tok('#');
                unget_tok(TOK_LINEFEED);

        } �� �� (tok == TOK_pack) {
                /* This may be:
                   #pragma pack(1) // set
                   #pragma pack() // reset to default
                   #pragma pack(push,1) // push & set
                   #pragma pack(pop) // restore previous */
                next();
                skip('(');
                �� (tok == TOK_ASM_pop) {
                        next();
                        �� (s1->pack_stack_ptr <= s1->pack_stack) {
                        stk_error:
                                tcc_error("out of pack stack");
                        }
                        s1->pack_stack_ptr--;
                } �� {
                        �� val = 0;
                        �� (tok != ')') {
                                �� (tok == TOK_ASM_push) {
                                        next();
                                        �� (s1->pack_stack_ptr >= s1->pack_stack + PACK_STACK_SIZE - 1)
                                                ��ת stk_error;
                                        s1->pack_stack_ptr++;
                                        skip(',');
                                }
                                �� (tok != TOK_CINT)
                                        ��ת pragma_err;
                                val = tokc.i;
                                �� (val < 1 || val > 16 || (val & (val - 1)) != 0)
                                        ��ת pragma_err;
                                next();
                        }
                        *s1->pack_stack_ptr = val;
                }
                �� (tok != ')')
                        ��ת pragma_err;

        } �� �� (tok == TOK_comment) {
                �� *p; �� t;
                next();
                skip('(');
                t = tok;
                next();
                skip(',');
                �� (tok != TOK_STR)
                        ��ת pragma_err;
                p = tcc_strdup((�� *)tokc.str.data);
                next();
                �� (tok != ')')
                        ��ת pragma_err;
                �� (t == TOK_lib) {
                        dynarray_add(&s1->pragma_libs, &s1->nb_pragma_libs, p);
                } �� {
                        �� (t == TOK_option)
                                tcc_set_options(s1, p);
                        tcc_free(p);
                }

        } �� �� (s1->warn_unsupported) {
                tcc_warning("#pragma %s is ignored", get_tok_str(tok, &tokc));
        }
        ����;

pragma_err:
        tcc_error("malformed #pragma directive");
        ����;
}

/* is_bof is true if first non space token at beginning of file */
ST_FUNC �� preprocess(�� is_bof)
{
        TCCState *s1 = tcc_state;
        �� i, c, n, saved_parse_flags;
        �� buf[1024], *q;
        Sym *s;

        saved_parse_flags = parse_flags;
        parse_flags = PARSE_FLAG_PREPROCESS
                | PARSE_FLAG_TOK_NUM
                | PARSE_FLAG_TOK_STR
                | PARSE_FLAG_LINEFEED
                | (parse_flags & PARSE_FLAG_ASM_FILE)
                ;

        next_nomacro();
 redo:
        ת��(tok) {
        ���� TOK_DEFINE:
        ���� TOK_DEFINE_CN:
                pp_debug_tok = tok;
                next_nomacro();
                pp_debug_symv = tok;
                parse_define();
                ����;
        ���� TOK_UNDEF:
        ���� TOK_UNDEF_CN:
                pp_debug_tok = tok;
                next_nomacro();
                pp_debug_symv = tok;
                s = define_find(tok);
                /* undefine symbol by putting an invalid name */
                �� (s)
                        define_undef(s);
                ����;
        ���� TOK_INCLUDE:
        ���� TOK_INCLUDE_CN:
        ���� TOK_INCLUDE_NEXT:
        ���� TOK_INCLUDE_NEXT_CN:
                ch = file->buf_ptr[0];
                /* XXX: incorrect if comments : use next_nomacro with a special mode */
                skip_spaces();
                �� (ch == '<') {
                        c = '>';
                        ��ת read_name;
                } �� �� (ch == '\"') {
                        c = ch;
                read_name:
                        inp();
                        q = buf;
                        �� (ch != c && ch != '\n' && ch != CH_EOF) {
                                �� ((q - buf) < �󳤶�(buf) - 1)
                                        *q++ = ch;
                                �� (ch == '\\') {
                                        �� (handle_stray_noerror() == 0)
                                                --q;
                                } ��
                                        inp();
                        }
                        *q = '\0';
                        minp();
#�� 0
                        /* eat all spaces and comments after include */
                        /* XXX: slightly incorrect */
                        �� (ch1 != '\n' && ch1 != CH_EOF)
                                inp();
#����
                } �� {
                        �� len;
                        /* computed #include : concatenate everything up to linefeed,
                           the result must be one of the two accepted forms.
                           Don't convert pp-tokens to tokens here.  */
                        parse_flags = (PARSE_FLAG_PREPROCESS
                                | PARSE_FLAG_LINEFEED
                                | (parse_flags & PARSE_FLAG_ASM_FILE));
                        next();
                        buf[0] = '\0';
                        �� (tok != TOK_LINEFEED) {
                                pstrcat(buf, �󳤶�(buf), get_tok_str(tok, &tokc));
                                next();
                        }
                        len = strlen(buf);
                        /* check syntax and remove '<>|""' */
                        �� ((len < 2 || ((buf[0] != '"' || buf[len-1] != '"') &&
                                (buf[0] != '<' || buf[len-1] != '>'))))
                                tcc_error("'#include' expects \"FILENAME\" or <FILENAME>");
                        c = buf[len-1];
                        memmove(buf, buf + 1, len - 2);
                        buf[len - 2] = '\0';
                }

                �� (s1->include_stack_ptr >= s1->include_stack + INCLUDE_STACK_SIZE)
                        tcc_error("#include recursion too deep");
                /* store current file in stack, but increment stack later below */
                *s1->include_stack_ptr = file;
                i = (tok == TOK_INCLUDE_NEXT || tok == TOK_INCLUDE_NEXT_CN) ? file->include_next_index : 0;
                n = 2 + s1->nb_include_paths + s1->nb_sysinclude_paths;
                ���� (; i < n; ++i) {
                        �� buf1[�󳤶� file->filename];
                        CachedInclude *e;
                        ���� �� *path;

                        �� (i == 0) {
                                /* check absolute include path */
                                �� (!IS_ABSPATH(buf))
                                        ����;
                                buf1[0] = 0;

                        } �� �� (i == 1) {
                                /* search in file's dir if "header.h" */
                                �� (c != '\"')
                                        ����;
                                /* https://savannah.nongnu.org/bugs/index.php?50847 */
                                path = file->true_filename;
                                pstrncpy(buf1, path, tcc_basename(path) - path);

                        } �� {
                                /* search in all the include paths */
                                �� j = i - 2, k = j - s1->nb_include_paths;
                                path = k < 0 ? s1->include_paths[j] : s1->sysinclude_paths[k];
                                pstrcpy(buf1, �󳤶�(buf1), path);
                                pstrcat(buf1, �󳤶�(buf1), "/");
                        }

                        pstrcat(buf1, �󳤶�(buf1), buf);
                        e = search_cached_include(s1, buf1, 0);
                        �� (e && (define_find(e->ifndef_macro) || e->once == pp_once)) {
                                /* no need to parse the include because the 'ifndef macro'
                                   is defined (or had #pragma once) */
#�綨�� INC_DEBUG
                                printf("%s: skipping cached %s\n", file->filename, buf1);
#����
                                ��ת include_done;
                        }

                        �� (tcc_open(s1, buf1) < 0)
                                ����;

                        file->include_next_index = i + 1;
#�綨�� INC_DEBUG
                        printf("%s: including %s\n", file->prev->filename, file->filename);
#����
                        /* update target deps */
                        dynarray_add(&s1->target_deps, &s1->nb_target_deps,
                                        tcc_strdup(buf1));
                        /* push current file in stack */
                        ++s1->include_stack_ptr;
                        /* add include file debug info */
                        �� (s1->do_debug)
                                put_stabs(file->filename, N_BINCL, 0, 0, 0);
                        tok_flags |= TOK_FLAG_BOF | TOK_FLAG_BOL;
                        ch = file->buf_ptr[0];
                        ��ת the_end;
                }
                tcc_error("include file '%s' not found", buf);
include_done:
                ����;
        ���� TOK_IFNDEF:
        ���� TOK_IFNDEF_CN:
                c = 1;
                ��ת do_ifdef;
        ���� TOK_IF:
        ���� TOK_IF_CN:
                c = expr_preprocess();
                ��ת do_if;
        ���� TOK_IFDEF:
        ���� TOK_IFDEF_CN:
                c = 0;
        do_ifdef:
                next_nomacro();
                �� (tok < TOK_IDENT)
                        tcc_error("invalid argument for '#if%sdef'", c ? "n" : "");
                �� (is_bof) {
                        �� (c) {
#�綨�� INC_DEBUG
                                printf("#ifndef %s\n", get_tok_str(tok, NULL));
#����
                                file->ifndef_macro = tok;
                        }
                }
                c = (define_find(tok) != 0) ^ c;
        do_if:
                �� (s1->ifdef_stack_ptr >= s1->ifdef_stack + IFDEF_STACK_SIZE)
                        tcc_error("memory full (�綨��)");
                *s1->ifdef_stack_ptr++ = c;
                ��ת test_skip;
        ���� TOK_ELSE:
        ���� TOK_ELSE_CN:
                �� (s1->ifdef_stack_ptr == s1->ifdef_stack)
                        tcc_error("#else without matching #if");
                �� (s1->ifdef_stack_ptr[-1] & 2)
                        tcc_error("#else after #else");
                c = (s1->ifdef_stack_ptr[-1] ^= 3);
                ��ת test_else;
        ���� TOK_ELIF:
        ���� TOK_ELIF_CN:
                �� (s1->ifdef_stack_ptr == s1->ifdef_stack)
                        tcc_error("#elif without matching #if");
                c = s1->ifdef_stack_ptr[-1];
                �� (c > 1)
                        tcc_error("#elif after #else");
                /* last #if/#elif expression was true: we skip */
                �� (c == 1) {
                        c = 0;
                } �� {
                        c = expr_preprocess();
                        s1->ifdef_stack_ptr[-1] = c;
                }
        test_else:
                �� (s1->ifdef_stack_ptr == file->ifdef_stack_ptr + 1)
                        file->ifndef_macro = 0;
        test_skip:
                �� (!(c & 1)) {
                        preprocess_skip();
                        is_bof = 0;
                        ��ת redo;
                }
                ����;
        ���� TOK_ENDIF:
        ���� TOK_ENDIF_CN:
                �� (s1->ifdef_stack_ptr <= file->ifdef_stack_ptr)
                        tcc_error("#endif without matching #if");
                s1->ifdef_stack_ptr--;
                /* '#ifndef macro' was at the start of file. Now we check if
                   an '#endif' is exactly at the end of file */
                �� (file->ifndef_macro &&
                        s1->ifdef_stack_ptr == file->ifdef_stack_ptr) {
                        file->ifndef_macro_saved = file->ifndef_macro;
                        /* need to set to zero to avoid false matches if another
                           #ifndef at middle of file */
                        file->ifndef_macro = 0;
                        �� (tok != TOK_LINEFEED)
                                next_nomacro();
                        tok_flags |= TOK_FLAG_ENDIF;
                        ��ת the_end;
                }
                ����;
        ���� TOK_PPNUM:
                n = strtoul((��*)tokc.str.data, &q, 10);
                ��ת _line_num;
        ���� TOK_LINE:
        ���� TOK_LINE_CN:
                next();
                �� (tok != TOK_CINT)
        _line_err:
                        tcc_error("wrong #line format");
                n = tokc.i;
        _line_num:
                next();
                �� (tok != TOK_LINEFEED) {
                        �� (tok == TOK_STR) {
                                �� (file->true_filename == file->filename)
                                        file->true_filename = tcc_strdup(file->filename);
                                pstrcpy(file->filename, �󳤶�(file->filename), (�� *)tokc.str.data);
                        } �� �� (parse_flags & PARSE_FLAG_ASM_FILE)
                                ����;
                        ��
                                ��ת _line_err;
                        --n;
                }
                �� (file->fd > 0)
                        total_lines += file->line_num - n;
                file->line_num = n;
                �� (s1->do_debug)
                        put_stabs(file->filename, N_BINCL, 0, 0, 0);
                ����;
        ���� TOK_ERROR:
        ���� TOK_ERROR_CN:
        ���� TOK_WARNING:
        ���� TOK_WARNING_CN:
                c = tok;
                ch = file->buf_ptr[0];
                skip_spaces();
                q = buf;
                �� (ch != '\n' && ch != CH_EOF) {
                        �� ((q - buf) < �󳤶�(buf) - 1)
                                *q++ = ch;
                        �� (ch == '\\') {
                                �� (handle_stray_noerror() == 0)
                                        --q;
                        } ��
                                inp();
                }
                *q = '\0';
                �� (c == TOK_ERROR || c == TOK_ERROR_CN)
                        tcc_error("#error %s", buf);
                ��
                        tcc_warning("#warning %s", buf);
                ����;
        ���� TOK_PRAGMA:
        ���� TOK_PRAGMA_CN:
                pragma_parse(s1);
                ����;
        ���� TOK_LINEFEED:
                ��ת the_end;
        ȱʡ:
                /* ignore gas line comment in an 'S' file. */
                �� (saved_parse_flags & PARSE_FLAG_ASM_FILE)
                        ��ת ignore;
                �� (tok == '!' && is_bof)
                        /* '!' is ignored at beginning to allow C scripts. */
                        ��ת ignore;
                tcc_warning("Ignoring unknown preprocessing directive #%s", get_tok_str(tok, &tokc));
        ignore:
                file->buf_ptr = parse_line_comment(file->buf_ptr - 1);
                ��ת the_end;
        }
        /* ignore other preprocess commands or #! for C scripts */
        �� (tok != TOK_LINEFEED)
                next_nomacro();
 the_end:
        parse_flags = saved_parse_flags;
}

/* evaluate escape codes in a string. */
��̬ �� parse_escape_string(CString *outstr, ���� uint8_t *buf, �� is_long)
{
        �� c, n;
        ���� uint8_t *p;

        p = buf;
        ����(;;) {
                c = *p;
                �� (c == '\0')
                        ����;
                �� (c == '\\') {
                        p++;
                        /* escape */
                        c = *p;
                        ת��(c) {
                        ���� '0': ���� '1': ���� '2': ���� '3':
                        ���� '4': ���� '5': ���� '6': ���� '7':
                                /* at most three octal digits */
                                n = c - '0';
                                p++;
                                c = *p;
                                �� (isoct(c)) {
                                        n = n * 8 + c - '0';
                                        p++;
                                        c = *p;
                                        �� (isoct(c)) {
                                                n = n * 8 + c - '0';
                                                p++;
                                        }
                                }
                                c = n;
                                ��ת add_char_nonext;
                        ���� 'x':
                        ���� 'u':
                        ���� 'U':
                                p++;
                                n = 0;
                                ����(;;) {
                                        c = *p;
                                        �� (c >= 'a' && c <= 'f')
                                                c = c - 'a' + 10;
                                        �� �� (c >= 'A' && c <= 'F')
                                                c = c - 'A' + 10;
                                        �� �� (isnum(c))
                                                c = c - '0';
                                        ��
                                                ����;
                                        n = n * 16 + c;
                                        p++;
                                }
                                c = n;
                                ��ת add_char_nonext;
                        ���� 'a':
                                c = '\a';
                                ����;
                        ���� 'b':
                                c = '\b';
                                ����;
                        ���� 'f':
                                c = '\f';
                                ����;
                        ���� 'n':
                                c = '\n';
                                ����;
                        ���� 'r':
                                c = '\r';
                                ����;
                        ���� 't':
                                c = '\t';
                                ����;
                        ���� 'v':
                                c = '\v';
                                ����;
                        ���� 'e':
                                �� (!gnu_ext)
                                        ��ת invalid_escape;
                                c = 27;
                                ����;
                        ���� '\'':
                        ���� '\"':
                        ���� '\\':
                        ���� '?':
                                ����;
                        ȱʡ:
                        invalid_escape:
                                �� (c >= '!' && c <= '~')
                                        tcc_warning("unknown escape sequence: \'\\%c\'", c);
                                ��
                                        tcc_warning("unknown escape sequence: \'\\x%x\'", c);
                                ����;
                        }
                }
                p++;
        add_char_nonext:
                �� (!is_long)
                        cstr_ccat(outstr, c);
                ��
                        cstr_wccat(outstr, c);
        }
        /* add a trailing '\0' */
        �� (!is_long)
                cstr_ccat(outstr, '\0');
        ��
                cstr_wccat(outstr, '\0');
}

��̬ �� parse_string(���� �� *s, �� len)
{
        uint8_t buf[1000], *p = buf;
        �� is_long, sep;

        �� ((is_long = *s == 'L'))
                ++s, --len;
        sep = *s++;
        len -= 2;
        �� (len >= �󳤶� buf)
                p = tcc_malloc(len + 1);
        memcpy(p, s, len);
        p[len] = 0;

        cstr_reset(&tokcstr);
        parse_escape_string(&tokcstr, p, is_long);
        �� (p != buf)
                tcc_free(p);

        �� (sep == '\'') {
                �� char_size;
                /* XXX: make it portable */
                �� (!is_long)
                        char_size = 1;
                ��
                        char_size = �󳤶�(nwchar_t);
                �� (tokcstr.size <= char_size)
                        tcc_error("empty character constant");
                �� (tokcstr.size > 2 * char_size)
                        tcc_warning("multi-character character constant");
                �� (!is_long) {
                        tokc.i = *(int8_t *)tokcstr.data;
                        tok = TOK_CCHAR;
                } �� {
                        tokc.i = *(nwchar_t *)tokcstr.data;
                        tok = TOK_LCHAR;
                }
        } �� {
                tokc.str.size = tokcstr.size;
                tokc.str.data = tokcstr.data;
                �� (!is_long)
                        tok = TOK_STR;
                ��
                        tok = TOK_LSTR;
        }
}

/* we use 64 bit numbers */
#���� BN_SIZE 2

/* bn = (bn << shift) | or_val */
��̬ �� bn_lshift(�޷� �� *bn, �� shift, �� or_val)
{
        �� i;
        �޷� �� v;
        ����(i=0;i<BN_SIZE;i++) {
                v = bn[i];
                bn[i] = (v << shift) | or_val;
                or_val = v >> (32 - shift);
        }
}

��̬ �� bn_zero(�޷� �� *bn)
{
        �� i;
        ����(i=0;i<BN_SIZE;i++) {
                bn[i] = 0;
        }
}

/* parse number in null terminated string 'p' and return it in the
   current token */
��̬ �� parse_number(���� �� *p)
{
        �� b, t, shift, frac_bits, s, exp_val, ch;
        �� *q;
        �޷� �� bn[BN_SIZE];
        ˫�� d;

        /* number */
        q = token_buf;
        ch = *p++;
        t = ch;
        ch = *p++;
        *q++ = t;
        b = 10;
        �� (t == '.') {
                ��ת float_frac_parse;
        } �� �� (t == '0') {
                �� (ch == 'x' || ch == 'X') {
                        q--;
                        ch = *p++;
                        b = 16;
                } �� �� (tcc_ext && (ch == 'b' || ch == 'B')) {
                        q--;
                        ch = *p++;
                        b = 2;
                }
        }
        /* parse all digits. cannot check octal numbers at this stage
           because of floating point constants */
        �� (1) {
                �� (ch >= 'a' && ch <= 'f')
                        t = ch - 'a' + 10;
                �� �� (ch >= 'A' && ch <= 'F')
                        t = ch - 'A' + 10;
                �� �� (isnum(ch))
                        t = ch - '0';
                ��
                        ����;
                �� (t >= b)
                        ����;
                �� (q >= token_buf + STRING_MAX_SIZE) {
                num_too_long:
                        tcc_error("number too long");
                }
                *q++ = ch;
                ch = *p++;
        }
        �� (ch == '.' ||
                ((ch == 'e' || ch == 'E') && b == 10) ||
                ((ch == 'p' || ch == 'P') && (b == 16 || b == 2))) {
                �� (b != 10) {
                        /* NOTE: strtox should support that for hexa numbers, but
                           non ISOC99 libcs do not support it, so we prefer to do
                           it by hand */
                        /* hexadecimal or binary floats */
                        /* XXX: handle overflows */
                        *q = '\0';
                        �� (b == 16)
                                shift = 4;
                        ��
                                shift = 1;
                        bn_zero(bn);
                        q = token_buf;
                        �� (1) {
                                t = *q++;
                                �� (t == '\0') {
                                        ����;
                                } �� �� (t >= 'a') {
                                        t = t - 'a' + 10;
                                } �� �� (t >= 'A') {
                                        t = t - 'A' + 10;
                                } �� {
                                        t = t - '0';
                                }
                                bn_lshift(bn, shift, t);
                        }
                        frac_bits = 0;
                        �� (ch == '.') {
                                ch = *p++;
                                �� (1) {
                                        t = ch;
                                        �� (t >= 'a' && t <= 'f') {
                                                t = t - 'a' + 10;
                                        } �� �� (t >= 'A' && t <= 'F') {
                                                t = t - 'A' + 10;
                                        } �� �� (t >= '0' && t <= '9') {
                                                t = t - '0';
                                        } �� {
                                                ����;
                                        }
                                        �� (t >= b)
                                                tcc_error("invalid digit");
                                        bn_lshift(bn, shift, t);
                                        frac_bits += shift;
                                        ch = *p++;
                                }
                        }
                        �� (ch != 'p' && ch != 'P')
                                expect("exponent");
                        ch = *p++;
                        s = 1;
                        exp_val = 0;
                        �� (ch == '+') {
                                ch = *p++;
                        } �� �� (ch == '-') {
                                s = -1;
                                ch = *p++;
                        }
                        �� (ch < '0' || ch > '9')
                                expect("exponent digits");
                        �� (ch >= '0' && ch <= '9') {
                                exp_val = exp_val * 10 + ch - '0';
                                ch = *p++;
                        }
                        exp_val = exp_val * s;

                        /* now we can generate the number */
                        /* XXX: should patch directly float number */
                        d = (˫��)bn[1] * 4294967296.0 + (˫��)bn[0];
                        d = ldexp(d, exp_val - frac_bits);
                        t = toup(ch);
                        �� (t == 'F') {
                                ch = *p++;
                                tok = TOK_CFLOAT;
                                /* float : should handle overflow */
                                tokc.f = (����)d;
                        } �� �� (t == 'L') {
                                ch = *p++;
#�綨�� TCC_TARGET_PE
                                tok = TOK_CDOUBLE;
                                tokc.d = d;
#��
                                tok = TOK_CLDOUBLE;
                                /* XXX: not large enough */
                                tokc.ld = (�� ˫��)d;
#����
                        } �� {
                                tok = TOK_CDOUBLE;
                                tokc.d = d;
                        }
                } �� {
                        /* decimal floats */
                        �� (ch == '.') {
                                �� (q >= token_buf + STRING_MAX_SIZE)
                                        ��ת num_too_long;
                                *q++ = ch;
                                ch = *p++;
                        float_frac_parse:
                                �� (ch >= '0' && ch <= '9') {
                                        �� (q >= token_buf + STRING_MAX_SIZE)
                                                ��ת num_too_long;
                                        *q++ = ch;
                                        ch = *p++;
                                }
                        }
                        �� (ch == 'e' || ch == 'E') {
                                �� (q >= token_buf + STRING_MAX_SIZE)
                                        ��ת num_too_long;
                                *q++ = ch;
                                ch = *p++;
                                �� (ch == '-' || ch == '+') {
                                        �� (q >= token_buf + STRING_MAX_SIZE)
                                                ��ת num_too_long;
                                        *q++ = ch;
                                        ch = *p++;
                                }
                                �� (ch < '0' || ch > '9')
                                        expect("exponent digits");
                                �� (ch >= '0' && ch <= '9') {
                                        �� (q >= token_buf + STRING_MAX_SIZE)
                                                ��ת num_too_long;
                                        *q++ = ch;
                                        ch = *p++;
                                }
                        }
                        *q = '\0';
                        t = toup(ch);
                        errno = 0;
                        �� (t == 'F') {
                                ch = *p++;
                                tok = TOK_CFLOAT;
                                tokc.f = strtof(token_buf, NULL);
                        } �� �� (t == 'L') {
                                ch = *p++;
#�綨�� TCC_TARGET_PE
                                tok = TOK_CDOUBLE;
                                tokc.d = strtod(token_buf, NULL);
#��
                                tok = TOK_CLDOUBLE;
                                tokc.ld = strtold(token_buf, NULL);
#����
                        } �� {
                                tok = TOK_CDOUBLE;
                                tokc.d = strtod(token_buf, NULL);
                        }
                }
        } �� {
                �޷� �� �� n, n1;
                �� lcount, ucount, must_64bit;
                ���� �� *p1;

                /* integer number */
                *q = '\0';
                q = token_buf;
                �� (b == 10 && *q == '0') {
                        b = 8;
                        q++;
                }
                n = 0;
                ��(1) {
                        t = *q++;
                        /* no need for checks except for base 10 / 8 errors */
                        �� (t == '\0')
                                ����;
                        �� �� (t >= 'a')
                                t = t - 'a' + 10;
                        �� �� (t >= 'A')
                                t = t - 'A' + 10;
                        ��
                                t = t - '0';
                        �� (t >= b)
                                tcc_error("invalid digit");
                        n1 = n;
                        n = n * b + t;
                        /* detect overflow */
                        /* XXX: this test is not reliable */
                        �� (n < n1)
                                tcc_error("integer constant overflow");
                }

                /* Determine the characteristics (unsigned and/or 64bit) the type of
                   the constant must have according to the constant suffix(es) */
                lcount = ucount = must_64bit = 0;
                p1 = p;
                ����(;;) {
                        t = toup(ch);
                        �� (t == 'L') {
                                �� (lcount >= 2)
                                        tcc_error("three 'l's in integer constant");
                                �� (lcount && *(p - 1) != ch)
                                        tcc_error("incorrect integer suffix: %s", p1);
                                lcount++;
                                �� (lcount == 2)
                                        must_64bit = 1;
                                ch = *p++;
                        } �� �� (t == 'U') {
                                �� (ucount >= 1)
                                        tcc_error("two 'u's in integer constant");
                                ucount++;
                                ch = *p++;
                        } �� {
                                ����;
                        }
                }

                /* Whether 64 bits are needed to hold the constant's value */
                �� (n & 0xffffffff00000000LL || must_64bit) {
                        tok = TOK_CLLONG;
                        n1 = n >> 32;
                } �� �� (lcount) {
#�綨�� TCC_LONG_ARE_64_BIT
                        n1 = n >> 32;
#��
                        n1 = n;
#����
                        tok = TOK_CLONG;
                } �� {
                        tok = TOK_CINT;
                        n1 = n;
                }

                /* Whether type must be unsigned to hold the constant's value */
                �� (ucount || ((n1 >> 31) && (b != 10))) {
                        �� (tok == TOK_CLLONG)
                                tok = TOK_CULLONG;
                        �� �� (tok == TOK_CLONG)
                                tok = TOK_CULONG;
                        ��
                                tok = TOK_CUINT;
                /* If decimal and no unsigned suffix, bump to 64 bits or throw error */
                } �� �� (n1 >> 31) {
                        �� (tok == TOK_CINT)
                                tok = TOK_CLLONG;
                        ��
                                tcc_error("integer constant overflow");
                }

                tokc.i = n;
        }
        �� (ch)
                tcc_error("invalid number\n");
}


#���� PARSE2(c1, tok1, c2, tok2)              \
        ���� c1:                                    \
                PEEKC(c, p);                            \
                �� (c == c2) {                          \
                        p++;                                \
                        tok = tok2;                         \
                } �� {                                \
                        tok = tok1;                         \
                }                                       \
                ����;

/* return next token without macro substitution */
��̬ ���� �� next_nomacro1(��)
{
        �� t, c, is_long, len;
        TokenSym *ts;
        uint8_t *p, *p1;
        �޷� �� h;

        p = file->buf_ptr;
 redo_no_start:
        c = *p;
        ת��(c) {
        ���� ' ':
        ���� '\t':
                tok = c;
                p++;
                �� (parse_flags & PARSE_FLAG_SPACES)
                        ��ת keep_tok_flags;
                �� (isidnum_table[*p - CH_EOF] & IS_SPC)
                        ++p;
                ��ת redo_no_start;
        ���� '\f':
        ���� '\v':
        ���� '\r':
                p++;
                ��ת redo_no_start;
        ���� '\\':
                /* first look if it is in fact an end of buffer */
                c = handle_stray1(p);
                p = file->buf_ptr;
                �� (c == '\\')
                        ��ת parse_simple;
                �� (c != CH_EOF)
                        ��ת redo_no_start;
                {
                        TCCState *s1 = tcc_state;
                        �� ((parse_flags & PARSE_FLAG_LINEFEED)
                                && !(tok_flags & TOK_FLAG_EOF)) {
                                tok_flags |= TOK_FLAG_EOF;
                                tok = TOK_LINEFEED;
                                ��ת keep_tok_flags;
                        } �� �� (!(parse_flags & PARSE_FLAG_PREPROCESS)) {
                                tok = TOK_EOF;
                        } �� �� (s1->ifdef_stack_ptr != file->ifdef_stack_ptr) {
                                tcc_error("missing #endif");
                        } �� �� (s1->include_stack_ptr == s1->include_stack) {
                                /* no include left : end of file. */
                                tok = TOK_EOF;
                        } �� {
                                tok_flags &= ~TOK_FLAG_EOF;
                                /* pop include file */

                                /* test if previous '#endif' was after a #ifdef at
                                   start of file */
                                �� (tok_flags & TOK_FLAG_ENDIF) {
#�綨�� INC_DEBUG
                                        printf("#endif %s\n", get_tok_str(file->ifndef_macro_saved, NULL));
#����
                                        search_cached_include(s1, file->filename, 1)
                                                ->ifndef_macro = file->ifndef_macro_saved;
                                        tok_flags &= ~TOK_FLAG_ENDIF;
                                }

                                /* add end of include file debug info */
                                �� (tcc_state->do_debug) {
                                        put_stabd(N_EINCL, 0, 0);
                                }
                                /* pop include stack */
                                tcc_close();
                                s1->include_stack_ptr--;
                                p = file->buf_ptr;
                                ��ת redo_no_start;
                        }
                }
                ����;

        ���� '\n':
                file->line_num++;
                tok_flags |= TOK_FLAG_BOL;
                p++;
maybe_newline:
                �� (0 == (parse_flags & PARSE_FLAG_LINEFEED))
                        ��ת redo_no_start;
                tok = TOK_LINEFEED;
                ��ת keep_tok_flags;

        ���� '#':
                /* XXX: simplify */
                PEEKC(c, p);
                �� ((tok_flags & TOK_FLAG_BOL) &&
                        (parse_flags & PARSE_FLAG_PREPROCESS)) {
                        file->buf_ptr = p;
                        preprocess(tok_flags & TOK_FLAG_BOF);
                        p = file->buf_ptr;
                        ��ת maybe_newline;
                } �� {
                        �� (c == '#') {
                                p++;
                                tok = TOK_TWOSHARPS;
                        } �� {
                                �� (parse_flags & PARSE_FLAG_ASM_FILE) {
                                        p = parse_line_comment(p - 1);
                                        ��ת redo_no_start;
                                } �� {
                                        tok = '#';
                                }
                        }
                }
                ����;

        /* dollar is allowed to start identifiers when not parsing asm */
        ���� '$':
                �� (!(isidnum_table[c - CH_EOF] & IS_ID)
                 || (parse_flags & PARSE_FLAG_ASM_FILE))
                        ��ת parse_simple;

        ���� 'a': ���� 'b': ���� 'c': ���� 'd':
        ���� 'e': ���� 'f': ���� 'g': ���� 'h':
        ���� 'i': ���� 'j': ���� 'k': ���� 'l':
        ���� 'm': ���� 'n': ���� 'o': ���� 'p':
        ���� 'q': ���� 'r': ���� 's': ���� 't':
        ���� 'u': ���� 'v': ���� 'w': ���� 'x':
        ���� 'y': ���� 'z':
        ���� 'A': ���� 'B': ���� 'C': ���� 'D':
        ���� 'E': ���� 'F': ���� 'G': ���� 'H':
        ���� 'I': ���� 'J': ���� 'K':
        ���� 'M': ���� 'N': ���� 'O': ���� 'P':
        ���� 'Q': ���� 'R': ���� 'S': ���� 'T':
        ���� 'U': ���� 'V': ���� 'W': ���� 'X':
        ���� 'Y': ���� 'Z':
        ���� '_':
        parse_ident_fast:
                p1 = p;
                h = TOK_HASH_INIT;
                h = TOK_HASH_FUNC(h, c);
                �� (c = *++p, isidnum_table[c - CH_EOF] & (IS_ID|IS_NUM))
                        h = TOK_HASH_FUNC(h, c);
                len = p - p1;
                �� (c != '\\') {
                        TokenSym **pts;

                        /* fast case : no stray found, so we have the full token
                           and we have already hashed it */
                        h &= (TOK_HASH_SIZE - 1);
                        pts = &hash_ident[h];
                        ����(;;) {
                                ts = *pts;
                                �� (!ts)
                                        ����;
                                �� (ts->len == len && !memcmp(ts->str, p1, len))
                                        ��ת token_found;
                                pts = &(ts->hash_next);
                        }
                        ts = tok_alloc_new(pts, (�� *) p1, len);
                token_found: ;
                } �� {
                        /* slower case */
                        cstr_reset(&tokcstr);
                        cstr_cat(&tokcstr, (�� *) p1, len);
                        p--;
                        PEEKC(c, p);
                parse_ident_slow:
                        �� (isidnum_table[c - CH_EOF] & (IS_ID|IS_NUM))
                        {
                                cstr_ccat(&tokcstr, c);
                                PEEKC(c, p);
                        }
                        ts = tok_alloc(tokcstr.data, tokcstr.size);
                }
                tok = ts->tok;
                ����;
        ���� 'L':
                t = p[1];
                �� (t != '\\' && t != '\'' && t != '\"') {
                        /* fast case */
                        ��ת parse_ident_fast;
                } �� {
                        PEEKC(c, p);
                        �� (c == '\'' || c == '\"') {
                                is_long = 1;
                                ��ת str_const;
                        } �� {
                                cstr_reset(&tokcstr);
                                cstr_ccat(&tokcstr, 'L');
                                ��ת parse_ident_slow;
                        }
                }
                ����;

        ���� '0': ���� '1': ���� '2': ���� '3':
        ���� '4': ���� '5': ���� '6': ���� '7':
        ���� '8': ���� '9':
                t = c;
                PEEKC(c, p);
                /* after the first digit, accept digits, alpha, '.' or sign if
                   prefixed by 'eEpP' */
        parse_num:
                cstr_reset(&tokcstr);
                ����(;;) {
                        cstr_ccat(&tokcstr, t);
                        �� (!((isidnum_table[c - CH_EOF] & (IS_ID|IS_NUM))
                                  || c == '.'
                                  || ((c == '+' || c == '-')
                                          && (((t == 'e' || t == 'E')
                                                        && !(parse_flags & PARSE_FLAG_ASM_FILE
                                                                /* 0xe+1 is 3 tokens in asm */
                                                                && ((��*)tokcstr.data)[0] == '0'
                                                                && toup(((��*)tokcstr.data)[1]) == 'X'))
                                                  || t == 'p' || t == 'P'))))
                                ����;
                        t = c;
                        PEEKC(c, p);
                }
                /* We add a trailing '\0' to ease parsing */
                cstr_ccat(&tokcstr, '\0');
                tokc.str.size = tokcstr.size;
                tokc.str.data = tokcstr.data;
                tok = TOK_PPNUM;
                ����;

        ���� '.':
                /* special dot handling because it can also start a number */
                PEEKC(c, p);
                �� (isnum(c)) {
                        t = '.';
                        ��ת parse_num;
                } �� �� ((isidnum_table['.' - CH_EOF] & IS_ID)
                                   && (isidnum_table[c - CH_EOF] & (IS_ID|IS_NUM))) {
                        *--p = c = '.';
                        ��ת parse_ident_fast;
                } �� �� (c == '.') {
                        PEEKC(c, p);
                        �� (c == '.') {
                                p++;
                                tok = TOK_DOTS;
                        } �� {
                                *--p = '.'; /* may underflow into file->unget[] */
                                tok = '.';
                        }
                } �� {
                        tok = '.';
                }
                ����;
        ���� '\'':
        ���� '\"':
                is_long = 0;
        str_const:
                cstr_reset(&tokcstr);
                �� (is_long)
                        cstr_ccat(&tokcstr, 'L');
                cstr_ccat(&tokcstr, c);
                p = parse_pp_string(p, c, &tokcstr);
                cstr_ccat(&tokcstr, c);
                cstr_ccat(&tokcstr, '\0');
                tokc.str.size = tokcstr.size;
                tokc.str.data = tokcstr.data;
                tok = TOK_PPSTR;
                ����;

        ���� '<':
                PEEKC(c, p);
                �� (c == '=') {
                        p++;
                        tok = TOK_LE;
                } �� �� (c == '<') {
                        PEEKC(c, p);
                        �� (c == '=') {
                                p++;
                                tok = TOK_A_SHL;
                        } �� {
                                tok = TOK_SHL;
                        }
                } �� {
                        tok = TOK_LT;
                }
                ����;
        ���� '>':
                PEEKC(c, p);
                �� (c == '=') {
                        p++;
                        tok = TOK_GE;
                } �� �� (c == '>') {
                        PEEKC(c, p);
                        �� (c == '=') {
                                p++;
                                tok = TOK_A_SAR;
                        } �� {
                                tok = TOK_SAR;
                        }
                } �� {
                        tok = TOK_GT;
                }
                ����;

        ���� '&':
                PEEKC(c, p);
                �� (c == '&') {
                        p++;
                        tok = TOK_LAND;
                } �� �� (c == '=') {
                        p++;
                        tok = TOK_A_AND;
                } �� {
                        tok = '&';
                }
                ����;

        ���� '|':
                PEEKC(c, p);
                �� (c == '|') {
                        p++;
                        tok = TOK_LOR;
                } �� �� (c == '=') {
                        p++;
                        tok = TOK_A_OR;
                } �� {
                        tok = '|';
                }
                ����;

        ���� '+':
                PEEKC(c, p);
                �� (c == '+') {
                        p++;
                        tok = TOK_INC;
                } �� �� (c == '=') {
                        p++;
                        tok = TOK_A_ADD;
                } �� {
                        tok = '+';
                }
                ����;

        ���� '-':
                PEEKC(c, p);
                �� (c == '-') {
                        p++;
                        tok = TOK_DEC;
                } �� �� (c == '=') {
                        p++;
                        tok = TOK_A_SUB;
                } �� �� (c == '>') {
                        p++;
                        tok = TOK_ARROW;
                } �� {
                        tok = '-';
                }
                ����;

        PARSE2('!', '!', '=', TOK_NE)
        PARSE2('=', '=', '=', TOK_EQ)
        PARSE2('*', '*', '=', TOK_A_MUL)
        PARSE2('%', '%', '=', TOK_A_MOD)
        PARSE2('^', '^', '=', TOK_A_XOR)

                /* comments or operator */
        ���� '/':
                PEEKC(c, p);
                �� (c == '*') {
                        p = parse_comment(p);
                        /* comments replaced by a blank */
                        tok = ' ';
                        ��ת keep_tok_flags;
                } �� �� (c == '/') {
                        p = parse_line_comment(p);
                        tok = ' ';
                        ��ת keep_tok_flags;
                } �� �� (c == '=') {
                        p++;
                        tok = TOK_A_DIV;
                } �� {
                        tok = '/';
                }
                ����;

                /* simple tokens */
        ���� '(':
        ���� ')':
        ���� '[':
        ���� ']':
        ���� '{':
        ���� '}':
        ���� ',':
        ���� ';':
        ���� ':':
        ���� '?':
        ���� '~':
        ���� '@': /* only used in assembler */
        parse_simple:
                tok = c;
                p++;
                ����;
        ȱʡ:
                �� (c >= 0x80 && c <= 0xFF) /* utf8 identifiers */
                        ��ת parse_ident_fast;
                �� (parse_flags & PARSE_FLAG_ASM_FILE)
                        ��ת parse_simple;
                tcc_error("unrecognized character \\x%02x", c);
                ����;
        }
        tok_flags = 0;
keep_tok_flags:
        file->buf_ptr = p;
#�� �Ѷ���(PARSE_DEBUG)
        printf("token = %s\n", get_tok_str(tok, &tokc));
#����
}

/* return next token without macro substitution. Can read input from
   macro_ptr buffer */
��̬ �� next_nomacro_spc(��)
{
        �� (macro_ptr) {
        redo:
                tok = *macro_ptr;
                �� (tok) {
                        TOK_GET(&tok, &macro_ptr, &tokc);
                        �� (tok == TOK_LINENUM) {
                                file->line_num = tokc.i;
                                ��ת redo;
                        }
                }
        } �� {
                next_nomacro1();
        }
        //printf("token = %s\n", get_tok_str(tok, &tokc));
}

ST_FUNC �� next_nomacro(��)
{
        ���� {
                next_nomacro_spc();
        } �� (tok < 256 && (isidnum_table[tok - CH_EOF] & IS_SPC));
}


��̬ �� macro_subst(
        TokenString *tok_str,
        Sym **nested_list,
        ���� �� *macro_str
        );

/* substitute arguments in replacement lists in macro_str by the values in
   args (field d) and return allocated string */
��̬ �� *macro_arg_subst(Sym **nested_list, ���� �� *macro_str, Sym *args)
{
        �� t, t0, t1, spc;
        ���� �� *st;
        Sym *s;
        CValue cval;
        TokenString str;
        CString cstr;

        tok_str_new(&str);
        t0 = t1 = 0;
        ��(1) {
                TOK_GET(&t, &macro_str, &cval);
                �� (!t)
                        ����;
                �� (t == '#') {
                        /* stringize */
                        TOK_GET(&t, &macro_str, &cval);
                        �� (!t)
                                ��ת bad_stringy;
                        s = sym_find2(args, t);
                        �� (s) {
                                cstr_new(&cstr);
                                cstr_ccat(&cstr, '\"');
                                st = s->d;
                                spc = 0;
                                �� (*st >= 0) {
                                        TOK_GET(&t, &st, &cval);
                                        �� (t != TOK_PLCHLDR
                                         && t != TOK_NOSUBST
                                         && 0 == check_space(t, &spc)) {
                                                ���� �� *s = get_tok_str(t, &cval);
                                                �� (*s) {
                                                        �� (t == TOK_PPSTR && *s != '\'')
                                                                add_char(&cstr, *s);
                                                        ��
                                                                cstr_ccat(&cstr, *s);
                                                        ++s;
                                                }
                                        }
                                }
                                cstr.size -= spc;
                                cstr_ccat(&cstr, '\"');
                                cstr_ccat(&cstr, '\0');
#�綨�� PP_DEBUG
                                printf("\nstringize: <%s>\n", (�� *)cstr.data);
#����
                                /* add string */
                                cval.str.size = cstr.size;
                                cval.str.data = cstr.data;
                                tok_str_add2(&str, TOK_PPSTR, &cval);
                                cstr_free(&cstr);
                        } �� {
                bad_stringy:
                                expect("macro parameter after '#'");
                        }
                } �� �� (t >= TOK_IDENT) {
                        s = sym_find2(args, t);
                        �� (s) {
                                �� l0 = str.len;
                                st = s->d;
                                /* if '##' is present before or after, no arg substitution */
                                �� (*macro_str == TOK_PPJOIN || t1 == TOK_PPJOIN) {
                                        /* special case for var arg macros : ## eats the ','
                                           if empty VA_ARGS variable. */
                                        �� (t1 == TOK_PPJOIN && t0 == ',' && gnu_ext && s->type.t) {
                                                �� (*st <= 0) {
                                                        /* suppress ',' '##' */
                                                        str.len -= 2;
                                                } �� {
                                                        /* suppress '##' and add variable */
                                                        str.len--;
                                                        ��ת add_var;
                                                }
                                        }
                                } �� {
                        add_var:
                                        �� (!s->next) {
                                                /* Expand arguments tokens and store them.  In most
                                                   cases we could also re-expand each argument if
                                                   used multiple times, but not if the argument
                                                   contains the __COUNTER__ macro.  */
                                                TokenString str2;
                                                sym_push2(&s->next, s->v, s->type.t, 0);
                                                tok_str_new(&str2);
                                                macro_subst(&str2, nested_list, st);
                                                tok_str_add(&str2, 0);
                                                s->next->d = str2.str;
                                        }
                                        st = s->next->d;
                                }
                                ����(;;) {
                                        �� t2;
                                        TOK_GET(&t2, &st, &cval);
                                        �� (t2 <= 0)
                                                ����;
                                        tok_str_add2(&str, t2, &cval);
                                }
                                �� (str.len == l0) /* expanded to empty string */
                                        tok_str_add(&str, TOK_PLCHLDR);
                        } �� {
                                tok_str_add(&str, t);
                        }
                } �� {
                        tok_str_add2(&str, t, &cval);
                }
                t0 = t1, t1 = t;
        }
        tok_str_add(&str, 0);
        ���� str.str;
}

��̬ �� ���� ab_month_name[12][4] =
{
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};

��̬ �� paste_tokens(�� t1, CValue *v1, �� t2, CValue *v2)
{
        CString cstr;
        �� n, ret = 1;

        cstr_new(&cstr);
        �� (t1 != TOK_PLCHLDR)
                cstr_cat(&cstr, get_tok_str(t1, v1), -1);
        n = cstr.size;
        �� (t2 != TOK_PLCHLDR)
                cstr_cat(&cstr, get_tok_str(t2, v2), -1);
        cstr_ccat(&cstr, '\0');

        tcc_open_bf(tcc_state, ":paste:", cstr.size);
        memcpy(file->buffer, cstr.data, cstr.size);
        tok_flags = 0;
        ���� (;;) {
                next_nomacro1();
                �� (0 == *file->buf_ptr)
                        ����;
                �� (is_space(tok))
                        ����;
                tcc_warning("pasting \"%.*s\" and \"%s\" does not give a valid"
                        " preprocessing token", n, cstr.data, (��*)cstr.data + n);
                ret = 0;
                ����;
        }
        tcc_close();
        //printf("paste <%s>\n", (char*)cstr.data);
        cstr_free(&cstr);
        ���� ret;
}

/* handle the '##' operator. Return NULL if no '##' seen. Otherwise
   return the resulting string (which must be freed). */
��̬ ���� �� *macro_twosharps(���� �� *ptr0)
{
        �� t;
        CValue cval;
        TokenString macro_str1;
        �� start_of_nosubsts = -1;
        ���� �� *ptr;

        /* we search the first '##' */
        ���� (ptr = ptr0;;) {
                TOK_GET(&t, &ptr, &cval);
                �� (t == TOK_PPJOIN)
                        ����;
                �� (t == 0)
                        ���� NULL;
        }

        tok_str_new(&macro_str1);

        //tok_print(" $$$", ptr0);
        ���� (ptr = ptr0;;) {
                TOK_GET(&t, &ptr, &cval);
                �� (t == 0)
                        ����;
                �� (t == TOK_PPJOIN)
                        ����;
                �� (*ptr == TOK_PPJOIN) {
                        �� t1; CValue cv1;
                        /* given 'a##b', remove nosubsts preceding 'a' */
                        �� (start_of_nosubsts >= 0)
                                macro_str1.len = start_of_nosubsts;
                        /* given 'a##b', remove nosubsts preceding 'b' */
                        �� ((t1 = *++ptr) == TOK_NOSUBST)
                                ;
                        �� (t1 && t1 != TOK_PPJOIN) {
                                TOK_GET(&t1, &ptr, &cv1);
                                �� (t != TOK_PLCHLDR || t1 != TOK_PLCHLDR) {
                                        �� (paste_tokens(t, &cval, t1, &cv1)) {
                                                t = tok, cval = tokc;
                                        } �� {
                                                tok_str_add2(&macro_str1, t, &cval);
                                                t = t1, cval = cv1;
                                        }
                                }
                        }
                }
                �� (t == TOK_NOSUBST) {
                        �� (start_of_nosubsts < 0)
                                start_of_nosubsts = macro_str1.len;
                } �� {
                        start_of_nosubsts = -1;
                }
                tok_str_add2(&macro_str1, t, &cval);
        }
        tok_str_add(&macro_str1, 0);
        //tok_print(" ###", macro_str1.str);
        ���� macro_str1.str;
}

/* peek or read [ws_str == NULL] next token from function macro call,
   walking up macro levels up to the file if necessary */
��̬ �� next_argstream(Sym **nested_list, TokenString *ws_str)
{
        �� t;
        ���� �� *p;
        Sym *sa;

        ���� (;;) {
                �� (macro_ptr) {
                        p = macro_ptr, t = *p;
                        �� (ws_str) {
                                �� (is_space(t) || TOK_LINEFEED == t || TOK_PLCHLDR == t)
                                        tok_str_add(ws_str, t), t = *++p;
                        }
                        �� (t == 0) {
                                end_macro();
                                /* also, end of scope for nested defined symbol */
                                sa = *nested_list;
                                �� (sa && sa->v == 0)
                                        sa = sa->prev;
                                �� (sa)
                                        sa->v = 0;
                                ����;
                        }
                } �� {
                        ch = handle_eob();
                        �� (ws_str) {
                                �� (is_space(ch) || ch == '\n' || ch == '/') {
                                        �� (ch == '/') {
                                                �� c;
                                                uint8_t *p = file->buf_ptr;
                                                PEEKC(c, p);
                                                �� (c == '*') {
                                                        p = parse_comment(p);
                                                        file->buf_ptr = p - 1;
                                                } �� �� (c == '/') {
                                                        p = parse_line_comment(p);
                                                        file->buf_ptr = p - 1;
                                                } ��
                                                        ����;
                                                ch = ' ';
                                        }
                                        �� (ch == '\n')
                                                file->line_num++;
                                        �� (!(ch == '\f' || ch == '\v' || ch == '\r'))
                                                tok_str_add(ws_str, ch);
                                        cinp();
                                }
                        }
                        t = ch;
                }

                �� (ws_str)
                        ���� t;
                next_nomacro_spc();
                ���� tok;
        }
}

/* do macro substitution of current token with macro 's' and add
   result to (tok_str,tok_len). 'nested_list' is the list of all
   macros we got inside to avoid recursing. Return non zero if no
   substitution needs to be done */
��̬ �� macro_subst_tok(
        TokenString *tok_str,
        Sym **nested_list,
        Sym *s)
{
        Sym *args, *sa, *sa1;
        �� parlevel, t, t1, spc;
        TokenString str;
        �� *cstrval;
        CValue cval;
        CString cstr;
        �� buf[32];

        /* if symbol is a macro, prepare substitution */
        /* special macros */
        �� (tok == TOK___LINE__ || tok == TOK___LINE___CN || tok == TOK___COUNTER__ || tok == TOK___COUNTER___CN) {
                t = (tok == TOK___LINE__ || tok == TOK___LINE___CN) ? file->line_num : pp_counter++;
                snprintf(buf, �󳤶�(buf), "%d", t);
                cstrval = buf;
                t1 = TOK_PPNUM;
                ��ת add_cstr1;
        } �� �� (tok == TOK___FILE__ || tok == TOK___FILE___CN) {
                cstrval = file->filename;
                ��ת add_cstr;
        } �� �� (tok == TOK___DATE__ || tok == TOK___DATE___CN || tok == TOK___TIME__ || tok == TOK___TIME___CN) {
                time_t ti;
                �ṹ tm *tm;

                time(&ti);
                tm = localtime(&ti);
                �� (tok == TOK___DATE__ || tok == TOK___DATE___CN) {
                        snprintf(buf, �󳤶�(buf), "%s %2d %d",
                                         ab_month_name[tm->tm_mon], tm->tm_mday, tm->tm_year + 1900);
                } �� {
                        snprintf(buf, �󳤶�(buf), "%02d:%02d:%02d",
                                         tm->tm_hour, tm->tm_min, tm->tm_sec);
                }
                cstrval = buf;
        add_cstr:
                t1 = TOK_STR;
        add_cstr1:
                cstr_new(&cstr);
                cstr_cat(&cstr, cstrval, 0);
                cval.str.size = cstr.size;
                cval.str.data = cstr.data;
                tok_str_add2(tok_str, t1, &cval);
                cstr_free(&cstr);
        } �� �� (s->d) {
                �� saved_parse_flags = parse_flags;
                �� *joined_str = NULL;
                �� *mstr = s->d;

                �� (s->type.t == MACRO_FUNC) {
                        /* whitespace between macro name and argument list */
                        TokenString ws_str;
                        tok_str_new(&ws_str);

                        spc = 0;
                        parse_flags |= PARSE_FLAG_SPACES | PARSE_FLAG_LINEFEED
                                | PARSE_FLAG_ACCEPT_STRAYS;

                        /* get next token from argument stream */
                        t = next_argstream(nested_list, &ws_str);
                        �� (t != '(') {
                                /* not a macro substitution after all, restore the
                                 * macro token plus all whitespace we've read.
                                 * whitespace is intentionally not merged to preserve
                                 * newlines. */
                                parse_flags = saved_parse_flags;
                                tok_str_add(tok_str, tok);
                                �� (parse_flags & PARSE_FLAG_SPACES) {
                                        �� i;
                                        ���� (i = 0; i < ws_str.len; i++)
                                                tok_str_add(tok_str, ws_str.str[i]);
                                }
                                tok_str_free_str(ws_str.str);
                                ���� 0;
                        } �� {
                                tok_str_free_str(ws_str.str);
                        }
                        ���� {
                                next_nomacro(); /* eat '(' */
                        } �� (tok == TOK_PLCHLDR);

                        /* argument macro */
                        args = NULL;
                        sa = s->next;
                        /* NOTE: empty args are allowed, except if no args */
                        ����(;;) {
                                ���� {
                                        next_argstream(nested_list, NULL);
                                } �� (is_space(tok) || TOK_LINEFEED == tok);
        empty_arg:
                                /* handle '()' case */
                                �� (!args && !sa && tok == ')')
                                        ����;
                                �� (!sa)
                                        tcc_error("macro '%s' used with too many args",
                                                  get_tok_str(s->v, 0));
                                tok_str_new(&str);
                                parlevel = spc = 0;
                                /* NOTE: non zero sa->t indicates VA_ARGS */
                                �� ((parlevel > 0 ||
                                                (tok != ')' &&
                                                 (tok != ',' || sa->type.t)))) {
                                        �� (tok == TOK_EOF || tok == 0)
                                                ����;
                                        �� (tok == '(')
                                                parlevel++;
                                        �� �� (tok == ')')
                                                parlevel--;
                                        �� (tok == TOK_LINEFEED)
                                                tok = ' ';
                                        �� (!check_space(tok, &spc))
                                                tok_str_add2(&str, tok, &tokc);
                                        next_argstream(nested_list, NULL);
                                }
                                �� (parlevel)
                                        expect(")");
                                str.len -= spc;
                                tok_str_add(&str, -1);
                                tok_str_add(&str, 0);
                                sa1 = sym_push2(&args, sa->v & ~SYM_FIELD, sa->type.t, 0);
                                sa1->d = str.str;
                                sa = sa->next;
                                �� (tok == ')') {
                                        /* special case for gcc var args: add an empty
                                           var arg argument if it is omitted */
                                        �� (sa && sa->type.t && gnu_ext)
                                                ��ת empty_arg;
                                        ����;
                                }
                                �� (tok != ',')
                                        expect(",");
                        }
                        �� (sa) {
                                tcc_error("macro '%s' used with too few args",
                                          get_tok_str(s->v, 0));
                        }

                        parse_flags = saved_parse_flags;

                        /* now subst each arg */
                        mstr = macro_arg_subst(nested_list, mstr, args);
                        /* free memory */
                        sa = args;
                        �� (sa) {
                                sa1 = sa->prev;
                                tok_str_free_str(sa->d);
                                �� (sa->next) {
                                        tok_str_free_str(sa->next->d);
                                        sym_free(sa->next);
                                }
                                sym_free(sa);
                                sa = sa1;
                        }
                }

                sym_push2(nested_list, s->v, 0, 0);
                parse_flags = saved_parse_flags;
                joined_str = macro_twosharps(mstr);
                macro_subst(tok_str, nested_list, joined_str ? joined_str : mstr);

                /* pop nested defined symbol */
                sa1 = *nested_list;
                *nested_list = sa1->prev;
                sym_free(sa1);
                �� (joined_str)
                        tok_str_free_str(joined_str);
                �� (mstr != s->d)
                        tok_str_free_str(mstr);
        }
        ���� 0;
}

/* do macro substitution of macro_str and add result to
   (tok_str,tok_len). 'nested_list' is the list of all macros we got
   inside to avoid recursing. */
��̬ �� macro_subst(
        TokenString *tok_str,
        Sym **nested_list,
        ���� �� *macro_str
        )
{
        Sym *s;
        �� t, spc, nosubst;
        CValue cval;

        spc = nosubst = 0;

        �� (1) {
                TOK_GET(&t, &macro_str, &cval);
                �� (t <= 0)
                        ����;

                �� (t >= TOK_IDENT && 0 == nosubst) {
                        s = define_find(t);
                        �� (s == NULL)
                                ��ת no_subst;

                        /* if nested substitution, do nothing */
                        �� (sym_find2(*nested_list, t)) {
                                /* and mark it as TOK_NOSUBST, so it doesn't get subst'd again */
                                tok_str_add2(tok_str, TOK_NOSUBST, NULL);
                                ��ת no_subst;
                        }

                        {
                                TokenString str;
                                str.str = (��*)macro_str;
                                begin_macro(&str, 2);

                                tok = t;
                                macro_subst_tok(tok_str, nested_list, s);

                                �� (str.alloc == 3) {
                                        /* already finished by reading function macro arguments */
                                        ����;
                                }

                                macro_str = macro_ptr;
                                end_macro ();
                        }
                        �� (tok_str->len)
                                spc = is_space(t = tok_str->str[tok_str->lastlen]);
                } �� {
                        �� (t == '\\' && !(parse_flags & PARSE_FLAG_ACCEPT_STRAYS))
                                tcc_error("stray '\\' in program");
no_subst:
                        �� (!check_space(t, &spc))
                                tok_str_add2(tok_str, t, &cval);

                        �� (nosubst) {
                                �� (nosubst > 1 && (spc || (++nosubst == 3 && t == '(')))
                                        ����;
                                nosubst = 0;
                        }
                        �� (t == TOK_NOSUBST)
                                nosubst = 1;
                }
                /* GCC supports 'defined' as result of a macto substitution */
                �� ((t == TOK_DEFINED || t == TOK_DEFINED_CN) && pp_expr)
                        nosubst = 2;
        }
}

/* return next token with macro substitution */
ST_FUNC �� next(��)
{
 redo:
        �� (parse_flags & PARSE_FLAG_SPACES)
                next_nomacro_spc();
        ��
                next_nomacro();

        �� (macro_ptr) {
                �� (tok == TOK_NOSUBST || tok == TOK_PLCHLDR) {
                /* discard preprocessor markers */
                        ��ת redo;
                } �� �� (tok == 0) {
                        /* end of macro or unget token string */
                        end_macro();
                        ��ת redo;
                }
        } �� �� (tok >= TOK_IDENT && (parse_flags & PARSE_FLAG_PREPROCESS)) {
                Sym *s;
                /* if reading from file, try to substitute macros */
                s = define_find(tok);
                �� (s) {
                        Sym *nested_list = NULL;
                        tokstr_buf.len = 0;
                        macro_subst_tok(&tokstr_buf, &nested_list, s);
                        tok_str_add(&tokstr_buf, 0);
                        begin_macro(&tokstr_buf, 2);
                        ��ת redo;
                }
        }
        /* convert preprocessor tokens into C tokens */
        �� (tok == TOK_PPNUM) {
                ��  (parse_flags & PARSE_FLAG_TOK_NUM)
                        parse_number((�� *)tokc.str.data);
        } �� �� (tok == TOK_PPSTR) {
                �� (parse_flags & PARSE_FLAG_TOK_STR)
                        parse_string((�� *)tokc.str.data, tokc.str.size - 1);
        }
}

/* push back current token and set current token to 'last_tok'. Only
   identifier case handled for labels. */
ST_INLN �� unget_tok(�� last_tok)
{

        TokenString *str = tok_str_alloc();
        tok_str_add2(str, tok, &tokc);
        tok_str_add(str, 0);
        begin_macro(str, 1);
        tok = last_tok;
}

ST_FUNC �� preprocess_start(TCCState *s1, �� is_asm)
{
        �� *buf;

        s1->include_stack_ptr = s1->include_stack;
        s1->ifdef_stack_ptr = s1->ifdef_stack;
        file->ifdef_stack_ptr = s1->ifdef_stack_ptr;
        pp_expr = 0;
        pp_counter = 0;
        pp_debug_tok = pp_debug_symv = 0;
        pp_once++;
        pvtop = vtop = vstack - 1;
        s1->pack_stack[0] = 0;
        s1->pack_stack_ptr = s1->pack_stack;

        set_idnum('$', s1->dollars_in_identifiers ? IS_ID : 0);
        set_idnum('.', is_asm ? IS_ID : 0);

        buf = tcc_malloc(3 + strlen(file->filename));
        sprintf(buf, "\"%s\"", file->filename);
        tcc_define_symbol(s1, "__BASE_FILE__", buf);
        tcc_free(buf);

        �� (s1->nb_cmd_include_files) {
                CString cstr;
                �� i;
                cstr_new(&cstr);
                ���� (i = 0; i < s1->nb_cmd_include_files; i++) {
                        cstr_cat(&cstr, "#include \"", -1);
                        cstr_cat(&cstr, s1->cmd_include_files[i], -1);
                        cstr_cat(&cstr, "\"\n", -1);
                }
                *s1->include_stack_ptr++ = file;
                tcc_open_bf(s1, "<command line>", cstr.size);
                memcpy(file->buffer, cstr.data, cstr.size);
                cstr_free(&cstr);
        }

        �� (is_asm)
                tcc_define_symbol(s1, "__ASSEMBLER__", NULL);

        parse_flags = is_asm ? PARSE_FLAG_ASM_FILE : 0;
        tok_flags = TOK_FLAG_BOL | TOK_FLAG_BOF;
}

/* cleanup from error/setjmp */
ST_FUNC �� preprocess_end(TCCState *s1)
{
        �� (macro_stack)
                end_macro();
        macro_ptr = NULL;
}

ST_FUNC �� tccpp_new(TCCState *s)
{
        �� i, c;
        ���� �� *p, *r;

        /* might be used in error() before preprocess_start() */
        s->include_stack_ptr = s->include_stack;
        s->ppfp = stdout;

        /* init isid table */
        ����(i = CH_EOF; i<128; i++)
                set_idnum(i,
                        is_space(i) ? IS_SPC
                        : isid(i) ? IS_ID
                        : isnum(i) ? IS_NUM
                        : 0);

        ����(i = 128; i<256; i++)
                set_idnum(i, IS_ID);

        /* init allocators */
        tal_new(&toksym_alloc, TOKSYM_TAL_LIMIT, TOKSYM_TAL_SIZE);
        tal_new(&tokstr_alloc, TOKSTR_TAL_LIMIT, TOKSTR_TAL_SIZE);
        tal_new(&cstr_alloc, CSTR_TAL_LIMIT, CSTR_TAL_SIZE);

        memset(hash_ident, 0, TOK_HASH_SIZE * �󳤶�(TokenSym *));
        cstr_new(&cstr_buf);
        cstr_realloc(&cstr_buf, STRING_MAX_SIZE);
        tok_str_new(&tokstr_buf);
        tok_str_realloc(&tokstr_buf, TOKSTR_MAX_SIZE);

        tok_ident = TOK_IDENT;
        p = tcc_keywords;
        �� (*p) {
                r = p;
                ����(;;) {
                        c = *r++;
                        �� (c == '\0')
                                ����;
                }
                tok_alloc(p, r - p - 1);
                p = r;
        }
}

ST_FUNC �� tccpp_delete(TCCState *s)
{
        �� i, n;

        /* free -D and compiler defines */
        free_defines(NULL);

        /* free tokens */
        n = tok_ident - TOK_IDENT;
        ����(i = 0; i < n; i++)
                tal_free(toksym_alloc, table_ident[i]);
        tcc_free(table_ident);
        table_ident = NULL;

        /* free static buffers */
        cstr_free(&tokcstr);
        cstr_free(&cstr_buf);
        cstr_free(&macro_equal_buf);
        tok_str_free_str(tokstr_buf.str);

        /* free allocators */
        tal_delete(toksym_alloc);
        toksym_alloc = NULL;
        tal_delete(tokstr_alloc);
        tokstr_alloc = NULL;
        tal_delete(cstr_alloc);
        cstr_alloc = NULL;
}

/* ------------------------------------------------------------------------- */
/* tcc -E [-P[1]] [-dD} support */

��̬ �� tok_print(���� �� *msg, ���� �� *str)
{
        FILE *fp;
        �� t, s = 0;
        CValue cval;

        fp = tcc_state->ppfp;
        fprintf(fp, "%s", msg);
        �� (str) {
                TOK_GET(&t, &str, &cval);
                �� (!t)
                        ����;
                fprintf(fp, " %s" + s, get_tok_str(t, &cval)), s = 1;
        }
        fprintf(fp, "\n");
}

��̬ �� pp_line(TCCState *s1, BufferedFile *f, �� level)
{
        �� d = f->line_num - f->line_ref;

        �� (s1->dflag & 4)
                ����;

        �� (s1->Pflag == LINE_MACRO_OUTPUT_FORMAT_NONE) {
                ;
        } �� �� (level == 0 && f->line_ref && d < 8) {
                �� (d > 0)
                        fputs("\n", s1->ppfp), --d;
        } �� �� (s1->Pflag == LINE_MACRO_OUTPUT_FORMAT_STD) {
                fprintf(s1->ppfp, "#line %d \"%s\"\n", f->line_num, f->filename);
        } �� {
                fprintf(s1->ppfp, "# %d \"%s\"%s\n", f->line_num, f->filename,
                        level > 0 ? " 1" : level < 0 ? " 2" : "");
        }
        f->line_ref = f->line_num;
}

��̬ �� define_print(TCCState *s1, �� v)
{
        FILE *fp;
        Sym *s;

        s = define_find(v);
        �� (NULL == s || NULL == s->d)
                ����;

        fp = s1->ppfp;
        fprintf(fp, "#define %s", get_tok_str(v, NULL));
        �� (s->type.t == MACRO_FUNC) {
                Sym *a = s->next;
                fprintf(fp,"(");
                �� (a)
                        ���� (;;) {
                                fprintf(fp,"%s", get_tok_str(a->v & ~SYM_FIELD, NULL));
                                �� (!(a = a->next))
                                        ����;
                                fprintf(fp,",");
                        }
                fprintf(fp,")");
        }
        tok_print("", s->d);
}

��̬ �� pp_debug_defines(TCCState *s1)
{
        �� v, t;
        ���� �� *vs;
        FILE *fp;

        t = pp_debug_tok;
        �� (t == 0)
                ����;

        file->line_num--;
        pp_line(s1, file, 0);
        file->line_ref = ++file->line_num;

        fp = s1->ppfp;
        v = pp_debug_symv;
        vs = get_tok_str(v, NULL);
        �� (t == TOK_DEFINE || t == TOK_DEFINE_CN) {
                define_print(s1, v);
        } �� �� (t == TOK_UNDEF || t == TOK_UNDEF_CN) {
                fprintf(fp, "#undef %s\n", vs);
        } �� �� (t == TOK_push_macro) {
                fprintf(fp, "#pragma push_macro(\"%s\")\n", vs);
        } �� �� (t == TOK_pop_macro) {
                fprintf(fp, "#pragma pop_macro(\"%s\")\n", vs);
        }
        pp_debug_tok = 0;
}

��̬ �� pp_debug_builtins(TCCState *s1)
{
        �� v;
        ���� (v = TOK_IDENT; v < tok_ident; ++v)
                define_print(s1, v);
}

/* Add a space between tokens a and b to avoid unwanted textual pasting */
��̬ �� pp_need_space(�� a, �� b)
{
        ���� 'E' == a ? '+' == b || '-' == b
                : '+' == a ? TOK_INC == b || '+' == b
                : '-' == a ? TOK_DEC == b || '-' == b
                : a >= TOK_IDENT ? b >= TOK_IDENT
                : a == TOK_PPNUM ? b >= TOK_IDENT
                : 0;
}

/* maybe hex like 0x1e */
��̬ �� pp_check_he0xE(�� t, ���� �� *p)
{
        �� (t == TOK_PPNUM && toup(strchr(p, 0)[-1]) == 'E')
                ���� 'E';
        ���� t;
}

/* Preprocess the current file */
ST_FUNC �� tcc_preprocess(TCCState *s1)
{
        BufferedFile **iptr;
        �� token_seen, spcs, level;
        ���� �� *p;
        �� white[400];

        parse_flags = PARSE_FLAG_PREPROCESS
                                | (parse_flags & PARSE_FLAG_ASM_FILE)
                                | PARSE_FLAG_LINEFEED
                                | PARSE_FLAG_SPACES
                                | PARSE_FLAG_ACCEPT_STRAYS
                                ;
        /* Credits to Fabrice Bellard's initial revision to demonstrate its
           capability to compile and run itself, provided all numbers are
           given as decimals. tcc -E -P10 will do. */
        �� (s1->Pflag == LINE_MACRO_OUTPUT_FORMAT_P10)
                parse_flags |= PARSE_FLAG_TOK_NUM, s1->Pflag = 1;

#�綨�� PP_BENCH
        /* for PP benchmarks */
        ���� next(); �� (tok != TOK_EOF);
        ���� 0;
#����

        �� (s1->dflag & 1) {
                pp_debug_builtins(s1);
                s1->dflag &= ~1;
        }

        token_seen = TOK_LINEFEED, spcs = 0;
        pp_line(s1, file, 0);
        ���� (;;) {
                iptr = s1->include_stack_ptr;
                next();
                �� (tok == TOK_EOF)
                        ����;

                level = s1->include_stack_ptr - iptr;
                �� (level) {
                        �� (level > 0)
                                pp_line(s1, *iptr, 0);
                        pp_line(s1, file, level);
                }
                �� (s1->dflag & 7) {
                        pp_debug_defines(s1);
                        �� (s1->dflag & 4)
                                ����;
                }

                �� (is_space(tok)) {
                        �� (spcs < �󳤶� white - 1)
                                white[spcs++] = tok;
                        ����;
                } �� �� (tok == TOK_LINEFEED) {
                        spcs = 0;
                        �� (token_seen == TOK_LINEFEED)
                                ����;
                        ++file->line_ref;
                } �� �� (token_seen == TOK_LINEFEED) {
                        pp_line(s1, file, 0);
                } �� �� (spcs == 0 && pp_need_space(token_seen, tok)) {
                        white[spcs++] = ' ';
                }

                white[spcs] = 0, fputs(white, s1->ppfp), spcs = 0;
                fputs(p = get_tok_str(tok, &tokc), s1->ppfp);
                token_seen = pp_check_he0xE(tok, p);
        }
        ���� 0;
}

/* ------------------------------------------------------------------------- */
