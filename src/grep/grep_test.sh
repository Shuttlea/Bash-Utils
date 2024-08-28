#!/bin/bash

OPTIONS=(
    i
    v
    c
    l
    n
    h
    s
    o
)

COUNTER=0
SUCCESS=0
FAIL=0

make_test() {
    ((COUNTER++))
    DIF_RES=`diff <(grep $1 $2 $3 $4 $5 $6 $7 $8 $9) <(./s21_grep $1 $2 $3 $4 $5 $6 $7 $8 $9) -s`

    if [[ $? == 0 ]]
        then echo "test $COUNTER: s21_grep $1 $2 $3 $4 $5 $6 $7 $8 $9: OK"
        ((SUCCESS++))
        else echo "test $COUNTER: s21_grep $1 $2 $3 $4 $5 $6 $7 $8 $9: FAIL"
        ((FAIL++))
    fi
}

test_alphabet() {
    for FILE in test_files/*.txt; do
        for OPTION in ${OPTIONS[@]}; do
            for RANDOM_PATTERN in {a..z}; do
                make_test -$OPTION $RANDOM_PATTERN $FILE
            done; echo
        done; echo
    done; echo
}

test_double_all() {
    for PATTERN_1 in pleasure small between; do
        make_test $PATTERN_1 test_files/cicero.txt
        make_test $PATTERN_1 test_files/kafka.txt test_files/faraway.txt

        for PATTERN_2 in will with that; do
            make_test -e $PATTERN_1 -e $PATTERN_2 test_files/languages.txt
            make_test -e $PATTERN_1 -e $PATTERN_2 test_files/pangram.txt test_files/werther.txt 1.txt

            for OPTION_1 in ${OPTIONS[@]}; do
                make_test -$OPTION_1 $PATTERN_1 test_files/cicero.txt
                make_test -$OPTION_1 $PATTERN_1 test_files/faraway.txt test_files/kafka.txt 1.txt
                make_test -$OPTION_1 -e $PATTERN_1 -e $PATTERN_2 test_files/languages.txt
                make_test -$OPTION_1 -e $PATTERN_1 -e $PATTERN_2 test_files/pangram.txt test_files/werther.txt 1.txt

                for OPTION_2 in ${OPTIONS[@]}; do
                    make_test -${OPTION_1}${OPTION_2} $PATTERN_1 test_files/cicero.txt
                    make_test -${OPTION_1}${OPTION_2} $PATTERN_1 test_files/cicero.txt test_files/kafka.txt
                    make_test -${OPTION_1}${OPTION_2} -e $PATTERN_1 -e $PATTERN_2 test_files/languages.txt
                    make_test -${OPTION_1}${OPTION_2} -e $PATTERN_1 -e $PATTERN_2 test_files/languages.txt test_files/pangram.txt 1.txt

                done; echo
            done; echo
        done; echo
    done; echo
}

test_files_template() {
    for OPTION_1 in ${OPTIONS[@]}; do
        make_test -$OPTION_1 -f test_files/template_1.txt -e denouncing test_files/cicero.txt
        make_test -$OPTION_1 -f test_files/template_1.txt -f test_files/template_2.txt test_files/cicero.txt test_files/kafka.txt

        for OPTION_2 in ${OPTIONS[@]}; do
            make_test -${OPTION_1}${OPTION_2} -f test_files/template_1.txt -e will test_files/cicero.txt
            make_test -${OPTION_1}${OPTION_2} -f test_files/template_1.txt -f test_files/template_2.txt test_files/pangram.txt test_files/werther.txt

        done; echo
    done; echo
}

valgrind_test() {
    valgrind --tool=memcheck --leak-check=yes --track-origins=yes -s ./s21_grep -f test_files/template_1.txt test_files/werther.txt
    valgrind --tool=memcheck --leak-check=yes --track-origins=yes -s ./s21_grep -e a the test_files/werther.txt
    for OPTION in ${OPTIONS[@]}; do
        valgrind --tool=memcheck --leak-check=yes --track-origins=yes -s ./s21_grep ${OPTION} -f test_files/template_2.txt test_files/werther.txt
        valgrind --tool=memcheck --leak-check=yes --track-origins=yes -s ./s21_grep ${OPTION} -e a test_files/werther.txt
    done; echo
}

 test_alphabet
 test_double_all
 test_files_template
# valgrind_test

echo "ALL: $COUNTER"
echo "SUCCESS: $SUCCESS"
echo "FAIL: $FAIL"
