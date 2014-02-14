/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medDatabasePreviewController.h>
#include "medDatabasePreviewSelector.h"

class medDatabasePreviewSelectorPrivate
{
public:
    QRectF rect;
    QString text;
};

medDatabasePreviewSelector::medDatabasePreviewSelector(QGraphicsItem *parent) : QObject(), QGraphicsItem(parent), d(new medDatabasePreviewSelectorPrivate)
{
    d->rect = QRectF(0, 0, medDatabasePreviewController::instance()->selectorWidth(), medDatabasePreviewController::instance()->selectorHeight());
}

medDatabasePreviewSelector::~medDatabasePreviewSelector(void)
{
    delete d;

    d = NULL;
}

QRectF medDatabasePreviewSelector::rect(void) const
{
    return d->rect;
}

QRectF medDatabasePreviewSelector::boundingRect(void) const
{
    return QRectF(0, 0, medDatabasePreviewController::instance()->selectorWidth(), medDatabasePreviewController::instance()->selectorHeight());
}

QString medDatabasePreviewSelector::text(void) const
{
    return d->text;
}

void medDatabasePreviewSelector::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setBrush(Qt::gray);
    painter->setPen(Qt::NoPen);
    painter->drawRoundedRect(d->rect, 12, 12);
    painter->setPen(Qt::white);
    painter->drawText(d->rect.adjusted(10, d->rect.height() - 17, -10, d->rect.height()), Qt::AlignRight | Qt::TextSingleLine, d->text);
    painter->restore();
}

void medDatabasePreviewSelector::setRect(const QRectF& rect)
{
    d->rect = rect;
}

void medDatabasePreviewSelector::setText(const QString& text)
{
    d->text = text;
}
