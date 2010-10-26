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
// medClutEditorHistogram
// /////////////////////////////////////////////////////////////////
class medClutEditorHistogramPrivate;

class medClutEditorHistogram : public QGraphicsItem
{
public:
     medClutEditorHistogram(QGraphicsItem *parent = 0);
    ~medClutEditorHistogram(void);

    QSizeF size(void) const;
    void setSize( QSizeF s );

    void addValue(qreal intensity, qreal number);
    qreal getRangeMin() const;
    qreal getRangeMax() const;
    QPointF valueToCoordinate( QPointF value ) const;

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0);

    QRectF boundingRect(void) const;

private:
    medClutEditorHistogramPrivate * d;
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
    medClutEditorTable(const QString & title = "Unknown", QGraphicsItem *parent = 0);
    ~medClutEditorTable(void);

    QString title();
    void setTitle(const QString & title);
    void setSize( const QSizeF & size );
    const QSizeF & size() const;
    void setRange( qreal min, qreal max );
    QPointF coordinateToValue( QPointF coord );

    void addVertex(medClutEditorVertex *vertex, bool interpolate = false);
    QList<medClutEditorVertex*> vertices();
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
// medClutEditorScene
// /////////////////////////////////////////////////////////////////

class medClutEditorScene : public QGraphicsScene
{
public:
     medClutEditorScene(QObject *parent = 0);
    ~medClutEditorScene(void);

    medClutEditorTable * table();
    // void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
};


// /////////////////////////////////////////////////////////////////
// medClutEditorView
// /////////////////////////////////////////////////////////////////

class medClutEditorView : public QGraphicsView
{
public:
     medClutEditorView(QWidget *parent = 0);
    ~medClutEditorView(void);

protected:
    medClutEditorTable * table();

    void resizeEvent(QResizeEvent *event);
    void wheelEvent ( QWheelEvent * event );
    void keyReleaseEvent( QKeyEvent * event);
    void keyPressEvent( QKeyEvent * event);
    void mousePressEvent(QMouseEvent * event );
    void mouseReleaseEvent(QMouseEvent * event );
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
    void onLoadTableAction(void);
    void onSaveTableAction(void);
    void onDeleteTableAction(void);
    void onApplyTablesAction(void);
    void onColorAction(void);
    void onDeleteAction(void);

private:
    medClutEditorPrivate *d;
};

#endif // MEDCLUTEDITOR_H
