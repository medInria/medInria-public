/* medDatabasePreviewSelector.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:44:04 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Dec 15 09:44:05 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 1
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDDATABASEPREVIEWSELECTOR_H
#define MEDDATABASEPREVIEWSELECTOR_H

#include <QtCore>
#include <QtGui>

class medDatabasePreviewSelectorPrivate;

class medDatabasePreviewSelector : public QObject, public QGraphicsItem
{
    Q_OBJECT

    Q_PROPERTY(QPointF pos READ pos WRITE setPos)
    Q_PROPERTY(QRectF rect READ rect WRITE setRect)
    Q_PROPERTY(QString text READ text WRITE setText)

    Q_INTERFACES(QGraphicsItem)

public:
     medDatabasePreviewSelector(QGraphicsItem *parent = 0);
    ~medDatabasePreviewSelector(void);

    QRectF rect(void) const;
    QRectF boundingRect(void) const;
    QString text(void) const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void setRect(const QRectF& rect);
    void setText(const QString& text);

private:
    medDatabasePreviewSelectorPrivate *d;
};

#endif // MEDDATABASEPREVIEWSELECTOR_H
