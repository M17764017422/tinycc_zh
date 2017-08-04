/* -------------------------------------------------------------- */
/*
 *  TCC - Tiny C Compiler
 *
 *  tcctools.c - extra tools and and -m32/64 support
 *
 */

/* -------------------------------------------------------------- */
/*
 * This program is for making libtcc1.a without ar
 * tiny_libmaker - tiny elf lib maker
 * usage: tiny_libmaker [lib] files...
 * Copyright (c) 2007 Timppa
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#���� "tcc.h"

//#���� ARMAG  "!<arch>\n"
#���� ARFMAG "`\n"

���Ͷ��� �ṹ {
    �� ar_name[16];
    �� ar_date[12];
    �� ar_uid[6];
    �� ar_gid[6];
    �� ar_mode[8];
    �� ar_size[10];
    �� ar_fmag[2];
} ArHdr;

��̬ �޷� �� le2belong(�޷� �� ul) {
    ���� ((ul & 0xFF0000)>>8)+((ul & 0xFF000000)>>24) +
        ((ul & 0xFF)<<24)+((ul & 0xFF00)<<8);
}

/* Returns 1 if s contains any of the chars of list, else 0 */
��̬ �� contains_any(���� �� *s, ���� �� *list) {
  ���� �� *l;
  ���� (; *s; s++) {
      ���� (l = list; *l; l++) {
          �� (*s == *l)
              ���� 1;
      }
  }
  ���� 0;
}

��̬ �� ar_usage(�� ret) {
    fprintf(stderr, "usage: tcc -ar [rcsv] lib file...\n");
    fprintf(stderr, "create library ([abdioptxN] not supported).\n");
    ���� ret;
}

ST_FUNC �� tcc_tool_ar(TCCState *s1, �� argc, �� **argv)
{
    ��̬ ArHdr arhdr = {
        "/               ",
        "            ",
        "0     ",
        "0     ",
        "0       ",
        "          ",
        ARFMAG
        };

    ��̬ ArHdr arhdro = {
        "                ",
        "            ",
        "0     ",
        "0     ",
        "0       ",
        "          ",
        ARFMAG
        };

    FILE *fi, *fh = NULL, *fo = NULL;
    ElfW(Ehdr) *ehdr;
    ElfW(Shdr) *shdr;
    ElfW(Sym) *sym;
    �� i, fsize, i_lib, i_obj;
    �� *buf, *shstr, *symtab = NULL, *strtab = NULL;
    �� symtabsize = 0;//, strtabsize = 0;
    �� *anames = NULL;
    �� *afpos = NULL;
    �� istrlen, strpos = 0, fpos = 0, funccnt = 0, funcmax, hofs;
    �� tfile[260], stmp[20];
    �� *file, *name;
    �� ret = 2;
    ���� �� *ops_conflict = "habdioptxN";  // unsupported but destructive if ignored.
    �� verbose = 0;

    i_lib = 0; i_obj = 0;  // will hold the index of the lib and first obj
    ���� (i = 1; i < argc; i++) {
        ���� �� *a = argv[i];
        �� (*a == '-' && strstr(a, "."))
            ret = 1; // -x.y is always invalid (same as gnu ar)
        �� ((*a == '-') || (i == 1 && !strstr(a, "."))) {  // options argument
            �� (contains_any(a, ops_conflict))
                ret = 1;
            �� (strstr(a, "v"))
                verbose = 1;
        } �� {  // lib or obj files: don't abort - keep validating all args.
            �� (!i_lib)  // first file is the lib
                i_lib = i;
            �� �� (!i_obj)  // second file is the first obj
                i_obj = i;
        }
    }

    �� (!i_obj)  // i_obj implies also i_lib. we require both.
        ret = 1;

    �� (ret == 1)
        ���� ar_usage(ret);

    �� ((fh = fopen(argv[i_lib], "wb")) == NULL)
    {
        fprintf(stderr, "tcc: ar: can't open file %s \n", argv[i_lib]);
        ��ת the_end;
    }

    sprintf(tfile, "%s.tmp", argv[i_lib]);
    �� ((fo = fopen(tfile, "wb+")) == NULL)
    {
        fprintf(stderr, "tcc: ar: can't create temporary file %s\n", tfile);
        ��ת the_end;
    }

    funcmax = 250;
    afpos = tcc_realloc(NULL, funcmax * �󳤶� *afpos); // 250 func
    memcpy(&arhdro.ar_mode, "100666", 6);

    // i_obj = first input object file
    �� (i_obj < argc)
    {
        �� (*argv[i_obj] == '-') {  // by now, all options start with '-'
            i_obj++;
            ����;
        }
        �� ((fi = fopen(argv[i_obj], "rb")) == NULL) {
            fprintf(stderr, "tcc: ar: can't open file %s \n", argv[i_obj]);
            ��ת the_end;
        }
        �� (verbose)
            printf("a - %s\n", argv[i_obj]);

        fseek(fi, 0, SEEK_END);
        fsize = ftell(fi);
        fseek(fi, 0, SEEK_SET);
        buf = tcc_malloc(fsize + 1);
        fread(buf, fsize, 1, fi);
        fclose(fi);

        // elf header
        ehdr = (ElfW(Ehdr) *)buf;
        �� (ehdr->e_ident[4] != ELFCLASSW)
        {
            fprintf(stderr, "tcc: ar: Unsupported Elf Class: %s\n", argv[i_obj]);
            ��ת the_end;
        }

        shdr = (ElfW(Shdr) *) (buf + ehdr->e_shoff + ehdr->e_shstrndx * ehdr->e_shentsize);
        shstr = (�� *)(buf + shdr->sh_offset);
        ���� (i = 0; i < ehdr->e_shnum; i++)
        {
            shdr = (ElfW(Shdr) *) (buf + ehdr->e_shoff + i * ehdr->e_shentsize);
            �� (!shdr->sh_offset)
                ����;
            �� (shdr->sh_type == SHT_SYMTAB)
            {
                symtab = (�� *)(buf + shdr->sh_offset);
                symtabsize = shdr->sh_size;
            }
            �� (shdr->sh_type == SHT_STRTAB)
            {
                �� (!strcmp(shstr + shdr->sh_name, ".strtab"))
                {
                    strtab = (�� *)(buf + shdr->sh_offset);
                    //strtabsize = shdr->sh_size;
                }
            }
        }

        �� (symtab && symtabsize)
        {
            �� nsym = symtabsize / �󳤶�(ElfW(Sym));
            //printf("symtab: info size shndx name\n");
            ���� (i = 1; i < nsym; i++)
            {
                sym = (ElfW(Sym) *) (symtab + i * �󳤶�(ElfW(Sym)));
                �� (sym->st_shndx &&
                    (sym->st_info == 0x10
                    || sym->st_info == 0x11
                    || sym->st_info == 0x12
                    )) {
                    //printf("symtab: %2Xh %4Xh %2Xh %s\n", sym->st_info, sym->st_size, sym->st_shndx, strtab + sym->st_name);
                    istrlen = strlen(strtab + sym->st_name)+1;
                    anames = tcc_realloc(anames, strpos+istrlen);
                    strcpy(anames + strpos, strtab + sym->st_name);
                    strpos += istrlen;
                    �� (++funccnt >= funcmax) {
                        funcmax += 250;
                        afpos = tcc_realloc(afpos, funcmax * �󳤶� *afpos); // 250 func more
                    }
                    afpos[funccnt] = fpos;
                }
            }
        }

        file = argv[i_obj];
        ���� (name = strchr(file, 0);
             name > file && name[-1] != '/' && name[-1] != '\\';
             --name);
        istrlen = strlen(name);
        �� (istrlen >= �󳤶�(arhdro.ar_name))
            istrlen = �󳤶�(arhdro.ar_name) - 1;
        memset(arhdro.ar_name, ' ', �󳤶�(arhdro.ar_name));
        memcpy(arhdro.ar_name, name, istrlen);
        arhdro.ar_name[istrlen] = '/';
        sprintf(stmp, "%-10d", fsize);
        memcpy(&arhdro.ar_size, stmp, 10);
        fwrite(&arhdro, �󳤶�(arhdro), 1, fo);
        fwrite(buf, fsize, 1, fo);
        tcc_free(buf);
        i_obj++;
        fpos += (fsize + �󳤶�(arhdro));
    }
    hofs = 8 + �󳤶�(arhdr) + strpos + (funccnt+1) * �󳤶�(��);
    fpos = 0;
    �� ((hofs & 1)) // align
        hofs++, fpos = 1;
    // write header
    fwrite("!<arch>\n", 8, 1, fh);
    sprintf(stmp, "%-10d", (��)(strpos + (funccnt+1) * �󳤶�(��)));
    memcpy(&arhdr.ar_size, stmp, 10);
    fwrite(&arhdr, �󳤶�(arhdr), 1, fh);
    afpos[0] = le2belong(funccnt);
    ���� (i=1; i<=funccnt; i++)
        afpos[i] = le2belong(afpos[i] + hofs);
    fwrite(afpos, (funccnt+1) * �󳤶�(��), 1, fh);
    fwrite(anames, strpos, 1, fh);
    �� (fpos)
        fwrite("", 1, 1, fh);
    // write objects
    fseek(fo, 0, SEEK_END);
    fsize = ftell(fo);
    fseek(fo, 0, SEEK_SET);
    buf = tcc_malloc(fsize + 1);
    fread(buf, fsize, 1, fo);
    fwrite(buf, fsize, 1, fh);
    tcc_free(buf);
    ret = 0;
the_end:
    �� (anames)
        tcc_free(anames);
    �� (afpos)
        tcc_free(afpos);
    �� (fh)
        fclose(fh);
    �� (fo)
        fclose(fo), remove(tfile);
    ���� ret;
}

/* -------------------------------------------------------------- */
/*
 * tiny_impdef creates an export definition file (.def) from a dll
 * on MS-Windows. Usage: tiny_impdef library.dll [-o outputfile]"
 *
 *  Copyright (c) 2005,2007 grischka
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#�綨�� TCC_TARGET_PE

ST_FUNC �� tcc_tool_impdef(TCCState *s1, �� argc, �� **argv)
{
    �� ret, v, i;
    �� infile[260];
    �� outfile[260];

    ���� �� *file;
    �� *p, *q;
    FILE *fp, *op;

#�綨�� _WIN32
    �� path[260];
#����

    infile[0] = outfile[0] = 0;
    fp = op = NULL;
    ret = 1;
    p = NULL;
    v = 0;

    ���� (i = 1; i < argc; ++i) {
        ���� �� *a = argv[i];
        �� ('-' == a[0]) {
            �� (0 == strcmp(a, "-v")) {
                v = 1;
            } �� �� (0 == strcmp(a, "-o")) {
                �� (++i == argc)
                    ��ת usage;
                strcpy(outfile, argv[i]);
            } ��
                ��ת usage;
        } �� �� (0 == infile[0])
            strcpy(infile, a);
        ��
            ��ת usage;
    }

    �� (0 == infile[0]) {
usage:
        fprintf(stderr,
            "usage: tcc -impdef library.dll [-v] [-o outputfile]\n"
            "create export definition file (.def) from dll\n"
            );
        ��ת the_end;
    }

    �� (0 == outfile[0]) {
        strcpy(outfile, tcc_basename(infile));
        q = strrchr(outfile, '.');
        �� (NULL == q)
            q = strchr(outfile, 0);
        strcpy(q, ".def");
    }

    file = infile;
#�綨�� _WIN32
    �� (SearchPath(NULL, file, ".dll", �󳤶� path, path, NULL))
        file = path;
#����
    ret = tcc_get_dllexports(file, &p);
    �� (ret || !p) {
        fprintf(stderr, "tcc: impdef: %s '%s'\n",
            ret == -1 ? "can't find file" :
            ret ==  1 ? "can't read symbols" :
            ret ==  0 ? "no symbols found in" :
            "unknown file type", file);
        ret = 1;
        ��ת the_end;
    }

    �� (v)
        printf("-> %s\n", file);

    op = fopen(outfile, "w");
    �� (NULL == op) {
        fprintf(stderr, "tcc: impdef: could not create output file: %s\n", outfile);
        ��ת the_end;
    }

    fprintf(op, "LIBRARY %s\n\nEXPORTS\n", tcc_basename(file));
    ���� (q = p, i = 0; *q; ++i) {
        fprintf(op, "%s\n", q);
        q += strlen(q) + 1;
    }

    �� (v)
        printf("<- %s (%d symbol%s)\n", outfile, i, &"s"[i<2]);

    ret = 0;

the_end:
    /* cannot free memory received from tcc_get_dllexports
       if it came from a dll */
    /* �� (p)
        tcc_free(p); */
    �� (fp)
        fclose(fp);
    �� (op)
        fclose(op);
    ���� ret;
}

#���� /* TCC_TARGET_PE */

/* -------------------------------------------------------------- */
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

/* re-execute the i386/x86_64 cross-compilers with tcc -m32/-m64: */

#�� !�Ѷ��� TCC_TARGET_I386 && !�Ѷ��� TCC_TARGET_X86_64

ST_FUNC �� tcc_tool_cross(TCCState *s, �� **argv, �� option)
{
    tcc_error("-m%d not implemented.", option);
}

#��
#�綨�� _WIN32
#���� <process.h>

��̬ �� *str_replace(���� �� *str, ���� �� *p, ���� �� *r)
{
    ���� �� *s, *s0;
    �� *d, *d0;
    �� sl, pl, rl;

    sl = strlen(str);
    pl = strlen(p);
    rl = strlen(r);
    ���� (d0 = NULL;; d0 = tcc_malloc(sl + 1)) {
        ���� (d = d0, s = str; s0 = s, s = strstr(s, p), s; s += pl) {
            �� (d) {
                memcpy(d, s0, sl = s - s0), d += sl;
                memcpy(d, r, rl), d += rl;
            } ��
                sl += rl - pl;
        }
        �� (d) {
            strcpy(d, s0);
            ���� d0;
        }
    }
}

��̬ �� execvp_win32(���� �� *prog, �� **argv)
{
    �� ret; �� **p;
    /* replace all " by \" */
    ���� (p = argv; *p; ++p)
        �� (strchr(*p, '"'))
            *p = str_replace(*p, "\"", "\\\"");
    ret = _spawnvp(P_NOWAIT, prog, (���� �� *����*)argv);
    �� (-1 == ret)
        ���� ret;
    _cwait(&ret, ret, WAIT_CHILD);
    exit(ret);
}
#���� execvp execvp_win32
#���� /* _WIN32 */

ST_FUNC �� tcc_tool_cross(TCCState *s, �� **argv, �� target)
{
    �� program[4096];
    �� *a0 = argv[0];
    �� prefix = tcc_basename(a0) - a0;

    snprintf(program, �󳤶� program,
        "%.*s%s"
#�綨�� TCC_TARGET_PE
        "-win32"
#����
        "-tcc"
#�綨�� _WIN32
        ".exe"
#����
        , prefix, a0, target == 64 ? "x86_64" : "i386");

    �� (strcmp(a0, program))
        execvp(argv[0] = program, argv);
    tcc_error("could not run '%s'", program);
}

#���� /* TCC_TARGET_I386 && TCC_TARGET_X86_64 */
/* -------------------------------------------------------------- */
/* enable commandline wildcard expansion (tcc -o x.exe *.c) */

#�綨�� _WIN32
�� _CRT_glob = 1;
#��δ���� _CRT_glob
�� _dowildcard = 1;
#����
#����

/* -------------------------------------------------------------- */
/* generate xxx.d file */

ST_FUNC �� gen_makedeps(TCCState *s, ���� �� *target, ���� �� *filename)
{
    FILE *depout;
    �� buf[1024];
    �� i;

    �� (!filename) {
        /* compute filename automatically: dir/file.o -> dir/file.d */
        snprintf(buf, �󳤶� buf, "%.*s.d",
            (��)(tcc_fileextension(target) - target), target);
        filename = buf;
    }

    �� (s->verbose)
        printf("<- %s\n", filename);

    /* XXX return err codes instead of error() ? */
    depout = fopen(filename, "w");
    �� (!depout)
        tcc_error("could not open '%s'", filename);

    fprintf(depout, "%s: \\\n", target);
    ���� (i=0; i<s->nb_target_deps; ++i)
        fprintf(depout, " %s \\\n", s->target_deps[i]);
    fprintf(depout, "\n");
    fclose(depout);
}

/* -------------------------------------------------------------- */
