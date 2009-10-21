/* medImageList.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Oct 16 11:39:37 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Oct 21 15:20:40 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 4
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDIMAGELIST_H
#define MEDIMAGELIST_H

#include "medGuiExport.h"

#include <QWidget>

class medImageListPrivate;

class MEDGUI_EXPORT medImageList : public QWidget
{
    Q_OBJECT

public:
     medImageList(QWidget *parent = 0);
    ~medImageList(void);

    void clear(void);

    void setListName(int list, QString name);
    void setListSize(int list, int size);

private:
    void paintEvent(QPaintEvent *event);

private:
    medImageListPrivate *d;
};

#endif
