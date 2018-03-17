/** @file
   Interfejs klasy wielomianów

   @author Piotr Szuberski <p.szuberski@student.uw.edu.pl>,
   @copyright Uniwersytet Warszawski
   @date 2017-05-25,
*/

#ifndef WIELOMIANY_POLY_POLY_H
#define WIELOMIANY_POLY_POLY_H

#include <stdbool.h>
#include <stddef.h>
#include <assert.h>
#include <stdio.h>

/** Typ współczynników wielomianu */
typedef long poly_coeff_t;

/** Typ wykładników wielomianu */
typedef int poly_exp_t;

/**
  * Wskaźnik typu wielomianu - wielomian może być stałą (SIMPLE)
  * lub być wielomianem normalnym (COMPLEX)
  */
enum UnionTest {
    SIMPLE, /**< wielomian stały */
    COMPLEX /**< wielomian normalny */
};

/**
 * Struktura przechowująca wielomian w postaci listy jednomianów
 * posortowanej rosnąco względem wykładnika.
 */
typedef struct Poly {
    enum UnionTest tag; /**< tag : wskazuje na typ wielomianu (stała/wielomian normalny) */
    union {
        struct Mono *m; /**< m : wskaźnik na listę monomianów w wielomianie */
        poly_coeff_t c; /**< c : wartość wielomianu gdy jest stałą */
    } type; /**< type : określa typ wartości reprezentowanej przez unię */
} Poly;

/**
  * Struktura przechowująca jednomian
  * Jednomian ma postać `p * x^e`.
  * Będzie on traktowany jako wielomian nad kolejną zmienną (nie nad x).
  */
typedef struct Mono {
    struct Poly p; /**< p : współczynnik monomianu, może być wielomianem */
    poly_exp_t exp; /**< exp : wykładnik monomianu */
    struct Mono *next; /**< next : wskaźnik na następny monomian */
} Mono;

/*!
 * Tworzy wielomian, który jest współczynnikiem.
 * @param[in] c : wartość współczynnika
 * @return wielomian jako stała
 */
static inline Poly PolyFromCoeff(poly_coeff_t c)
{
    return (Poly) {.tag = SIMPLE, .type.c = c};
}

/*!
 * Tworzy wielomian tożsamościowo równy zeru.
 * @return wielomian zerowy
 */
static inline Poly PolyZero()
{
    return (Poly) {.tag = SIMPLE, .type.c = 0};
}

/*!
 * Tworzy jednomian `p * x^e`.
 * Przejmuje na własność zawartość struktury wskazywanej przez @p p.
 * @param[in] p : wielomian - współczynnik jednomianu
 * @param[in] e : wykładnik
 * @return jednomian `p * x^e`
 */
static inline Mono MonoFromPoly(const Poly *p, poly_exp_t e)
{
    return (Mono) {.exp = e, .p = *p, .next = NULL};
}

/**
 * Sprawdza, czy wielomian jest współczynnikiem.
 * @param[in] p : wielomian
 * @return Czy wielomian jest współczynnikiem?
 */
static inline bool PolyIsCoeff(const Poly *p)
{
    return p->tag == SIMPLE;
}
/**
 * Sprawdza, czy wielomian jest tożsamościowo równy zeru.
 * @param[in] p : wielomian
 * @return Czy wielomian jest równy zero?
 */
static inline bool PolyIsZero(const Poly *p)
{
    return ((p->tag == SIMPLE) && (p->type.c == 0)) ||
           ((p->tag == COMPLEX) && (p->type.m == NULL));
}
/**
 * Usuwa wielomian z pamięci.
 * @param[in] p : wielomian
 */
void PolyDestroy(Poly *p);

/**
 * Usuwa jednomian z pamięci.
 * @param[in] m : jednomian
 */
static inline void MonoDestroy(Mono *m)
{
    PolyDestroy(&m->p);
}

/**
 * Robi pełną, głęboką kopię wielomianu.
 * @param[in] p : wielomian
 * @return skopiowany wielomian
 */
Poly PolyClone(const Poly *p);


/**
 * Robi pełną, głęboką kopię jednomianu.
 * @param[in] m : jednomian
 * @return skopiowany jednomian
 */
static inline Mono MonoClone(const Mono *m)
{
    return (Mono) {.exp = m->exp, .p = PolyClone(&m->p)};
}

/**
 * Dodaje dwa wielomiany.
 * @param[in] p : wielomian
 * @param[in] q : wielomian
 * @return `p + q`
 */
Poly PolyAdd(const Poly *p, const Poly *q);

/**
 * Sumuje listę jednomianów i tworzy z nich wielomian.
 * Przejmuje na własność zawartość tablicy @p monos.
 * @param[in] count : liczba jednomianów
 * @param[in] monos : tablica jednomianów
 * @return wielomian będący sumą jednomianów
 */
Poly PolyAddMonos(unsigned count, const Mono monos[]);

/**
 * Mnoży dwa wielomiany.
 * @param[in] p : wielomian
 * @param[in] q : wielomian
 * @return `p * q`
 */
Poly PolyMul(const Poly *p, const Poly *q);

/**
 * Zwraca przeciwny wielomian.
 * @param[in] p : wielomian
 * @return `-p`
 */
Poly PolyNeg(const Poly *p);

/**
 * Odejmuje wielomian od wielomianu.
 * @param[in] p : wielomian
 * @param[in] q : wielomian
 * @return `p - q`
 */
Poly PolySub(const Poly *p, const Poly *q);

/**
 * Zwraca stopień wielomianu ze względu na zadaną zmienną (-1 dla wielomianu
 * tożsamościowo równego zeru).
 * Zmienne indeksowane są od 0.
 * Zmienna o indeksie 0 oznacza zmienną główną tego wielomianu.
 * Większe indeksy oznaczają zmienne wielomianów znajdujących się
 * we współczynnikach.
 * @param[in] p : wielomian
 * @param[in] var_idx : indeks zmiennej
 * @return stopień wielomianu @p p z względu na zmienną o indeksie @p var_idx
 */
poly_exp_t PolyDegBy(const Poly *p, unsigned var_idx);

/**
 * Zwraca stopień wielomianu (-1 dla wielomianu tożsamościowo równego zeru).
 * @param[in] p : wielomian
 * @return stopień wielomianu @p p
 */
poly_exp_t PolyDeg(const Poly *p);

/**
 * Sprawdza równość dwóch wielomianów.
 * @param[in] p : wielomian
 * @param[in] q : wielomian
 * @return `p = q`
 */
bool PolyIsEq(const Poly *p, const Poly *q);

/**
 * Wylicza wartość wielomianu w punkcie @p x.
 * Wstawia pod pierwszą zmienną wielomianu wartość @p x.
 * W wyniku może powstać wielomian, jeśli współczynniki są wielomianem
 * i zmniejszane są indeksy zmiennych w takim wielomianie o jeden.
 * Formalnie dla wielomianu @f$p(x_0, x_1, x_2, \ldots)@f$ wynikiem jest
 * wielomian @f$p(x, x_0, x_1, \ldots)@f$.
 * @param[in] p
 * @param[in] x
 * @return @f$p(x, x_0, x_1, \ldots)@f$
 */
Poly PolyAt(const Poly *p, poly_coeff_t x);

/**
 * Funkcja wypisująca wielomian w postaci
 * (WSPÓŁCZYNNIK, WYKŁADNIK)+(WSPÓŁCZYNNIK, WYKŁADNIK)+...
 * w porządku rosnącym.
 * @param p wypisywany wielomian
 */
void PolyPrint(const Poly *p);

/**
 * Funkcja zwraca wielomian p, w którym pod zmienną xi podstawiamy wielomian
 * polys[i]
 * @param[in] p : wielomian, pod który podstawiane są wielomiany
 * @param[in] count : ilość możliwych wielomianów do podstawienia
 * @param[in] x : tablica wielomianów podstawianych
 * @return wielomian p z podstawionymi wielomianami z polys pod odpowiednie xi
 */
Poly PolyCompose(const Poly *p, unsigned count, const Poly x[]);

#endif //WIELOMIANY_POLY_POLY_H
