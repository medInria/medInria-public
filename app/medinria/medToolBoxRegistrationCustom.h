/* medToolBoxRegistrationCustom.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Nov 10 14:16:25 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 15:46:03 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 23
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDTOOLBOXREGISTRATIONCUSTOM_H
#define MEDTOOLBOXREGISTRATIONCUSTOM_H

#include <medGui/medToolBox.h>

class medToolBoxRegistration;
class medToolBoxRegistrationCustomPrivate;

class medToolBoxRegistrationCustom : public medToolBox
{
    Q_OBJECT

public:
             medToolBoxRegistrationCustom(QWidget *parent = 0);
    virtual ~medToolBoxRegistrationCustom(void);

    void setRegistrationToolBox(medToolBoxRegistration *toolbox);

protected:
    medToolBoxRegistration *parent(void);

private:
    medToolBoxRegistrationCustomPrivate *d;
};

#endif
