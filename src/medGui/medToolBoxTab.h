/* medToolBoxTab.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Nov 10 10:09:39 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 10:10:38 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 5
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDTOOLBOXTAB_H
#define MEDTOOLBOXTAB_H

#include "medGuiExport.h"

#include <QtGui>

class medToolBoxTabPrivate;

class MEDGUI_EXPORT medToolBoxTab : public QTabWidget
{
    Q_OBJECT

    Q_PROPERTY(int height READ height WRITE setFixedHeight)

public:
     medToolBoxTab(QWidget *parent = 0);
    ~medToolBoxTab(void);

    void setOrientation(Qt::Orientation);

protected:
    void paintEvent(QPaintEvent *event);

private:
    medToolBoxTabPrivate *d;
};

#endif
