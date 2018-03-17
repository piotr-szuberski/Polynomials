#!/bin/bash
#Script usage: ./chain_poly ./program ./directory

PROGRAM="./release/calc_poly"

if [ $# != 2 ]; then
#	Nieprawidłowa liczba parametrów
	exit 1
fi

if [ $1 != "$PROGRAM" -o ! -f "$PROGRAM" ]; then
#	Nieprawidłowa nazwa parametru
	exit 1
	if [ ! -f "$PROGRAM" ]; then
#		Parametr nie wskazuje na plik
		exit 1
		if [ ! -x "$PROGRAM" ]; then
#			Plik nie jest plikiem wykonywalnym
			exit 1
		fi
	fi
fi

if [ ! -d $2 ]; then
#	Nie znaleziono katalogu
	exit 1
fi

#Poszukuję pliku z pierwszym wierszem "START"
for FILE in $2/* ; do
	LINE1=`head -1 "$FILE"`
	if [ "$LINE1" = "START" ]; then
		STARTER="$FILE"
	fi
done

i=1

#tworzę tymczasowy katalog tmpxxx (nie tmp, bo ten często występuje
#i mógłby być błąd
mkdir ./tmpxxx

#Ekstrahuję ostatni wiersz z pliku do zmiennej STRING
STRING=`tail -1 "$STARTER"`

#Kopiuję plik bez ostatniego wiersza do ./tmpxxx/temp.in
head -n -1 "$STARTER" > ./tmpxxx/temp.in

#Kopiuję plik bez pierwszego i ostatniego wiersza do ./tmpxxx/input1.in
tail -n +2 ./tmpxxx/temp.in > ./tmpxxx/input$[i].in

i=$(( $i + 1 ))

#Sprawdzam czy w ostatnim wierszu pliku nie było opcji STOP
#Jeśli nie było, to wyciągam pierwszy wyraz (z poprawności danych zakładam, 
#że jest to FILE), następnie zapisuję tablicę do zmiennej NEXTFILE
if [ "$STRING" == "STOP" ]; then
	ENDER="STOP"
else
	ENDER="CONTINUE"
	read -a ARRAY <<< $STRING
	unset ARRAY[0]
	NEXTFILE=`echo ${ARRAY[@]}`
fi

#Powtarzam proces (już bez wyciągania pierwszego wiersza), zapisuję skopiowane
#pliki do kolejnych plików input*.in
while [ "$ENDER" != "STOP" ]; do
	STRING=`tail -1 "$2/$NEXTFILE"`
	head -n -1 "$2/$NEXTFILE" > ./tmpxxx/input$[i].in
	i=$(( $i + 1 ))
	if [ "$STRING" = "STOP" ]; then
		ENDER="STOP"
	else
		read -a ARRAY <<< $STRING
		unset ARRAY[0]
		NEXTFILE=`echo ${ARRAY[@]}`
	fi
done

#Tworzę puste wejście dla pierwszej iteracji
touch ./tmpxxx/output1.out

#Wykonuję program i zapisuję jego wyjście do plików output[j].out,
#następnie dodaję je na początek inputu następnej iteracji
for (( j=1; $j < $i; j++ )); do
	cat ./tmpxxx/output$[j].out ./tmpxxx/input$[j].in | $PROGRAM > ./tmpxxx/output$[j+1].out
done

#wypisuję wyjście z ostatniej iteracji
cat ./tmpxxx/output$[i].out

#Kasuję tymczasowy katalog tmpxxx
rm -r ./tmpxxx

#Program kończy swoje działanie z wynikiem 0
exit 0
