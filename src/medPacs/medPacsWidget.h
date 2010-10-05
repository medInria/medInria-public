/* medPacsWidget.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Oct  5 11:07:02 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Oct  5 16:33:19 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 14
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDPACSWIDGET_H
#define MEDPACSWIDGET_H

#include <QtGui/QTreeWidget>

#include "medPacsExport.h"

class medPacsWidgetPrivate;

class MEDPACS_EXPORT medPacsWidget : public QTreeWidget
{
    Q_OBJECT

public:
     medPacsWidget(QWidget* parent);
    ~medPacsWidget(void);

    void setHostTitle(const QString& title);
    void setHostAddress(const QString& address);
    void setHostPort(const QString& port);

    void setNodeTitle(const QString& title);
    void setNodeAddress(const QString& address);
    void setNodePort(const QString& port);

public slots:
    void echoTest(void);
    void findTest(void);

private:
    medPacsWidgetPrivate *d;
};

#endif
