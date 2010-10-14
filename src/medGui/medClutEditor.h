/* medClutEditor.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue May  5 12:22:54 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Thu Oct  7 14:09:46 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 15
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDCLUTEDITOR_H
#define MEDCLUTEDITOR_H

#include "medGuiExport.h"

#include <QtGui>

class dtkAbstractData;
class medAbstractView;


// /////////////////////////////////////////////////////////////////
// medClutEditorVertex
// /////////////////////////////////////////////////////////////////

class medClutEditorVertexPrivate;

class medClutEditorVertex : public QObject, public QGraphicsItem
{
    Q_OBJECT

    Q_INTERFACES(QGraphicsItem)

public:
     medClutEditorVertex(qreal x, qreal y, QColor color = Qt::white,
                         QGraphicsItem *parent = 0);
    ~medClutEditorVertex(void);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0);

    QRectF boundingRect(void) const;
    QColor color(void) const;
    void setColor(QColor color);

    void forceGeometricalConstraints( const QRectF & limits );
    void interpolate( medClutEditorVertex * pred, medClutEditorVertex * next );

    static bool LessThan(const medClutEditorVertex *v1,
                         const medClutEditorVertex *v2) {
        return (v1->x() < v2->x());
    }

protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    // void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private :
    void setAlpha();
    medClutEditorVertexPrivate * d;
};


// /////////////////////////////////////////////////////////////////
// medClutEditorTable
// /////////////////////////////////////////////////////////////////
class medClutEditorTablePrivate;

class medClutEditorTable : public QObject, public QGraphicsItem
{
    Q_OBJECT

    Q_INTERFACES(QGraphicsItem)

public:
     medClutEditorTable(QGraphicsItem *parent = 0);
    ~medClutEditorTable(void);

    void setSize( const QSizeF & size );
    const QSizeF & size() const;
    void setRange( qreal min, qreal max );
    QPointF coordinateToValue( QPointF coord );

    void addVertex(medClutEditorVertex *vertex, bool interpolate = false);
    void forceGeometricalConstraints();

    QRectF boundingRect(void) const;

    void setSelectedAllVertices( bool isSelected );
    void deleteSelection();
    void setColorOfSelection( const QColor & color );
    void setColorOfSelection();

    void setup(float min, float max, int size, int *table);
    void setupTransferFunction(QList<double> &scalars, QList<QColor> &colors );

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0);


    // void keyPressEvent(QKeyEvent *event);
    // void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

public slots:
    void onDeleteVertex(medClutEditorVertex * v);

private:

    medClutEditorTablePrivate *d;
};


// /////////////////////////////////////////////////////////////////
// medClutEditor
// /////////////////////////////////////////////////////////////////

class medClutEditorPrivate;

class MEDGUI_EXPORT medClutEditor : public QWidget
{
    Q_OBJECT

public:
     medClutEditor(QWidget *parent = 0);
    ~medClutEditor(void);

    void setData(dtkAbstractData *data);
    void setView(medAbstractView *view);
    void applyTable(void);

protected:
    void initializeTable(void);
    void deleteTable(void);

    void mousePressEvent(QMouseEvent *event);

protected slots:
    void onNewTableAction(void);
    void onApplyTablesAction(void);
    void onColorAction(void);
    void onDeleteAction(void);

private:
    medClutEditorPrivate *d;
};

#endif // MEDCLUTEDITOR_H
