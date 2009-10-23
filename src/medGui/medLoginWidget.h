/* medLoginWidget.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Oct  5 15:34:43 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Fri Oct 23 10:49:03 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 15
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

    enum State {
        NONE,
        LOGGED,
        LOGGING
    };

signals:
    void accepted(void);
    void rejected(void);

protected slots:
    void onTimeOut(void);
    void onReturnPressed(void);
    void onLoginButtonClicked(void);
    void onAdminButtonClicked(void);

protected:
    void paintEvent(QPaintEvent *event);

private:
    medLoginWidgetPrivate *d;
};

#endif
