/* medRegistrationWidget.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Jan 18 11:14:24 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Mon Jan 18 11:16:23 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 5
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

#include <QLabel>
#include <QWidget>

// /////////////////////////////////////////////////////////////////
// medRegistrationWidgetItem
// /////////////////////////////////////////////////////////////////

class medRegistrationWidgetItemPrivate;

class MEDGUI_EXPORT medRegistrationWidgetItem : public QLabel
{
    Q_OBJECT

public:
     medRegistrationWidgetItem(QWidget *parent = 0);
    ~medRegistrationWidgetItem(void);

    QSize sizeHint(void) const;

protected:
    void dragEnterEvent(QDragEnterEvent *event);
    void dragMoveEvent(QDragMoveEvent *event);
    void dragLeaveEvent(QDragLeaveEvent *event);
    void dropEvent(QDropEvent *event);
    void paintEvent(QPaintEvent *event);

private:
    medRegistrationWidgetItemPrivate *d;
};

// /////////////////////////////////////////////////////////////////
// medRegistrationWidget
// /////////////////////////////////////////////////////////////////

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
