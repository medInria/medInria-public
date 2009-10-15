/* medImageReflector.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Oct  9 14:57:22 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Fri Oct  9 16:02:10 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 64
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medImageReflector.h"

#include <QtGui>

// /////////////////////////////////////////////////////////////////
// Helper functions
// /////////////////////////////////////////////////////////////////

QImage filter(const QImage& image)
{
    QImage filtered = image.convertToFormat(QImage::Format_ARGB32_Premultiplied).scaled(100, 100, Qt::KeepAspectRatio);

    QPainter p(&filtered);
    p.setPen(QPen(Qt::white, 2));
    p.drawRect(filtered.rect());
    p.end();

    return filtered;
}

QImage mirror(const QImage& image)
{
    QImage mirrored = image.convertToFormat(QImage::Format_ARGB32_Premultiplied).mirrored().scaled(image.width(), image.height()/2);

    QPoint p1, p2; p2.setY(mirrored.height());
    
    QLinearGradient gradient(p1, p2);
    gradient.setColorAt(0, Qt::white);
    gradient.setColorAt(0.25, QColor(0, 0, 0, 20));
    gradient.setColorAt(1, Qt::transparent);
    
    QPainter p(&mirrored);
    p.setCompositionMode(QPainter::CompositionMode_DestinationIn);
    p.fillRect(0, 0, mirrored.width(), mirrored.height(), gradient);
    p.end();

    return mirrored;
}

// /////////////////////////////////////////////////////////////////
// medImageReflector
// /////////////////////////////////////////////////////////////////

class medImageReflectorPrivate
{
public:
    QImage filtered_image;
    QImage mirrored_image;
};

medImageReflector::medImageReflector(QWidget *parent) : QWidget(parent), d(new medImageReflectorPrivate)
{
    this->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
}

medImageReflector::~medImageReflector(void)
{

}

QSize medImageReflector::sizeHint(void) const
{
    return QSize(d->mirrored_image.width(), d->filtered_image.height() + d->mirrored_image.height());
}

void medImageReflector::setImage(QImage image)
{
    d->filtered_image = filter(image);
    d->mirrored_image = mirror(d->filtered_image);
}

void medImageReflector::paintEvent(QPaintEvent *event)
{
    QPainter painter;

    painter.begin(this);
    painter.drawImage(0, 0, d->filtered_image);
    painter.drawImage(0, d->filtered_image.height(), d->mirrored_image);
    painter.end();
}
