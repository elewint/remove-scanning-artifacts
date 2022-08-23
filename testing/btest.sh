#! /bin/sh

if [[ $1 = "v" ]]
then
    echo
    valgrind --leak-check=full --track-origins=yes ./unblackedges hyphen.pbm
    # echo "Exit value: $?"
elif [[ -z $1 ]]
then
    echo
    ./unblackedges hyphen.pbm
    # echo "Exit value: $?"
# else
#     ./sudoku sudokuDEMO
fi

