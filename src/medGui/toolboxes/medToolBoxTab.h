/* medToolBoxTab.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Nov 10 10:09:39 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Mon Dec 20 16:16:56 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 9
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#pragma once

#include "medGuiExport.h"

#include <QtGui>

class medToolBoxTabPrivate;

class MEDGUI_EXPORT medToolBoxTab : public QTabWidget
{
    Q_OBJECT
    Q_PROPERTY(int height READ height WRITE setFixedHeight)

public:
     medToolBoxTab(QWidget *parent = NULL);
    ~medToolBoxTab(void);

protected:
    void paintEvent(QPaintEvent *event);

private:
    medToolBoxTabPrivate *d;
};


