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
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
}

v3dViewWidget::~v3dViewWidget(void)
{

}
