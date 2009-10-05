/* medLoginWidget.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Oct  5 15:34:43 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Mon Oct  5 15:56:55 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 4
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDLOGINWIDGET_H
#define MEDLOGINWIDGET_H

#include <QtGui>

class medLoginWidgetPrivate;

class medLoginWidget : public QWidget
{
    Q_OBJECT

public:
     medLoginWidget(QWidget *parent = 0);
    ~medLoginWidget(void);

protected slots:
    void update(void);

private:
    medLoginWidgetPrivate *d;
};

#endif
