/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medCoreExport.h>

#include <QtGui>
#include <QtWidgets>

class medAbstractData;
class medAbstractImageView;


// /////////////////////////////////////////////////////////////////
// medClutEditorVertex
// /////////////////////////////////////////////////////////////////

class medClutEditorVertexPrivate;

// TODO use QGraphicsObjactItem noobs.
class medClutEditorVertex : public QObject, public QGraphicsItem
{
    Q_OBJECT

public:
    medClutEditorVertex(QPointF value, QPointF coord, QColor color = Qt::white,
			QGraphicsItem *parent = 0);
    medClutEditorVertex( const medClutEditorVertex & other,
                         QGraphicsItem *parent = 0);
    ~medClutEditorVertex();

    const QPointF & value() const;
    void shiftValue( qreal amount, bool forceConstraints = true );

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0);

    QRectF boundingRect() const;
    QColor color() const;
    void setColor(QColor color);

    void initiateMove();
    void finalizeMove();
    void forceGeometricalConstraints( const QRectF & limits,
				      bool inManhattan = false );

    void interpolate( medClutEditorVertex * pred, medClutEditorVertex * next );

    void updateCoordinates();
    void updateValue();

    static bool LessThan(const medClutEditorVertex *v1,
                         const medClutEditorVertex *v2) {
        return (v1->x() < v2->x());
    }

protected:
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

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

public:
    //medClutEditorTable(QGraphicsItem *parent = 0);
    medClutEditorTable(const medClutEditorTable & table);
    medClutEditorTable(const QString & title = "Unknown", QGraphicsItem *parent = 0);
    ~medClutEditorTable();

    const QString & title() const;
    void setTitle(const QString & title);
    void setMinSize( const QSizeF & size );
    const QSizeF & minSize() const;
    void setMinRange( qreal min, qreal max );
    void setSize( const QSizeF & size );
    const QSizeF & size() const;
    // void setRange( qreal min, qreal max );

    void addVertex(medClutEditorVertex *vertex, bool interpolate = false);
    QList<medClutEditorVertex *> & vertices();
    const QList<medClutEditorVertex *> & vertices() const;

    void initiateMoveSelection();
    void constrainMoveSelection( medClutEditorVertex * driver,
				 bool inManhattan = false );
    void finalizeMoveSelection();
    void updateCoordinates();

    QRectF boundingRect() const;
    void range( qreal & min, qreal & max ) const;

    void setSelectedAllVertices( bool isSelected );
    void deleteSelection();
    void setColorOfSelection( const QColor & color );
    void setColorOfSelection();

    void scaleWindowWidth( qreal factor );
    void shiftWindowCenter( qreal amount );

    void setup(float min, float max, int size, int *table);
    void getTransferFunction(QList<double> &scalars, QList<QColor> &colors );
    void setTransferFunction(QList<double> &scalars, QList<QColor> &colors );
    void simplifyTransferFunction();

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0);

    void changeDisplayAlpha( qreal step );
    void resetDisplayAlpha();

    void triggerVertexChanged();
    // void keyPressEvent(QKeyEvent *event);
    // void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

signals:
    void vertexChanged();
    void vertexRemoved();
    void vertexAdded();
    
// public slots:
//     void onDeleteVertex(medClutEditorVertex * v);

private:
    void deleteAllVertices();

    medClutEditorTablePrivate *d;
};


// /////////////////////////////////////////////////////////////////
// medClutEditorHistogram
// /////////////////////////////////////////////////////////////////
class medClutEditorHistogramPrivate;

class medClutEditorHistogram : public QGraphicsItem
{
public:
     medClutEditorHistogram(QGraphicsItem *parent = 0);
    ~medClutEditorHistogram();

    // QSizeF size() const;
    // void setSize( QSizeF s );

    void range( qreal & min, qreal & max ) const;
    qreal getRangeMin() const;
    qreal getRangeMax() const;
    void updateCoordinates();

    const QMap< qreal, qreal > & values();
    void setValues(const QMap<qreal, qreal> & bins);
    void addValue(qreal intensity, qreal number);
    void adjustScales();
    // QPointF valueToCoordinate( QPointF value ) const;

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = 0);

    QRectF boundingRect() const;

private:
    medClutEditorHistogramPrivate * d;
};


// /////////////////////////////////////////////////////////////////
// medClutEditorScene
// /////////////////////////////////////////////////////////////////
class medClutEditorView;
class medClutEditorScenePrivate;

class medClutEditorScene : public QGraphicsScene
{
public:
     medClutEditorScene(QObject *parent = 0);
    ~medClutEditorScene();

    medClutEditorTable * table();
    medClutEditorHistogram * histogram();
    medClutEditorView * view();

    QRectF  plotArea();
    QPointF coordinateToValue( QPointF coord );
    QPointF valueToCoordinate( QPointF value );

    void updateCoordinates();
    void setSize( QSizeF size );
    void setRange( qreal min, qreal max );
    void adjustRange();
    void scaleRange( qreal factor );
    void shiftRange( qreal amount );
    // void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
private:
    medClutEditorScenePrivate * d;
};


// /////////////////////////////////////////////////////////////////
// medClutEditorView
// /////////////////////////////////////////////////////////////////

class medClutEditorView : public QGraphicsView
{
public:
     medClutEditorView(QWidget *parent = 0);
    ~medClutEditorView();

protected:
    medClutEditorTable * table();

    void resizeEvent	   ( QResizeEvent * event );
    void wheelEvent 	   ( QWheelEvent  * event );
    void keyReleaseEvent   ( QKeyEvent    * event );
    void keyPressEvent     ( QKeyEvent    * event );
    void mousePressEvent   ( QMouseEvent  * event );
    void mouseReleaseEvent ( QMouseEvent  * event );
};



// /////////////////////////////////////////////////////////////////
// medClutEditor
// /////////////////////////////////////////////////////////////////

class medClutEditorPrivate;

class MEDCORE_EXPORT medClutEditor : public QWidget
{
    Q_OBJECT

public:
     medClutEditor(QWidget *parent = 0);
    ~medClutEditor();

    void setData(medAbstractData *data);
    void setView(medAbstractImageView *view, bool force = false);
    void applyTable();

protected:
    void initializeTable();
    void deleteTable();

    void mousePressEvent(QMouseEvent *event);

protected slots:
    void onNewTableAction();
    void onLoadTableAction();
    void onSaveTableAction();
    // void onDeleteTableAction();
    void onApplyTablesAction();
    // void onColorAction();
    // void onDeleteAction();
    void onVertexMoved();
    void onToggleDirectUpdateAction();
private:
    medClutEditorPrivate *d;
};


