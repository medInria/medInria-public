/* medToolBoxHeader.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Nov 10 10:10:59 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Mon Dec 20 13:45:08 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 8
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDTOOLBOXHEADER_H
#define MEDTOOLBOXHEADER_H

#include "medGuiExport.h"

#include <QtGui>

class medToolBoxHeaderPrivate;

class MEDGUI_EXPORT medToolBoxHeader : public QFrame
{
    Q_OBJECT

public:
     medToolBoxHeader(QWidget *parent = 0);
    ~medToolBoxHeader(void);

    QSize sizeHint(void) const;

    void setTitle(const QString& title);
    void setOrientation(Qt::Orientation orientation);

protected:
    void paintEvent(QPaintEvent *event);

private:
    medToolBoxHeaderPrivate *d;
};

#endif
