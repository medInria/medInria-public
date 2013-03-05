/* medToolBoxBody.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Nov 10 10:11:44 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Nov 10 10:12:26 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 5
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDTOOLBOXBODY_H
#define MEDTOOLBOXBODY_H

#include "medGuiExport.h"

#include <QtGui>

class medToolBoxBodyPrivate;
class medToolBoxTab;

class MEDGUI_EXPORT medToolBoxBody : public QFrame
{
    Q_OBJECT

public:
     medToolBoxBody(QWidget *parent = 0);
    ~medToolBoxBody();

    void addWidget(QWidget *widget);

    void setTabWidget(medToolBoxTab * tab);
    void clear();

    void setOrientation(Qt::Orientation);

private:
    medToolBoxBodyPrivate *d;
};

#endif
