# !/bin/bash

#########################################################################
#
#                    converting all png files in headers
#
#                         09/2006 N. Toussaint
#########################################################################

E_BADARGS=65
if [ ! -n "$2" ]
then
  echo
  echo
  echo "#########################  IMAGE RESOURCE INCLUDER  ###########################"
  echo
  echo "Use : ConvertImages myimagesdirectory/ myheaderfilepath"
  echo
  echo "images have to be in png format"
  echo "the headerfilepath should be full path to the header"
  echo
  echo "###############################################################################"
  
  exit $E_BADARGS
fi  
echo

whereweare=${PWD}
dir=$1
filepath=$2
space=" "
cd ${dir}



for i in *.xpm
do
j=`echo $i | sed s/.xpm/.png/g`

if [ -e $i ]
then
  convert $i $j
  echo "file $i converted"
fi
done


for file in *.png
do
  if [ -e ${dir}${file} ]
  then
    echo ${dir}${file}
    files=${files}${dir}${file}${space}
  fi
done

cd ${whereweare}
/user/nitoussa/home/ToolKits/KWWidgets/build/bin/KWConvertImageToHeader ${filepath} ${files}
