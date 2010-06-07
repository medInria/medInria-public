/* medToolBoxSource.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Thu May 13 15:19:01 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Thu May 13 16:20:55 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 11
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDTOOLBOXSOURCE_H
#define MEDTOOLBOXSOURCE_H

#include "medToolBox.h"

class medToolBoxSourcePrivate;

class MEDGUI_EXPORT medToolBoxSource : public medToolBox
{
    Q_OBJECT

public:
     medToolBoxSource(QWidget *parent = 0);
    ~medToolBoxSource(void);

signals:
    void expandAll(void);
    void collapseAll(void);

    void importCurrent(void);
    void exportCurrent(void);
    void viewCurrent(void);

    void indexChanged(int);

private:
    medToolBoxSourcePrivate *d;
};

#endif
