/* medToolBoxDiffusion.h ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Feb 19 09:02:09 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 11:03:19 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 14
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#ifndef MEDTOOLBOXDIFFUSION_H
#define MEDTOOLBOXDIFFUSION_H

#include "medToolBox.h"
#include "medGuiExport.h"

class dtkAbstractView;
class dtkAbstractData;
class dtkAbstractProcess;
class medToolBoxDiffusionPrivate;
class medDataIndex;

class MEDGUI_EXPORT medToolBoxDiffusion : public medToolBox
{
    Q_OBJECT
public:
     medToolBoxDiffusion(QWidget *parent = 0);
    ~medToolBoxDiffusion(void);

    dtkAbstractData *output(void) const;
    
public slots:
    // void run (void);
    void clear(void);

    void onToolBoxChosen(const QString&);
    
private:
    medToolBoxDiffusionPrivate *d;

};

#endif // MEDTOOLBOXDIFFUSION_H
