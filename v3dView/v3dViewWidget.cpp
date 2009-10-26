/* v3dViewWidget.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Oct 19 10:42:24 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Mon Oct 19 15:06:42 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 21
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "v3dViewWidget.h"

#include <QtCore>
#include <QtGui>

v3dViewWidget::v3dViewWidget(QWidget *parent) : QVTKWidget(parent)
{

}

v3dViewWidget::~v3dViewWidget(void)
{

}

void v3dViewWidget::focusInEvent(QFocusEvent *event)
{
    QVTKWidget::focusInEvent(event);

    if (QWidget *parent = dynamic_cast<QWidget *>(this->parent()))
        parent->setFocus();
}

void v3dViewWidget::focusOutEvent(QFocusEvent *event)
{
    QVTKWidget::focusOutEvent(event);

    event->ignore();
}
