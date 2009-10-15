/* medImagePreview.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Oct  7 16:13:54 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Oct 14 18:20:40 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 17
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
    QLinearGradient gradient;
    gradient.setStart(event->rect().topLeft());
    gradient.setFinalStop(event->rect().bottomLeft());

    gradient.setColorAt(0.0, QColor(0x49, 0x49, 0x49));
    gradient.setColorAt(1.0, QColor(0x31, 0x31, 0x31));

    QPainter painter(this);
    painter.fillRect(event->rect(), gradient);
    painter.end();
}
