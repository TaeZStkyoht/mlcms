#!/bin/bash
TOP_LEVEL="$(git rev-parse --show-toplevel)"

for file in $(git ls-files | grep -E "\.(cpp|hpp)$"); do
    if [[ -n $(diff -u <(cat $file) <(clang-format -style=file $file)) ]]; then
        clang-format-15 -style=file --verbose -i $file
    fi
done
