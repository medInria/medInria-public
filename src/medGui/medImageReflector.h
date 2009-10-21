/* medImageReflector.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Oct  9 14:55:15 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Oct 21 15:21:36 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 10
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDIMAGEREFLECTOR_H
#define MEDIMAGEREFLECTOR_H

#include "medGuiExport.h"

#include <QWidget>

class medImageReflectorPrivate;

class MEDGUI_EXPORT medImageReflector : public QWidget
{
    Q_OBJECT

public:
     medImageReflector(QWidget *parent = 0);
    ~medImageReflector(void);

    QSize sizeHint(void) const;

    void setImage(QImage image);

protected:
    void paintEvent(QPaintEvent *event);

private:
    medImageReflectorPrivate *d;
};

#endif
