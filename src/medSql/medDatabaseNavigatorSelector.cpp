/* medDatabaseNavigatorSelector.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:41:09 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Dec 15 09:41:10 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 1
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medDatabaseNavigatorController.h"
#include "medDatabaseNavigatorSelector.h"

class medDatabaseNavigatorSelectorPrivate
{
public:
    QRectF rect;
    QString text;
};

medDatabaseNavigatorSelector::medDatabaseNavigatorSelector(QGraphicsItem *parent) : QObject(), QGraphicsItem(parent), d(new medDatabaseNavigatorSelectorPrivate)
{
    d->rect = QRectF(0, 0, medDatabaseNavigatorController::instance()->selectorWidth(), medDatabaseNavigatorController::instance()->selectorHeight());
}

medDatabaseNavigatorSelector::~medDatabaseNavigatorSelector(void)
{
    delete d;

    d = NULL;
}


QRectF medDatabaseNavigatorSelector::rect(void) const
{
    return d->rect;
}

QRectF medDatabaseNavigatorSelector::boundingRect(void) const
{
    return QRectF(0, 0, medDatabaseNavigatorController::instance()->selectorWidth(), medDatabaseNavigatorController::instance()->selectorHeight());
}

QString medDatabaseNavigatorSelector::text(void) const
{
    return d->text;
}

void medDatabaseNavigatorSelector::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setBrush(Qt::gray);
    painter->setPen(Qt::NoPen);
    painter->drawRoundRect(d->rect, 12, 12);
    painter->setPen(Qt::white);
    painter->drawText(d->rect.adjusted(10, d->rect.height() - 17, -10, d->rect.height()), Qt::AlignRight | Qt::TextSingleLine, d->text);
    painter->restore();
}

void medDatabaseNavigatorSelector::setRect(const QRectF& rect)
{
    d->rect = rect;
}

void medDatabaseNavigatorSelector::setText(const QString& text)
{
    d->text = text;
}
