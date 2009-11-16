/* medImagePreview.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Oct  7 16:13:54 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Nov  3 09:39:06 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 20
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

QSize medImagePreview::sizeHint(void) const
{
    return QSize();
}

void medImagePreview::setup(int imageId)
{
    Q_UNUSED(imageId);
}

void medImagePreview::paintEvent(QPaintEvent *event)
{
    // QLinearGradient gradient;
    // gradient.setStart(event->rect().topLeft());
    // gradient.setFinalStop(event->rect().bottomLeft());

    // gradient.setColorAt(0.0, QColor(0x49, 0x49, 0x49));
    // gradient.setColorAt(1.0, QColor(0x31, 0x31, 0x31));

    QPainter painter(this);
    painter.fillRect(event->rect(), QColor(0x49, 0x49, 0x49));
    painter.end();
}
