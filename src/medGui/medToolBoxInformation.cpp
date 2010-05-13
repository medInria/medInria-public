/* medToolBoxInformation.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Thu May 13 15:20:03 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Thu May 13 16:14:57 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 14
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medToolBoxInformation.h"

#include <QtGui>

class medToolBoxInformationPrivate
{
public:

};

medToolBoxInformation::medToolBoxInformation(QWidget *parent) : medToolBox(parent), d(new medToolBoxInformationPrivate)
{
    QWidget *widget = new QWidget(this);
    widget->setMinimumHeight(200);

    this->setTitle("Information");
    this->setWidget(widget);
}

medToolBoxInformation::~medToolBoxInformation(void)
{
    delete d;

    d = NULL;
}
