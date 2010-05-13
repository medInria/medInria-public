/* medToolBoxJobs.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Thu May 13 15:21:29 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Thu May 13 16:02:58 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 4
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDTOOLBOXJOBS_H
#define MEDTOOLBOXJOBS_H

#include "medToolBox.h"

class medProgressionStack;
class medToolBoxJobsPrivate;

class MEDGUI_EXPORT medToolBoxJobs : public medToolBox
{
    Q_OBJECT

public:
     medToolBoxJobs(QWidget *parent = 0);
    ~medToolBoxJobs(void);

    medProgressionStack *stack(void);

private:
    medToolBoxJobsPrivate *d;
};

#endif
