/* medToolBoxDiffusion.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Feb 19 09:02:09 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Fri Feb 19 11:01:16 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 7
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

class medToolBoxDiffusionPrivate;

class MEDGUI_EXPORT medToolBoxDiffusion : public medToolBox
{
    Q_OBJECT

public:
     medToolBoxDiffusion(QWidget *parent = 0);
    ~medToolBoxDiffusion(void);

private:
    medToolBoxDiffusionPrivate *d;
};

#endif
