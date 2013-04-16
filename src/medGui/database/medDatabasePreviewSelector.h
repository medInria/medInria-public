/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

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
    ~medDatabasePreviewSelector();

    QRectF rect() const;
    QRectF boundingRect() const;
    QString text() const;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    void setRect(const QRectF& rect);
    void setText(const QString& text);

private:
    medDatabasePreviewSelectorPrivate *d;
};


