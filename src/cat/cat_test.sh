#!/bin/bash

OPTIONS=(
    -n
    -b
    -s
    -e
    -E
    -t
    -T
    -v
)

LONG_OPTIONS=(
    --number-nonblank
    --number
    --squeeze-blank
)

COUNTER=0
SUCCESS=0
FAIL=0

make_test() {
    ((COUNTER++))
    DIF_RES=`diff <(cat $1 $2 $3 $4) <(./s21_cat $1 $2 $3 $4) -s`

    if [[ $? == 0 ]]
        then echo "test $COUNTER: s21_cat $1 $2 $3 $4: OK"
        ((SUCCESS++))
        else echo "test $COUNTER: s21_cat $1 $2 $3 $4: FAIL"
        ((FAIL++))
    fi
}

# all single options for different 1-2 files 
test_group_1() {
    echo "===== short single options with 1 or 2 files ====="
    for FILE in test_files/*.txt; do
        make_test $FILE # no option 1 file
        for FILE_2 in test_files/*.txt; do
            make_test $FILE $FILE_2 # no option 2 files
            for OPTION in ${OPTIONS[@]}; do
                make_test $OPTION $FILE # 1 option 1 file
                make_test $OPTION $FILE $FILE_2 # 1 option 2 files
            done; echo
        done; echo
    done
}

# all long options for different 1 file
test_group_2() {
    echo "===== all long single options with 1 file ====="
    for FILE in test_files/*.txt; do
    make_test $FILE # no option 1 file
        for OPTION in ${LONG_OPTIONS[@]}; do
            make_test $OPTION $FILE # 1 option 1 file
        done; echo
    done
}

# error no file and invalid option
test_group_3() {
    echo "===== error option ====="
    make_test -x 1.txt
    make_test -nx 2.txt
    make_test -s -x 3.txt
    make_test --xerox --number 4.txt
    make_test --s 5.txt
    make_test ---number 5.txt
    echo

    echo "===== file not exist ====="
    make_test abcdef.txt
    make_test -n dajw.txt
    make_test --number-nonblank 2112.txt
    echo
}

# 3 options for 1 file
test_group_4() {
    echo "===== 3 option ====="
    FILE="test_files/1.txt"
    for OPTION in ${OPTIONS[@]}; do
        for OPTION_2 in ${OPTIONS[@]}; do
            for OPTION_3 in ${OPTIONS[@]}; do
                    make_test $OPTION $OPTION_2 $OPTION_3 $FILE # 3 option
            done; echo
        done; echo
    done
}

valgrind_test() {
    if [[ `uname -s` == "Linux" ]]; then
    for FILE in test_files/*.txt; do
    valgrind --tool=memcheck --leak-check=yes ./s21_cat $FILE $FILE # no option 2 file
        for OPTION in ${OPTIONS[@]}; do
            valgrind --tool=memcheck --leak-check=yes ./s21_cat $OPTION $FILE # 1 option 1 file
        done; echo
    done
    fi
}

test_group_1
test_group_2
test_group_3
test_group_4

echo "ALL: $COUNTER"
echo "SUCCESS: $SUCCESS"
echo "FAIL: $FAIL"

#valgrind_test
