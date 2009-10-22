/* medImagePreview.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Oct  7 16:12:23 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Oct 21 15:21:15 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 13
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDIMAGEPREVIEW_H
#define MEDIMAGEPREVIEW_H

#include "medGuiExport.h"

#include <QtGui>

class medImagePreviewPrivate;

class MEDGUI_EXPORT medImagePreview : public QWidget
{
    Q_OBJECT

public:
     medImagePreview(QWidget *parent = 0);
    ~medImagePreview(void);

    QSize sizeHint(void) const;

    void setup(int imageId);

protected:
    void paintEvent(QPaintEvent *event);

private:
    medImagePreviewPrivate *d;
};

#endif
