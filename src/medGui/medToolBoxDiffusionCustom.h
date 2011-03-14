/* medToolBoxDiffusionCustom.h ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Nov 10 14:16:25 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 15:46:03 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 1
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

    virtual dtkAbstractProcess *create (void) = 0;

    virtual dtkAbstractData *output (void) const;

    void setProcess (dtkAbstractProcess *process);
    dtkAbstractProcess *process (void) const;
    
protected:
    medToolBoxDiffusion *parent(void);

protected:
    medToolBoxDiffusionCustomPrivate *d;
};

#endif // MEDTOOLBOXDIFFUSIONCUSTOM_H
