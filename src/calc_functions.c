/** @file
   Zbiór funkcji wykonywanych przez kalkulator

   @author Piotr Szuberski <p.szuberski@student.uw.edu.pl>,
   @copyright Uniwersytet Warszawski
   @date 2017-06-07,
*/

#include <stdlib.h>
#include <limits.h>
#include <memory.h>
#include "utils.h"
#include "calc_functions.h"

/**
 * Funkcja podwajająca rozmiar tablicy wielomianów
 * @param[in] polys : tablica wielomianów
 * @param[in] length : długość tablicy
 */
static void ExpandPoly(Poly **polys, unsigned *length);

void Add(int line, PolyStack **ps)
{
    Poly poly_p, poly_q, poly_result;

    if (!PolyStackIsEmpty(*ps)) {
        poly_p = PolyStackPop(ps);
        if (!PolyStackIsEmpty(*ps)) {
            poly_q = PolyStackPop(ps);
            poly_result = PolyAdd(&poly_p, &poly_q);
            PolyDestroy(&poly_p);
            PolyDestroy(&poly_q);
            PolyStackPush(ps, poly_result);
        }
        else {
            ErrorStackUnderflow(line);
            PolyStackPush(ps, poly_p);
        }
    }
    else {
        ErrorStackUnderflow(line);
    }
}

void Clone(int line, PolyStack **ps)
{
    Poly poly_p, poly_result;

    if (!PolyStackIsEmpty(*ps)) {
        poly_p = PolyStackTop(*ps);
        poly_result = PolyClone(&poly_p);
        PolyStackPush(ps, poly_result);
    }
    else {
        ErrorStackUnderflow(line);
    }
}

void Mul(int line, PolyStack **ps)
{
    Poly poly_p, poly_q, poly_result;

    if (!PolyStackIsEmpty(*ps)) {
        poly_p = PolyStackPop(ps);
        if (!PolyStackIsEmpty(*ps)) {
            poly_q = PolyStackPop(ps);
            poly_result = PolyMul(&poly_p, &poly_q);
            PolyDestroy(&poly_p);
            PolyDestroy(&poly_q);
            PolyStackPush(ps, poly_result);
        }
        else {
            ErrorStackUnderflow(line);
            PolyStackPush(ps, poly_p);
        }
    }
    else {
        ErrorStackUnderflow(line);
    }
}


void Neg(int line, PolyStack **ps)
{
    Poly poly_p, poly_result;

    if (!PolyStackIsEmpty(*ps)) {
        poly_p = PolyStackPop(ps);
        poly_result = PolyNeg(&poly_p);
        PolyDestroy(&poly_p);
        PolyStackPush(ps, poly_result);
    }
    else {
        ErrorStackUnderflow(line);
    }
}

void Sub(int line, PolyStack **ps)
{
    Poly poly_p, poly_q, poly_result;

    if (!PolyStackIsEmpty(*ps)) {
        poly_p = PolyStackPop(ps);
        if (!PolyStackIsEmpty(*ps)) {
            poly_q = PolyStackPop(ps);
            poly_result = PolySub(&poly_p, &poly_q);
            PolyDestroy(&poly_p);
            PolyDestroy(&poly_q);
            PolyStackPush(ps, poly_result);
        }
        else {
            ErrorStackUnderflow(line);
            PolyStackPush(ps, poly_p);
        }
    }
    else {
        ErrorStackUnderflow(line);
    }
}

void IsEq(int line, PolyStack **ps)
{
    Poly poly_p, poly_q;

    if (!PolyStackIsEmpty(*ps)) {
        poly_p = PolyStackPop(ps);
        if (!PolyStackIsEmpty(*ps)) {
            poly_q = PolyStackPop(ps);
            printf("%d\n", PolyIsEq(&poly_p, &poly_q));
            PolyStackPush(ps, poly_q);
            PolyStackPush(ps, poly_p);
        }
        else {
            ErrorStackUnderflow(line);
            PolyStackPush(ps, poly_p);
        }
    }
    else {
        ErrorStackUnderflow(line);
    }
}

void IsCoeff(int line, const PolyStack *ps)
{
    Poly poly_p;

    if (!PolyStackIsEmpty(ps)) {
        poly_p = PolyStackTop(ps);
        printf("%d\n", PolyIsCoeff(&poly_p));
    }
    else {
        ErrorStackUnderflow(line);
    }
}

void IsZero(int line, const PolyStack *ps)
{
    Poly poly_p;

    if (!PolyStackIsEmpty(ps)) {
        poly_p = PolyStackTop(ps);
        printf("%d\n", PolyIsZero(&poly_p));
    }
    else {
        ErrorStackUnderflow(line);
    }
}

void Deg(int line, const PolyStack *ps)
{
    Poly poly_p;
    if (!PolyStackIsEmpty(ps)) {
        poly_p = PolyStackTop(ps);
        printf("%d\n", PolyDeg(&poly_p));
    }
    else {
        ErrorStackUnderflow(line);
    }
}

void Print(int line, const PolyStack *ps)
{
    Poly poly_p;

    if (!PolyStackIsEmpty(ps)) {
        poly_p = PolyStackTop(ps);
        PolyPrint(&poly_p);
    }
    else {
        ErrorStackUnderflow(line);
    }
}

void Pop(int line, PolyStack **ps)
{
    Poly poly_p;

    if (!PolyStackIsEmpty(*ps)) {
        poly_p = PolyStackPop(ps);
        PolyDestroy(&poly_p);
    }
    else {
        ErrorStackUnderflow(line);
    }
}

void DegBy(const char *input, int line, const PolyStack *ps)
{
    int res;
    int a, b, c;
    unsigned long deg_by_value;
    Poly poly_p;

    if (!PolyStackIsEmpty(ps)) {
        res = (sscanf(input, "DEG_BY%n %n%lu%n", &a, &b, &deg_by_value, &c));
        if ((res == 1 && (b - a) == 1 && input[b] >= '0' && input[b] <= '9' &&
             input[c] == END) && deg_by_value <= UINT_MAX) {
            poly_p = PolyStackTop(ps);
            printf("%d\n", PolyDegBy(&poly_p, (unsigned) deg_by_value));
        }
        else {
            ErrorCorruptedDegBy(line);
        }
    }
    else {
        ErrorStackUnderflow(line);
    }
}

void At(const char *input, int line, PolyStack **ps)
{
    unsigned int index, i = 0;
    size_t length = strlen(input);
    char at_string[length - 3 + 1];
    char comparator[LMIN_LENGTH + 1];
    poly_coeff_t at_value;
    Poly poly_p, poly_result;

    if (!PolyStackIsEmpty(*ps)) {
        index = 3;
        if(!((length == 3 && input[2] == SPACE) ||
             (length == 4 && input[2] != SPACE && input[3] != '-'))) {
            if (input[3] == '-') {
                at_string[i] = input[3];
                i++;
                index++;
            }
            while ((index < length) && input[index] >= '0' &&
                   input[index] <= '9') {
                at_string[i] = input[index];
                i++;
                index++;
            }
            at_string[i] = END;
            if (i <= LMIN_LENGTH && index == length) {
                at_value = atol(at_string);
                sprintf(comparator, "%ld", at_value);
                if (strcmp(comparator, at_string) == 0) {
                    poly_p = PolyStackPop(ps);
                    poly_result = PolyAt(&poly_p, at_value);
                    PolyDestroy(&poly_p);
                    PolyStackPush(ps, poly_result);
                }
                else {
                    ErrorCorruptedAt(line);
                }
            }
            else {
                ErrorCorruptedAt(line);
            }
        }
        else {
            ErrorCorruptedAt(line);
        }
    }
    else {
        ErrorStackUnderflow(line);
    }
}

static void ExpandPoly(Poly **polys, unsigned *length)
{
    unsigned prev_size = *length;
    *length *= 2;
    *polys = (Poly*) realloc(*polys, sizeof(Poly) * (*length));
    for (unsigned i = prev_size; i < *length; i++) {
        (*polys)[i] = PolyZero();
    }
}

void Compose(const char *input, int line, PolyStack **ps)
{
    int res;
    int a, b, c;
    bool go_on = true;
    unsigned long compose_value;
    unsigned fader, count = 0, polys_length = START;
    Poly *polys;
    Poly poly_p, poly_result;

    res = (sscanf(input, "COMPOSE%n %n%lu%n", &a, &b, &compose_value, &c));
    if ((res == 1 && (b - a) == 1 && input[b] >= '0' && input[b] <= '9' &&
         input[c] == END) && !PolyStackIsEmpty(*ps)) {
        if  (compose_value < UINT_MAX) {
            poly_p = PolyStackPop(ps);
            polys = (Poly *) malloc(sizeof(Poly) * polys_length);
            fader = (unsigned) compose_value;
            while (go_on && fader > 0) {
                if (!PolyStackIsEmpty(*ps)) {
                    if (count == polys_length) {
                        ExpandPoly(&polys, &polys_length);
                    }
                    polys[count++] = PolyStackPop(ps);
                    fader--;
                } else {
                    go_on = false;
                    ErrorStackUnderflow(line);
                }
            }
            if (go_on) {
                poly_result = PolyCompose(&poly_p, count, polys);
                PolyStackPush(ps, poly_result);
                for (unsigned i = 0; i < count; i++) {
                    PolyDestroy(&polys[i]);
                }
                PolyDestroy(&poly_p);
            } else {
                for (long i = (long) count - 1; i >= 0; i--) {
                    PolyStackPush(ps, polys[i]);
                }
                PolyStackPush(ps, poly_p);
            }
            free(polys);
        }
        else {
            ErrorStackUnderflow(line);
        }
    }

    else if (!(res == 1 && (b - a) == 1 && input[b] >= '0' && input[b] <= '9' &&
               input[c] == END)) {
        ErrorCorruptedCompose(line);
    }
    else if (PolyStackIsEmpty(*ps)) {
        ErrorStackUnderflow(line);
    }
}