/* medSeriesPreview.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Thu Oct  8 19:47:43 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Thu Oct  8 19:52:59 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 3
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

void medSeriesPreview::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.fillRect(event->rect(), Qt::blue);
}
