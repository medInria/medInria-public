#!/bin/sh

against=$1

$PWD/utils/git/check-non-ascii-filenames.sh $against
$PWD/utils/git/restyle.sh $against
