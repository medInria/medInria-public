#!/usr/bin/env python
'''
This scripts install all qt dependencies we have for a running
medinria
'''
import sys,os,shutil

'''
Deprecated, not used any more,
just put plugins in their subdirectories directly
in the executable's directory.
'''
def create_qt_conf (install_dir):

    f = open(os.path.join(install_dir,"qt.conf"),'w')
    f.write("[Paths]\nPlugins=plugins")
    return

def copy_dlls (files_list, qt_install_dir, install_dir):
    '''
    copy dlls in the file list, from the qt_installation_dir
    to the installation directory.
    '''
    for dll in files_list:
        shutil.copy(os.path.join(qt_install_dir,dll),
            os.path.join(install_dir,dll))


def main (argv):
    #use the first arg as an installation dir
    if len (argv) != 3 :
        print "usage:",argv[0]," qt_install_dir install_dir"
        sys.exit(1)
    install_dir = argv[2]
    qt_install_dir = argv[1]
    qt_install_dir_bin =  os.path.join(qt_install_dir,"bin")
    install_dir_bin = os.path.join(install_dir,"bin")
    try:
        os.makedirs(install_dir_bin)
    except OSError,e:
        print e
        print '...no problem, continuing'

    #not needed if plugins are in bin directory and not bin/plugins.
    #create_qt_conf(install_dir_bin)
    plugin_dlls = "imageformats/qgif4.dll \
    imageformats/qico4.dll \
    imageformats/qjpeg4.dll \
    imageformats/qmng4.dll \
    imageformats/qsvg4.dll \
    imageformats/qtiff4.dll \
    sqldrivers/qsqlite4.dll"


    dlls="QtCLucene4.dll \
    QtCore4.dll \
	QtGui4.dll \
	QtHelp4.dll \
	QtNetwork4.dll \
	QtOpenGL4.dll \
	QtSql4.dll \
	QtWebkit4.dll \
	QtXml4.dll \
	QtXmlPatterns4.dll \
    QtScript4.dll \
    QtDeclarative4.dll \
    phonon4.dll"

    try:
        os.makedirs(os.path.join(install_dir_bin,"sqldrivers"))
    except OSError,e:
        print e
        print '...no problem, continuing'

    try:
        os.makedirs(os.path.join(install_dir_bin,"imageformats"))
    except OSError,e:
        print e
        print '...no problem, continuing'


    copy_dlls(dlls.split(),qt_install_dir_bin, install_dir_bin)

    copy_dlls(plugin_dlls.split(),os.path.join(qt_install_dir,"plugins"),
            install_dir_bin)

if __name__ == "__main__":
    main(sys.argv)
