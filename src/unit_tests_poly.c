/** @file
   Testy jednostkowe z użyciem biblioteki cmocka do funkcji PolyCompose
   z biblioteki poly.c i COMPOSE z kalkulatora wielomianów wielu zmiennych

   @author Piotr Szuberski <p.szuberski@student.uw.edu.pl>,
   @copyright Uniwersytet Warszawski
   @date 2017-05-25,
*/


#include <limits.h>
#include <stdarg.h>
#include <string.h>
#include <setjmp.h>
#include "cmocka.h"
#include "calc_poly.h"

/**
 * podstawiona main z calc_poly
 */
int poly_main(void);

/**
 * zmienna typu jmp_buf do atrapy funkcji main
 */
static jmp_buf jmp_at_exit;

/**
 * zmienna do atrapy exit
 */
static int exit_status;

/**
 * Atrapa funkcji main
 */
int mock_main(void) {
    if (!setjmp(jmp_at_exit))
        return poly_main();
    return exit_status;
}

/**
 * Atrapa funkcji exit
 */
void mock_exit(int status) {
    exit_status = status;
    longjmp(jmp_at_exit, 1);
}

int mock_fprintf(FILE* const file, const char *format, ...) CMOCKA_PRINTF_ATTRIBUTE(2, 3);
int mock_printf(const char *format, ...) CMOCKA_PRINTF_ATTRIBUTE(1, 2);

/**
 * Pomocnicze bufory, do których piszą atrapy funkcji printf i fprintf oraz
 * pozycje zapisu w tych buforach. Pozycja zapisu wskazuje bajt o wartości 0.
 */
static char fprintf_buffer[256]; /**< bufor trzymający ostatniego fprintfa */
static char printf_buffer[256]; /**< bufor trzymający ostatniego printfa */
static int fprintf_position = 0; /**< iterator do buforu fprintfa */
static int printf_position = 0; /**< iterator do buforu printfa */

/**
 * Atrapa funkcji fprintf sprawdzająca poprawność wypisywania na stderr.
 */
int mock_fprintf(FILE* const file, const char *format, ...) {
    int return_value;
    va_list args;

    assert_true(file == stderr);
    assert_true((size_t)fprintf_position < sizeof(fprintf_buffer));

    va_start(args, format);
    return_value = vsnprintf(fprintf_buffer + fprintf_position,
                             sizeof(fprintf_buffer) - fprintf_position,
                             format,
                             args);
    va_end(args);

    fprintf_position += return_value;
    assert_true((size_t)fprintf_position < sizeof(fprintf_buffer));
    return return_value;
}

/**
 * Atrapa funkcji printf sprawdzająca poprawność wypisywania na stderr.
 */
int mock_printf(const char *format, ...) {
    int return_value;
    va_list args;

    assert_true((size_t)printf_position < sizeof(printf_buffer));

    va_start(args, format);
    return_value = vsnprintf(printf_buffer + printf_position,
                             sizeof(printf_buffer) - printf_position,
                             format,
                             args);
    va_end(args);

    printf_position += return_value;
    assert_true((size_t)printf_position < sizeof(printf_buffer));
    return return_value;
}

/**
 *  Pomocniczy bufor, z którego korzystają atrapy funkcji operujących na stdin.
 */
static char input_stream_buffer[256]; /**< bufor imitujący stdin */
static int input_stream_position = 0; /**< wskazuje, w którym miejscu jest stdin */
static int input_stream_end = 0; /**< wskazuje koniec stdin */
int read_char_count; /**< licznik znaków */

/**
 * Atrapa funkcji scanf używana do przechwycenia czytania z stdin.
 */
int mock_scanf(const char *format, ...) {
    va_list fmt_args;
    int ret;

    va_start(fmt_args, format);
    ret = vsscanf(input_stream_buffer + input_stream_position, format, fmt_args);
    va_end(fmt_args);

    if (ret < 0) { /* ret == EOF */
        input_stream_position = input_stream_end;
    }
    else {
        assert_true(read_char_count >= 0);
        input_stream_position += read_char_count;
        if (input_stream_position > input_stream_end) {
            input_stream_position = input_stream_end;
        }
    }
    return ret;
}

/**
 * Atrapa funkcji getchar używana do przechwycenia czytania z stdin.
 */
int mock_getchar() {
    if (input_stream_position < input_stream_end)
        return input_stream_buffer[input_stream_position++];
    else
        return EOF;
}

/**
 * Atrapa funkcji ungetc.
 * Obsługiwane jest tylko standardowe wejście.
 */
int mock_ungetc(int c, FILE *stream) {
    assert_true(stream == stdin);
    if (input_stream_position > 0)
        return input_stream_buffer[--input_stream_position] = c;
    else
        return EOF;
}

/**
 * Funkcja zerująca bufory stdin i stdout, wołana przed każdym wywołaniem
 * testów korzystających z stdin i stdout
 * @param[in] state : nieużywany
 * @return 0, w przypadku sukcesu
 */
static int test_setup(void **state) {
    (void) state;

    memset(input_stream_buffer, 0, sizeof(input_stream_buffer));
    memset(printf_buffer, 0, sizeof(printf_buffer));
    memset(fprintf_buffer, 0, sizeof(fprintf_buffer));

    input_stream_position = 0;
    input_stream_end = 0;
    printf_position = 0;
    fprintf_position = 0;

    assert((size_t)input_stream_end < sizeof(input_stream_buffer));

    return 0;
}

/**
 * Pomocnicze zmienne do testowania funkcji PolyCompose
 */
Poly poly_base; /**< podstawowy wielomian */
Poly poly_model; /**< wielomian wzorcowy */
Poly x_0; /**< wielomian podstawiany w PolyCompose */
Poly poly_res; /**< wynik funkcji PolyCompose */

/**
 * Pomocnicza funkcja do otrzymywania wielomianu w formie ax^b
 * @param[in] a : współczynnik
 * @param[in] b : wykładnik
 * @return ax^b
 */
static Poly create_p_poly(long a, int b)
{
    Poly p;
    Mono tmp;

    p = PolyFromCoeff(a);
    tmp = MonoFromPoly(&p, b);
    p = PolyAddMonos(1, &tmp);

    return p;
}

/**
 * Test z wielomianem zerowym i count zerowym
 * @param[in] state : nieużywany
 */
static void p_0_count_0_test(void **state) {
    (void) state;

    poly_model = PolyZero();
    poly_base = PolyZero();
    poly_res = PolyCompose(&poly_base, 0, NULL);

    assert_true(PolyIsEq(&poly_model, &poly_res));
}

/**
 * Test z wielomianem zerowym i count równym 1, podstawiającym stałą wartość
 * @param[in] state : nieużywany
 */
static void p_0_count_1_x0_const_test(void **state) {
    (void) state;

    poly_model = PolyZero();
    poly_base = PolyZero();
    x_0 = PolyFromCoeff(1);
    poly_res = PolyCompose(&poly_base, 1, &x_0);

    assert_true(PolyIsEq(&poly_model, &poly_res));
}

/**
 * Test z wielomianem stałym i count zerowym
 * @param[in] state : nieużywany
 */
static void p_const_count_0_test(void **state) {
    (void) state;

    poly_model = PolyFromCoeff(1);
    poly_base = PolyFromCoeff(1);
    poly_res = PolyCompose(&poly_base, 0, NULL);

    assert_true(PolyIsEq(&poly_model, &poly_res));
}

/**
 * Test z wielomianem stałym i count równym 1, podstawiającym wartość
 * stałą różną od wielomianu
 * @param[in] state : nieużywany
 */
static void p_const_count_1_x0_const_not_p_test(void **state) {
    (void) state;

    poly_model = PolyFromCoeff(5);
    poly_base = PolyFromCoeff(5);
    x_0 = PolyFromCoeff(2);
    poly_res = PolyCompose(&poly_base, 1, &x_0);

    assert_true(PolyIsEq(&poly_model, &poly_res));
}

/**
 * Test z wielomianem normalnym i count zerowym
 * @param[in] state : nieużywany
 */
static void  p_poly_count_0_test(void **state) {
    (void) state;

    poly_model = PolyZero();
    poly_base = create_p_poly(2, 2); /* poly_base = 2x^2 */

    poly_res = PolyCompose(&poly_base, 0, NULL);

    assert_true(PolyIsEq(&poly_model, &poly_res));

    PolyDestroy(&poly_base);
    PolyDestroy(&poly_res);
}

/**
 * Test z wielomianem normalnym i count równym 1, podstawiającym wartość stałą
 * @param[in] state : nieużywany
 */
static void p_poly_count_1_x0_const_test(void **state) {
    (void) state;

    poly_model = PolyFromCoeff(2);
    poly_base = create_p_poly(2, 2); /* poly_base = 2x^2 */
    x_0 = PolyFromCoeff(1);
    poly_res = PolyCompose(&poly_base, 1, &x_0);

    assert_true(PolyIsEq(&poly_model, &poly_res));

    PolyDestroy(&poly_base);
    PolyDestroy(&poly_res);
}

/**
 * Test z wielomianem normalnym i count równym 1, podstawiającym wielomian
 * normalny
 * @param[in] state : nieużywany
 */
static void p_poly_count_1_x0_poly_test(void **state) {
    (void) state;

    poly_model = create_p_poly(18, 6); /* poly_model = 18x^6 */
    poly_base = create_p_poly(2, 2); /* poly_base = 2x^2 */
    x_0 = create_p_poly(3, 3); /* x_0 = 3x^3 */
    poly_res = PolyCompose(&poly_base, 1, &x_0);

    assert_true(PolyIsEq(&poly_model, &poly_res));

    PolyDestroy(&poly_model);
    PolyDestroy(&poly_base);
    PolyDestroy(&poly_res);
    PolyDestroy(&x_0);
}

/**
 * Test COMPOSE bez parametru
 * @param[in] state : nieużywany
 */
static void no_param_test(void **state) {
    (void) state;

    char expected[] = "ERROR 1 WRONG COUNT\n";

    strcpy(input_stream_buffer, "COMPOSE\n");
    input_stream_end = (int) strlen(input_stream_buffer);

    mock_main();

    assert_string_equal(fprintf_buffer, expected);
}

/**
 * Test COMPOSE 0
 * @param[in] state : nieużywany
 */
static void compose_min_test(void **state) {
    (void) state;

    char expected[] = "5\n";

    strcpy(input_stream_buffer, \
            "((2,5),6)+(3,3)+(3,0)+(2,0)\nCOMPOSE 0\nPRINT\n");
    input_stream_end = (int) strlen(input_stream_buffer);

    mock_main();

    assert_string_equal(printf_buffer, expected);
}

/**
 * Test COMPOSE z maksymalną wartością
 * @param[in] state : nieużywany
 */
static void compose_max_test(void **state) {
    (void) state;

    char expected[] = "ERROR 1 STACK UNDERFLOW\n";

    sprintf(input_stream_buffer, "COMPOSE %u\n", UINT_MAX);
    input_stream_end = (int) strlen(input_stream_buffer);

    mock_main();

    assert_string_equal(fprintf_buffer, expected);
}

/**
 * Test COMPOSE -1
 * @param[in] state : nieużywany
 */
static void compose_one_below_min_test(void **state) {
    (void) state;

    char expected[] = "ERROR 1 WRONG COUNT\n";

    strcpy(input_stream_buffer, "COMPOSE -1\n");
    input_stream_end = (int) strlen(input_stream_buffer);

    mock_main();

    assert_string_equal(fprintf_buffer, expected);
}

/**
 * Test COMPOSE z maksymalną wartością + 1
 * @param[in] state : nieużywany
 */
static void compose_one_over_max_test(void **state) {
    (void) state;

    char expected[] = "ERROR 1 STACK UNDERFLOW\n";

    sprintf(input_stream_buffer, "COMPOSE %lu\n",\
            ((unsigned long) UINT_MAX + 1));
    input_stream_end = (int) strlen(input_stream_buffer);

    mock_main();

    assert_string_equal(fprintf_buffer, expected);
}

/**
 * Test COMPOSE z wartością dużo większą niż maksymalna
 * @param[in] state : nieużywany
 */
static void compose_way_over_max_test(void **state) {
    (void) state;

    char expected[] = "ERROR 1 STACK UNDERFLOW\n";

    strcpy(input_stream_buffer, "COMPOSE 9999999999999999999999999999999\n");
    input_stream_end = (int) strlen(input_stream_buffer);

    mock_main();

    assert_string_equal(fprintf_buffer, expected);
}

/**
 * Test COMPOSE z niepoprawną wartością ze znaków bez cyfr
 * @param[in] state : nieużywany
 */
static void compose_letters_test(void **state) {
    (void) state;

    char expected[] = "ERROR 1 WRONG COUNT\n";

    strcpy(input_stream_buffer, "COMPOSE sdjkfndsifnaui\n");
    input_stream_end = (int) strlen(input_stream_buffer);

    mock_main();

    assert_string_equal(fprintf_buffer, expected);
}

/**
 * Test COMPOSE z niepoprawną wartością ze znaków z cyframi
 * @param[in] state : nieużywany
 */
static void compose_digits_and_letters_test(void **state) {
    (void) state;

    char expected[] = "ERROR 1 WRONG COUNT\n";

    strcpy(input_stream_buffer, "COMPOSE 543frd34fds4wrt434s\n");
    input_stream_end = (int) strlen(input_stream_buffer);

    mock_main();

    assert_string_equal(fprintf_buffer, expected);
}


/**
 * Funkcja main testująca funkcję PolyCompose i COMPOSE kalkulatora
 */
int main(void) {
    const struct CMUnitTest tests1[] = {
            /* PolyCompose tests */
            cmocka_unit_test(p_0_count_0_test),
            cmocka_unit_test(p_0_count_1_x0_const_test),
            cmocka_unit_test(p_const_count_0_test),
            cmocka_unit_test(p_const_count_1_x0_const_not_p_test),
            cmocka_unit_test(p_poly_count_0_test),
            cmocka_unit_test(p_poly_count_1_x0_const_test),
            cmocka_unit_test(p_poly_count_1_x0_poly_test)
    };

    const struct CMUnitTest tests2[] = {
            /* COMPOSE tests */
            cmocka_unit_test_setup(no_param_test, test_setup),
            cmocka_unit_test_setup(compose_min_test, test_setup),
            cmocka_unit_test_setup(compose_max_test, test_setup),
            cmocka_unit_test_setup(compose_one_below_min_test, test_setup),
            cmocka_unit_test_setup(compose_one_over_max_test, test_setup),
            cmocka_unit_test_setup(compose_way_over_max_test, test_setup),
            cmocka_unit_test_setup(compose_letters_test, test_setup),
            cmocka_unit_test_setup(compose_digits_and_letters_test, test_setup)
    };

    return cmocka_run_group_tests(tests1, NULL, NULL) ||
            cmocka_run_group_tests(tests2, NULL, NULL);
}