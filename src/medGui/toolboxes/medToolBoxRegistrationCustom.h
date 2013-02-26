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

#include "medToolBox.h"
#include "medGuiExport.h"

class medToolBoxRegistration;
class medToolBoxRegistrationCustomPrivate;

class MEDGUI_EXPORT medToolBoxRegistrationCustom : public medToolBox
{
    Q_OBJECT

public:
             medToolBoxRegistrationCustom(QWidget *parentToolBox = 0);
    virtual ~medToolBoxRegistrationCustom(void);

    virtual void setRegistrationToolBox(medToolBoxRegistration *toolbox);

protected:
    medToolBoxRegistration *parentToolBox(void);

private:
    medToolBoxRegistrationCustomPrivate *d;
};

#endif
