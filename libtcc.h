#��δ���� LIBTCC_H
#���� LIBTCC_H

#��δ���� LIBTCCAPI
# ���� LIBTCCAPI
#����

#�綨�� __cplusplus
�ⲿ "C" {
#����

�ṹ TCCState;

���Ͷ��� �ṹ TCCState TCCState;

/* create a new TCC compilation context */
LIBTCCAPI TCCState *tcc_new(��);

/* free a TCC compilation context */
LIBTCCAPI �� tcc_delete(TCCState *s);

/* set CONFIG_TCCDIR at runtime */
LIBTCCAPI �� tcc_set_lib_path(TCCState *s, ���� �� *path);

/* set error/warning display callback */
LIBTCCAPI �� tcc_set_error_func(TCCState *s, �� *error_opaque,
    �� (*error_func)(�� *opaque, ���� �� *msg));

/* set options as from command line (multiple supported) */
LIBTCCAPI �� tcc_set_options(TCCState *s, ���� �� *str);

/*****************************/
/* preprocessor */

/* add include path */
LIBTCCAPI �� tcc_add_include_path(TCCState *s, ���� �� *pathname);

/* add in system include path */
LIBTCCAPI �� tcc_add_sysinclude_path(TCCState *s, ���� �� *pathname);

/* define preprocessor symbol 'sym'. Can put optional value */
LIBTCCAPI �� tcc_define_symbol(TCCState *s, ���� �� *sym, ���� �� *value);

/* undefine preprocess symbol 'sym' */
LIBTCCAPI �� tcc_undefine_symbol(TCCState *s, ���� �� *sym);

/*****************************/
/* compiling */

/* add a file (C file, dll, object, library, ld script). Return -1 if error. */
LIBTCCAPI �� tcc_add_file(TCCState *s, ���� �� *filename);

/* compile a string containing a C source. Return -1 if error. */
LIBTCCAPI �� tcc_compile_string(TCCState *s, ���� �� *buf);

/*****************************/
/* linking commands */

/* set output type. MUST BE CALLED before any compilation */
LIBTCCAPI �� tcc_set_output_type(TCCState *s, �� output_type);
#���� TCC_OUTPUT_MEMORY   1 /* output will be run in memory (default) */
#���� TCC_OUTPUT_EXE      2 /* executable file */
#���� TCC_OUTPUT_DLL      3 /* dynamic library */
#���� TCC_OUTPUT_OBJ      4 /* object file */
#���� TCC_OUTPUT_PREPROCESS 5 /* only preprocess (used internally) */

/* equivalent to -Lpath option */
LIBTCCAPI �� tcc_add_library_path(TCCState *s, ���� �� *pathname);

/* the library name is the same as the argument of the '-l' option */
LIBTCCAPI �� tcc_add_library(TCCState *s, ���� �� *libraryname);

/* add a symbol to the compiled program */
LIBTCCAPI �� tcc_add_symbol(TCCState *s, ���� �� *name, ���� �� *val);

/* output an executable, library or object file. DO NOT call
   tcc_relocate() before. */
LIBTCCAPI �� tcc_output_file(TCCState *s, ���� �� *filename);

/* link and run main() function and return its value. DO NOT call
   tcc_relocate() before. */
LIBTCCAPI �� tcc_run(TCCState *s, �� argc, �� **argv);

/* do all relocations (needed before using tcc_get_symbol()) */
LIBTCCAPI �� tcc_relocate(TCCState *s1, �� *ptr);
/* possible values for 'ptr':
   - TCC_RELOCATE_AUTO : Allocate and manage memory internally
   - NULL              : return required memory size for the step below
   - memory address    : copy code to memory passed by the caller
   returns -1 if error. */
#���� TCC_RELOCATE_AUTO (��*)1

/* return symbol value or NULL if not found */
LIBTCCAPI �� *tcc_get_symbol(TCCState *s, ���� �� *name);

#�綨�� __cplusplus
}
#����

#����
