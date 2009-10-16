/* medSeriesPreview.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Thu Oct  8 19:47:43 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Oct 14 18:20:55 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 6
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medSeriesPreview.h"

class medSeriesPreviewPrivate
{
public:
};

medSeriesPreview::medSeriesPreview(QWidget *parent) : QWidget(parent), d(new medSeriesPreviewPrivate)
{

}

medSeriesPreview::~medSeriesPreview(void)
{
    delete d;

    d = NULL;
}

QSize medSeriesPreview::sizeHint(void) const
{
    return QSize();
}

void medSeriesPreview::setup(int seriesId)
{
    Q_UNUSED(seriesId);
}

void medSeriesPreview::paintEvent(QPaintEvent *event)
{
    QLinearGradient gradient;
    gradient.setStart(event->rect().topLeft());
    gradient.setFinalStop(event->rect().bottomLeft());

    gradient.setColorAt(0.0, QColor(0x49, 0x49, 0x49));
    gradient.setColorAt(1.0, QColor(0x31, 0x31, 0x31));

    QPainter painter(this);
    painter.fillRect(event->rect(), gradient);
    painter.end();
}
