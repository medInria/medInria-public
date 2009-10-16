/* medSeriesPreview.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Thu Oct  8 19:45:42 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Oct 14 18:19:13 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 3
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDSERIESPREVIEW_H
#define MEDSERIESPREVIEW_H

#include <QtGui>

class medSeriesPreviewPrivate;

class medSeriesPreview : public QWidget
{
    Q_OBJECT

public:
     medSeriesPreview(QWidget *parent = 0);
    ~medSeriesPreview(void);

    QSize sizeHint(void) const;

    void setup(int seriesId);

protected:
    void paintEvent(QPaintEvent *event);

private:
    medSeriesPreviewPrivate *d;
};

#endif
