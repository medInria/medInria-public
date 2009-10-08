/* medImagePreview.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Oct  7 16:13:54 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Thu Oct  8 19:54:07 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 15
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medImagePreview.h"

class medImagePreviewPrivate
{
public:
};

medImagePreview::medImagePreview(QWidget *parent) : QWidget(parent), d(new medImagePreviewPrivate)
{

}

medImagePreview::~medImagePreview(void)
{
    delete d;

    d = NULL;
}

void medImagePreview::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.fillRect(event->rect(), Qt::yellow);
}
