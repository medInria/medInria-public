/* medStudyPreview.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Thu Oct  8 19:47:21 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Oct 14 18:21:28 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 12
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medStudyPreview.h"

class medStudyPreviewPrivate
{
public:
};

medStudyPreview::medStudyPreview(QWidget *parent) : QWidget(parent), d(new medStudyPreviewPrivate)
{

}

medStudyPreview::~medStudyPreview(void)
{
    delete d;

    d = NULL;
}

QSize medStudyPreview::sizeHint(void) const
{
    return QSize();
}

void medStudyPreview::setup(int studyId)
{
    Q_UNUSED(studyId);
}

void medStudyPreview::paintEvent(QPaintEvent *event)
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
