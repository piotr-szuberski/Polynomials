/** @file
   Header zbioru funkcji wykonywanych przez kalkulator

   @author Piotr Szuberski <p.szuberski@student.uw.edu.pl>,
   @copyright Uniwersytet Warszawski
   @date 2017-06-07,
*/

#ifndef WIELOMIANY_CALC_FUNCTIONS_H
#define WIELOMIANY_CALC_FUNCTIONS_H

#include "calc_poly.h"

/**
 * Wykonuje dodawanie dwóch wielomianów z wierzchu stosu
 * @param[in] line : numer aktualnego wiersza
 * @param[in] ps : stos wielomianów
 */
void Add(int line, PolyStack **ps);

/**
 * Klonuje wielomian z wierzchu stosu
 * @param[in] line : numer aktualnego wiersza
 * @param[in] ps : stos wielomianów
 */
void Clone(int line, PolyStack **ps);

/**
 * Wykonuje mnożenie dwóch wielomianów z wierzchu stosu
 * @param[in] line : numer aktualnego wiersza
 * @param[in] ps : stos wielomianów
 */
void Mul(int line, PolyStack **ps);

/**
 * Zastępuje wielomian z wierzchu stosu jego odwrotnością
 * @param[in] line : numer aktualnego wiersza
 * @param[in] ps : stos wielomianów
 */
void Neg(int line, PolyStack **ps);

/**
 * Wykonuje odejmowanie dwóch wielomianów z wierzchu stosu
 * @param[in] line : numer aktualnego wiersza
 * @param[in] ps : stos wielomianów
 */
void Sub(int line, PolyStack **ps);

/**
 * Informuje czy dwa wielomiany na wierzchołku stosu są równe
 * @param[in] line : numer aktualnego wiersza
 * @param[in] ps : stos wielomianów
 */
void IsEq(int line, PolyStack **ps);

/**
 * Informuje czy wielomian na wierzchołku stosu jest współczynnikiem
 * @param[in] line : numer aktualnego wiersza
 * @param[in] ps : stos wielomianów
 */
void IsCoeff(int line, const PolyStack *ps);

/**
 * Informuje czy wielomian na wierzchołku stosu jest zerowy
 * @param[in] line : numer aktualnego wiersza
 * @param[in] ps : stos wielomianów
 */
void IsZero(int line, const PolyStack *ps);

/**
 * Podaje stopień wielomianu
 * @param[in] line : numer aktualnego wiersza
 * @param[in] ps : stos wielomianów
 */
void Deg(int line, const PolyStack *ps);

/**
 * Wypisuje wielomian z wierzchu stosu
 * @param[in] line : numer aktualnego wiersza
 * @param[in] ps : stos wielomianów
 */
void Print(int line, const PolyStack *ps);

/**
 * Usuwa wielomian z wierzchu stosu
 * @param[in] line : numer aktualnego wiersza
 * @param[in] ps : stos wielomianów
 */
void Pop(int line, PolyStack **ps);

/**
 * Oblicza wartość wielomianu w punkcie
 * @param[in] input : napis z wartością do wyliczenia
 * @param[in] line : numer aktualnego wiersza
 * @param[in] ps : stos wielomianów
 */
void At(const char *input, int line, PolyStack **ps);

/**
 * Podaje stopień wielomianu na określonej głębokości
 * @param[in] input : napis z wartością głębokości wielomianu
 * @param[in] line : numer aktualnego wiersza
 * @param[in] ps : stos wielomianów
 */
void DegBy(const char *input, int line, const PolyStack *ps);

/**
 * Ściąga ze stosu tyle wielomianow ile jest w argumencie + 1 i podstawia do
 * pierwszego z nich kolejno wielomiany
 * @param[in] input : napis z wartością do wyliczenia
 * @param[in] line : numer aktualnego wiersza
 * @param[in] ps : stos wielomianów
 * @return utworzony wielomian
 */
void Compose(const char *input, int line, PolyStack **ps);


#endif //WIELOMIANY_CALC_FUNCTIONS_H
