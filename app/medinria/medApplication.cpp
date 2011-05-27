/* medApplication.cpp ---
 * 
 * Author: John Stark
 * Copyright (C) 2011 - John Stark, Inria.
 * Created: May 2011
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

