/**************************************************************************/
/*  COFF.H                                                                */
/*     COFF data structures and related definitions used by the linker    */
/**************************************************************************/

/*------------------------------------------------------------------------*/
/*  COFF FILE HEADER                                                      */
/*------------------------------------------------------------------------*/
�ṹ filehdr {
        �޷� ��  f_magic;        /* magic number */
        �޷� ��  f_nscns;        /* number of sections */
        ��            f_timdat;       /* time & date stamp */
        ��            f_symptr;       /* file pointer to symtab */
        ��            f_nsyms;        /* number of symtab entries */
        �޷� ��  f_opthdr;       /* �󳤶�(optional hdr) */
        �޷� ��  f_flags;        /* flags */
        �޷� ��  f_TargetID;     /* for C6x = 0x0099 */
        };

/*------------------------------------------------------------------------*/
/*  File header flags                                                     */
/*------------------------------------------------------------------------*/
#����  F_RELFLG   0x01       /* relocation info stripped from file       */
#����  F_EXEC     0x02       /* file is executable (no unresolved refs)  */
#����  F_LNNO     0x04       /* line numbers stripped from file          */
#����  F_LSYMS    0x08       /* local symbols stripped from file         */
#����  F_GSP10    0x10       /* 34010 version                            */
#����  F_GSP20    0x20       /* 34020 version                            */
#����  F_SWABD    0x40       /* bytes swabbed (in names)                 */
#����  F_AR16WR   0x80       /* byte ordering of an AR16WR (PDP-11)      */
#����  F_LITTLE   0x100      /* byte ordering of an AR32WR (vax)         */
#����  F_BIG      0x200      /* byte ordering of an AR32W (3B, maxi)     */
#����  F_PATCH    0x400      /* contains "patch" list in optional header */
#����  F_NODF     0x400   

#���� F_VERSION    (F_GSP10  | F_GSP20)   
#���� F_BYTE_ORDER (F_LITTLE | F_BIG)
#���� FILHDR  �ṹ filehdr

/* #���� FILHSZ  �󳤶�(FILHDR)  */
#���� FILHSZ  22                /* above rounds to align on 4 bytes which causes problems */

#���� COFF_C67_MAGIC 0x00c2

/*------------------------------------------------------------------------*/
/*  Macros to recognize magic numbers                                     */
/*------------------------------------------------------------------------*/
#���� ISMAGIC(x)      (((�޷� ��)(x))==(�޷� ��)magic)
#���� ISARCHIVE(x)    ((((�޷� ��)(x))==(�޷� ��)ARTYPE))
#���� BADMAGIC(x)     (((�޷� ��)(x) & 0x8080) && !ISMAGIC(x))


/*------------------------------------------------------------------------*/
/*  OPTIONAL FILE HEADER                                                  */
/*------------------------------------------------------------------------*/
���Ͷ��� �ṹ aouthdr {
        ��   magic;          /* see magic.h                          */
        ��   vstamp;         /* version stamp                        */
        ��    tsize;          /* text size in bytes, padded to FW bdry*/
        ��    dsize;          /* initialized data "  "                */
        ��    bsize;          /* uninitialized data "   "             */
        ��    entrypt;        /* entry pt.                            */
        ��    text_start;     /* base of text used for this file      */
        ��    data_start;     /* base of data used for this file      */
} AOUTHDR;

#���� AOUTSZ  �󳤶�(AOUTHDR)

/*----------------------------------------------------------------------*/
/*      When a UNIX aout header is to be built in the optional header,  */
/*      the following magic numbers can appear in that header:          */ 
/*                                                                      */
/*              AOUT1MAGIC : default : readonly sharable text segment   */
/*              AOUT2MAGIC:          : writable text segment            */
/*              PAGEMAGIC  :         : configured for paging            */
/*----------------------------------------------------------------------*/
#���� AOUT1MAGIC 0410
#���� AOUT2MAGIC 0407
#���� PAGEMAGIC  0413


/*------------------------------------------------------------------------*/
/*  COMMON ARCHIVE FILE STRUCTURES                                        */
/*                                                                        */
/*       ARCHIVE File Organization:                                       */
/*       _______________________________________________                  */
/*       |__________ARCHIVE_MAGIC_STRING_______________|                  */
/*       |__________ARCHIVE_FILE_MEMBER_1______________|                  */
/*       |                                             |                  */
/*       |       Archive File Header "ar_hdr"          |                  */
/*       |.............................................|                  */
/*       |       Member Contents                       |                  */
/*       |               1. External symbol directory  |                  */
/*       |               2. Text file                  |                  */
/*       |_____________________________________________|                  */
/*       |________ARCHIVE_FILE_MEMBER_2________________|                  */
/*       |               "ar_hdr"                      |                  */
/*       |.............................................|                  */
/*       |       Member Contents (.o or text file)     |                  */
/*       |_____________________________________________|                  */
/*       |       .               .               .     |                  */
/*       |       .               .               .     |                  */
/*       |       .               .               .     |                  */
/*       |_____________________________________________|                  */
/*       |________ARCHIVE_FILE_MEMBER_n________________|                  */
/*       |               "ar_hdr"                      |                  */
/*       |.............................................|                  */
/*       |               Member Contents               |                  */
/*       |_____________________________________________|                  */
/*                                                                        */
/*------------------------------------------------------------------------*/

#���� COFF_ARMAG   "!<arch>\n"
#���� SARMAG  8
#���� ARFMAG  "`\n"

�ṹ ar_hdr           /* archive file member header - printable ascii */
{
        ��    ar_name[16];    /* file member name - `/' terminated */
        ��    ar_date[12];    /* file member date - decimal */
        ��    ar_uid[6];      /* file member user id - decimal */
        ��    ar_gid[6];      /* file member group id - decimal */
        ��    ar_mode[8];     /* file member mode - octal */
        ��    ar_size[10];    /* file member size - decimal */
        ��    ar_fmag[2];     /* ARFMAG - string to end header */
};


/*------------------------------------------------------------------------*/
/*  SECTION HEADER                                                        */
/*------------------------------------------------------------------------*/
�ṹ scnhdr {
        ��            s_name[8];      /* section name */
        ��            s_paddr;        /* physical address */
        ��            s_vaddr;        /* virtual address */
        ��            s_size;         /* section size */
        ��            s_scnptr;       /* file ptr to raw data for section */
        ��            s_relptr;       /* file ptr to relocation */
        ��            s_lnnoptr;      /* file ptr to line numbers */
        �޷� �� s_nreloc;       /* number of relocation entries */
        �޷� �� s_nlnno;        /* number of line number entries */
        �޷� �� s_flags;        /* flags */
                �޷� ��   s_reserved;     /* reserved byte */
                �޷� ��  s_page;         /* memory page id */
        };

#���� SCNHDR  �ṹ scnhdr
#���� SCNHSZ  �󳤶�(SCNHDR)

/*------------------------------------------------------------------------*/
/* Define constants for names of "special" sections                       */
/*------------------------------------------------------------------------*/
/* #���� _TEXT    ".text" */
#���� _DATA    ".data"
#���� _BSS     ".bss"
#���� _CINIT   ".cinit"
#���� _TV      ".tv"

/*------------------------------------------------------------------------*/
/* The low 4 bits of s_flags is used as a section "type"                  */
/*------------------------------------------------------------------------*/
#���� STYP_REG    0x00  /* "regular" : allocated, relocated, loaded */
#���� STYP_DSECT  0x01  /* "dummy"   : not allocated, relocated, not loaded */
#���� STYP_NOLOAD 0x02  /* "noload"  : allocated, relocated, not loaded */
#���� STYP_GROUP  0x04  /* "grouped" : formed of input sections */
#���� STYP_PAD    0x08  /* "padding" : not allocated, not relocated, loaded */
#���� STYP_COPY   0x10  /* "copy"    : used for C init tables - 
                                                not allocated, relocated,
                                                loaded;  reloc & lineno
                                                entries processed normally */
#���� STYP_TEXT   0x20   /* section contains text only */
#���� STYP_DATA   0x40   /* section contains data only */
#���� STYP_BSS    0x80   /* section contains bss only */

#���� STYP_ALIGN  0x100  /* align flag passed by old version assemblers */
#���� ALIGN_MASK  0x0F00 /* part of s_flags that is used for align vals */
#���� ALIGNSIZE(x) (1 << ((x & ALIGN_MASK) >> 8))


/*------------------------------------------------------------------------*/
/*  RELOCATION ENTRIES                                                    */
/*------------------------------------------------------------------------*/
�ṹ reloc
{
   ��            r_vaddr;        /* (virtual) address of reference */
   ��           r_symndx;       /* index into symbol table */
   �޷� ��  r_disp;         /* additional bits for address calculation */
   �޷� ��  r_type;         /* relocation type */
};

#���� RELOC   �ṹ reloc
#���� RELSZ   10                 /* �󳤶�(RELOC) */

/*--------------------------------------------------------------------------*/
/*   define all relocation types                                            */
/*--------------------------------------------------------------------------*/

#���� R_ABS           0         /* absolute address - no relocation       */
#���� R_DIR16         01        /* UNUSED                                 */
#���� R_REL16         02        /* UNUSED                                 */
#���� R_DIR24         04        /* UNUSED                                 */
#���� R_REL24         05        /* 24 bits, direct                        */
#���� R_DIR32         06        /* UNUSED                                 */
#���� R_RELBYTE      017        /* 8 bits, direct                         */
#���� R_RELWORD      020        /* 16 bits, direct                        */
#���� R_RELLONG      021        /* 32 bits, direct                        */
#���� R_PCRBYTE      022        /* 8 bits, PC-relative                    */
#���� R_PCRWORD      023        /* 16 bits, PC-relative                   */
#���� R_PCRLONG      024        /* 32 bits, PC-relative                   */
#���� R_OCRLONG      030        /* GSP: 32 bits, one's complement direct  */
#���� R_GSPPCR16     031        /* GSP: 16 bits, PC relative (in words)   */
#���� R_GSPOPR32     032        /* GSP: 32 bits, direct big-endian        */
#���� R_PARTLS16     040        /* Brahma: 16 bit offset of 24 bit address*/
#���� R_PARTMS8      041        /* Brahma: 8 bit page of 24 bit address   */
#���� R_PARTLS7      050        /* DSP: 7 bit offset of 16 bit address    */
#���� R_PARTMS9      051        /* DSP: 9 bit page of 16 bit address      */
#���� R_REL13        052        /* DSP: 13 bits, direct                   */


/*------------------------------------------------------------------------*/
/*  LINE NUMBER ENTRIES                                                   */
/*------------------------------------------------------------------------*/
�ṹ lineno
{
        ����
        {
                ��    l_symndx ;      /* sym. table index of function name
                                                iff l_lnno == 0      */
                ��    l_paddr ;       /* (physical) address of line number */
        }               l_addr ;
        �޷� ��  l_lnno ;        /* line number */
};

#���� LINENO  �ṹ lineno
#���� LINESZ  6       /* �󳤶�(LINENO) */


/*------------------------------------------------------------------------*/
/*   STORAGE CLASSES                                                      */
/*------------------------------------------------------------------------*/
#����  C_EFCN          -1    /* physical end of function */
#����  C_NULL          0
#����  C_AUTO          1     /* automatic variable */
#����  C_EXT           2     /* external symbol */
#����  C_STAT          3     /* static */
#����  C_REG           4     /* register variable */
#����  C_EXTDEF        5     /* external definition */
#����  C_LABEL         6     /* label */
#����  C_ULABEL        7     /* undefined label */
#����  C_MOS           8     /* member of structure */
#����  C_ARG           9     /* function argument */
#����  C_STRTAG        10    /* structure tag */
#����  C_MOU           11    /* member of union */
#����  C_UNTAG         12    /* union tag */
#����  C_TPDEF         13    /* type definition */
#���� C_USTATIC        14    /* undefined static */
#����  C_ENTAG         15    /* enumeration tag */
#����  C_MOE           16    /* member of enumeration */
#����  C_REGPARM       17    /* register parameter */
#����  C_FIELD         18    /* bit field */

#����  C_BLOCK         100   /* ".bb" or ".eb" */
#����  C_FCN           101   /* ".bf" or ".ef" */
#����  C_EOS           102   /* end of structure */
#����  C_FILE          103   /* file name */
#����  C_LINE          104   /* dummy sclass for line number entry */
#����  C_ALIAS         105   /* duplicate tag */
#����  C_HIDDEN        106   /* special storage class for external */
                               /* symbols in dmert public libraries  */

/*------------------------------------------------------------------------*/
/*  SYMBOL TABLE ENTRIES                                                  */
/*------------------------------------------------------------------------*/

#����  SYMNMLEN   8      /*  Number of characters in a symbol name */
#����  FILNMLEN   14     /*  Number of characters in a file name */
#����  DIMNUM     4      /*  Number of array dimensions in auxiliary entry */


�ṹ syment
{
        ����
        {
                ��            _n_name[SYMNMLEN];      /* old COFF version */
                �ṹ
                {
                        ��    _n_zeroes;      /* new == 0 */
                        ��    _n_offset;      /* offset into string table */
                } _n_n;
                ��            *_n_nptr[2];    /* allows for overlaying */
        } _n;
        ��                    n_value;        /* value of symbol */
        ��                   n_scnum;        /* section number */
        �޷� ��          n_type;         /* type and derived type */
        ��                    n_sclass;       /* storage class */
        ��                    n_numaux;       /* number of aux. entries */
};

#���� n_name          _n._n_name
#���� n_nptr          _n._n_nptr[1]
#���� n_zeroes        _n._n_n._n_zeroes
#���� n_offset        _n._n_n._n_offset

/*------------------------------------------------------------------------*/
/* Relocatable symbols have a section number of the                       */
/* section in which they are defined.  Otherwise, section                 */
/* numbers have the following meanings:                                   */
/*------------------------------------------------------------------------*/
#����  N_UNDEF  0                     /* undefined symbol */
#����  N_ABS    -1                    /* value of symbol is absolute */
#����  N_DEBUG  -2                    /* special debugging symbol  */
#����  N_TV     (�޷� ��)-3    /* needs transfer vector (preload) */
#����  P_TV     (�޷� ��)-4    /* needs transfer vector (postload) */


/*------------------------------------------------------------------------*/
/* The fundamental type of a symbol packed into the low                   */
/* 4 bits of the word.                                                    */
/*------------------------------------------------------------------------*/
#����  _EF    ".ef"

#����  T_NULL     0          /* no type info */
#����  T_ARG      1          /* function argument (only used by compiler) */
#����  T_CHAR     2          /* character */
#����  T_SHORT    3          /* short integer */
#����  T_INT      4          /* integer */
#����  T_LONG     5          /* long integer */
#����  T_FLOAT    6          /* floating point */
#����  T_DOUBLE   7          /* double word */
#����  T_STRUCT   8          /* structure  */
#����  T_UNION    9          /* union  */
#����  T_ENUM     10         /* enumeration  */
#����  T_MOE      11         /* member of enumeration */
#����  T_UCHAR    12         /* unsigned character */
#����  T_USHORT   13         /* unsigned short */
#����  T_UINT     14         /* unsigned integer */
#����  T_ULONG    15         /* unsigned long */

/*------------------------------------------------------------------------*/
/* derived types are:                                                     */
/*------------------------------------------------------------------------*/
#����  DT_NON      0          /* no derived type */
#����  DT_PTR      1          /* pointer */
#����  DT_FCN      2          /* function */
#����  DT_ARY      3          /* array */

#���� MKTYPE(basic, d1,d2,d3,d4,d5,d6) \
       ((basic) | ((d1) <<  4) | ((d2) <<  6) | ((d3) <<  8) |\
                  ((d4) << 10) | ((d5) << 12) | ((d6) << 14))

/*------------------------------------------------------------------------*/
/* type packing constants and macros                                      */
/*------------------------------------------------------------------------*/
#����  N_BTMASK_COFF     017
#����  N_TMASK_COFF      060
#����  N_TMASK1_COFF     0300
#����  N_TMASK2_COFF     0360
#����  N_BTSHFT_COFF     4
#����  N_TSHIFT_COFF     2

#����  BTYPE_COFF(x)  ((x) & N_BTMASK_COFF)  
#����  ISINT(x)  (((x) >= T_CHAR && (x) <= T_LONG) ||   \
                    ((x) >= T_UCHAR && (x) <= T_ULONG) || (x) == T_ENUM)
#����  ISFLT_COFF(x)  ((x) == T_DOUBLE || (x) == T_FLOAT)
#����  ISPTR_COFF(x)  (((x) & N_TMASK_COFF) == (DT_PTR << N_BTSHFT_COFF)) 
#����  ISFCN_COFF(x)  (((x) & N_TMASK_COFF) == (DT_FCN << N_BTSHFT_COFF))
#����  ISARY_COFF(x)  (((x) & N_TMASK_COFF) == (DT_ARY << N_BTSHFT_COFF))
#����  ISTAG_COFF(x)  ((x)==C_STRTAG || (x)==C_UNTAG || (x)==C_ENTAG)

#����  INCREF_COFF(x) ((((x)&~N_BTMASK_COFF)<<N_TSHIFT_COFF)|(DT_PTR<<N_BTSHFT_COFF)|(x&N_BTMASK_COFF))
#����  DECREF_COFF(x) ((((x)>>N_TSHIFT_COFF)&~N_BTMASK_COFF)|((x)&N_BTMASK_COFF))


/*------------------------------------------------------------------------*/
/*  AUXILIARY SYMBOL ENTRY                                                */
/*------------------------------------------------------------------------*/
���� auxent
{
        �ṹ
        {
                ��            x_tagndx;       /* str, un, or enum tag indx */
                ����
                {
                        �ṹ
                        {
                                �޷� ��  x_lnno; /* declaration line number */
                                �޷� ��  x_size; /* str, union, array size */
                        } x_lnsz;
                        ��    x_fsize;        /* size of function */
                } x_misc;
                ����
                {
                        �ṹ                  /* if ISFCN, tag, or .bb */
                        {
                                ��    x_lnnoptr;      /* ptr to fcn line # */
                                ��    x_endndx;       /* entry ndx past block end */
                        }       x_fcn;
                        �ṹ                  /* if ISARY, up to 4 dimen. */
                        {
                                �޷� ��  x_dimen[DIMNUM];
                        }       x_ary;
                }               x_fcnary;
                �޷� ��  x_regcount;   /* number of registers used by func */
        }       x_sym;
        �ṹ
        {
                ��    x_fname[FILNMLEN];
        }       x_file;
        �ṹ
        {
                ��    x_scnlen;          /* section length */
                �޷� ��  x_nreloc;  /* number of relocation entries */
                �޷� ��  x_nlinno;  /* number of line numbers */
        }       x_scn;
};

#���� SYMENT  �ṹ syment
#���� SYMESZ  18      /* �󳤶�(SYMENT) */

#���� AUXENT  ���� auxent
#���� AUXESZ  18      /* �󳤶�(AUXENT) */

/*------------------------------------------------------------------------*/
/*  NAMES OF "SPECIAL" SYMBOLS                                            */
/*------------------------------------------------------------------------*/
#���� _STEXT          ".text"
#���� _ETEXT          "etext"
#���� _SDATA          ".data"
#���� _EDATA          "edata"
#���� _SBSS           ".bss"
#���� _END            "end"
#���� _CINITPTR       "cinit"

/*--------------------------------------------------------------------------*/
/*  ENTRY POINT SYMBOLS                                                     */
/*--------------------------------------------------------------------------*/
#���� _START          "_start"
#���� _MAIN           "_main"
    /*  _CSTART         "_c_int00"          (defined in params.h)  */


#���� _TVORIG         "_tvorig"
#���� _TORIGIN        "_torigin"
#���� _DORIGIN        "_dorigin"

#���� _SORIGIN        "_sorigin"
