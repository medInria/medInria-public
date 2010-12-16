/* medToolBoxHeader.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Nov 10 10:10:59 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 10:11:33 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 5
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

    void setTitle(const QString& title);
    void setOrientation(Qt::Orientation orientation);

signals:
    void clicked(void);

private:
    medToolBoxHeaderPrivate *d;
};

#endif
