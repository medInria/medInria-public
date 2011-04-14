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

class MEDGUI_EXPORT medToolBoxDiffusionCustom : public medToolBox
{
    Q_OBJECT

public:
             medToolBoxDiffusionCustom(QWidget *parent = 0);
    virtual ~medToolBoxDiffusionCustom(void);

    virtual void setDiffusionToolBox(medToolBoxDiffusion *toolbox);

    virtual QString description (void) const = 0;

    virtual dtkAbstractData *output (void) const;
    
protected:
    medToolBoxDiffusion *parent(void);

protected:
    medToolBoxDiffusionCustomPrivate *d;
};

#endif // MEDTOOLBOXDIFFUSIONCUSTOM_H
