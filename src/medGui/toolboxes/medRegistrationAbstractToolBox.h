/* medRegistrationAbstractToolBox.h ---
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

#ifndef medRegistrationSelectorToolBoxCUSTOM_H
#define medRegistrationSelectorToolBoxCUSTOM_H

#include "medToolBox.h"
#include "medGuiExport.h"

class medRegistrationSelectorToolBox;
class medRegistrationAbstractToolBoxPrivate;

class MEDGUI_EXPORT medRegistrationAbstractToolBox : public medToolBox
{
    Q_OBJECT

public:
             medRegistrationAbstractToolBox(QWidget *parentToolBox = 0);
    virtual ~medRegistrationAbstractToolBox();

    void setRegistrationToolBox(medRegistrationSelectorToolBox *toolbox);

protected:
    medRegistrationSelectorToolBox *parentToolBox();

private:
    medRegistrationAbstractToolBoxPrivate *d;
};

#endif
