#!/bin/bash

# run from github qgama repository

help() {
    echo $0 "  [inp_repository]  out_repository  [-q | -u | -s | -h]" ;
	exit 1;
}

REP1="";    # repositories
REP2=".";
OPTS="";    # options
for opt in "$@"
do
    if [ "$opt" = "-h" ];  then
    help;
    fi

    case $opt in
        -q | -u | -s)  OPTS="${OPTS} $opt";;
        *)             REP1="$REP2";
                       REP2="$opt";;
    esac
done

compare() {
    echo "  " "$1" " "

    for i in `find -name  "$1" | grep -v ./build | grep -v ./gama/ | \
                   grep -v ./dox/`;
    do
        diff ${OPTS} $REP1/$i $REP2/$i;
    done
}

echo
echo "Finding differences for"
compare "*.cpp"
compare "*.h"
compare "*.txt"
compare "*.md"
compare '*.gkf'
compare "*.dox"
compare "*.html"
compare "*.sql"
compare "*.sh"
compare "README*"
compare "Doxy*"
compare "*.qrc"
compare "ChangeLog*"
echo
