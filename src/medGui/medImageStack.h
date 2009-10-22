/* medImageStack.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Thu Oct  8 20:02:04 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Oct 21 15:21:51 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 13
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDIMAGESTACK_H
#define MEDIMAGESTACK_H

#include "medGuiExport.h"

#include <QWidget>

class medImageStackPrivate;

class MEDGUI_EXPORT medImageStack : public QWidget
{
    Q_OBJECT

public:
     medImageStack(QWidget *parent = 0);
    ~medImageStack(void);

    void clear(void);

    void setStackName(int stack, QString name);
    void setStackSize(int stack, int size);

private:
    void paintEvent(QPaintEvent *event);

private:
    medImageStackPrivate *d;
};

#endif
