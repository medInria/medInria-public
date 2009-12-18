/* v3dViewWidget.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Oct 19 10:08:46 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Mon Oct 19 10:42:54 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 10
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef V3DVIEWWIDGET_H
#define V3DVIEWWIDGET_H

#include <QVTKWidget.h>

class v3dViewWidgetPrivate;

class v3dViewWidget : public QVTKWidget
{
    Q_OBJECT

public:
             v3dViewWidget(QWidget *parent = 0);
    virtual ~v3dViewWidget(void);
};

#endif
