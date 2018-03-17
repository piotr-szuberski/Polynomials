/** @file
   Biblioteka operacji na wielomianach wielu zmiennych

   @author Piotr Szuberski <p.szuberski@student.uw.edu.pl>,
   @copyright Uniwersytet Warszawski
   @date 2017-05-25,
*/

#include <stdlib.h>
#include "utils.h"
#include "poly.h"

/**
 * Pomocnicza funkcja dla PolyAddMonos do quicksorta z biblioteki,
 * do sortowania malejąco jednomianów względem wykładnika.
 * @param[in] x1
 * @param[in] x2
 * @return wartość porównywaną
 */
static poly_exp_t MonoExpComparator(const void *x1, const void *x2);

/**
 * Uzupełnia listę monomianów o "ogon" pozostałej listy monomianów,
 * pomocnicza funkcja funkcji PolyMul.
 * @param[in] toComplete: lista monomianów, która będzie uzupełniona
 * o monomiany z klonowanej listy
 * @param[in] cloned: klonowana lista
 */
static void MonoComplete(Mono **toComplete, Mono *cloned);

/**
 * Tworzy zaalokowany monomian posiadający zerowy współczynnik
 * dla zainicjalizowania pola p, później musi zostać nadpisany.
 * @param[in] x: wykładnik monomianu
 * @return zaalokowany monomian
 */
static Mono *MonoEmpty(poly_exp_t x);

/**
 * Na podstawie konstrukcji listy monomianów wybiera,
 * jaki wielomian utworzyć, pomocnicza funkcja dla PolyAdd
 * i PolyAddMonos.
 * @param[in] m: lista monomianów
 * @return wybrany wielomian
 */
static Poly PolyChoose(Mono *m);

/**
 * Pomocnicza funkcja dla PolyMul, liczy liczbę bloków
 * potrzebnych do zaalokowania w pamięci
 * @param[in] m: lista monomianów
 * @return liczba monomianów w liście
 */
static poly_exp_t MonoCountBlocks(const Mono *m);

/**
 * Usuwa dynamicznie zaalokowany jednomian z pamięci.
 * @param[in] m: jednomian
 */
static void PolyDestroyMono(Mono *m);

/**
 * Funkcja wykonująca algorytm binpower
 * @param[in] x: podstawa potęgi
 * @param[in] n: wykładnik potęgi
 * @return 'x^n'
 */
static poly_coeff_t PolyPower(poly_coeff_t x, poly_exp_t n);

/**
 * Dołącza pozostałą część listy
 * @param[in] toComplete: do niego dołącza
 * @param[in] cloned: przekazuje wartości
 */
static void MonoCompleteNoConsts(Mono *toComplete, Mono *cloned);

/**
 * Funkcja pomocnicza funkcji wypisującej wielomian
 * @param[in] p: wypisywany wielomian
 */
static void PolyPrintMain(const Poly *p);

/**
 * Funkcja dodająca wielomiany i usuwająca swoje argumenty,
 * @param[in] p
 * @param[in] q
 * @return 'p + q'
 */
static Poly PolyAddNoConsts(Poly *p, Poly *q);

/**
 * Funkcja pomocnicza dla PolyCompose, dodatkowo zlicza, jak głęboko
 * w danym wywołaniu rekurencyjnym się znajduje
 * @param[in] p : wielomian, na którym wykonuje operacje PolyCompose
 * @param[in] i : zlicza, na jakim poziomie w wielomianie się znajduje funkcja
 * @param[in] count : ilość pozostałych możliwych zejść wgłąb wielomianu
 * @param[in] x : tablica wielomianów do podstawienia
 * @return wielomian p podstawiony wielomianami z tablicy polys
 */
static Poly PolyComposeHelper(const Poly *p, unsigned  i, unsigned count,
                              const Poly x[]);

/**
 * Funkcja wykonująca szybkie potęgowanie na wielomianach wielu zmiennych
 * @param[in] p : wielomian bazowy
 * @param[in] n : wykładnik potęgi
 * @return p^n
 */
static Poly PolyBinPower(const Poly *p, poly_exp_t n);


void PolyDestroy(Poly *p)
{
    if (!PolyIsCoeff(p)) {
        while (p->type.m != NULL) {
            Mono *tmp = p->type.m;
            p->type.m = p->type.m->next;
            PolyDestroy(&tmp->p);
            free(tmp);
        }
    }
}

Poly PolyClone(const Poly *p)
{
    Poly p_clone;
    Mono *doll, *tmp, *wanderer, *helper;

    if (PolyIsCoeff(p)) {
        return PolyFromCoeff(p->type.c);
    }
    else {
        tmp = p->type.m;
        doll = MonoEmpty(-1);
        wanderer = doll;
        while (tmp != NULL) {
            helper = MonoEmpty(tmp->exp);
            helper->p = PolyClone(&tmp->p);
            wanderer->next = helper;
            wanderer = helper;
            tmp = tmp->next;
        }
        p_clone.tag = p->tag;
        p_clone.type.m = doll->next;
        PolyDestroyMono(doll);
        return p_clone;
    }
}

Poly PolyAdd(const Poly *p, const Poly *q)
{
    Mono *mono_zero, *doll, *wanderer, *new_mono,
            *mono_p, *mono_q;
    Poly added, new;

    if (PolyIsZero(p)) {
        return PolyClone(q);
    }
    else if (PolyIsZero(q)) {
        return PolyClone(p);
    }
    else if (PolyIsCoeff(p) && PolyIsCoeff(q)) {
        if (p->type.c + q->type.c != 0) {
            return PolyFromCoeff(p->type.c + q->type.c);
        }
        else {
            return PolyZero();
        }
    }
    else if (PolyIsCoeff(p) || PolyIsCoeff(q)) {
        mono_zero = MonoEmpty(0);
        new.tag = COMPLEX;
        new.type.m = mono_zero;
        if (PolyIsCoeff(p)) {
            mono_zero->p = PolyFromCoeff(p->type.c);
            added = PolyAdd(&new, q);
        }
        else {
            mono_zero->p = PolyFromCoeff(q->type.c);
            added = PolyAdd(&new, p);
        }
        PolyDestroy(&new);
        return added;
    }
    else {
        doll = MonoEmpty(-2);
        wanderer = doll;
        mono_p = p->type.m;
        mono_q = q->type.m;
        while (mono_p != NULL && mono_q != NULL) {
            new_mono = MonoEmpty(-2);
            if (mono_p->exp != mono_q->exp) {
                if (mono_p->exp < mono_q->exp) {
                    new_mono->exp = mono_p->exp;
                    new_mono->p = PolyClone(&mono_p->p);
                    mono_p = mono_p->next;
                }
                else {
                    new_mono->exp = mono_q->exp;
                    new_mono->p = PolyClone(&mono_q->p);
                    mono_q = mono_q->next;
                }
                wanderer->next = new_mono;
                wanderer = wanderer->next;
            }
            else {
                new_mono->p = PolyAdd(&mono_p->p, &mono_q->p);
                if (PolyIsZero(&new_mono->p)) {
                    PolyDestroyMono(new_mono);
                }
                else {
                    new_mono->exp = mono_p->exp;
                    wanderer->next = new_mono;
                    wanderer = wanderer->next;
                }
                mono_p = mono_p->next;
                mono_q = mono_q->next;
            }
        }
        if (mono_p != NULL || mono_q != NULL) {
            MonoComplete(&wanderer, mono_p);
            MonoComplete(&wanderer, mono_q);
        }
        wanderer = doll->next;
        PolyDestroyMono(doll);
        added = PolyChoose(wanderer);
        return added;
    }
}

Poly PolyAddMonos(unsigned count, const Mono monos[])
{
    Poly p;
    Mono *doll, *backer, *wanderer, *newMono;
    unsigned i = 0;
    Mono *tmp = (Mono*) monos;

    qsort(tmp, count, sizeof(Mono), MonoExpComparator);
    doll = MonoEmpty(-2);
    backer = NULL;
    wanderer = doll;
    while (i < count) {
        if (!PolyIsZero(&tmp[i].p)) {
            if (wanderer->exp != tmp[i].exp) {
                newMono = MonoEmpty(tmp[i].exp);
                newMono->p = tmp[i].p;
                wanderer->next = newMono;
                backer = wanderer;
                wanderer = wanderer->next;
            }
            else {
                wanderer->p = PolyAddNoConsts(&wanderer->p, &tmp[i].p);
                if (PolyIsZero(&wanderer->p)) {
                    PolyDestroyMono(wanderer);
                    wanderer = backer;
                    wanderer->next = NULL;
                }
            }
        }
        i++;
    }
    wanderer = doll->next;
    free(doll);
    p = PolyChoose(wanderer);
    return p;
}


Poly PolyMul(const Poly *p, const Poly *q)
{
    Poly to_multiply, score, tmp;
    Mono *monoZero, *header_p, *header_q;
    poly_exp_t counter, counter1 = 0, counter2 = 0;

    if (PolyIsZero(p) || PolyIsZero(q)) {
        return PolyZero();
    }
    else if (PolyIsCoeff(p) || PolyIsCoeff(q)) {
        if (PolyIsCoeff(p) && PolyIsCoeff(q)) {
            return PolyFromCoeff(p->type.c * q->type.c);
        }
        else {
            monoZero = MonoEmpty(0);
            to_multiply.tag = COMPLEX;
            to_multiply.type.m = monoZero;
            if (PolyIsCoeff(p)) {
                monoZero->p = PolyFromCoeff(p->type.c);
                score = PolyMul(&to_multiply, q);
            }
            else {
                monoZero->p = PolyFromCoeff(q->type.c);
                score = PolyMul(&to_multiply, p);
            }
            PolyDestroy(&to_multiply);
            return score;
        }
    }
    else {
        header_p = p->type.m;
        header_q = q->type.m;
        counter1 = MonoCountBlocks(header_p);
        counter2 = MonoCountBlocks(header_q);
        counter = counter1 * counter2;
        Mono *monos = (Mono*) malloc(sizeof(Mono) * counter);
        assert(monos != NULL);
        poly_exp_t i = 0;
        while (header_p != NULL) {
            header_q = q->type.m;
            while (header_q != NULL) {
                tmp = PolyMul(&header_p->p, &header_q->p);
                monos[i++] = MonoFromPoly(&tmp, header_p->exp + header_q->exp);
                header_q = header_q->next;
            }
            header_p = header_p->next;
        }
        Poly score2 = PolyAddMonos((unsigned)counter, monos);
        free(monos);
        return score2;
    }
}

Poly PolyNeg(const Poly *p)
{
    Poly p_neg;
    Mono *doll, *tmp, *wanderer, *helper;

    if (PolyIsCoeff(p)) {
        return PolyFromCoeff(p->type.c * (-1));
    }
    else {
        tmp = p->type.m;
        doll = MonoEmpty(-1);
        wanderer = doll;
        while (tmp != NULL) {
            helper = MonoEmpty(tmp->exp);
            helper->p = PolyNeg(&tmp->p);
            wanderer->next = helper;
            wanderer = helper;
            tmp = tmp->next;
        }
        p_neg.tag = p->tag;
        p_neg.type.m = doll->next;
        PolyDestroyMono(doll);
        return p_neg;
    }
}

Poly PolySub(const Poly *p, const Poly *q)
{
    Poly q_neg = PolyNeg(q);
    Poly subbed = PolyAdd(p, &q_neg);
    PolyDestroy(&q_neg);
    return subbed;
}

poly_exp_t PolyDegBy(const Poly *p, unsigned var_idx)
{
    poly_exp_t deg = -2, y;
    Mono *helper;

    if (PolyIsZero(p)) {
        return -1;
    }
    else if (PolyIsCoeff(p)) {
        return 0;
    }
    else if (var_idx == 0) {
        helper = p->type.m;
        while (helper->next != NULL) {
            helper = helper->next;
        }
        return helper->exp;
    }
    else {
        helper = p->type.m;
        while (helper != NULL) {
            y = PolyDegBy(&helper->p, var_idx - 1);
            if (y > deg) {
                deg = y;
            }
            helper = helper->next;
        }
        return deg;
    }
}

poly_exp_t PolyDeg(const Poly *p)
{
    poly_exp_t deg = -2, current;
    Mono *header;

    if (PolyIsZero(p)) {
        return -1;
    }
    else if (PolyIsCoeff(p)) {
        return 0;
    }
    else {
        header = p->type.m;
        while (header != NULL) {
            current = PolyDeg(&header->p);
            current += header->exp;
            if (current > deg) {
                deg = current;
            }
            header = header->next;
        }
        return deg;
    }
}

bool PolyIsEq(const Poly *p, const Poly *q)
{
    bool is_eq;
    Mono *tmp_p, *tmp_q;

    if (PolyIsZero(p) || PolyIsZero(q)) {
        is_eq = PolyIsZero(p) && PolyIsZero(q);
    }
    else if (PolyIsCoeff(p) || PolyIsCoeff(q)) {
        if (!(PolyIsCoeff(p) && PolyIsCoeff(q))) {
            is_eq = false;
        }
        else {
            is_eq = p->type.c == q->type.c;
        }
    }
    else {
        is_eq = true;
        tmp_p = p->type.m;
        tmp_q = q->type.m;

        while (tmp_p != NULL && tmp_q != NULL && is_eq) {
            if (tmp_p->exp != tmp_q->exp) {
                is_eq = false;
            }
            else {
                is_eq = PolyIsEq(&tmp_p->p, &tmp_q->p);
            }
            tmp_p = tmp_p->next;
            tmp_q = tmp_q->next;
        }
        if (!(tmp_q == NULL && tmp_p == NULL && is_eq)) {
            is_eq = false;
        }
    }
    return is_eq;
}

Poly PolyAt(const Poly *p, poly_coeff_t x)
{
    Poly atted, helper, number, tmp;
    Mono *header;
    poly_coeff_t  counter = 0, multiplier;

    if (PolyIsZero(p)) {
        return PolyZero();
    }
    else if (PolyIsCoeff(p)) {
        return PolyFromCoeff(p->type.c);
    }
    else {
        atted = PolyZero();
        header = p->type.m;
        while (header != NULL) {
            multiplier = PolyPower(x, header->exp);
            if (PolyIsCoeff(&header->p)) {
                counter += multiplier * header->p.type.c;
            }
            else {
                number = PolyFromCoeff(multiplier);
                tmp = PolyMul(&number, &header->p);
                atted = PolyAddNoConsts(&tmp, &atted);
            }
            header = header->next;
        }
        if (counter != 0) {
            number = PolyFromCoeff(counter);
            helper = PolyAdd(&number, &atted);
            PolyDestroy(&atted);
            atted = helper;
        }
        return atted;
    }
}

void PolyPrint(const Poly *p)
{
    PolyPrintMain(p);
    printf("\n");
}

static void PolyPrintMain(const Poly *p)
{
    if (PolyIsZero(p)) {
        printf("0");
    }
    else if (PolyIsCoeff(p)) {
        printf("%ld", p->type.c);
    }
    else {
        Mono *helper = p->type.m;
        if (helper->next != NULL ) {
            while (helper->next != NULL) {
                printf("(");
                PolyPrintMain(&helper->p);
                printf(",%d)+", helper->exp);
                helper = helper->next;
            }
        }
        printf("(");
        PolyPrintMain(&helper->p);
        printf(",%d)", helper->exp);
    }
}

static Poly PolyBinPower(const Poly *p, poly_exp_t n)
{
    Poly a_tmp, b_tmp;
    Poly a = PolyFromCoeff(1), b = PolyClone(p);
    poly_exp_t c = n;

    while (c > 0) {
        if (c % 2 == 1) {
            a_tmp = a;
            a = PolyMul(&a, &b);
            PolyDestroy(&a_tmp);
        }
        b_tmp = b;
        b = PolyMul(&b, &b);
        PolyDestroy(&b_tmp);
        c /= 2;
    }
    PolyDestroy(&b);
    return a;
}

static Poly PolyComposeHelper(const Poly *p, unsigned  i, unsigned count,
                              const Poly x[])
{
    Mono *helper = p->type.m;
    Poly *polys_array;
    unsigned polys_length = 1, polys_count = 0;
    poly_exp_t n;
    Poly coeff, powered, result, result_tmp;

    if (count == 0) {
        while (helper->exp == 0 && !PolyIsCoeff(&helper->p) &&
                helper->p.type.m->exp == 0) {
            helper = helper->p.type.m;
        }
        if (helper->exp == 0 && PolyIsCoeff(&helper->p)) {
            return PolyFromCoeff(helper->p.type.c);
        }
        else {
            return PolyZero();
        }
    }
    else {
        polys_array = (Poly*) malloc(sizeof(Poly) * polys_length);
        while (helper != NULL) {
            n = helper->exp;
            if (PolyIsCoeff(&helper->p)) {
                coeff = PolyFromCoeff(helper->p.type.c);
            }
            else {
                coeff = PolyComposeHelper(&helper->p, i + 1, count - 1, x);
            }
            if (!PolyIsZero(&coeff)) {
                powered = PolyBinPower(&x[i], n);
                result = PolyMul(&powered, &coeff);
                PolyDestroy(&powered);
                PolyDestroy(&coeff);
                if (polys_count == polys_length) {
                    polys_length *= 2;
                    polys_array = (Poly *) realloc(polys_array,
                                                   sizeof(Poly) * polys_length);
                }
                polys_array[polys_count++] = result;
            }
            helper = helper->next;
        }
        result = PolyZero();
        for (unsigned j = 0; j < polys_count; j++) {
            result_tmp = result;
            result = PolyAdd(&result, &polys_array[j]);
            PolyDestroy(&result_tmp);
            PolyDestroy(&(polys_array[j]));
        }
        free(polys_array);
        return result;
    }
}

Poly PolyCompose(const Poly *p, unsigned count, const Poly x[])
{
    if (PolyIsCoeff(p)) {
        return PolyFromCoeff(p->type.c);
    }
    else {
        return PolyComposeHelper(p, 0, count, x);
    }
}

static void PolyDestroyMono(Mono *m)
{
    PolyDestroy(&m->p);
    free(m);
}

static Mono *MonoEmpty(poly_exp_t x)
{
    Mono *empty = (Mono*) malloc(sizeof(Mono));
    assert(empty != NULL);

    empty->exp = x;
    empty->next = NULL;
    empty->p.tag = COMPLEX;
    empty->p.type.m = NULL;
    return empty;
}

static Poly PolyChoose(Mono *m)
{
    Poly chosen_one;

    if (m == NULL) {
        chosen_one = PolyZero();
    }
    else {
        if (m->exp == 0 &&
            m->next == NULL &&
            m->p.tag == SIMPLE) {
            chosen_one = PolyFromCoeff(m->p.type.c);
            PolyDestroyMono(m);
        }
        else {
            chosen_one.type.m = m;
            chosen_one.tag = COMPLEX;
        }
    }
    return chosen_one;
}

static void MonoComplete(Mono **toComplete, Mono *cloned)
{
    Mono *new_mono;

    while (cloned != NULL) {
        new_mono = MonoEmpty(cloned->exp);
        new_mono->p = PolyClone(&cloned->p);
        (*toComplete)->next = new_mono;
        *toComplete = (*toComplete)->next;
        cloned = cloned->next;
    }
}

static poly_exp_t MonoExpComparator(const void *x1, const void *x2)
{
    Mono *y1 = (Mono*) x1;
    Mono *y2 = (Mono*) x2;
    return y1->exp - y2->exp;
}

static poly_exp_t MonoCountBlocks(const Mono *m)
{
    poly_exp_t count = 0;

    while (m != NULL) {
        count++;
        m = m->next;
    }
    return count;
}

static poly_coeff_t PolyPower(poly_coeff_t x, poly_exp_t n)
{
    poly_coeff_t a = 1, b = x;
    poly_exp_t c = n;

    while (c > 0) {
        if (c % 2 == 1) {
            a *= b;
        }
        b *= b;
        c = c / 2;
    }
    return a;
}

static void MonoCompleteNoConsts(Mono *toComplete, Mono *cloned)
{
    while (cloned != NULL) {
        toComplete->next = cloned;
        toComplete = toComplete->next;
        cloned = cloned->next;
    }
}

static Poly PolyAddNoConsts(Poly *p, Poly *q)
{
    Mono *mono_zero, *doll, *wanderer, *mono_p,
        *mono_q, *destroyer_p, *destroyer_q;
    Poly added, new, helper;

    if (PolyIsZero(p) || PolyIsZero(q)) {
        if (PolyIsZero(p) && PolyIsZero(q)) {
            return PolyZero();
        }
        else if (PolyIsZero(p)) {
            return *q;
        }
        else {
            return *p;
        }
    }
    else if (PolyIsCoeff(p) && PolyIsCoeff(q)) {
        if (p->type.c + q->type.c != 0) {
            return PolyFromCoeff(p->type.c + q->type.c);
        }
        else {
            return PolyZero();
        }
    }
    else if (PolyIsCoeff(p) || PolyIsCoeff(q)) {
        mono_zero = MonoEmpty(0);
        new.tag = COMPLEX;
        new.type.m = mono_zero;

        if (PolyIsCoeff(p)) {
            mono_zero->p = PolyFromCoeff(p->type.c);
            added = PolyAddNoConsts(&new, q);
        }
        else {
            mono_zero->p = PolyFromCoeff(q->type.c);
            added = PolyAddNoConsts(&new, p);
        }
        return added;
    }
    else {
        doll = MonoEmpty(-2);
        wanderer = doll;
        mono_p = p->type.m;
        mono_q = q->type.m;
        while (mono_p != NULL && mono_q != NULL) {
            if (mono_p->exp != mono_q->exp) {
                if (mono_p->exp < mono_q->exp) {
                    wanderer->exp = mono_p->exp;
                    wanderer->next = mono_p;
                    mono_p = mono_p->next;
                }
                else {
                    wanderer->exp = mono_q->exp;
                    wanderer->next = mono_q;
                    mono_q = mono_q->next;
                }
                wanderer = wanderer->next;
            }
            else {
                helper = PolyAddNoConsts(&mono_p->p, &mono_q->p);
                destroyer_p = mono_p;
                destroyer_q = mono_q;
                mono_p = mono_p->next;
                mono_q = mono_q->next;
                if (PolyIsZero(&helper)) {
                    free(destroyer_p);
                    free(destroyer_q);
                }
                else {
                    wanderer->next = destroyer_p;
                    destroyer_p->p = helper;
                    wanderer = wanderer->next;
                    free(destroyer_q);
                }
            }
        }
        if (mono_p != NULL || mono_q != NULL) {
            MonoCompleteNoConsts(wanderer, mono_p);
            MonoCompleteNoConsts(wanderer, mono_q);
        }
        wanderer = doll->next;
        free(doll);
        added = PolyChoose(wanderer);
        return added;
    }
}