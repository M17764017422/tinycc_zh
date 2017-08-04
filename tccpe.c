/*
 *  TCCPE.C - PE file output for the Tiny C Compiler
 *
 *  Copyright (c) 2005-2007 grischka
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

#���� PE_MERGE_DATA
/* #���� PE_PRINT_SECTIONS */

#��δ���� _WIN32
#���� stricmp strcasecmp
#���� strnicmp strncasecmp
#����

#�綨�� TCC_TARGET_X86_64
# ���� ADDR3264 ULONGLONG
# ���� REL_TYPE_DIRECT R_X86_64_64
# ���� R_XXX_THUNKFIX R_X86_64_PC32
# ���� R_XXX_RELATIVE R_X86_64_RELATIVE
# ���� IMAGE_FILE_MACHINE 0x8664
# ���� RSRC_RELTYPE 3

#���� �Ѷ��� TCC_TARGET_ARM
# ���� ADDR3264 DWORD
# ���� REL_TYPE_DIRECT R_ARM_ABS32
# ���� R_XXX_THUNKFIX R_ARM_ABS32
# ���� R_XXX_RELATIVE R_ARM_RELATIVE
# ���� IMAGE_FILE_MACHINE 0x01C0
# ���� RSRC_RELTYPE 7 /* ??? (not tested) */

#���� �Ѷ��� TCC_TARGET_I386
# ���� ADDR3264 DWORD
# ���� REL_TYPE_DIRECT R_386_32
# ���� R_XXX_THUNKFIX R_386_32
# ���� R_XXX_RELATIVE R_386_RELATIVE
# ���� IMAGE_FILE_MACHINE 0x014C
# ���� RSRC_RELTYPE 7 /* DIR32NB */

#����

#�� 0
#�綨�� _WIN32
�� dbg_printf (���� �� *fmt, ...)
{
    �� buffer[4000];
    va_list arg;
    �� x;
    va_start(arg, fmt);
    x = vsprintf (buffer, fmt, arg);
    strcpy(buffer+x, "\n");
    OutputDebugString(buffer);
}
#����
#����

/* ----------------------------------------------------------- */
#��δ���� IMAGE_NT_SIGNATURE
/* ----------------------------------------------------------- */
/* definitions below are from winnt.h */

���Ͷ��� �޷� �� BYTE;
���Ͷ��� �޷� �� WORD;
���Ͷ��� �޷� �� DWORD;
���Ͷ��� �޷� �� �� ULONGLONG;
#��ע pack(push, 1)

���Ͷ��� �ṹ _IMAGE_DOS_HEADER {  /* DOS .EXE header */
    WORD e_magic;         /* Magic number */
    WORD e_cblp;          /* Bytes on last page of file */
    WORD e_cp;            /* Pages in file */
    WORD e_crlc;          /* Relocations */
    WORD e_cparhdr;       /* Size of header in paragraphs */
    WORD e_minalloc;      /* Minimum extra paragraphs needed */
    WORD e_maxalloc;      /* Maximum extra paragraphs needed */
    WORD e_ss;            /* Initial (relative) SS value */
    WORD e_sp;            /* Initial SP value */
    WORD e_csum;          /* Checksum */
    WORD e_ip;            /* Initial IP value */
    WORD e_cs;            /* Initial (relative) CS value */
    WORD e_lfarlc;        /* File address of relocation table */
    WORD e_ovno;          /* Overlay number */
    WORD e_res[4];        /* Reserved words */
    WORD e_oemid;         /* OEM identifier (for e_oeminfo) */
    WORD e_oeminfo;       /* OEM information; e_oemid specific */
    WORD e_res2[10];      /* Reserved words */
    DWORD e_lfanew;        /* File address of new exe header */
} IMAGE_DOS_HEADER, *PIMAGE_DOS_HEADER;

#���� IMAGE_NT_SIGNATURE  0x00004550  /* PE00 */
#���� SIZE_OF_NT_SIGNATURE 4

���Ͷ��� �ṹ _IMAGE_FILE_HEADER {
    WORD    Machine;
    WORD    NumberOfSections;
    DWORD   TimeDateStamp;
    DWORD   PointerToSymbolTable;
    DWORD   NumberOfSymbols;
    WORD    SizeOfOptionalHeader;
    WORD    Characteristics;
} IMAGE_FILE_HEADER, *PIMAGE_FILE_HEADER;


#���� IMAGE_SIZEOF_FILE_HEADER 20

���Ͷ��� �ṹ _IMAGE_DATA_DIRECTORY {
    DWORD   VirtualAddress;
    DWORD   Size;
} IMAGE_DATA_DIRECTORY, *PIMAGE_DATA_DIRECTORY;


���Ͷ��� �ṹ _IMAGE_OPTIONAL_HEADER {
    /* Standard fields. */
    WORD    Magic;
    BYTE    MajorLinkerVersion;
    BYTE    MinorLinkerVersion;
    DWORD   SizeOfCode;
    DWORD   SizeOfInitializedData;
    DWORD   SizeOfUninitializedData;
    DWORD   AddressOfEntryPoint;
    DWORD   BaseOfCode;
#��δ���� TCC_TARGET_X86_64
    DWORD   BaseOfData;
#����
    /* NT additional fields. */
    ADDR3264 ImageBase;
    DWORD   SectionAlignment;
    DWORD   FileAlignment;
    WORD    MajorOperatingSystemVersion;
    WORD    MinorOperatingSystemVersion;
    WORD    MajorImageVersion;
    WORD    MinorImageVersion;
    WORD    MajorSubsystemVersion;
    WORD    MinorSubsystemVersion;
    DWORD   Win32VersionValue;
    DWORD   SizeOfImage;
    DWORD   SizeOfHeaders;
    DWORD   CheckSum;
    WORD    Subsystem;
    WORD    DllCharacteristics;
    ADDR3264 SizeOfStackReserve;
    ADDR3264 SizeOfStackCommit;
    ADDR3264 SizeOfHeapReserve;
    ADDR3264 SizeOfHeapCommit;
    DWORD   LoaderFlags;
    DWORD   NumberOfRvaAndSizes;
    IMAGE_DATA_DIRECTORY DataDirectory[16];
} IMAGE_OPTIONAL_HEADER32, IMAGE_OPTIONAL_HEADER64, IMAGE_OPTIONAL_HEADER;

#���� IMAGE_DIRECTORY_ENTRY_EXPORT          0   /* Export Directory */
#���� IMAGE_DIRECTORY_ENTRY_IMPORT          1   /* Import Directory */
#���� IMAGE_DIRECTORY_ENTRY_RESOURCE        2   /* Resource Directory */
#���� IMAGE_DIRECTORY_ENTRY_EXCEPTION       3   /* Exception Directory */
#���� IMAGE_DIRECTORY_ENTRY_SECURITY        4   /* Security Directory */
#���� IMAGE_DIRECTORY_ENTRY_BASERELOC       5   /* Base Relocation Table */
#���� IMAGE_DIRECTORY_ENTRY_DEBUG           6   /* Debug Directory */
/*      IMAGE_DIRECTORY_ENTRY_COPYRIGHT       7      (X86 usage) */
#���� IMAGE_DIRECTORY_ENTRY_ARCHITECTURE    7   /* Architecture Specific Data */
#���� IMAGE_DIRECTORY_ENTRY_GLOBALPTR       8   /* RVA of GP */
#���� IMAGE_DIRECTORY_ENTRY_TLS             9   /* TLS Directory */
#���� IMAGE_DIRECTORY_ENTRY_LOAD_CONFIG    10   /* Load Configuration Directory */
#���� IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT   11   /* Bound Import Directory in headers */
#���� IMAGE_DIRECTORY_ENTRY_IAT            12   /* Import Address Table */
#���� IMAGE_DIRECTORY_ENTRY_DELAY_IMPORT   13   /* Delay Load Import Descriptors */
#���� IMAGE_DIRECTORY_ENTRY_COM_DESCRIPTOR 14   /* COM Runtime descriptor */

/* Section header format. */
#���� IMAGE_SIZEOF_SHORT_NAME         8

���Ͷ��� �ṹ _IMAGE_SECTION_HEADER {
    BYTE    Name[IMAGE_SIZEOF_SHORT_NAME];
    ���� {
            DWORD   PhysicalAddress;
            DWORD   VirtualSize;
    } Misc;
    DWORD   VirtualAddress;
    DWORD   SizeOfRawData;
    DWORD   PointerToRawData;
    DWORD   PointerToRelocations;
    DWORD   PointerToLinenumbers;
    WORD    NumberOfRelocations;
    WORD    NumberOfLinenumbers;
    DWORD   Characteristics;
} IMAGE_SECTION_HEADER, *PIMAGE_SECTION_HEADER;

#���� IMAGE_SIZEOF_SECTION_HEADER     40

���Ͷ��� �ṹ _IMAGE_EXPORT_DIRECTORY {
    DWORD Characteristics;
    DWORD TimeDateStamp;
    WORD MajorVersion;
    WORD MinorVersion;
    DWORD Name;
    DWORD Base;
    DWORD NumberOfFunctions;
    DWORD NumberOfNames;
    DWORD AddressOfFunctions;
    DWORD AddressOfNames;
    DWORD AddressOfNameOrdinals;
} IMAGE_EXPORT_DIRECTORY,*PIMAGE_EXPORT_DIRECTORY;

���Ͷ��� �ṹ _IMAGE_IMPORT_DESCRIPTOR {
    ���� {
        DWORD Characteristics;
        DWORD OriginalFirstThunk;
    };
    DWORD TimeDateStamp;
    DWORD ForwarderChain;
    DWORD Name;
    DWORD FirstThunk;
} IMAGE_IMPORT_DESCRIPTOR;

���Ͷ��� �ṹ _IMAGE_BASE_RELOCATION {
    DWORD   VirtualAddress;
    DWORD   SizeOfBlock;
//  WORD    TypeOffset[1];
} IMAGE_BASE_RELOCATION;

#���� IMAGE_SIZEOF_BASE_RELOCATION     8

#���� IMAGE_REL_BASED_ABSOLUTE         0
#���� IMAGE_REL_BASED_HIGH             1
#���� IMAGE_REL_BASED_LOW              2
#���� IMAGE_REL_BASED_HIGHLOW          3
#���� IMAGE_REL_BASED_HIGHADJ          4
#���� IMAGE_REL_BASED_MIPS_JMPADDR     5
#���� IMAGE_REL_BASED_SECTION          6
#���� IMAGE_REL_BASED_REL32            7

#��ע pack(pop)

/* ----------------------------------------------------------- */
#���� /* ndef IMAGE_NT_SIGNATURE */
/* ----------------------------------------------------------- */
#��ע pack(push, 1)

�ṹ pe_header
{
    IMAGE_DOS_HEADER doshdr;
    BYTE dosstub[0x40];
    DWORD nt_sig;
    IMAGE_FILE_HEADER filehdr;
#�綨�� TCC_TARGET_X86_64
    IMAGE_OPTIONAL_HEADER64 opthdr;
#��
#�綨�� _WIN64
    IMAGE_OPTIONAL_HEADER32 opthdr;
#��
    IMAGE_OPTIONAL_HEADER opthdr;
#����
#����
};

�ṹ pe_reloc_header {
    DWORD offset;
    DWORD size;
};

�ṹ pe_rsrc_header {
    �ṹ _IMAGE_FILE_HEADER filehdr;
    �ṹ _IMAGE_SECTION_HEADER sectionhdr;
};

�ṹ pe_rsrc_reloc {
    DWORD offset;
    DWORD size;
    WORD type;
};

#��ע pack(pop)

/* ------------------------------------------------------------- */
/* internal temporary structures */

/*
#���� IMAGE_SCN_CNT_CODE                  0x00000020
#���� IMAGE_SCN_CNT_INITIALIZED_DATA      0x00000040
#���� IMAGE_SCN_CNT_UNINITIALIZED_DATA    0x00000080
#���� IMAGE_SCN_MEM_DISCARDABLE           0x02000000
#���� IMAGE_SCN_MEM_SHARED                0x10000000
#���� IMAGE_SCN_MEM_EXECUTE               0x20000000
#���� IMAGE_SCN_MEM_READ                  0x40000000
#���� IMAGE_SCN_MEM_WRITE                 0x80000000
*/

ö�� {
    sec_text = 0,
    sec_data ,
    sec_bss ,
    sec_idata ,
    sec_pdata ,
    sec_other ,
    sec_rsrc ,
    sec_stab ,
    sec_reloc ,
    sec_last
};

��̬ ���� DWORD pe_sec_flags[] = {
    0x60000020, /* ".text"     , */
    0xC0000040, /* ".data"     , */
    0xC0000080, /* ".bss"      , */
    0x40000040, /* ".idata"    , */
    0x40000040, /* ".pdata"    , */
    0xE0000060, /* < other >   , */
    0x40000040, /* ".rsrc"     , */
    0x42000802, /* ".stab"     , */
    0x42000040, /* ".reloc"    , */
};

�ṹ section_info {
    �� cls, ord;
    �� name[32];
    DWORD sh_addr;
    DWORD sh_size;
    DWORD sh_flags;
    �޷� �� *data;
    DWORD data_size;
    IMAGE_SECTION_HEADER ish;
};

�ṹ import_symbol {
    �� sym_index;
    �� iat_index;
    �� thk_offset;
};

�ṹ pe_import_info {
    �� dll_index;
    �� sym_count;
    �ṹ import_symbol **symbols;
};

�ṹ pe_info {
    TCCState *s1;
    Section *reloc;
    Section *thunk;
    ���� �� *filename;
    �� type;
    DWORD sizeofheaders;
    ADDR3264 imagebase;
    ���� �� *start_symbol;
    DWORD start_addr;
    DWORD imp_offs;
    DWORD imp_size;
    DWORD iat_offs;
    DWORD iat_size;
    DWORD exp_offs;
    DWORD exp_size;
    �� subsystem;
    DWORD section_align;
    DWORD file_align;
    �ṹ section_info *sec_info;
    �� sec_count;
    �ṹ pe_import_info **imp_info;
    �� imp_count;
};

#���� PE_NUL 0
#���� PE_DLL 1
#���� PE_GUI 2
#���� PE_EXE 3
#���� PE_RUN 4

/* --------------------------------------------*/

��̬ ���� �� *pe_export_name(TCCState *s1, ElfW(Sym) *sym)
{
    ���� �� *name = (��*)symtab_section->link->data + sym->st_name;
    �� (s1->leading_underscore && name[0] == '_' && !(sym->st_other & ST_PE_STDCALL))
        ���� name + 1;
    ���� name;
}

��̬ �� pe_find_import(TCCState * s1, ElfW(Sym) *sym)
{
    �� buffer[200];
    ���� �� *s, *p;
    �� sym_index = 0, n = 0;
    �� a, err = 0;

    ���� {
        s = pe_export_name(s1, sym);
        a = 0;
        �� (n) {
            /* second try: */
            �� (sym->st_other & ST_PE_STDCALL) {
                /* try w/0 stdcall deco (windows API convention) */
                p = strrchr(s, '@');
                �� (!p || s[0] != '_')
                    ����;
                strcpy(buffer, s+1)[p-s-1] = 0;
            } �� �� (s[0] != '_') { /* try non-ansi function */
                buffer[0] = '_', strcpy(buffer + 1, s);
            } �� �� (0 == memcmp(s, "__imp_", 6)) { /* mingw 2.0 */
                strcpy(buffer, s + 6), a = 1;
            } �� �� (0 == memcmp(s, "_imp__", 6)) { /* mingw 3.7 */
                strcpy(buffer, s + 6), a = 1;
            } �� {
                ����;
            }
            s = buffer;
        }
        sym_index = find_elf_sym(s1->dynsymtab_section, s);
        // printf("find (%d) %d %s\n", n, sym_index, s);
        �� (sym_index
            && ELFW(ST_TYPE)(sym->st_info) == STT_OBJECT
            && 0 == (sym->st_other & ST_PE_IMPORT)
            && 0 == a
            ) err = -1, sym_index = 0;
    } �� (0 == sym_index && ++n < 2);
    ���� n == 2 ? err : sym_index;
}

/*----------------------------------------------------------------------------*/

��̬ �� dynarray_assoc(�� **pp, �� n, �� key)
{
    �� i;
    ���� (i = 0; i < n; ++i, ++pp)
    �� (key == **(�� **) pp)
        ���� i;
    ���� -1;
}

#�� 0
ST_FN DWORD umin(DWORD a, DWORD b)
{
    ���� a < b ? a : b;
}
#����

��̬ DWORD umax(DWORD a, DWORD b)
{
    ���� a < b ? b : a;
}

��̬ DWORD pe_file_align(�ṹ pe_info *pe, DWORD n)
{
    ���� (n + (pe->file_align - 1)) & ~(pe->file_align - 1);
}

��̬ DWORD pe_virtual_align(�ṹ pe_info *pe, DWORD n)
{
    ���� (n + (pe->section_align - 1)) & ~(pe->section_align - 1);
}

��̬ �� pe_align_section(Section *s, �� a)
{
    �� i = s->data_offset & (a-1);
    �� (i)
        section_ptr_add(s, a - i);
}

��̬ �� pe_set_datadir(�ṹ pe_header *hdr, �� dir, DWORD addr, DWORD size)
{
    hdr->opthdr.DataDirectory[dir].VirtualAddress = addr;
    hdr->opthdr.DataDirectory[dir].Size = size;
}

��̬ �� pe_fwrite(�� *data, �޷� len, FILE *fp, DWORD *psum)
{
    �� (psum) {
        DWORD sum = *psum;
        WORD *p = data;
        �� i;
        ���� (i = len; i > 0; i -= 2) {
            sum += (i >= 2) ? *p++ : *(BYTE*)p;
            sum = (sum + (sum >> 16)) & 0xFFFF;
        }
        *psum = sum;
    }
    ���� len == fwrite(data, 1, len, fp) ? 0 : -1;
}

��̬ �� pe_fpad(FILE *fp, DWORD new_pos)
{
    DWORD pos = ftell(fp);
    �� (++pos <= new_pos)
        fputc(0, fp);
}

/*----------------------------------------------------------------------------*/
��̬ �� pe_write(�ṹ pe_info *pe)
{
    ��̬ ���� �ṹ pe_header pe_template = {
    {
    /* IMAGE_DOS_HEADER doshdr */
    0x5A4D, /*WORD e_magic;         Magic number */
    0x0090, /*WORD e_cblp;          Bytes on last page of file */
    0x0003, /*WORD e_cp;            Pages in file */
    0x0000, /*WORD e_crlc;          Relocations */

    0x0004, /*WORD e_cparhdr;       Size of header in paragraphs */
    0x0000, /*WORD e_minalloc;      Minimum extra paragraphs needed */
    0xFFFF, /*WORD e_maxalloc;      Maximum extra paragraphs needed */
    0x0000, /*WORD e_ss;            Initial (relative) SS value */

    0x00B8, /*WORD e_sp;            Initial SP value */
    0x0000, /*WORD e_csum;          Checksum */
    0x0000, /*WORD e_ip;            Initial IP value */
    0x0000, /*WORD e_cs;            Initial (relative) CS value */
    0x0040, /*WORD e_lfarlc;        File address of relocation table */
    0x0000, /*WORD e_ovno;          Overlay number */
    {0,0,0,0}, /*WORD e_res[4];     Reserved words */
    0x0000, /*WORD e_oemid;         OEM identifier (for e_oeminfo) */
    0x0000, /*WORD e_oeminfo;       OEM information; e_oemid specific */
    {0,0,0,0,0,0,0,0,0,0}, /*WORD e_res2[10];      Reserved words */
    0x00000080  /*DWORD   e_lfanew;        File address of new exe header */
    },{
    /* BYTE dosstub[0x40] */
    /* 14 code bytes + "This program cannot be run in DOS mode.\r\r\n$" + 6 * 0x00 */
    0x0e,0x1f,0xba,0x0e,0x00,0xb4,0x09,0xcd,0x21,0xb8,0x01,0x4c,0xcd,0x21,0x54,0x68,
    0x69,0x73,0x20,0x70,0x72,0x6f,0x67,0x72,0x61,0x6d,0x20,0x63,0x61,0x6e,0x6e,0x6f,
    0x74,0x20,0x62,0x65,0x20,0x72,0x75,0x6e,0x20,0x69,0x6e,0x20,0x44,0x4f,0x53,0x20,
    0x6d,0x6f,0x64,0x65,0x2e,0x0d,0x0d,0x0a,0x24,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    },
    0x00004550, /* DWORD nt_sig = IMAGE_NT_SIGNATURE */
    {
    /* IMAGE_FILE_HEADER filehdr */
    IMAGE_FILE_MACHINE, /*WORD    Machine; */
    0x0003, /*WORD    NumberOfSections; */
    0x00000000, /*DWORD   TimeDateStamp; */
    0x00000000, /*DWORD   PointerToSymbolTable; */
    0x00000000, /*DWORD   NumberOfSymbols; */
#�� �Ѷ���(TCC_TARGET_X86_64)
    0x00F0, /*WORD    SizeOfOptionalHeader; */
    0x022F  /*WORD    Characteristics; */
#���� CHARACTERISTICS_DLL 0x222E
#���� �Ѷ���(TCC_TARGET_I386)
    0x00E0, /*WORD    SizeOfOptionalHeader; */
    0x030F  /*WORD    Characteristics; */
#���� CHARACTERISTICS_DLL 0x230E
#���� �Ѷ���(TCC_TARGET_ARM)
    0x00E0, /*WORD    SizeOfOptionalHeader; */
    0x010F, /*WORD    Characteristics; */
#���� CHARACTERISTICS_DLL 0x230F
#����
},{
    /* IMAGE_OPTIONAL_HEADER opthdr */
    /* Standard fields. */
#�綨�� TCC_TARGET_X86_64
    0x020B, /*WORD    Magic; */
#��
    0x010B, /*WORD    Magic; */
#����
    0x06, /*BYTE    MajorLinkerVersion; */
    0x00, /*BYTE    MinorLinkerVersion; */
    0x00000000, /*DWORD   SizeOfCode; */
    0x00000000, /*DWORD   SizeOfInitializedData; */
    0x00000000, /*DWORD   SizeOfUninitializedData; */
    0x00000000, /*DWORD   AddressOfEntryPoint; */
    0x00000000, /*DWORD   BaseOfCode; */
#��δ���� TCC_TARGET_X86_64
    0x00000000, /*DWORD   BaseOfData; */
#����
    /* NT additional fields. */
#�� �Ѷ���(TCC_TARGET_ARM)
    0x00100000,     /*DWORD   ImageBase; */
#��
    0x00400000,     /*DWORD   ImageBase; */
#����
    0x00001000, /*DWORD   SectionAlignment; */
    0x00000200, /*DWORD   FileAlignment; */
    0x0004, /*WORD    MajorOperatingSystemVersion; */
    0x0000, /*WORD    MinorOperatingSystemVersion; */
    0x0000, /*WORD    MajorImageVersion; */
    0x0000, /*WORD    MinorImageVersion; */
    0x0004, /*WORD    MajorSubsystemVersion; */
    0x0000, /*WORD    MinorSubsystemVersion; */
    0x00000000, /*DWORD   Win32VersionValue; */
    0x00000000, /*DWORD   SizeOfImage; */
    0x00000200, /*DWORD   SizeOfHeaders; */
    0x00000000, /*DWORD   CheckSum; */
    0x0002, /*WORD    Subsystem; */
    0x0000, /*WORD    DllCharacteristics; */
    0x00100000, /*DWORD   SizeOfStackReserve; */
    0x00001000, /*DWORD   SizeOfStackCommit; */
    0x00100000, /*DWORD   SizeOfHeapReserve; */
    0x00001000, /*DWORD   SizeOfHeapCommit; */
    0x00000000, /*DWORD   LoaderFlags; */
    0x00000010, /*DWORD   NumberOfRvaAndSizes; */

    /* IMAGE_DATA_DIRECTORY DataDirectory[16]; */
    {{0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0},
     {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}, {0,0}}
    }};

    �ṹ pe_header pe_header = pe_template;

    �� i;
    FILE *op;
    DWORD file_offset, sum;
    �ṹ section_info *si;
    IMAGE_SECTION_HEADER *psh;

    op = fopen(pe->filename, "wb");
    �� (NULL == op) {
        tcc_error_noabort("could not write '%s': %s", pe->filename, strerror(errno));
        ���� -1;
    }

    pe->sizeofheaders = pe_file_align(pe,
        �󳤶� (�ṹ pe_header)
        + pe->sec_count * �󳤶� (IMAGE_SECTION_HEADER)
        );

    file_offset = pe->sizeofheaders;

    �� (2 == pe->s1->verbose)
        printf("-------------------------------"
               "\n  virt   file   size  section" "\n");
    ���� (i = 0; i < pe->sec_count; ++i) {
        DWORD addr, size;
        ���� �� *sh_name;

        si = pe->sec_info + i;
        sh_name = si->name;
        addr = si->sh_addr - pe->imagebase;
        size = si->sh_size;
        psh = &si->ish;

        �� (2 == pe->s1->verbose)
            printf("%6x %6x %6x  %s\n",
                (�޷�)addr, (�޷�)file_offset, (�޷�)size, sh_name);

        ת�� (si->cls) {
            ���� sec_text:
                pe_header.opthdr.BaseOfCode = addr;
                ����;

            ���� sec_data:
#��δ���� TCC_TARGET_X86_64
                pe_header.opthdr.BaseOfData = addr;
#����
                ����;

            ���� sec_bss:
                ����;

            ���� sec_reloc:
                pe_set_datadir(&pe_header, IMAGE_DIRECTORY_ENTRY_BASERELOC, addr, size);
                ����;

            ���� sec_rsrc:
                pe_set_datadir(&pe_header, IMAGE_DIRECTORY_ENTRY_RESOURCE, addr, size);
                ����;

            ���� sec_pdata:
                pe_set_datadir(&pe_header, IMAGE_DIRECTORY_ENTRY_EXCEPTION, addr, size);
                ����;

            ���� sec_stab:
                ����;
        }

        �� (pe->thunk == pe->s1->sections[si->ord]) {
            �� (pe->imp_size) {
                pe_set_datadir(&pe_header, IMAGE_DIRECTORY_ENTRY_IMPORT,
                    pe->imp_offs + addr, pe->imp_size);
                pe_set_datadir(&pe_header, IMAGE_DIRECTORY_ENTRY_IAT,
                    pe->iat_offs + addr, pe->iat_size);
            }
            �� (pe->exp_size) {
                pe_set_datadir(&pe_header, IMAGE_DIRECTORY_ENTRY_EXPORT,
                    pe->exp_offs + addr, pe->exp_size);
            }
        }

        strncpy((��*)psh->Name, sh_name, �󳤶� psh->Name);

        psh->Characteristics = pe_sec_flags[si->cls];
        psh->VirtualAddress = addr;
        psh->Misc.VirtualSize = size;
        pe_header.opthdr.SizeOfImage =
            umax(pe_virtual_align(pe, size + addr), pe_header.opthdr.SizeOfImage);

        �� (si->data_size) {
            psh->PointerToRawData = file_offset;
            file_offset = pe_file_align(pe, file_offset + si->data_size);
            psh->SizeOfRawData = file_offset - psh->PointerToRawData;
            �� (si->cls == sec_text)
                pe_header.opthdr.SizeOfCode += psh->SizeOfRawData;
            ��
                pe_header.opthdr.SizeOfInitializedData += psh->SizeOfRawData;
        }
    }

    //pe_header.filehdr.TimeDateStamp = time(NULL);
    pe_header.filehdr.NumberOfSections = pe->sec_count;
    pe_header.opthdr.AddressOfEntryPoint = pe->start_addr;
    pe_header.opthdr.SizeOfHeaders = pe->sizeofheaders;
    pe_header.opthdr.ImageBase = pe->imagebase;
    pe_header.opthdr.Subsystem = pe->subsystem;
    �� (pe->s1->pe_stack_size)
        pe_header.opthdr.SizeOfStackReserve = pe->s1->pe_stack_size;
    �� (PE_DLL == pe->type)
        pe_header.filehdr.Characteristics = CHARACTERISTICS_DLL;
    pe_header.filehdr.Characteristics |= pe->s1->pe_characteristics;

    sum = 0;
    pe_fwrite(&pe_header, �󳤶� pe_header, op, &sum);
    ���� (i = 0; i < pe->sec_count; ++i)
        pe_fwrite(&pe->sec_info[i].ish, �󳤶�(IMAGE_SECTION_HEADER), op, &sum);
    pe_fpad(op, pe->sizeofheaders);
    ���� (i = 0; i < pe->sec_count; ++i) {
        si = pe->sec_info + i;
        psh = &si->ish;
        �� (si->data_size) {
            pe_fwrite(si->data, si->data_size, op, &sum);
            file_offset = psh->PointerToRawData + psh->SizeOfRawData;
            pe_fpad(op, file_offset);
        }
    }

    pe_header.opthdr.CheckSum = sum + file_offset;
    fseek(op, offsetof(�ṹ pe_header, opthdr.CheckSum), SEEK_SET);
    pe_fwrite(&pe_header.opthdr.CheckSum, �󳤶� pe_header.opthdr.CheckSum, op, NULL);
    fclose (op);

    �� (2 == pe->s1->verbose)
        printf("-------------------------------\n");
    �� (pe->s1->verbose)
        printf("<- %s (%u bytes)\n", pe->filename, (�޷�)file_offset);

    ���� 0;
}

/*----------------------------------------------------------------------------*/

��̬ �ṹ import_symbol *pe_add_import(�ṹ pe_info *pe, �� sym_index)
{
    �� i;
    �� dll_index;
    �ṹ pe_import_info *p;
    �ṹ import_symbol *s;
    ElfW(Sym) *isym;

    isym = (ElfW(Sym) *)pe->s1->dynsymtab_section->data + sym_index;
    dll_index = isym->st_size;

    i = dynarray_assoc ((��**)pe->imp_info, pe->imp_count, dll_index);
    �� (-1 != i) {
        p = pe->imp_info[i];
        ��ת found_dll;
    }
    p = tcc_mallocz(�󳤶� *p);
    p->dll_index = dll_index;
    dynarray_add(&pe->imp_info, &pe->imp_count, p);

found_dll:
    i = dynarray_assoc ((��**)p->symbols, p->sym_count, sym_index);
    �� (-1 != i)
        ���� p->symbols[i];

    s = tcc_mallocz(�󳤶� *s);
    dynarray_add(&p->symbols, &p->sym_count, s);
    s->sym_index = sym_index;
    ���� s;
}

�� pe_free_imports(�ṹ pe_info *pe)
{
    �� i;
    ���� (i = 0; i < pe->imp_count; ++i) {
        �ṹ pe_import_info *p = pe->imp_info[i];
        dynarray_reset(&p->symbols, &p->sym_count);
    }
    dynarray_reset(&pe->imp_info, &pe->imp_count);
}

/*----------------------------------------------------------------------------*/
��̬ �� pe_build_imports(�ṹ pe_info *pe)
{
    �� thk_ptr, ent_ptr, dll_ptr, sym_cnt, i;
    DWORD rva_base = pe->thunk->sh_addr - pe->imagebase;
    �� ndlls = pe->imp_count;

    ���� (sym_cnt = i = 0; i < ndlls; ++i)
        sym_cnt += pe->imp_info[i]->sym_count;

    �� (0 == sym_cnt)
        ����;

    pe_align_section(pe->thunk, 16);

    pe->imp_offs = dll_ptr = pe->thunk->data_offset;
    pe->imp_size = (ndlls + 1) * �󳤶�(IMAGE_IMPORT_DESCRIPTOR);
    pe->iat_offs = dll_ptr + pe->imp_size;
    pe->iat_size = (sym_cnt + ndlls) * �󳤶�(ADDR3264);
    section_ptr_add(pe->thunk, pe->imp_size + 2*pe->iat_size);

    thk_ptr = pe->iat_offs;
    ent_ptr = pe->iat_offs + pe->iat_size;

    ���� (i = 0; i < pe->imp_count; ++i) {
        IMAGE_IMPORT_DESCRIPTOR *hdr;
        �� k, n, dllindex;
        ADDR3264 v;
        �ṹ pe_import_info *p = pe->imp_info[i];
        ���� �� *name;
        DLLReference *dllref;

        dllindex = p->dll_index;
        �� (dllindex)
            name = (dllref = pe->s1->loaded_dlls[dllindex-1])->name;
        ��
            name = "", dllref = NULL;

        /* put the dll name into the import header */
        v = put_elf_str(pe->thunk, name);
        hdr = (IMAGE_IMPORT_DESCRIPTOR*)(pe->thunk->data + dll_ptr);
        hdr->FirstThunk = thk_ptr + rva_base;
        hdr->OriginalFirstThunk = ent_ptr + rva_base;
        hdr->Name = v + rva_base;

        ���� (k = 0, n = p->sym_count; k <= n; ++k) {
            �� (k < n) {
                �� iat_index = p->symbols[k]->iat_index;
                �� sym_index = p->symbols[k]->sym_index;
                ElfW(Sym) *imp_sym = (ElfW(Sym) *)pe->s1->dynsymtab_section->data + sym_index;
                ElfW(Sym) *org_sym = (ElfW(Sym) *)symtab_section->data + iat_index;
                ���� �� *name = (��*)pe->s1->dynsymtab_section->link->data + imp_sym->st_name;
                �� ordinal;

                org_sym->st_value = thk_ptr;
                org_sym->st_shndx = pe->thunk->sh_num;

                �� (dllref)
                    v = 0, ordinal = imp_sym->st_value; /* ordinal from pe_load_def */
                ��
                    ordinal = 0, v = imp_sym->st_value; /* address from tcc_add_symbol() */

#�綨�� TCC_IS_NATIVE
                �� (pe->type == PE_RUN) {
                    �� (dllref) {
                        �� ( !dllref->handle )
                            dllref->handle = LoadLibrary(dllref->name);
                        v = (ADDR3264)GetProcAddress(dllref->handle, ordinal?(��*)0+ordinal:name);
                    }
                    �� (!v)
                        tcc_error_noabort("can't build symbol '%s'", name);
                } ��
#����
                �� (ordinal) {
                    v = ordinal | (ADDR3264)1 << (�󳤶�(ADDR3264)*8 - 1);
                } �� {
                    v = pe->thunk->data_offset + rva_base;
                    section_ptr_add(pe->thunk, �󳤶�(WORD)); /* hint, not used */
                    put_elf_str(pe->thunk, name);
                }

            } �� {
                v = 0; /* last entry is zero */
            }

            *(ADDR3264*)(pe->thunk->data+thk_ptr) =
            *(ADDR3264*)(pe->thunk->data+ent_ptr) = v;
            thk_ptr += �󳤶� (ADDR3264);
            ent_ptr += �󳤶� (ADDR3264);
        }
        dll_ptr += �󳤶�(IMAGE_IMPORT_DESCRIPTOR);
    }
}

/* ------------------------------------------------------------- */

�ṹ pe_sort_sym
{
    �� index;
    ���� �� *name;
};

��̬ �� sym_cmp(���� �� *va, ���� �� *vb)
{
    ���� �� *ca = (*(�ṹ pe_sort_sym**)va)->name;
    ���� �� *cb = (*(�ṹ pe_sort_sym**)vb)->name;
    ���� strcmp(ca, cb);
}

��̬ �� pe_build_exports(�ṹ pe_info *pe)
{
    ElfW(Sym) *sym;
    �� sym_index, sym_end;
    DWORD rva_base, func_o, name_o, ord_o, str_o;
    IMAGE_EXPORT_DIRECTORY *hdr;
    �� sym_count, ord;
    �ṹ pe_sort_sym **sorted, *p;

    FILE *op;
    �� buf[260];
    ���� �� *dllname;
    ���� �� *name;

    rva_base = pe->thunk->sh_addr - pe->imagebase;
    sym_count = 0, sorted = NULL, op = NULL;

    sym_end = symtab_section->data_offset / �󳤶�(ElfW(Sym));
    ���� (sym_index = 1; sym_index < sym_end; ++sym_index) {
        sym = (ElfW(Sym)*)symtab_section->data + sym_index;
        name = pe_export_name(pe->s1, sym);
        �� ((sym->st_other & ST_PE_EXPORT)
            /* export only symbols from actually written sections */
            && pe->s1->sections[sym->st_shndx]->sh_addr) {
            p = tcc_malloc(�󳤶� *p);
            p->index = sym_index;
            p->name = name;
            dynarray_add(&sorted, &sym_count, p);
        }
#�� 0
        �� (sym->st_other & ST_PE_EXPORT)
            printf("export: %s\n", name);
        �� (sym->st_other & ST_PE_STDCALL)
            printf("stdcall: %s\n", name);
#����
    }

    �� (0 == sym_count)
        ����;

    qsort (sorted, sym_count, �󳤶� *sorted, sym_cmp);

    pe_align_section(pe->thunk, 16);
    dllname = tcc_basename(pe->filename);

    pe->exp_offs = pe->thunk->data_offset;
    func_o = pe->exp_offs + �󳤶�(IMAGE_EXPORT_DIRECTORY);
    name_o = func_o + sym_count * �󳤶� (DWORD);
    ord_o = name_o + sym_count * �󳤶� (DWORD);
    str_o = ord_o + sym_count * �󳤶�(WORD);

    hdr = section_ptr_add(pe->thunk, str_o - pe->exp_offs);
    hdr->Characteristics        = 0;
    hdr->Base                   = 1;
    hdr->NumberOfFunctions      = sym_count;
    hdr->NumberOfNames          = sym_count;
    hdr->AddressOfFunctions     = func_o + rva_base;
    hdr->AddressOfNames         = name_o + rva_base;
    hdr->AddressOfNameOrdinals  = ord_o + rva_base;
    hdr->Name                   = str_o + rva_base;
    put_elf_str(pe->thunk, dllname);

#�� 1
    /* automatically write exports to <output-filename>.def */
    pstrcpy(buf, �󳤶� buf, pe->filename);
    strcpy(tcc_fileextension(buf), ".def");
    op = fopen(buf, "w");
    �� (NULL == op) {
        tcc_error_noabort("could not create '%s': %s", buf, strerror(errno));
    } �� {
        fprintf(op, "LIBRARY %s\n\nEXPORTS\n", dllname);
        �� (pe->s1->verbose)
            printf("<- %s (%d symbol%s)\n", buf, sym_count, &"s"[sym_count < 2]);
    }
#����

    ���� (ord = 0; ord < sym_count; ++ord)
    {
        p = sorted[ord], sym_index = p->index, name = p->name;
        /* insert actual address later in pe_relocate_rva */
        put_elf_reloc(symtab_section, pe->thunk,
            func_o, R_XXX_RELATIVE, sym_index);
        *(DWORD*)(pe->thunk->data + name_o)
            = pe->thunk->data_offset + rva_base;
        *(WORD*)(pe->thunk->data + ord_o)
            = ord;
        put_elf_str(pe->thunk, name);
        func_o += �󳤶� (DWORD);
        name_o += �󳤶� (DWORD);
        ord_o += �󳤶� (WORD);
        �� (op)
            fprintf(op, "%s\n", name);
    }
    pe->exp_size = pe->thunk->data_offset - pe->exp_offs;
    dynarray_reset(&sorted, &sym_count);
    �� (op)
        fclose(op);
}

/* ------------------------------------------------------------- */
��̬ �� pe_build_reloc (�ṹ pe_info *pe)
{
    DWORD offset, block_ptr, addr;
    �� count, i;
    ElfW_Rel *rel, *rel_end;
    Section *s = NULL, *sr;

    offset = addr = block_ptr = count = i = 0;
    rel = rel_end = NULL;

    ����(;;) {
        �� (rel < rel_end) {
            �� type = ELFW(R_TYPE)(rel->r_info);
            addr = rel->r_offset + s->sh_addr;
            ++ rel;
            �� (type != REL_TYPE_DIRECT)
                ����;
            �� (count == 0) { /* new block */
                block_ptr = pe->reloc->data_offset;
                section_ptr_add(pe->reloc, �󳤶�(�ṹ pe_reloc_header));
                offset = addr & 0xFFFFFFFF<<12;
            }
            �� ((addr -= offset)  < (1<<12)) { /* one block spans 4k addresses */
                WORD *wp = section_ptr_add(pe->reloc, �󳤶� (WORD));
                *wp = addr | IMAGE_REL_BASED_HIGHLOW<<12;
                ++count;
                ����;
            }
            -- rel;

        } �� �� (i < pe->sec_count) {
            sr = (s = pe->s1->sections[pe->sec_info[i++].ord])->reloc;
            �� (sr) {
                rel = (ElfW_Rel *)sr->data;
                rel_end = (ElfW_Rel *)(sr->data + sr->data_offset);
            }
            ����;
        }

        �� (count) {
            /* store the last block and ready for a new one */
            �ṹ pe_reloc_header *hdr;
            �� (count & 1) /* align for DWORDS */
                section_ptr_add(pe->reloc, �󳤶�(WORD)), ++count;
            hdr = (�ṹ pe_reloc_header *)(pe->reloc->data + block_ptr);
            hdr -> offset = offset - pe->imagebase;
            hdr -> size = count * �󳤶�(WORD) + �󳤶�(�ṹ pe_reloc_header);
            count = 0;
        }

        �� (rel >= rel_end)
            ����;
    }
}

/* ------------------------------------------------------------- */
��̬ �� pe_section_class(Section *s)
{
    �� type, flags;
    ���� �� *name;

    type = s->sh_type;
    flags = s->sh_flags;
    name = s->name;
    �� (flags & SHF_ALLOC) {
        �� (type == SHT_PROGBITS) {
            �� (flags & SHF_EXECINSTR)
                ���� sec_text;
            �� (flags & SHF_WRITE)
                ���� sec_data;
            �� (0 == strcmp(name, ".rsrc"))
                ���� sec_rsrc;
            �� (0 == strcmp(name, ".iedat"))
                ���� sec_idata;
            �� (0 == strcmp(name, ".pdata"))
                ���� sec_pdata;
            ���� sec_other;
        } �� �� (type == SHT_NOBITS) {
            �� (flags & SHF_WRITE)
                ���� sec_bss;
        }
    } �� {
        �� (0 == strcmp(name, ".reloc"))
            ���� sec_reloc;
        �� (0 == strncmp(name, ".stab", 5)) /* .stab and .stabstr */
            ���� sec_stab;
    }
    ���� -1;
}

��̬ �� pe_assign_addresses (�ṹ pe_info *pe)
{
    �� i, k, o, c;
    DWORD addr;
    �� *section_order;
    �ṹ section_info *si;
    Section *s;

    �� (PE_DLL == pe->type)
        pe->reloc = new_section(pe->s1, ".reloc", SHT_PROGBITS, 0);

    // pe->thunk = new_section(pe->s1, ".iedat", SHT_PROGBITS, SHF_ALLOC);

    section_order = tcc_malloc(pe->s1->nb_sections * �󳤶� (��));
    ���� (o = k = 0 ; k < sec_last; ++k) {
        ���� (i = 1; i < pe->s1->nb_sections; ++i) {
            s = pe->s1->sections[i];
            �� (k == pe_section_class(s)) {
                // printf("%s %d\n", s->name, k);
                s->sh_addr = pe->imagebase;
                section_order[o++] = i;
            }
        }
    }

    pe->sec_info = tcc_mallocz(o * �󳤶� (�ṹ section_info));
    addr = pe->imagebase + 1;

    ���� (i = 0; i < o; ++i)
    {
        k = section_order[i];
        s = pe->s1->sections[k];
        c = pe_section_class(s);
        si = &pe->sec_info[pe->sec_count];

#�綨�� PE_MERGE_DATA
        �� (c == sec_bss && pe->sec_count && si[-1].cls == sec_data) {
            /* append .bss to .data */
            s->sh_addr = addr = ((addr-1) | (s->sh_addralign-1)) + 1;
            addr += s->data_offset;
            si[-1].sh_size = addr - si[-1].sh_addr;
            ����;
        }
#����
        �� (c == sec_stab && 0 == pe->s1->do_debug)
            ����;

        strcpy(si->name, s->name);
        si->cls = c;
        si->ord = k;
        si->sh_addr = s->sh_addr = addr = pe_virtual_align(pe, addr);
        si->sh_flags = s->sh_flags;

        �� (c == sec_data && NULL == pe->thunk)
            pe->thunk = s;

        �� (s == pe->thunk) {
            pe_build_imports(pe);
            pe_build_exports(pe);
        }

        �� (c == sec_reloc)
            pe_build_reloc (pe);

        �� (s->data_offset)
        {
            �� (s->sh_type != SHT_NOBITS) {
                si->data = s->data;
                si->data_size = s->data_offset;
            }

            addr += s->data_offset;
            si->sh_size = s->data_offset;
            ++pe->sec_count;
        }
        // printf("%08x %05x %s\n", si->sh_addr, si->sh_size, si->name);
    }

#�� 0
    ���� (i = 1; i < pe->s1->nb_sections; ++i) {
        Section *s = pe->s1->sections[i];
        �� type = s->sh_type;
        �� flags = s->sh_flags;
        printf("section %-16s %-10s %5x %s,%s,%s\n",
            s->name,
            type == SHT_PROGBITS ? "progbits" :
            type == SHT_NOBITS ? "nobits" :
            type == SHT_SYMTAB ? "symtab" :
            type == SHT_STRTAB ? "strtab" :
            type == SHT_RELX ? "rel" : "???",
            s->data_offset,
            flags & SHF_ALLOC ? "alloc" : "",
            flags & SHF_WRITE ? "write" : "",
            flags & SHF_EXECINSTR ? "exec" : ""
            );
    }
    pe->s1->verbose = 2;
#����

    tcc_free(section_order);
    ���� 0;
}

/* ------------------------------------------------------------- */
��̬ �� pe_relocate_rva (�ṹ pe_info *pe, Section *s)
{
    Section *sr = s->reloc;
    ElfW_Rel *rel, *rel_end;
    rel_end = (ElfW_Rel *)(sr->data + sr->data_offset);
    ����(rel = (ElfW_Rel *)sr->data; rel < rel_end; rel++) {
        �� (ELFW(R_TYPE)(rel->r_info) == R_XXX_RELATIVE) {
            �� sym_index = ELFW(R_SYM)(rel->r_info);
            DWORD addr = s->sh_addr;
            �� (sym_index) {
                ElfW(Sym) *sym = (ElfW(Sym) *)symtab_section->data + sym_index;
                addr = sym->st_value;
            }
            // printf("reloc rva %08x %08x %s\n", (DWORD)rel->r_offset, addr, s->name);
            *(DWORD*)(s->data + rel->r_offset) += addr - pe->imagebase;
        }
    }
}

/*----------------------------------------------------------------------------*/

��̬ �� pe_isafunc(�� sym_index)
{
    Section *sr = text_section->reloc;
    ElfW_Rel *rel, *rel_end;
    Elf32_Word info = ELF32_R_INFO(sym_index, R_386_PC32);
    �� (!sr)
        ���� 0;
    rel_end = (ElfW_Rel *)(sr->data + sr->data_offset);
    ���� (rel = (ElfW_Rel *)sr->data; rel < rel_end; rel++)
        �� (rel->r_info == info)
            ���� 1;
    ���� 0;
}

/*----------------------------------------------------------------------------*/
��̬ �� pe_check_symbols(�ṹ pe_info *pe)
{
    ElfW(Sym) *sym;
    �� sym_index, sym_end;
    �� ret = 0;

    pe_align_section(text_section, 8);

    sym_end = symtab_section->data_offset / �󳤶�(ElfW(Sym));
    ���� (sym_index = 1; sym_index < sym_end; ++sym_index) {

        sym = (ElfW(Sym) *)symtab_section->data + sym_index;
        �� (sym->st_shndx == SHN_UNDEF) {

            ���� �� *name = (��*)symtab_section->link->data + sym->st_name;
            �޷� type = ELFW(ST_TYPE)(sym->st_info);
            �� imp_sym = pe_find_import(pe->s1, sym);
            �ṹ import_symbol *is;

            �� (imp_sym <= 0)
                ��ת not_found;

            �� (type == STT_NOTYPE) {
                /* symbols from assembler have no type, find out which */
                �� (pe_isafunc(sym_index))
                    type = STT_FUNC;
                ��
                    type = STT_OBJECT;
            }

            is = pe_add_import(pe, imp_sym);

            �� (type == STT_FUNC) {
                �޷� �� offset = is->thk_offset;
                �� (offset) {
                    /* got aliased symbol, like stricmp and _stricmp */

                } �� {
                    �� buffer[100];
                    WORD *p;

                    offset = text_section->data_offset;
                    /* add the 'jmp IAT[x]' instruction */
#�綨�� TCC_TARGET_ARM
                    p = section_ptr_add(text_section, 8+4); // room for code and address
                    (*(DWORD*)(p)) = 0xE59FC000; // arm code ldr ip, [pc] ; PC+8+0 = 0001xxxx
                    (*(DWORD*)(p+2)) = 0xE59CF000; // arm code ldr pc, [ip]
#��
                    p = section_ptr_add(text_section, 8);
                    *p = 0x25FF;
#�綨�� TCC_TARGET_X86_64
                    *(DWORD*)(p+1) = (DWORD)-4;
#����
#����
                    /* add a helper symbol, will be patched later in
                       pe_build_imports */
                    sprintf(buffer, "IAT.%s", name);
                    is->iat_index = put_elf_sym(
                        symtab_section, 0, �󳤶�(DWORD),
                        ELFW(ST_INFO)(STB_GLOBAL, STT_OBJECT),
                        0, SHN_UNDEF, buffer);
#�綨�� TCC_TARGET_ARM
                    put_elf_reloc(symtab_section, text_section,
                        offset + 8, R_XXX_THUNKFIX, is->iat_index); // offset to IAT position
#��
                    put_elf_reloc(symtab_section, text_section, 
                        offset + 2, R_XXX_THUNKFIX, is->iat_index);
#����
                    is->thk_offset = offset;
                }

                /* tcc_realloc might have altered sym's address */
                sym = (ElfW(Sym) *)symtab_section->data + sym_index;

                /* patch the original symbol */
                sym->st_value = offset;
                sym->st_shndx = text_section->sh_num;
                sym->st_other &= ~ST_PE_EXPORT; /* do not export */
                ����;
            }

            �� (type == STT_OBJECT) { /* data, ptr to that should be */
                �� (0 == is->iat_index) {
                    /* original symbol will be patched later in pe_build_imports */
                    is->iat_index = sym_index;
                    ����;
                }
            }

        not_found:
            �� (ELFW(ST_BIND)(sym->st_info) == STB_WEAK)
                /* STB_WEAK undefined symbols are accepted */
                ����;
            tcc_error_noabort("undefined symbol '%s'%s", name,
                imp_sym < 0 ? ", missing __declspec(dllimport)?":"");
            ret = -1;

        } �� �� (pe->s1->rdynamic
                   && ELFW(ST_BIND)(sym->st_info) != STB_LOCAL) {
            /* if -rdynamic option, then export all non local symbols */
            sym->st_other |= ST_PE_EXPORT;
        }
    }
    ���� ret;
}

/*----------------------------------------------------------------------------*/
#�綨�� PE_PRINT_SECTIONS
��̬ �� pe_print_section(FILE * f, Section * s)
{
    /* just if you're curious */
    BYTE *p, *e, b;
    �� i, n, l, m;
    p = s->data;
    e = s->data + s->data_offset;
    l = e - p;

    fprintf(f, "section  \"%s\"", s->name);
    �� (s->link)
        fprintf(f, "\nlink     \"%s\"", s->link->name);
    �� (s->reloc)
        fprintf(f, "\nreloc    \"%s\"", s->reloc->name);
    fprintf(f, "\nv_addr   %08X", (�޷�)s->sh_addr);
    fprintf(f, "\ncontents %08X", (�޷�)l);
    fprintf(f, "\n\n");

    �� (s->sh_type == SHT_NOBITS)
        ����;

    �� (0 == l)
        ����;

    �� (s->sh_type == SHT_SYMTAB)
        m = �󳤶�(ElfW(Sym));
    �� �� (s->sh_type == SHT_RELX)
        m = �󳤶�(ElfW_Rel);
    ��
        m = 16;

    fprintf(f, "%-8s", "offset");
    ���� (i = 0; i < m; ++i)
        fprintf(f, " %02x", i);
    n = 56;

    �� (s->sh_type == SHT_SYMTAB || s->sh_type == SHT_RELX) {
        ���� �� *fields1[] = {
            "name",
            "value",
            "size",
            "bind",
            "type",
            "other",
            "shndx",
            NULL
        };

        ���� �� *fields2[] = {
            "offs",
            "type",
            "symb",
            NULL
        };

        ���� �� **p;

        �� (s->sh_type == SHT_SYMTAB)
            p = fields1, n = 106;
        ��
            p = fields2, n = 58;

        ���� (i = 0; p[i]; ++i)
            fprintf(f, "%6s", p[i]);
        fprintf(f, "  symbol");
    }

    fprintf(f, "\n");
    ���� (i = 0; i < n; ++i)
        fprintf(f, "-");
    fprintf(f, "\n");

    ���� (i = 0; i < l;)
    {
        fprintf(f, "%08X", i);
        ���� (n = 0; n < m; ++n) {
            �� (n + i < l)
                fprintf(f, " %02X", p[i + n]);
            ��
                fprintf(f, "   ");
        }

        �� (s->sh_type == SHT_SYMTAB) {
            ElfW(Sym) *sym = (ElfW(Sym) *) (p + i);
            ���� �� *name = s->link->data + sym->st_name;
            fprintf(f, "  %04X  %04X  %04X   %02X    %02X    %02X   %04X  \"%s\"",
                    (�޷�)sym->st_name,
                    (�޷�)sym->st_value,
                    (�޷�)sym->st_size,
                    (�޷�)ELFW(ST_BIND)(sym->st_info),
                    (�޷�)ELFW(ST_TYPE)(sym->st_info),
                    (�޷�)sym->st_other,
                    (�޷�)sym->st_shndx,
                    name);

        } �� �� (s->sh_type == SHT_RELX) {
            ElfW_Rel *rel = (ElfW_Rel *) (p + i);
            ElfW(Sym) *sym =
                (ElfW(Sym) *) s->link->data + ELFW(R_SYM)(rel->r_info);
            ���� �� *name = s->link->link->data + sym->st_name;
            fprintf(f, "  %04X   %02X   %04X  \"%s\"",
                    (�޷�)rel->r_offset,
                    (�޷�)ELFW(R_TYPE)(rel->r_info),
                    (�޷�)ELFW(R_SYM)(rel->r_info),
                    name);
        } �� {
            fprintf(f, "   ");
            ���� (n = 0; n < m; ++n) {
                �� (n + i < l) {
                    b = p[i + n];
                    �� (b < 32 || b >= 127)
                        b = '.';
                    fprintf(f, "%c", b);
                }
            }
        }
        i += m;
        fprintf(f, "\n");
    }
    fprintf(f, "\n\n");
}

��̬ �� pe_print_sections(TCCState *s1, ���� �� *fname)
{
    Section *s;
    FILE *f;
    �� i;
    f = fopen(fname, "w");
    ���� (i = 1; i < s1->nb_sections; ++i) {
        s = s1->sections[i];
        pe_print_section(f, s);
    }
    pe_print_section(f, s1->dynsymtab_section);
    fclose(f);
}
#����

/* ------------------------------------------------------------- */
/* helper function for load/store to insert one more indirection */

#��δ���� TCC_TARGET_ARM
ST_FUNC SValue *pe_getimport(SValue *sv, SValue *v2)
{
    �� r2;
    �� ((sv->r & (VT_VALMASK|VT_SYM)) != (VT_CONST|VT_SYM) || (sv->r2 != VT_CONST))
        ���� sv;
    �� (!sv->sym->a.dllimport)
        ���� sv;
    // printf("import %04x %04x %04x %s\n", sv->type.t, sv->sym->type.t, sv->r, get_tok_str(sv->sym->v, NULL));
    memset(v2, 0, �󳤶� *v2);
    v2->type.t = VT_PTR;
    v2->r = VT_CONST | VT_SYM | VT_LVAL;
    v2->sym = sv->sym;

    r2 = get_reg(RC_INT);
    load(r2, v2);
    v2->r = r2;
    �� ((uint32_t)sv->c.i) {
        vpushv(v2);
        vpushi(sv->c.i);
        gen_opi('+');
        *v2 = *vtop--;
    }
    v2->type.t = sv->type.t;
    v2->r |= sv->r & VT_LVAL;
    ���� v2;
}
#����

ST_FUNC �� pe_putimport(TCCState *s1, �� dllindex, ���� �� *name, addr_t value)
{
    ���� set_elf_sym(
        s1->dynsymtab_section,
        value,
        dllindex, /* st_size */
        ELFW(ST_INFO)(STB_GLOBAL, STT_NOTYPE),
        0,
        value ? SHN_ABS : SHN_UNDEF,
        name
        );
}

��̬ �� add_dllref(TCCState *s1, ���� �� *dllname)
{
    DLLReference *dllref;
    �� i;
    ���� (i = 0; i < s1->nb_loaded_dlls; ++i)
        �� (0 == strcmp(s1->loaded_dlls[i]->name, dllname))
            ���� i + 1;
    dllref = tcc_mallocz(�󳤶�(DLLReference) + strlen(dllname));
    strcpy(dllref->name, dllname);
    dynarray_add(&s1->loaded_dlls, &s1->nb_loaded_dlls, dllref);
    ���� s1->nb_loaded_dlls;
}

/* ------------------------------------------------------------- */

��̬ �� read_mem(�� fd, �޷� offset, �� *buffer, �޷� len)
{
    lseek(fd, offset, SEEK_SET);
    ���� len == read(fd, buffer, len);
}

/* ------------------------------------------------------------- */

PUB_FUNC �� tcc_get_dllexports(���� �� *filename, �� **pp)
{
    �� l, i, n, n0, ret;
    �� *p;
    �� fd;

    IMAGE_SECTION_HEADER ish;
    IMAGE_EXPORT_DIRECTORY ied;
    IMAGE_DOS_HEADER dh;
    IMAGE_FILE_HEADER ih;
    DWORD sig, ref, addr, ptr, namep;

    �� pef_hdroffset, opt_hdroffset, sec_hdroffset;

    n = n0 = 0;
    p = NULL;
    ret = -1;

    fd = open(filename, O_RDONLY | O_BINARY);
    �� (fd < 0)
        ��ת the_end_1;
    ret = 1;
    �� (!read_mem(fd, 0, &dh, �󳤶� dh))
        ��ת the_end;
    �� (!read_mem(fd, dh.e_lfanew, &sig, �󳤶� sig))
        ��ת the_end;
    �� (sig != 0x00004550)
        ��ת the_end;
    pef_hdroffset = dh.e_lfanew + �󳤶� sig;
    �� (!read_mem(fd, pef_hdroffset, &ih, �󳤶� ih))
        ��ת the_end;
    opt_hdroffset = pef_hdroffset + �󳤶� ih;
    �� (ih.Machine == 0x014C) {
        IMAGE_OPTIONAL_HEADER32 oh;
        sec_hdroffset = opt_hdroffset + �󳤶� oh;
        �� (!read_mem(fd, opt_hdroffset, &oh, �󳤶� oh))
            ��ת the_end;
        �� (IMAGE_DIRECTORY_ENTRY_EXPORT >= oh.NumberOfRvaAndSizes)
            ��ת the_end_0;
        addr = oh.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    } �� �� (ih.Machine == 0x8664) {
        IMAGE_OPTIONAL_HEADER64 oh;
        sec_hdroffset = opt_hdroffset + �󳤶� oh;
        �� (!read_mem(fd, opt_hdroffset, &oh, �󳤶� oh))
            ��ת the_end;
        �� (IMAGE_DIRECTORY_ENTRY_EXPORT >= oh.NumberOfRvaAndSizes)
            ��ת the_end_0;
        addr = oh.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT].VirtualAddress;
    } ��
        ��ת the_end;

    //printf("addr: %08x\n", addr);
    ���� (i = 0; i < ih.NumberOfSections; ++i) {
        �� (!read_mem(fd, sec_hdroffset + i * �󳤶� ish, &ish, �󳤶� ish))
            ��ת the_end;
        //printf("vaddr: %08x\n", ish.VirtualAddress);
        �� (addr >= ish.VirtualAddress && addr < ish.VirtualAddress + ish.SizeOfRawData)
            ��ת found;
    }
    ��ת the_end_0;

found:
    ref = ish.VirtualAddress - ish.PointerToRawData;
    �� (!read_mem(fd, addr - ref, &ied, �󳤶� ied))
        ��ת the_end;

    namep = ied.AddressOfNames - ref;
    ���� (i = 0; i < ied.NumberOfNames; ++i) {
        �� (!read_mem(fd, namep, &ptr, �󳤶� ptr))
            ��ת the_end;
        namep += �󳤶� ptr;
        ���� (l = 0;;) {
            �� (n+1 >= n0)
                p = tcc_realloc(p, n0 = n0 ? n0 * 2 : 256);
            �� (!read_mem(fd, ptr - ref + l++, p + n, 1)) {
                tcc_free(p), p = NULL;
                ��ת the_end;
            }
            �� (p[n++] == 0)
                ����;
        }
    }
    �� (p)
        p[n] = 0;
the_end_0:
    ret = 0;
the_end:
    close(fd);
the_end_1:
    *pp = p;
    ���� ret;
}

/* -------------------------------------------------------------
 *  This is for compiled windows resources in 'coff' format
 *  as generated by 'windres.exe -O coff ...'.
 */

��̬ �� pe_load_res(TCCState *s1, �� fd)
{
    �ṹ pe_rsrc_header hdr;
    Section *rsrc_section;
    �� i, ret = -1;
    BYTE *ptr;
    �޷� offs;

    �� (!read_mem(fd, 0, &hdr, �󳤶� hdr))
        ��ת quit;

    �� (hdr.filehdr.Machine != IMAGE_FILE_MACHINE
        || hdr.filehdr.NumberOfSections != 1
        || strcmp((��*)hdr.sectionhdr.Name, ".rsrc") != 0)
        ��ת quit;

    rsrc_section = new_section(s1, ".rsrc", SHT_PROGBITS, SHF_ALLOC);
    ptr = section_ptr_add(rsrc_section, hdr.sectionhdr.SizeOfRawData);
    offs = hdr.sectionhdr.PointerToRawData;
    �� (!read_mem(fd, offs, ptr, hdr.sectionhdr.SizeOfRawData))
        ��ת quit;
    offs = hdr.sectionhdr.PointerToRelocations;
    ���� (i = 0; i < hdr.sectionhdr.NumberOfRelocations; ++i)
    {
        �ṹ pe_rsrc_reloc rel;
        �� (!read_mem(fd, offs, &rel, �󳤶� rel))
            ��ת quit;
        // printf("rsrc_reloc: %x %x %x\n", rel.offset, rel.size, rel.type);
        �� (rel.type != RSRC_RELTYPE)
            ��ת quit;
        put_elf_reloc(symtab_section, rsrc_section,
            rel.offset, R_XXX_RELATIVE, 0);
        offs += �󳤶� rel;
    }
    ret = 0;
quit:
    ���� ret;
}

/* ------------------------------------------------------------- */

��̬ �� *trimfront(�� *p)
{
    �� (*p && (�޷� ��)*p <= ' ')
        ++p;
    ���� p;
}

��̬ �� *trimback(�� *a, �� *e)
{
    �� (e > a && (�޷� ��)e[-1] <= ' ')
        --e;
    *e = 0;;
    ���� a;
}

/* ------------------------------------------------------------- */
��̬ �� pe_load_def(TCCState *s1, �� fd)
{
    �� state = 0, ret = -1, dllindex = 0, ord;
    �� line[400], dllname[80], *p, *x;
    FILE *fp;

    fp = fdopen(dup(fd), "rb");
    �� (fgets(line, �󳤶� line, fp))
    {
        p = trimfront(trimback(line, strchr(line, 0)));
        �� (0 == *p || ';' == *p)
            ����;

        ת�� (state) {
        ���� 0:
            �� (0 != strnicmp(p, "LIBRARY", 7))
                ��ת quit;
            pstrcpy(dllname, �󳤶� dllname, trimfront(p+7));
            ++state;
            ����;

        ���� 1:
            �� (0 != stricmp(p, "EXPORTS"))
                ��ת quit;
            ++state;
            ����;

        ���� 2:
            dllindex = add_dllref(s1, dllname);
            ++state;
            /* fall through */
        ȱʡ:
            /* get ordinal and will store in sym->st_value */
            ord = 0;
            x = strchr(p, ' ');
            �� (x) {
                *x = 0, x = strrchr(x + 1, '@');
                �� (x) {
                    �� *d;
                    ord = (��)strtol(x + 1, &d, 10);
                    �� (*d)
                        ord = 0;
                }
            }
            pe_putimport(s1, dllindex, p, ord);
            ����;
        }
    }
    ret = 0;
quit:
    fclose(fp);
    ���� ret;
}

/* ------------------------------------------------------------- */
��̬ �� pe_load_dll(TCCState *s1, ���� �� *filename)
{
    �� *p, *q;
    �� index, ret;

    ret = tcc_get_dllexports(filename, &p);
    �� (ret) {
        ���� -1;
    } �� �� (p) {
        index = add_dllref(s1, tcc_basename(filename));
        ���� (q = p; *q; q += 1 + strlen(q))
            pe_putimport(s1, index, q, 0);
        tcc_free(p);
    }
    ���� 0;
}

/* ------------------------------------------------------------- */
ST_FUNC �� pe_load_file(�ṹ TCCState *s1, ���� �� *filename, �� fd)
{
    �� ret = -1;
    �� buf[10];
    �� (0 == strcmp(tcc_fileextension(filename), ".def"))
        ret = pe_load_def(s1, fd);
    �� �� (pe_load_res(s1, fd) == 0)
        ret = 0;
    �� �� (read_mem(fd, 0, buf, 4) && 0 == memcmp(buf, "MZ\220", 4))
        ret = pe_load_dll(s1, filename);
    ���� ret;
}

/* ------------------------------------------------------------- */
#�綨�� TCC_TARGET_X86_64
��̬ �޷� pe_add_uwwind_info(TCCState *s1)
{
    �� (NULL == s1->uw_pdata) {
        s1->uw_pdata = find_section(tcc_state, ".pdata");
        s1->uw_pdata->sh_addralign = 4;
        s1->uw_sym = put_elf_sym(symtab_section, 0, 0, 0, 0, text_section->sh_num, NULL);
    }

    �� (0 == s1->uw_offs) {
        /* As our functions all have the same stackframe, we use one entry for all */
        ��̬ ���� �޷� �� uw_info[] = {
            0x01, // UBYTE: 3 Version , UBYTE: 5 Flags
            0x04, // UBYTE Size of prolog
            0x02, // UBYTE Count of unwind codes
            0x05, // UBYTE: 4 Frame Register (rbp), UBYTE: 4 Frame Register offset (scaled)
            // USHORT * n Unwind codes array
            // 0x0b, 0x01, 0xff, 0xff, // stack size
            0x04, 0x03, // set frame ptr (mov rsp -> rbp)
            0x01, 0x50  // push reg (rbp)
        };

        Section *s = text_section;
        �޷� �� *p;

        section_ptr_add(s, -s->data_offset & 3); /* align */
        s1->uw_offs = s->data_offset;
        p = section_ptr_add(s, �󳤶� uw_info);
        memcpy(p, uw_info, �󳤶� uw_info);
    }

    ���� s1->uw_offs;
}

ST_FUNC �� pe_add_unwind_data(�޷� start, �޷� end, �޷� stack)
{
    TCCState *s1 = tcc_state;
    Section *pd;
    �޷� o, n, d;
    �ṹ /* _RUNTIME_FUNCTION */ {
      DWORD BeginAddress;
      DWORD EndAddress;
      DWORD UnwindData;
    } *p;

    d = pe_add_uwwind_info(s1);
    pd = s1->uw_pdata;
    o = pd->data_offset;
    p = section_ptr_add(pd, �󳤶� *p);

    /* record this function */
    p->BeginAddress = start;
    p->EndAddress = end;
    p->UnwindData = d;

    /* put relocations on it */
    ���� (n = o + �󳤶� *p; o < n; o += �󳤶� p->BeginAddress)
        put_elf_reloc(symtab_section, pd, o,  R_X86_64_RELATIVE, s1->uw_sym);
}
#����
/* ------------------------------------------------------------- */
#�綨�� TCC_TARGET_X86_64
#���� PE_STDSYM(n,s) n
#��
#���� PE_STDSYM(n,s) "_" n s
#����

��̬ �� pe_add_runtime(TCCState *s1, �ṹ pe_info *pe)
{
    ���� �� *start_symbol;
    �� pe_type = 0;
    �� unicode_entry = 0;

    �� (find_elf_sym(symtab_section, PE_STDSYM("WinMain","@16")))
        pe_type = PE_GUI;
    ��
    �� (find_elf_sym(symtab_section, PE_STDSYM("wWinMain","@16"))) {
        pe_type = PE_GUI;
        unicode_entry = PE_GUI;
    }
    ��
    �� (TCC_OUTPUT_DLL == s1->output_type) {
        pe_type = PE_DLL;
        /* need this for 'tccelf.c:relocate_section()' */
        s1->output_type = TCC_OUTPUT_EXE;
    }
    �� {
        pe_type = PE_EXE;
        �� (find_elf_sym(symtab_section, "wmain"))
            unicode_entry = PE_EXE;
    }

    start_symbol =
        TCC_OUTPUT_MEMORY == s1->output_type
        ? PE_GUI == pe_type ? (unicode_entry ? "__runwwinmain" : "__runwinmain")
            : (unicode_entry ? "__runwmain" : "__runmain")
        : PE_DLL == pe_type ? PE_STDSYM("__dllstart","@12")
            : PE_GUI == pe_type ? (unicode_entry ? "__wwinstart": "__winstart")
                : (unicode_entry ? "__wstart" : "__start")
        ;

    �� (!s1->leading_underscore || strchr(start_symbol, '@'))
        ++start_symbol;

    /* grab the startup code from libtcc1 */
#�綨�� TCC_IS_NATIVE
    �� (TCC_OUTPUT_MEMORY != s1->output_type || s1->runtime_main)
#����
    set_elf_sym(symtab_section,
        0, 0,
        ELFW(ST_INFO)(STB_GLOBAL, STT_NOTYPE), 0,
        SHN_UNDEF, start_symbol);

    tcc_add_pragma_libs(s1);

    �� (0 == s1->nostdlib) {
        ��̬ ���� �� *libs[] = {
            TCC_LIBTCC1, "msvcrt", "kernel32", "", "user32", "gdi32", NULL
        };
        ���� �� **pp, *p;
        ���� (pp = libs; 0 != (p = *pp); ++pp) {
            �� (0 == *p) {
                �� (PE_DLL != pe_type && PE_GUI != pe_type)
                    ����;
            } �� �� (pp == libs && tcc_add_dll(s1, p, 0) >= 0) {
                ����;
            } �� {
                tcc_add_library_err(s1, p);
            }
        }
    }

    �� (TCC_OUTPUT_MEMORY == s1->output_type)
        pe_type = PE_RUN;
    pe->type = pe_type;
    pe->start_symbol = start_symbol;
}

��̬ �� pe_set_options(TCCState * s1, �ṹ pe_info *pe)
{
    �� (PE_DLL == pe->type) {
        /* XXX: check if is correct for arm-pe target */
        pe->imagebase = 0x10000000;
    } �� {
#�� �Ѷ���(TCC_TARGET_ARM)
        pe->imagebase = 0x00010000;
#��
        pe->imagebase = 0x00400000;
#����
    }

#�� �Ѷ���(TCC_TARGET_ARM)
    /* we use "console" subsystem by default */
    pe->subsystem = 9;
#��
    �� (PE_DLL == pe->type || PE_GUI == pe->type)
        pe->subsystem = 2;
    ��
        pe->subsystem = 3;
#����
    /* Allow override via -Wl,-subsystem=... option */
    �� (s1->pe_subsystem != 0)
        pe->subsystem = s1->pe_subsystem;

    /* set default file/section alignment */
    �� (pe->subsystem == 1) {
        pe->section_align = 0x20;
        pe->file_align = 0x20;
    } �� {
        pe->section_align = 0x1000;
        pe->file_align = 0x200;
    }

    �� (s1->section_align != 0)
        pe->section_align = s1->section_align;
    �� (s1->pe_file_align != 0)
        pe->file_align = s1->pe_file_align;

    �� ((pe->subsystem >= 10) && (pe->subsystem <= 12))
        pe->imagebase = 0;

    �� (s1->has_text_addr)
        pe->imagebase = s1->text_addr;
}

ST_FUNC �� pe_output_file(TCCState *s1, ���� �� *filename)
{
    �� ret;
    �ṹ pe_info pe;
    �� i;

    memset(&pe, 0, �󳤶� pe);
    pe.filename = filename;
    pe.s1 = s1;

    tcc_add_bcheck(s1);
    pe_add_runtime(s1, &pe);
    relocate_common_syms(); /* assign bss addresses */
    tcc_add_linker_symbols(s1);
    pe_set_options(s1, &pe);

    ret = pe_check_symbols(&pe);
    �� (ret)
        ;
    �� �� (filename) {
        pe_assign_addresses(&pe);
        relocate_syms(s1, s1->symtab, 0);
        ���� (i = 1; i < s1->nb_sections; ++i) {
            Section *s = s1->sections[i];
            �� (s->reloc) {
                relocate_section(s1, s);
                pe_relocate_rva(&pe, s);
            }
        }
        pe.start_addr = (DWORD)
            ((uintptr_t)tcc_get_symbol_err(s1, pe.start_symbol)
                - pe.imagebase);
        �� (s1->nb_errors)
            ret = -1;
        ��
            ret = pe_write(&pe);
        tcc_free(pe.sec_info);
    } �� {
#�綨�� TCC_IS_NATIVE
        pe.thunk = data_section;
        pe_build_imports(&pe);
        s1->runtime_main = pe.start_symbol;
#����
    }

    pe_free_imports(&pe);

#�綨�� PE_PRINT_SECTIONS
    pe_print_sections(s1, "tcc.log");
#����
    ���� ret;
}

/* ------------------------------------------------------------- */
