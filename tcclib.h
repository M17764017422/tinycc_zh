/* Simple libc header for TCC 
 * 
 * Add any function you want from the libc there. This file is here
 * only for your convenience so that you do not need to put the whole
 * glibc include files on your floppy disk 
 */
#��δ���� _TCCLIB_H
#���� _TCCLIB_H

#���� <stddef.h>
#���� <stdarg.h>

/* stdlib.h */
�� *calloc(size_t nmemb, size_t size);
�� *malloc(size_t size);
�� free(�� *ptr);
�� *realloc(�� *ptr, size_t size);
�� atoi(���� �� *nptr);
�� �� strtol(���� �� *nptr, �� **endptr, �� base);
�޷� �� �� strtoul(���� �� *nptr, �� **endptr, �� base);
�� exit(��);

/* stdio.h */
���Ͷ��� �ṹ __FILE FILE;
#���� EOF (-1)
�ⲿ FILE *stdin;
�ⲿ FILE *stdout;
�ⲿ FILE *stderr;
FILE *fopen(���� �� *path, ���� �� *mode);
FILE *fdopen(�� fildes, ���� �� *mode);
FILE *freopen(����  �� *path, ���� �� *mode, FILE *stream);
�� fclose(FILE *stream);
size_t  fread(�� *ptr, size_t size, size_t nmemb, FILE *stream);
size_t  fwrite(�� *ptr, size_t size, size_t nmemb, FILE *stream);
�� fgetc(FILE *stream);
�� *fgets(�� *s, �� size, FILE *stream);
�� getc(FILE *stream);
�� getchar(��);
�� *gets(�� *s);
�� ungetc(�� c, FILE *stream);
�� fflush(FILE *stream);
�� putchar (�� c);

�� printf(���� �� *format, ...);
�� fprintf(FILE *stream, ���� �� *format, ...);
�� sprintf(�� *str, ���� �� *format, ...);
�� snprintf(�� *str, size_t size, ����  ��  *format, ...);
�� asprintf(�� **strp, ���� �� *format, ...);
�� dprintf(�� fd, ���� �� *format, ...);
�� vprintf(���� �� *format, va_list ap);
�� vfprintf(FILE  *stream,  ����  �� *format, va_list ap);
�� vsprintf(�� *str, ���� �� *format, va_list ap);
�� vsnprintf(�� *str, size_t size, ���� ��  *format, va_list ap);
�� vasprintf(��  **strp,  ����  �� *format, va_list ap);
�� vdprintf(�� fd, ���� �� *format, va_list ap);

�� perror(���� �� *s);

/* string.h */
�� *strcat(�� *dest, ���� �� *src);
�� *strchr(���� �� *s, �� c);
�� *strrchr(���� �� *s, �� c);
�� *strcpy(�� *dest, ���� �� *src);
�� *memcpy(�� *dest, ���� �� *src, size_t n);
�� *memmove(�� *dest, ���� �� *src, size_t n);
�� *memset(�� *s, �� c, size_t n);
�� *strdup(���� �� *s);
size_t strlen(���� �� *s);

/* dlfcn.h */
#���� RTLD_LAZY       0x001
#���� RTLD_NOW        0x002
#���� RTLD_GLOBAL     0x100

�� *dlopen(���� �� *filename, �� flag);
���� �� *dlerror(��);
�� *dlsym(�� *handle, �� *symbol);
�� dlclose(�� *handle);

#���� /* _TCCLIB_H */
