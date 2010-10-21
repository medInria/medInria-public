#!/bin/bash

echo "[Paths]\rPlugins=plugin" > /cygdrive/c/Program\ Files/inria/bin/qt.conf

files="QtCLucene4.dll \
    QtCore4.dll \
	QtGui4.dll \
	QtHelp4.dll \
	QtNetwork4.dll \
	QtOpenGL4.dll \
	QtSql4.dll \
	QtWebkit4.dll \
	QtXml4.dll \
	QtXmlPatterns4.dll \
	phonon4.dll"
	
mkdir -p  /cygdrive/c/Program\ Files/inria/bin

for f in $files ; do
    cp /cygdrive/c/Qt/4.6.3/bin/$f /cygdrive/c/Program\ Files/inria/bin/$f
done

files2="imageformats/qgif4.dll \
	imageformats/qgifd4.dll \
	imageformats/qico4.dll \
	imageformats/qico4d.dll \
	imageformats/qjpeg4.dll \
	imageformats/qjpeg4d.dll \
	imageformats/qmng4.dll \
	imageformats/qmng4d.dll \
	imageformats/qsvg4.dll \
	imageformats/qsvg4d.dll \
	imageformats/qtiff4.dll \
	imageformats/qtiff4d.dll \
	sqldrivers/qsqlite4.dll"

mkdir -p  /cygdrive/c/Program\ Files/inria/bin/plugins/imageformats
mkdir -p  /cygdrive/c/Program\ Files/inria/bin/plugins/sqldrivers

for f in $files2 ; do
    cp /cygdrive/c/Qt/4.6.3/plugins/$f /cygdrive/c/Program\ Files/inria/bin/plugins/$f
done

cp /cygdrive/z/windows_deps/installed/Tcl/bin/tcl85.dll  /cygdrive/c/Program\ Files/inria/bin/

cp /cygdrive/z/windows_deps/installed/Python26.bak/python26.dll /cygdrive/c/Program\ Files/inria/bin/

