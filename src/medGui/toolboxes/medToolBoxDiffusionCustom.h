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

class MEDGUI_EXPORT medToolBoxDiffusionCustom : public medToolBox
{
public:
             medToolBoxDiffusionCustom(QWidget *parentToolBox = 0);
    virtual ~medToolBoxDiffusionCustom(void);

    virtual void setDiffusionToolBox(medToolBoxDiffusion *toolbox);

    virtual QString description (void) const = 0;

    virtual dtkAbstractData *output (void) const;

    /**
     * @brief Returns the plugin this custom Toolbox comes from.
     *
     * Used to fill the aboutPlugin button in the parent toolbox.
     *
     */
    virtual dtkPlugin *plugin( void )const = 0;

protected:
    medToolBoxDiffusion *parentToolBox(void);

    medToolBoxDiffusionCustomPrivate *d;
};

#endif // MEDTOOLBOXDIFFUSIONCUSTOM_H
