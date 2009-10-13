/* medStudyPreview.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Thu Oct  8 19:47:21 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Thu Oct  8 19:52:47 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 3
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

void medStudyPreview::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);

    painter.fillRect(event->rect(), Qt::green);
}
