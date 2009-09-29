/* main.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Thu Sep 17 08:29:18 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Fri Sep 25 14:10:50 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 65
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include <QtGui/QApplication>

#include "medMainWindow.h"

#include <dtkCore/dtkPluginManager.h>

#include <dtkScript/dtkScriptInterpreter.h>
#include <dtkScript/dtkScriptInterpreterPool.h>
#include <dtkScript/dtkScriptManager.h>

#include <dtkGui/dtkInterpreter.h>

#include <medGui/medStyle.h>

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);
    application.setApplicationName("medular");
    application.setApplicationVersion("0.0.1");
    application.setOrganizationName("inria");
    application.setOrganizationDomain("fr");
    application.setStyle(new medStyle);

    // Initialize managers

    dtkPluginManager::instance()->initialize();
    dtkScriptManager::instance()->initialize();

    // Setting up main window

    medMainWindow mainwindow;
    mainwindow.show();

    // Setting up interpreter

    dtkScriptInterpreter *interpreter;
    interpreter = dtkScriptInterpreterPool::instance()->python();
    
    QObject::connect(interpreter, SIGNAL(stopped()), &application, SLOT(quit()));
    QObject::connect(&application, SIGNAL(aboutToQuit()), interpreter, SLOT(stop()));

#if defined(Q_OS_WIN)
    dtkInterpreter *ginterpreter;
    ginterpreter->registerInterpreter(interpreter);
    ginterpreter->readSettings();
    ginterpreter->show();

    QObject::connect(&application, SIGNAL(aboutToQuit()), ginterpreter, SLOT(writeSettings()));
#else
    interpreter->start();
#endif

    // Run event loop

    int status = application.exec();
    
    // Uninitialize managers

    dtkPluginManager::instance()->uninitialize();
    dtkScriptManager::instance()->uninitialize();
    
    return status;
}
