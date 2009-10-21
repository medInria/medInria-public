/* medLoginWidget.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Oct  5 15:34:43 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Oct 21 15:22:29 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 10
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDLOGINWIDGET_H
#define MEDLOGINWIDGET_H

#include "medGuiExport.h"

#include <QtGui>

class medLoginWidgetPrivate;

class MEDGUI_EXPORT medLoginWidget : public QWidget
{
    Q_OBJECT

public:
     medLoginWidget(QWidget *parent = 0);
    ~medLoginWidget(void);

protected slots:
    void onTimeOut(void);
    void onReturnPressed(void);

protected:
    void paintEvent(QPaintEvent *event);

private:
    medLoginWidgetPrivate *d;
};

#endif
