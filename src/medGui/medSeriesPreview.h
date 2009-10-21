/* medSeriesPreview.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Thu Oct  8 19:45:42 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Oct 21 15:38:20 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 4
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDSERIESPREVIEW_H
#define MEDSERIESPREVIEW_H

#include "medGuiExport.h"

#include <QtGui>

class medSeriesPreviewPrivate;

class MEDGUI_EXPORT medSeriesPreview : public QWidget
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
