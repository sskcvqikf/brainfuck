#!/bin/bash

flag=0
function is_eq {
    if cmp -s "$1" "$2"; then
        printf 'The file "%s" is the same as "%s"\n' "$1" "$2"
    else
        printf 'The file "%s" is different from "%s"\n' "$1" "$2"
        flag=1
    fi
}

for i in *.b; do
    f=$(basename $i .b)
    input="$f.b"
    output="$f.result"
    expected="$f.expected"
    echo "Running $input"
    ../../brainfuck "$(cat $input)" > $output
    sed -i "s/\r//g" $expected
    sed -i "s/\r//g" $output
    is_eq $output $expected
done

if [[ $flag -eq 1 ]]; then
    exit 1
fi
