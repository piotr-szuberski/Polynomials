/** @file
   Kalkulator wykonujący operacje na wielomianach wielu zmiennych

   @author Piotr Szuberski <p.szuberski@student.uw.edu.pl>,
   @copyright Uniwersytet Warszawski
   @date 2017-05-25,
*/
#include <strings.h>
#include <memory.h>
#include <stdlib.h>
#include "utils.h"
#include "calc_poly.h"
#include "calc_functions.h"


char *ExtractNumber(int *index, size_t *new_length, char *c)
{
    char *number = (char*) malloc(sizeof(char) * (START + 1));
    assert(number != NULL);
    unsigned int i = 0;

    *new_length = START;
    if (*c == '-') {
        number[i] = *c;
        i++;
        *c = getchar();
        (*index)++;
    }
    while (*c >= '0' && *c <= '9') {
        if (i == *new_length) {
            Expand(&number, new_length);
        }
        number[i] = *c;
        i++;
        (*index)++;
        *c = getchar();
    }
    number[i] = END;
    *new_length = (size_t) i;
    if (i == 0 || (i == 1 && number[0] == '-')) {
        free(number);
        return NULL;
    }
    else {
        return number;
    }
}

bool ParseExp(char *input, poly_exp_t *exp)
{
    poly_exp_t value;
    size_t length = strlen(input);
    char *comparator;

    if (length == 0 || input[0] == '-' || length > IMAX_LENGTH) {
        if (length > 0) {
            free(input);
        }
        return false;
    }
    comparator = (char*) malloc(sizeof(char) * (length + 1));
    assert(comparator != NULL);
    value = atoi(input);
    sprintf(comparator, "%d", value);
    if (strcmp(comparator, input) == 0) {
        *exp = (poly_exp_t) value;
        free(comparator);
        free(input);
        return true;
    }
    else {
        free(input);
        free(comparator);
        return false;
    }
}

bool ParseCoeff(char *input, poly_coeff_t *coeff, bool *minus)
{
    long int value;
    char *comparator;
    size_t length = strlen(input);

    *minus = (length != 0 && input[0] == '-') ? true : false;
    if (length == 0 ||
        (length > 1 && (input[0] == '-' &&
                input[1] > '9' && input[1] < '0')) ||
        (input[0] == '-' && length > 20) ||
        (input[0] >= '0' && input[0] <= '9' && length > 19)) {
        free(input);
        return false;
    }
    value = atol(input);
    comparator = (char*) malloc(sizeof(char) * (length + 1));
    assert(comparator != NULL);
    sprintf(comparator, "%ld", value);
    if (strcmp(input, comparator) == 0 || strcmp(input, "-0") == 0) {
        *coeff = value;
        free(input);
        free(comparator);
        return true;
    }
    else {
        free(input);
        free(comparator);
        return false;
    }
}

void OpenBracketCase(char *c, int line, int *column, bool *go_on,
                            DigitStack **ds)
{
    DigitStackPush(ds, *c);
    (*column)++;
    *c = getchar();
    if (!(*c == '(' || (*c >= '0' && *c <= '9') || *c == '-')) {
        *go_on = ErrorParsingPoly(line, *column);
    }
}

void PlusCase(char *c, int line, int *column, bool *go_on,
                     DigitStack **ds, const MonoStack *ms)
{
    DigitStackPush(ds, *c);
    (*column)++;
    *c = getchar();
    if (*c != '(' || MonoStackIsEmpty(ms)) {
        *go_on = ErrorParsingPoly(line, *column);
    }
}

void NumberCase(char *c, int line, int *column, bool *go_on,
                       const DigitStack *ds, PolyStack **ps)
{
    bool minus;
    char *coe_str;
    size_t coe_len;
    poly_coeff_t coeff;
    Poly poly;

    coe_str = ExtractNumber(column, &coe_len, c);
    *go_on = ParseCoeff(coe_str, &coeff, &minus);
    if (*go_on) {
        if ((*c == ',' && !DigitStackIsEmpty(ds) &&
             DigitStackTop(ds) == '(') ||
            ((*c == NEW_LINE || *c == EOF) &&
             DigitStackIsEmpty(ds))) {
            poly = PolyFromCoeff(coeff);
            PolyStackPush(ps, poly);
        }
        else {
            *go_on = ErrorParsingPoly(line, *column);
        }
    } else {
        if (coe_len == 0) {
            *go_on = ErrorParsingPoly(line, *column);
        }
        else if (minus) {
            *go_on = ErrorParsingPoly(line, *column -
                                            (int) coe_len + LMIN_LENGTH - 1);
        }
        else {
            *go_on = ErrorParsingPoly(line, *column -
                                            (int) coe_len + LMAX_LENGTH - 1);
        }
    }
}

void ComaCase(char *c, int line, int *column, bool *go_on,
                     DigitStack **ds, PolyStack **ps, MonoStack **ms)
{
    size_t exp_len;
    char *exp_str;
    poly_exp_t exp;
    Poly poly;
    Mono mono_tmp;

    (*column)++;
    *c = getchar();
    if (*c >= '0' && *c <= '9') {
        exp_str = ExtractNumber(column, &exp_len, c);
        *go_on = ParseExp(exp_str, &exp);
        if (*go_on && !DigitStackIsEmpty(*ds) &&
            DigitStackPop(ds) == '(' && *c == ')') {
            poly = PolyStackPop(ps);
            mono_tmp = MonoFromPoly(&poly, exp);
            MonoStackPush(ms, mono_tmp);
        }
        else {
            if (exp_len == 0 || DigitStackIsEmpty(*ds)) {
                *go_on = ErrorParsingPoly(line, *column);
            }
            else if (exp_len >= IMAX_LENGTH) {
                *go_on = ErrorParsingPoly(line, *column -
                                              (int) exp_len + IMAX_LENGTH - 1);
            }
            else {
                *go_on = ErrorParsingPoly(line, *column);
            }
        }
    }
    else {
        *go_on = ErrorParsingPoly(line, *column);
    }
}

void ClosingBracketCase(char *c, int line, int *column, bool *go_on,
                               DigitStack **ds, PolyStack **ps, MonoStack **ms)
{
    unsigned mono_length, i;
    Mono *monos;
    Poly poly;

    (*column)++;
    *c = getchar();
    if (!MonoStackIsEmpty(*ms) &&
        (*c == NEW_LINE || *c == EOF || *c == '+' || *c == ',')) {
        if (*c == NEW_LINE || *c == EOF || *c == ',') {
            i = 0;
            mono_length = 1;
            monos = (Mono*) malloc(sizeof(Mono) * mono_length);
            assert(monos != NULL);
            if (!MonoStackIsEmpty(*ms)) {
                monos[i++] = MonoStackPop(ms);
            }
            else {
                free(monos);
                *go_on = ErrorParsingPoly(line, *column);
            }
            while (*go_on && !DigitStackIsEmpty(*ds) &&
                   DigitStackTop(*ds) == '+') {
                DigitStackPop(ds);
                if (!MonoStackIsEmpty(*ms)) {
                    if (i == mono_length) {
                        mono_length = 2 * mono_length;
                        monos = (Mono*) realloc(monos,
                                                mono_length * sizeof(Mono));
                        assert(monos != NULL);
                    }
                    monos[i++] = MonoStackPop(ms);
                }
                else {
                    free(monos);
                    *go_on = ErrorParsingPoly(line, *column);
                }
            }
            mono_length = i;
            if (*go_on) {
                poly = PolyAddMonos(mono_length, monos);
                PolyStackPush(ps, poly);
                free(monos);
            }
        }
        else if (*c != '+') {
            *go_on = ErrorParsingPoly(line, *column);
        }
    }
    else {
        *go_on = ErrorParsingPoly(line, *column);
    }
}

bool ParsePoly(Poly *p, int line, char c)
{
    DigitStack *ds;
    PolyStack *ps;
    MonoStack *ms;
    bool go_on;
    int column = 1;

    DigitStackInit(&ds);
    PolyStackInit(&ps);
    MonoStackInit(&ms);
    go_on = true;
    if (!((c >= '0' && c <= '9') || c == '-' || c == '(')) {
        go_on = ErrorParsingPoly(line, column);
    }
    while (go_on && (c != NEW_LINE && c != EOF)) {
        switch (c) {
            case '(' :
                OpenBracketCase(&c, line, &column, &go_on, &ds);
                break;
            case '+' :
                PlusCase(&c, line, &column, &go_on, &ds, ms);
                break;
            case '-': case '0': case '1': case '2': case '3': case '4':
                      case '5': case '6': case '7': case '8': case '9':
                NumberCase(&c, line, &column, &go_on, ds, &ps);
                break;
            case ',' :
                ComaCase(&c, line, &column, &go_on, &ds, &ps, &ms);
                break;
            case ')' :
                ClosingBracketCase(&c, line, &column, &go_on, &ds, &ps, &ms);
                break;
            default :
                go_on = ErrorParsingPoly(line, column);
                break;
        }
    }
    if (go_on && (!DigitStackIsEmpty(ds)
                  || !MonoStackIsEmpty(ms)
                  || PolyStackIsEmpty(ps))) {
        go_on = ErrorParsingPoly(line, column);
    }
    else if (go_on && !PolyStackIsEmpty(ps)) {
        *p = PolyStackPop(&ps);
        if (!PolyStackIsEmpty(ps)) {
            PolyDestroy(p);
            go_on = ErrorParsingPoly(line, column);
        }
    }
    while (c != NEW_LINE && c != EOF) {
        c = getchar();
    }
    MonoStackDelete(&ms);
    DigitStackDelete(&ds);
    PolyStackDelete(&ps);
    return go_on;
}

void Expand(char **input, size_t *length)
{
    *length = 2 * (*length);
    *input = (char*) realloc(*input, *length + 1);
    assert(*input != NULL);
}

char *ReadLine(char in)
{
    char c;
    char *tmp;
    unsigned int i = 0;
    size_t length = START;

    tmp = (char*) malloc(sizeof(char) * (length + 1));
    assert(tmp != NULL);
    tmp[i++] = in;
    c = getchar();
    while (c != NEW_LINE && c != EOF) {
        if (i == length) {
            Expand(&tmp, &length);
        }
        tmp[i++] = c;
        c = getchar();
    }
    tmp[i] = END;
    return tmp;
}

int main(void)
{
    int line = 1;
    char *input = NULL;
    char in;
    PolyStack *ps;
    Poly poly_result;

    PolyStackInit(&ps);
    in = getchar();
    while (in != EOF) {
        if ((in >= 'a' && in <= 'z') || (in >= 'A' && in <= 'Z')) {
            input = ReadLine(in);
            if (strcmp(input, ADD) == 0) {
                Add(line, &ps);
            } else if (strcmp(input, ZERO) == 0) {
                PolyStackPush(&ps, PolyZero());
            } else if (strcmp(input, CLONE) == 0) {
                Clone(line, &ps);
            } else if (strcmp(input, MUL) == 0) {
                Mul(line, &ps);
            } else if (strcmp(input, NEG) == 0) {
                Neg(line, &ps);
            } else if (strcmp(input, SUB) == 0) {
                Sub(line, &ps);
            } else if (strcmp(input, IS_EQ) == 0) {
                IsEq(line, &ps);
            } else if (strcmp(input, IS_COEFF) == 0) {
                IsCoeff(line, ps);
            } else if (strcmp(input, IS_ZERO) == 0) {
                IsZero(line, ps);
            } else if (strcmp(input, DEG) == 0) {
                Deg(line, ps);
            } else if (strcmp(input, PRINT) == 0) {
                Print(line, ps);
            } else if (strcmp(input, POP) == 0) {
                Pop(line, &ps);
            } else if (memcmp(input, DEG_BY, 6) == 0) {
                DegBy(input, line, ps);
            } else if (memcmp(input, AT, 2) == 0) {
                At(input, line, &ps);
            } else if (memcmp(input, COMPOSE, 7) == 0) {
                Compose(input, line, &ps);
            } else {
                ErrorWrongCommand(line);
            }
            free(input);
        }
        else {
            if (ParsePoly(&poly_result, line, in)) {
                PolyStackPush(&ps, poly_result);
            }
        }
        line++;
        in = getchar();
    }
    PolyStackDelete(&ps);
    return 0;
}