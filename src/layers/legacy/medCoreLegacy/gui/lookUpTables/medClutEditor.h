#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QtGui>
#include <QtWidgets>

#include <medCoreLegacyExport.h>

class medAbstractData;
class medAbstractImageView;

// /////////////////////////////////////////////////////////////////
// medClutEditorVertex
// /////////////////////////////////////////////////////////////////

class medClutEditorVertexPrivate;

class MEDCORELEGACY_EXPORT medClutEditorVertex : public QGraphicsObject
{
    Q_OBJECT

public:
    medClutEditorVertex(QPointF value, QPointF coord, QColor color = Qt::white, QGraphicsItem *parent = nullptr);
    medClutEditorVertex( const medClutEditorVertex & other, QGraphicsItem *parent = nullptr);
    ~medClutEditorVertex();

    const QPointF & value() const;
    void shiftValue( qreal amount, bool forceConstraints = true );

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr);

    QRectF boundingRect() const;
    QColor color() const;
    void setColor(QColor color);
    QAction *getColorAction();

    void initiateMove();
    void finalizeMove();
    void forceGeometricalConstraints( const QRectF & limits, bool inManhattan = false );

    void interpolate( medClutEditorVertex * pred, medClutEditorVertex * next );

    void updateCoordinates();
    void updateValue();

    static bool LessThan(const medClutEditorVertex *v1, const medClutEditorVertex *v2)
    {
        return (v1->x() < v2->x());
    }

public slots:
    void setValue (void);
    void showColorSelection();

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

class MEDCORELEGACY_EXPORT medClutEditorTable : public QGraphicsObject
{
    Q_OBJECT

public:
    medClutEditorTable(const medClutEditorTable & table);
    medClutEditorTable(const QString & title = "Unknown", QGraphicsItem *parent = nullptr);
    ~medClutEditorTable();

    const QString & title() const;
    void setTitle(const QString & title);
    void setMinSize( const QSizeF & size );
    const QSizeF & minSize() const;
    void setMinRange( qreal min, qreal max );
    void setSize( const QSizeF & size );
    const QSizeF & size() const;

    void setDiscreteMode(bool);
    void invertLUT(bool);
    QHash<medClutEditorVertex *, medClutEditorVertex *> *calculateCoupledVertices(QList<medClutEditorVertex *>);
    void updateVerticesToDisplay();

    void addVertex(medClutEditorVertex *vertex, bool interpolate = false);
    void deleteAllVertices();
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
               QWidget *widget = nullptr);

    void changeDisplayAlpha( qreal step );
    void resetDisplayAlpha();

    void triggerVertexMoving();
    void triggerVertexSet();

signals:
    void vertexMoving();
    void vertexSet();
    void vertexRemoved();
    void vertexAdded();
    void switchMode();

private:
    medClutEditorTablePrivate *d;
};


// /////////////////////////////////////////////////////////////////
// medClutEditorHistogram
// /////////////////////////////////////////////////////////////////
class medClutEditorHistogramPrivate;

class MEDCORELEGACY_EXPORT medClutEditorHistogram : public QGraphicsItem
{
public:
     medClutEditorHistogram(QGraphicsItem *parent = nullptr);
    ~medClutEditorHistogram();

    void range( qreal & min, qreal & max ) const;
    qreal getRangeMin() const;
    qreal getRangeMax() const;
    void updateCoordinates();

    const QMap< qreal, qreal > & values();
    void setValues(const QMap<qreal, qreal> & bins);
    void addValue(qreal intensity, qreal number);
    void adjustScales();

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget = nullptr);

    QRectF boundingRect() const;

private:
    medClutEditorHistogramPrivate * d;
};


// /////////////////////////////////////////////////////////////////
// medClutEditorScene
// /////////////////////////////////////////////////////////////////
class medClutEditorView;
class medClutEditorScenePrivate;

class MEDCORELEGACY_EXPORT medClutEditorScene : public QGraphicsScene
{
public:
     medClutEditorScene(QObject *parent = nullptr);
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

private:
    medClutEditorScenePrivate * d;
};


// /////////////////////////////////////////////////////////////////
// medClutEditorView
// /////////////////////////////////////////////////////////////////

class MEDCORELEGACY_EXPORT medClutEditorView : public QGraphicsView
{
public:
     medClutEditorView(QWidget *parent = nullptr);
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
