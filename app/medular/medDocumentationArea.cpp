/* medDocumentationArea.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 22 09:04:25 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Fri Mar 19 18:45:06 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 33
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medDocumentationArea.h"

// /////////////////////////////////////////////////////////////////
// medDocumentationArea
// /////////////////////////////////////////////////////////////////

class medDocumentationAreaPrivate
{
public:
    QStatusBar *status;
};

medDocumentationArea::medDocumentationArea(QWidget *parent) : QWidget(parent), d(new medDocumentationAreaPrivate)
{

}

medDocumentationArea::~medDocumentationArea(void)
{
    delete d;

    d = NULL;
}

void medDocumentationArea::setup(QStatusBar *status)
{
    d->status = status;
}

void medDocumentationArea::setdw(QStatusBar *status)
{
    d->status = status;
}
