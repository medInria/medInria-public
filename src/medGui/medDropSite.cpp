/* medDropSite.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Feb 19 17:41:43 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Mon Feb 22 21:15:03 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 18
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include <dtkCore/dtkGlobal.h>

#include <medCore/medDataIndex.h>

#include "medDropSite.h"

#include <QtGui>

class medDropSitePrivate
{
public:
    medDataIndex index;
};

medDropSite::medDropSite(QWidget *parent) : QLabel(parent), d(new medDropSitePrivate)
{
    setAlignment(Qt::AlignCenter);
    setAcceptDrops(true);
    setBackgroundRole(QPalette::Base);
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
}

medDropSite::~medDropSite(void)
{
    delete d;

    d = NULL;
}

QSize medDropSite::sizeHint(void) const
{
    return QSize(128, 128);
}

medDataIndex medDropSite::index(void) const
{
    return d->index;
}

void medDropSite::dragEnterEvent(QDragEnterEvent *event)
{
    setBackgroundRole(QPalette::Highlight);

    event->acceptProposedAction();
}

void medDropSite::dragMoveEvent(QDragMoveEvent *event)
{
    event->acceptProposedAction();
}

void medDropSite::dragLeaveEvent(QDragLeaveEvent *event)
{
    setBackgroundRole(QPalette::Base);

    event->accept();
}

void medDropSite::dropEvent(QDropEvent *event)
{
    const QMimeData *mimeData = event->mimeData();

    if (mimeData->hasImage()) {
        setPixmap(qvariant_cast<QPixmap>(mimeData->imageData()));
    }

    if (mimeData->hasFormat("med/index")) {
        QStringList ids = QString(mimeData->data("med/index")).split(":");
        int patientId = ids.at(0).toInt();
        int   studyId = ids.at(1).toInt();
        int  seriesId = ids.at(2).toInt();
        int   imageId = ids.at(3).toInt();
        d->index = medDataIndex(patientId, studyId, seriesId, imageId);
    }

    setBackgroundRole(QPalette::Base);

    event->acceptProposedAction();
}

void medDropSite::paintEvent(QPaintEvent *event)
{
    QLabel::paintEvent(event);

//    // Optionally draw something (e.g. a tag) over the label in case it is a pixmap

//    if(!this->pixmap())
//        return;
//
//    QPainter painter;
//    painter.begin(this);
//    painter.setPen(Qt::white);
//    painter.drawText(event->rect(), "Overlay", QTextOption(Qt::AlignHCenter | Qt::AlignCenter));
//    painter.end();
}
