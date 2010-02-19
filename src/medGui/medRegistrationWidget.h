/* medRegistrationWidget.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Jan 18 11:14:24 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Fri Feb 19 17:41:04 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 8
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDREGISTRATIONWIDGET_H
#define MEDREGISTRATIONWIDGET_H

#include "medGuiExport.h"

#include <QtGui/QWidget>

class medRegistrationWidgetPrivate;

class MEDGUI_EXPORT medRegistrationWidget : public QWidget
{
    Q_OBJECT

public:
     medRegistrationWidget(QWidget *parent = 0);
    ~medRegistrationWidget(void);

private:
    medRegistrationWidgetPrivate *d;
};

#endif // MEDREGISTRATIONWIDGET_H
