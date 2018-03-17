/** @file
   Header dla kalkulatora

   @author Piotr Szuberski <p.szuberski@student.uw.edu.pl>,
   @copyright Uniwersytet Warszawski
   @date 2017-05-25,
*/

#ifndef WIELOMIANY_CALC_POLY_H
#define WIELOMIANY_CALC_POLY_H

#include "poly.h"

#define START 1 /**< początkowa wartość dynamicznych tablic */
#define END '\0' /**< znak '\0' kończący napis */
#define LMAX_LENGTH 19 /**< długość long_max */
#define LMIN_LENGTH 20 /**< długość long_min */
#define IMAX_LENGTH 10 /**< długość int_max */
#define NEW_LINE '\n' /**< znak nowej linii */
#define SPACE ' ' /**< znak spacji */
#define ADD "ADD" /**< napis ADD */
#define ZERO "ZERO" /**< napis ZERO */
#define CLONE "CLONE" /**< napis CLONE */
#define MUL "MUL" /**< napis MUL */
#define NEG "NEG" /**< napis NEG */
#define SUB "SUB" /**< napis SUB */
#define IS_EQ "IS_EQ" /**< napis IS_EQ */
#define IS_COEFF "IS_COEFF" /**< napis IS_COEFF */
#define IS_ZERO "IS_ZERO" /**< napis IS_ZERO */
#define DEG "DEG" /**< napis DEG */
#define PRINT "PRINT" /**< napis PRINT */
#define POP "POP" /**< napis POP */
#define AT "AT" /**< napis AT */
#define DEG_BY "DEG_BY" /**< napis DEG_BY */
#define COMPOSE "COMPOSE" /**< napis COMPOSE */

/**
 * Struktura stosowa przechowująca wielomiany
 */
typedef struct PolyStack {
    struct Poly p; /**< p : wartość przechowywana na stosie */
    struct PolyStack *next; /**< next : wskaźnik do następnego wielomianu */
} PolyStack;

/**
 * Struktura stosowa przechowująca znaki
 */
typedef struct DigitStack {
    char c; /**< c : znak */
    struct DigitStack *next; /**< next : wskaźnik do następnego znaku*/
} DigitStack;

/**
 * Struktura stosowa przechowująca monomiany
 */
typedef struct MonoStack {
    Mono m; /**< m : monomian */
    struct MonoStack *next; /**< next : wskaźnik do następnego monomianu */
} MonoStack;

/**
 * Parsuje liczbę long int z napisu
 * @param[in] input : napis z liczbą
 * @param[in] coeff : zwracana liczba
 * @param[in] minus : informacja, czy liczba jest ujemna
 * @return 'czy liczba jest poprawna?'
 */
bool ParseCoeff(char *input, poly_coeff_t *coeff, bool *minus);

/**
 * Parsuje liczbę int >= 0 z napisu
 * @param[in] input : napis z liczbą
 * @param[in] exp : zwracana liczba
 * @return 'czy liczba jest poprawna?'
 */
bool ParseExp(char *input, poly_exp_t *exp);

/**
 * Funkcja pomocnicza dla ParsePoly, przypadek nawias otwierający
 * @param[in] c : zmienna przebiegająca input
 * @param[in] line : numer wiersza
 * @param[in] column : numer kolumny
 * @param[in] go_on : wskaźnik pomyślności parsowania
 * @param[in] ds : stos znakowy
 */
void OpenBracketCase(char *c, int line, int *column, bool *go_on,
                     DigitStack **ds);

/**
 * Funkcja pomocnicza dla ParsePoly, przypadek plus
 * @param[in] c : zmienna przebiegająca input
 * @param[in] line : numer wiersza
 * @param[in] column : numer kolumny
 * @param[in] go_on : wskaźnik pomyślności parsowania
 * @param[in] ds : stos znakowy
 * @param[in] ms : stos monomianów
 */
void PlusCase(char *c, int line, int *column, bool *go_on,
              DigitStack **ds, const MonoStack *ms);

/**
 * Funkcja pomocnicza dla ParsePoly, przypadek liczba
 * @param[in] c : zmienna przebiegająca input
 * @param[in] line : numer wiersza
 * @param[in] column : numer kolumny
 * @param[in] go_on : wskaźnik pomyślności parsowania
 * @param[in] ds : stos znakowy
 * @param[in] ps : stos wielomianów
 */
void NumberCase(char *c, int line, int *column, bool *go_on,
                const DigitStack *ds, PolyStack **ps);

/**
 * Funkcja pomocnicza dla ParsePoly, przypadek przecinek
 * @param[in] c : zmienna przebiegająca input
 * @param[in] line : numer wiersza
 * @param[in] column : numer kolumny
 * @param[in] go_on : wskaźnik pomyślności parsowania
 * @param[in] ds : stos znakowy
 * @param[in] ps : stos wielomianów
 * @param[in] ms : stos monomianów
 */
void ComaCase(char *c, int line, int *column, bool *go_on,
              DigitStack **ds, PolyStack **ps, MonoStack **ms);

/**
 * Funkcja pomocnicza dla ParsePoly, przypadek nawias domykający
 * @param[in] c : zmienna przebiegająca input
 * @param[in] line : numer wiersza
 * @param[in] column : numer kolumny
 * @param[in] go_on : wskaźnik pomyślności parsowania
 * @param[in] ds : stos znakowy
 * @param[in] ps : stos wielomianów
 * @param[in] ms : stos monomianów
 */
void ClosingBracketCase(char *c, int line, int *column, bool *go_on,
                        DigitStack **ds, PolyStack **ps, MonoStack **ms);

/**
 * Parsuje napis na wielomian
 * @param[in] p : zwracany wielomian (jeśli zakończona sukcesem)
 * @param[in] line : numer aktualnej komendy
 * @param[in] c : pierwszy znak komendy
 * @return 'czy powiodło się parsowanie wielomianu?'
 */
bool ParsePoly(Poly *p, int line, char c);

/**
 * Dwukrotnie powiększa pojemność napisu
 * @param[in] input : powiększany napis
 * @param[in] length : długość napisu
 */
void Expand(char **input, size_t *length);

/**
 * Wczytuje cały wiersz komendy
 * @param[in] in : pierwszy znak komendy
 * @return napis (cały wiersz)
 */
char *ReadLine(char in);

/**
 * Wyciąga napis numeryczny z wielomianu w postaci napisu
 * @param[in] index : aktualny numer kolumny w napisie
 * @param[in] new_length : rozmiar wyciągniętego napisu
 * @param[in] c : zmienna przebiegająca standardowe wejście
 * @return napis z liczbą
 */
char *ExtractNumber(int *index, size_t *new_length, char *c);

/**
 * Funkcja wykonawcza kalkulatora.
 * Pobiera napisy z zewnątrz, przetwarza je i wykonuje operacje na stosie
 * @return 0
 */
int main(void);

/**
 * Inicjuje stos wielomianów
 * @param[in] s : inicjowany stos wielomianów
 */
static inline void PolyStackInit(PolyStack **s)
{
    *s = NULL;
}

/**
 * Umieszcza wartość na stosie.
 * @param[in] s : stos wielomianów
 * @param[in] p : wartość umieszczana
 */
static inline void PolyStackPush(PolyStack **s, Poly p)
{
    PolyStack *pusher = (PolyStack*) malloc(sizeof(PolyStack));
    assert(pusher != NULL);
    pusher->p = p;
    pusher->next = *s;
    *s = pusher;
}

/**
 * Informuje czy stos jest pusty.
 * @param[in] s : stos wielomianów
 * @return : 'czy jest pusty?'
 */
static inline bool PolyStackIsEmpty(const PolyStack *s)
{
    return s == NULL;
}

/**
 * Zdjemuje wielomian ze szczytu stosu.
 * @param[in] s : stos wielomianów
 * @return wielomian z wierzchu stosu
 */
static inline Poly PolyStackPop(PolyStack **s)
{
    PolyStack *deleter;
    Poly p;

    p = (*s)->p;
    deleter = *s;
    *s = (*s)->next;
    free(deleter);

    return p;
}

/**
 * Niszczy stos wielomianów
 * @param[in] s : stos wielomianów
 */
static inline void PolyStackDelete(PolyStack **s)
{
    Poly p;
    while (*s != NULL) {
        p = PolyStackPop(s);
        PolyDestroy(&p);
    }
}

/**
 * Zwraca wartość z wierzchu stosu, nie zdejmując jej.
 * @param[in] s : stos wielomianów
 * @return wartość z wierzchu stosu
 */
static inline Poly PolyStackTop(const PolyStack *s)
{
    return s->p;
}

/**
 * Inicjuje stos znaków
 * @param[in] s : inicjowany stos znaków
 */
static inline void DigitStackInit(DigitStack **s)
{
    *s = NULL;
}

/**
 * Umieszcza znak na stosie
 * @param[in] s : stos znaków
 * @param[in] c : znak umieszczany
 */
static inline void DigitStackPush(DigitStack **s, char c)
{
    DigitStack *pusher = (DigitStack*) malloc(sizeof(DigitStack));
    assert(pusher != NULL);

    pusher->c = c;
    pusher->next = *s;
    *s = pusher;
}

/**
 * Informuje czy stos znaków jest pusty
 * @param[in] s : stos znaków
 * @return 'czy jest pusty?'
 */
static inline bool DigitStackIsEmpty(const DigitStack *s)
{
    return s == NULL;
}

/**
 * Zdejmuje znak z wierzchołka stosu i zwraca ją
 * @param[in] s : stos znaków
 * @return znak z wierzchu stosu
 */
static inline char DigitStackPop(DigitStack **s)
{
    DigitStack *deleter;
    char c;

    c = (*s)->c;
    deleter = *s;
    *s = (*s)->next;
    free(deleter);

    return c;
}

/**
 * Kasuje stos znaków
 * @param[in] s : stos znaków
 */
static inline void DigitStackDelete(DigitStack **s)
{
    while (*s != NULL) {
        DigitStackPop(s);
    }
}

/**
 * Przekazuje wartość z wierzchu stosu nie zdejmując jej
 * @param[in] s stos znaków
 * @return znak z wierzchu stosu znaków
 */
static inline char DigitStackTop(const DigitStack *s)
{
    return s->c;
}

/**
 * Inicjuje stos monomianów
 * @param[in] s : inicjowany stos monomianów
 */
static inline void MonoStackInit(MonoStack **s)
{
    *s = NULL;
}

/**
 * Umieszcza monomian na stosie
 * @param[in] s : stos monomianów
 * @param[in] m : umieszczona wartość
 */
static inline void MonoStackPush(MonoStack **s, Mono m)
{
    MonoStack *pusher = (MonoStack*) malloc(sizeof(MonoStack));
    assert(pusher != NULL);

    pusher->m = m;
    pusher->next = *s;
    *s = pusher;
}

/**
 * Informuje czy stos jest pusty
 * @param[in] s : stos monomianów
 * @return 'czy jest pusty?'
 */
static inline bool MonoStackIsEmpty(const MonoStack *s)
{
    return s == NULL;
}

/**
 * Zdejmuje monomian z wierzchołka stosu i zwraca ją
 * @param[in] s : stos monomianów
 * @return monomian z wierzchołka stosu
 */
static inline Mono MonoStackPop(MonoStack **s)
{
    MonoStack *deleter;
    Mono m;

    m = (*s)->m;
    deleter = *s;
    *s = (*s)->next;
    free(deleter);

    return m;
}

/**
 * Usuwa stos monomianów
 * @param[in] s : stos monomianów
 */
static inline void MonoStackDelete(MonoStack **s)
{
    while (*s != NULL) {
        MonoStackPop(s);
    }
}

/**
 * Wypisuje na wyjście diagnostyczne informację o błędzie pustego stosu
 * i numer aktualnego wiersza
 * @param[in] line : numer aktualnego wiersza
 */
static inline void ErrorStackUnderflow(int line)
{
    fprintf(stderr, "ERROR %d STACK UNDERFLOW\n", line);
}

/**
 * Wypisuje na wyjście diagnostyczne informację o błędzie parsowania
 * wielomianu, numer aktualnego wiersza i kolumny
 * @param[in] line : numer aktualnego wiersza
 * @param[in] column : numer aktualnej kolumny
 * @return 'zatrzymaj parsowanie wielomianu'
 */
static inline bool ErrorParsingPoly(int line, int column)
{
    fprintf(stderr, "ERROR %d %d\n", line, column);
    return false;
}

/**
 * Wypisuje na wyjście diagnostyczne informację o błędnej komendzie
 * i numer aktualnego wiersza
 * @param[in] line : numer aktualnego wiersza
 */
static inline void ErrorWrongCommand(int line)
{
    fprintf(stderr, "ERROR %d WRONG COMMAND\n", line);
}

/**
 * Wypisuje na wyjście diagnostyczne informację o błędnej wartości w AT
 * i numer aktualnego wiersza
 * @param[in] line : numer aktualnego wiersza
 */
static inline void ErrorCorruptedAt(int line)
{
    fprintf(stderr, "ERROR %d WRONG VALUE\n", line);
}

/**
 * Wypisuje na wyjście diagnostyczne informację o błędnej wartości w DEG_BY
 * i numer aktualnego wiersza
 * @param[in] line : numer aktualnego wiersza
 */
static inline void ErrorCorruptedDegBy(int line)
{
    fprintf(stderr, "ERROR %d WRONG VARIABLE\n", line);
}

/**
 * Wypisuje na wyjście diagnostyczne informację o błędnej wartości w COMPOSE
 * i numer aktualnego wiersza
 * @param[in] line : numer aktualnego wiersza
 */
static inline void ErrorCorruptedCompose(int line)
{
    fprintf(stderr, "ERROR %d WRONG COUNT\n", line);
}

#endif //WIELOMIANY_CALC_POLY_H
