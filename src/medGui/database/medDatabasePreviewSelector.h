/* medDatabasePreviewSelector.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:44:04 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Mar  3 21:53:10 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 2
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#pragma once

#include <QtCore>
#include <QtGui>

class medDatabasePreviewSelectorPrivate;

class medDatabasePreviewSelector : public QObject, public QGraphicsItem
{
    Q_OBJECT

    Q_PROPERTY(QPointF pos READ pos WRITE setPos)
    Q_PROPERTY(QRectF rect READ rect WRITE setRect)
    Q_PROPERTY(QString text READ text WRITE setText)

#if QT_VERSION >= 0x040600
    Q_INTERFACES(QGraphicsItem)
#endif

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


