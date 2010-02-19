/* medToolBoxRegistration.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Feb 19 09:06:02 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Fri Feb 19 10:43:14 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 34
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medRegistrationWidget.h"
#include "medToolBoxRegistration.h"

#include <QtGui>

class medToolBoxRegistrationPrivate
{
public:

};

medToolBoxRegistration::medToolBoxRegistration(QWidget *parent) : medToolBox(parent), d(new medToolBoxRegistrationPrivate)
{
    medRegistrationWidget *registrationWidget = new medRegistrationWidget(this);

    this->setTitle("Registration");
    this->setWidget(registrationWidget);
}

medToolBoxRegistration::~medToolBoxRegistration(void)
{
    delete d;

    d = NULL;
}
