/* medBrowserJobsToolBox.h ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Thu May 13 15:21:29 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 11:13:47 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 7
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#ifndef medBrowserJobsToolBox_H
#define medBrowserJobsToolBox_H

#include "medToolBox.h"
#include "medGuiExport.h"

class medProgressionStack;
class medBrowserJobsToolBoxPrivate;

class MEDGUI_EXPORT medBrowserJobsToolBox : public medToolBox
{
    Q_OBJECT

public:
     medBrowserJobsToolBox(QWidget *parent = 0);
    ~medBrowserJobsToolBox(void);

    medProgressionStack *stack(void);

private:
    medBrowserJobsToolBoxPrivate *d;
};

#endif
