/** @file
   Header przedefiniowujący standardowe funkcje

   @author Piotr Szuberski <p.szuberski@student.uw.edu.pl>,
   @copyright Uniwersytet Warszawski
   @date 2017-05-25,
*/

#ifndef UTILS_H
#define UTILS_H

#ifdef UNIT_TESTING

#include <stdio.h>

#ifdef exit
#undef exit
#endif /* exit */
#define exit(status) mock_exit(status)
extern void mock_exit(int status);

#ifdef printf
#undef printf
#endif /* printf */
#define printf(...) mock_printf(__VA_ARGS__)
extern int mock_printf(const char *format, ...);

#ifdef fprintf
#undef fprintf
#endif /* fprintf */
#define fprintf(...) mock_fprintf(__VA_ARGS__)
extern int mock_fprintf(FILE * const file, const char *format, ...);

#ifdef scanf
#undef scanf
#endif /* scanf */
#define scanf(format, ...) mock_scanf(format"%n", ##__VA_ARGS__, &read_char_count)
extern int mock_scanf(const char *format, ...);

#ifdef getchar
#undef getchar
#endif /* getchar */
#define getchar() mock_getchar()
extern int mock_getchar();

#ifdef ungetc
#undef ungetc
#endif /* ungetc */
#define ungetc(c, stream) mock_ungetc(c, stream)
extern int mock_ungetc(int c, FILE *stream);

/* Redirect assert to mock_assert() so assertions can be caught by cmocka. */
#ifdef assert
#undef assert
#endif /* assert */
#define assert(expression) \
    mock_assert((int)(expression), #expression, __FILE__, __LINE__)
void mock_assert(const int result, const char* expression, const char *file,
                 const int line);

#ifdef calloc
#undef calloc
#endif /* calloc */
#define calloc(num, size) _test_calloc(num, size, __FILE__, __LINE__)
#ifdef malloc
#undef malloc
#endif /* malloc */
#define malloc(size) _test_malloc(size, __FILE__, __LINE__)
#ifdef realloc
#undef realloc
#endif /* realloc */
#define realloc(ptr, size) _test_realloc(ptr, size, __FILE__, __LINE__)
#ifdef free
#undef free
#endif /* free */
#define free(ptr) _test_free(ptr, __FILE__, __LINE__)
void* _test_calloc(size_t number_of_elements, size_t size, const char* file, int line);
void* _test_malloc(size_t size, const char* file, int line);
void* _test_realloc(void* const ptr, size_t size, const char* file, int line);
void _test_free(void* const ptr, const char* file, const int line);


int poly_main();
#define main(...) poly_main(__VA_ARGS__)

/* #define static */

#endif /* UNIT_TESTING */

#endif /* UTILS_H*/