/* medImagePreview.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Oct  7 16:12:23 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Thu Oct  8 19:46:29 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 10
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDIMAGEPREVIEW_H
#define MEDIMAGEPREVIEW_H

#include <QtGui>

class medImagePreviewPrivate;

class medImagePreview : public QWidget
{
    Q_OBJECT

public:
     medImagePreview(QWidget *parent = 0);
    ~medImagePreview(void);

protected:
    void paintEvent(QPaintEvent *event);

private:
    medImagePreviewPrivate *d;
};

#endif
