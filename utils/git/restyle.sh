#!/bin/sh

against=$1

#   Impose style of C++ files.

restyle=$(git config hooks.uncrustify)

if [ "$restyle" == false ]; then
    echo "Before committing to this repository, you must install"
    echo "uncrustify on this computer so that the code is beautified"
    echo "before committing. Then, reconfigure with cmake before"
    echo "committing again"
fi

files=$(git diff --cached --name-status $against | grep -v ^D | cut -c3-)

uncrustify=$(git config uncrustify.command)
for file in $files; do
    case $file in (*.cpp | *.h)
        $uncrustify -c $PWD/utils/uncrustify.cfg --replace --mtime -lCPP $file;;
    esac
    git add $file
done
