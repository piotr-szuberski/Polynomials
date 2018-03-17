#!/bin/bash
# Usage ./test_calc.sh ./release/poly_calc ./tests

PASSED=0
ALL=0
TOTAL_TIME=0
DEBUG_CHECK=0

OK=$'\e[32;01m [OK]\e[0m'
WRONG=$'\e[31;01m [WRONG]\e[0m'

ILLEGAL_ARGUMENTS="Usage: ./test.sh [prog] [tests directory]"
EXECUTABLE_DOESNT_EXIST="Executable file not found"
TESTS_DOESNT_EXIST="Test Directory not fount"

if [ $# == 2 ]; then
    EXECUTABLE=$1
    DIRECTORY=$2
else
    echo $ILLEGAL_ARGUMENTS
    exit 0
fi


if [ -f $EXECUTABLE ]; then
    EXECUTABLE="./$EXECUTABLE"
else
    echo $EXECUTABLE_DOESNT_EXIST
    exit 0
fi

if [ ! -d $DIRECTORY ]; then
    echo $TESTS_DOESNT_EXIST
    exit 0
fi

for file in $DIRECTORY/test*.in; do
    FILENAME=${file/$DIRECTORY/""}

    START=$(date +%s.%N)
    $EXECUTABLE < $file > out 2> err
    END=$(date +%s.%N)

    time=$(echo "$END - $START" | bc)
    TOTAL_TIME=$(echo "$TOTAL_TIME + $time" | bc)

    if diff out "${file%.in}.out" >/dev/null 2>&1; then
        echo "Output Test ${FILENAME} $OK (took $time seconds)"

       if diff err "${file%.in}.err" >/dev/null 2>&1; then
           echo "Error Test ${FILENAME} $OK"
           PASSED=$((PASSED+1))
       else
           echo "Error Test ${FILENAME} $WRONG"
       fi
    else
        echo "Test ${FILENAME} $WRONG"
    fi
    ALL=$(($ALL+1))
done
rm -f out
rm -f err

echo "Your application has passed $PASSED out of $ALL tests"
echo "It has taken $TOTAL_TIME seconds"
