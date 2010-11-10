/* medBrowserToolBoxJobs.h --- 
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

#ifndef MEDBROWSERTOOLBOXJOBS_H
#define MEDBROWSERTOOLBOXJOBS_H

#include <medGui/medToolBox.h>

class medProgressionStack;
class medBrowserToolBoxJobsPrivate;

class MEDGUI_EXPORT medBrowserToolBoxJobs : public medToolBox
{
    Q_OBJECT

public:
     medBrowserToolBoxJobs(QWidget *parent = 0);
    ~medBrowserToolBoxJobs(void);

    medProgressionStack *stack(void);

private:
    medBrowserToolBoxJobsPrivate *d;
};

#endif
