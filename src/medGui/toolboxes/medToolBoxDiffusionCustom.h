/* medToolBoxDiffusionCustom.h ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Nov 10 14:16:25 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Mar  2 15:03:07 2011 (+0100)
 *           By: Jaime Garcia_Guevara
 *     Update #: 3
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */


#ifndef MEDTOOLBOXDIFFUSIONCUSTOM_H
#define MEDTOOLBOXDIFFUSIONCUSTOM_H

#include "medToolBox.h"
#include "medGuiExport.h"

class medToolBoxDiffusion;
class medToolBoxDiffusionCustomPrivate;
class dtkAbstractProcess;
class dtkAbstractData;
class dtkPlugin;

class MEDGUI_EXPORT medToolBoxDiffusionCustom: public medToolBox {
public:
             medToolBoxDiffusionCustom(QWidget *parent = 0);
    virtual ~medToolBoxDiffusionCustom();

    virtual void setDiffusionToolBox(medToolBoxDiffusion *toolbox);

    virtual QString description() const = 0;

    virtual dtkAbstractData *output() const;

protected:

    medToolBoxDiffusion *parent();

    medToolBoxDiffusionCustomPrivate *d;
};

#endif // MEDTOOLBOXDIFFUSIONCUSTOM_H
