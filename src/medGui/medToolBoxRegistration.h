/* medToolBoxRegistration.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Feb 19 09:02:09 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Fri Feb 19 10:41:41 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 19
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDTOOLBOXREGISTRATION_H
#define MEDTOOLBOXREGISTRATION_H

#include "medToolBox.h"

class medToolBoxRegistrationPrivate;

class MEDGUI_EXPORT medToolBoxRegistration : public medToolBox
{
    Q_OBJECT

public:
     medToolBoxRegistration(QWidget *parent = 0);
    ~medToolBoxRegistration(void);

private:
    medToolBoxRegistrationPrivate *d;
};

#endif
