/* medBrowserArea.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Sep 25 12:23:43 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 11:15:55 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 461
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medApplication.h"

#include <dtkCore/dtkGlobal.h>

#include <QtGui>

class medApplicationPrivate
{
public:
};

// /////////////////////////////////////////////////////////////////
// medApplication
// /////////////////////////////////////////////////////////////////

medApplication::medApplication(int & argc, char**argv) :
        QApplication(argc,argv),
        d(new medApplicationPrivate)
{
    this->setApplicationName("medinria");
    this->setApplicationVersion("0.0.1");
    this->setOrganizationName("inria");
    this->setOrganizationDomain("fr");
    this->setWindowIcon(QIcon(":/medinria.ico"));

    //The ownership of the style object is transferred.
    this->setStyle( new QPlastiqueStyle() );
    this->setStyleSheet(dtkReadFile(":/medinria.qss"));

}

medApplication::~medApplication(void)
{
    delete d;
    d = NULL;
}

