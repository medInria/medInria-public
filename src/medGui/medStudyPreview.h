/* medStudyPreview.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Thu Oct  8 19:45:13 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Oct 14 18:18:56 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 6
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

    QSize sizeHint(void) const;

    void setup(int studyId);

protected:
    void paintEvent(QPaintEvent *event);

private:
    medStudyPreviewPrivate *d;
};

#endif
