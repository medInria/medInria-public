/* medStudyPreview.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Thu Oct  8 19:45:13 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Thu Oct  8 19:46:15 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 3
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDSTUDYPREVIEW_H
#define MEDSTUDYPREVIEW_H

#include <QtGui>

class medStudyPreviewPrivate;

class medStudyPreview : public QWidget
{
    Q_OBJECT

public:
     medStudyPreview(QWidget *parent = 0);
    ~medStudyPreview(void);

protected:
    void paintEvent(QPaintEvent *event);

private:
    medStudyPreviewPrivate *d;
};

#endif
