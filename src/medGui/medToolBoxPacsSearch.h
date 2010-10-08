/* medToolBoxPacsSearch.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Oct  5 15:45:46 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Oct  6 10:39:31 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 14
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDTOOLBOXPACSSEARCH_H
#define MEDTOOLBOXPACSSEARCH_H

#include "medToolBox.h"

class medToolBoxPacsSearchPrivate;

class MEDGUI_EXPORT medToolBoxPacsSearch : public medToolBox
{
    Q_OBJECT

public:
     medToolBoxPacsSearch(QWidget *parent = 0);
    ~medToolBoxPacsSearch(void);

signals:
    void search(QString query);

protected slots:
    void onSearch(void);

private:
    medToolBoxPacsSearchPrivate *d;
};

#endif
