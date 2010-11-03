/* medClutEditor.cpp ---
 *
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue May  5 12:20:38 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Feb  9 23:11:38 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 274
 */

/* Commentary:
 *
 */

/* Change log:
 *
 */

#include <dtkCore/dtkAbstractData.h>
#include <dtkCore/dtkAbstractDataImage.h>
#include <dtkCore/dtkAbstractView.h>
#include <medCore/medAbstractView.h>
#include <medCore/medStorage.h>
#include "medClutEditor.h"

#include <math.h>
#include <climits>

#include "medLUTtoXMLWriter.h"
#include "medXMLtoLUTReader.h"
#include "medSaveLUTDialog.h"
#include "medLoadLUTDialog.h"

// /////////////////////////////////////////////////////////////////
// medClutEditorVertex
// /////////////////////////////////////////////////////////////////

class medClutEditorVertexPrivate
{
public:
    QColor color;
    QColor fgColor;
    QColor bgColor;
};

medClutEditorVertex::medClutEditorVertex(qreal x, qreal y, QColor color,
                                         QGraphicsItem * parent)
  : QGraphicsItem(parent)
{
    d = new medClutEditorVertexPrivate;
    this->setPos(x, y);
    this->setZValue(1);
    setAlpha();
    setColor( color );

    this->setFlag(QGraphicsItem::ItemIsMovable, true);
    this->setFlag(QGraphicsItem::ItemIsSelectable, true);
}

medClutEditorVertex::medClutEditorVertex( const medClutEditorVertex & other,
					  QGraphicsItem * parent)
  : QGraphicsItem( parent )
{
    d = new medClutEditorVertexPrivate;
    d->color   = other.d->color;
    d->fgColor = other.d->fgColor;
    d->bgColor = other.d->bgColor;

    this->setPos( other.pos() );
    this->setZValue(1);
    setAlpha();
    setColor( d->color );

    this->setFlag(QGraphicsItem::ItemIsMovable, true);
    this->setFlag(QGraphicsItem::ItemIsSelectable, true);
}

medClutEditorVertex::~medClutEditorVertex(void)
{
    delete d;
}

void medClutEditorVertex::interpolate( medClutEditorVertex * prev,
                                       medClutEditorVertex * next )
{
  if ( prev != NULL && next == NULL )
      setColor( prev->color() );
  else if ( prev == NULL && next != NULL )
      setColor( next->color() );
  else if ( prev != NULL && next != NULL ) {
      if ( ( prev->color().alpha() == 0 ) &&
           ( next->color().alpha() > 0 ) )
          setColor( next->color() );
      else if ( ( prev->color().alpha() > 0 ) &&
                ( next->color().alpha() == 0 ) )
          setColor( prev->color() );
      else if ( ( this->x() - prev->x() ) < ( next->x() - this->x() ) )
          setColor( prev->color() );
      else
          setColor( next->color() );
  }
}

void medClutEditorVertex::paint(QPainter *painter,
                                const QStyleOptionGraphicsItem *option,
                                QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    qreal rInner = 5.0;
    qreal rOuter = 10.0;

    setAlpha();
    painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter->setPen( QPen( this->isSelected() ? Qt::lightGray : Qt::black, 2 ));
    painter->setBrush(d->bgColor);
    painter->drawEllipse(-rOuter, -rOuter, 2 * rOuter, 2 * rOuter);

    painter->setPen( Qt::NoPen );
    painter->setBrush(d->fgColor);
    painter->drawEllipse(-rInner, -rInner, 2 * rInner, 2 * rInner);
}

QRectF medClutEditorVertex::boundingRect(void) const
{
    return QRectF(-10.0, -10.0, 20.0, 20.0);
}

QColor medClutEditorVertex::color(void) const
{
    return d->color;
}

void medClutEditorVertex::setColor( QColor color )
{
    color.setAlphaF( d->color.alphaF() );
    if ( d->color != color ) {
        d->color   = color;
        d->bgColor = color;
        d->fgColor = color;
        d->fgColor.setAlphaF( 1.0 );
        this->update();
    }
}

void medClutEditorVertex::forceGeometricalConstraints( const QRectF & limits )
{
    bool forced = false;

    if ( this->x() < limits.left() ) {
        this->setX( limits.left() );
        forced = true;
    }
    else if ( this->x() > limits.right() ) {
        this->setX( limits.right() );
        forced = true;
    }
    if ( this->y() < limits.top() ) {
        this->setY( limits.top() );
        forced = true;
    }
    else if ( this->y() > limits.bottom() ) {
        this->setY( limits.bottom() );
        forced = true;
    }

    if ( forced )
        this->update();
}

void medClutEditorVertex::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    this->QGraphicsItem::mouseMoveEvent( event );

    if ( medClutEditorTable * table =
         dynamic_cast< medClutEditorTable * >( this->parentItem() ) )
        table->forceGeometricalConstraints();
}

void medClutEditorVertex::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);

    if ( medClutEditorTable * table =
         dynamic_cast< medClutEditorTable * >( this->parentItem() ) )
        table->setColorOfSelection( d->color );
}

void medClutEditorVertex::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
        event->ignore();
    else
        this->QGraphicsItem::mousePressEvent(event);

    // if (event->button() == Qt::RightButton) {
    //          this->setSelected( !this->isSelected() );
    //          this->update();
    //          event->accept();
    //     // QMenu menu("Edit Marker", 0) ;
    //     // menu.setWindowOpacity(0.8) ;
    //     // menu.addAction(d->setColorAction);
    //     // menu.addAction(d->deleteAction) ;
    //     // menu.exec(event->screenPos());
    // }

    // this->QGraphicsItem::mousePressEvent(event);
}

void medClutEditorVertex::setAlpha()
{
    qreal alpha = 1.0;
    if ( medClutEditorTable * table =
         dynamic_cast< medClutEditorTable * >( this->parentItem() ) ) {
        alpha = table->coordinateToValue( this->pos() ).y();
        alpha = qMin( alpha, 1.0 );
    }

    d->color.setAlphaF( alpha );
    d->bgColor.setAlphaF( alpha );
}

// /////////////////////////////////////////////////////////////////
// medClutEditorTable
// /////////////////////////////////////////////////////////////////

class medClutEditorTablePrivate
{
 public:
    QSizeF size;
    qreal rangeMin;
    qreal rangeMax;
    QList<medClutEditorVertex *> vertices;
    QString title;
    qreal displayAlpha;
};


medClutEditorTable::medClutEditorTable(const QString & title,  QGraphicsItem *parent)
  : QGraphicsItem(parent)
{
    d = new medClutEditorTablePrivate;
    d->size = QSizeF( 500.0, 300.0 );
    d->title = title;
    d->displayAlpha = 1.0;

    //this->setFlag(QGraphicsItem::ItemIsMovable, true);
    this->setFlag(QGraphicsItem::ItemIsFocusable, true);
    this->setZValue(0);
}

medClutEditorTable::medClutEditorTable(const medClutEditorTable & table)
  : QGraphicsItem( static_cast< QGraphicsItem *>( table.parentItem() ) )
    // : medClutEditorTable (table.title(),table.parent())
{
    d = new medClutEditorTablePrivate;
    d->title = table.title();
    d->size = table.size();

    foreach ( const medClutEditorVertex * vertex, table.vertices())
    {
        d->vertices << new medClutEditorVertex( * vertex);
    }

    //TODO: what about rangeMin and rangeMax?
    // actual values are set by medClutEditor::initializeTable()
    d->rangeMin = 0.0;
    d->rangeMax = 255.0;

    this->setFlag(QGraphicsItem::ItemIsFocusable, true);
    this->setZValue(0);
}

medClutEditorTable::~medClutEditorTable(void)
{
    delete d;
}

const QString & medClutEditorTable::title() const
{
    return d->title;
}

void medClutEditorTable::setTitle(const QString & title)
{
    d->title = title;
}

void medClutEditorTable::setSize( const QSizeF & size )
{
    d->size = size;
}

const QSizeF & medClutEditorTable::size() const
{
    return d->size;
}

void medClutEditorTable::setRange( qreal min, qreal max )
{
    d->rangeMin = min;
    d->rangeMax = max;
}

void medClutEditorTable::forceGeometricalConstraints()
{
    int n = d->vertices.count();
    for ( int i = 0 ; i < n; ++i ) {
        QRectF limits( QPointF( 0.0, 0.0), d->size );
        qreal left  = static_cast< qreal >( i );
        qreal right = limits.right() - static_cast< qreal >( n - i - 1 );

        for ( int j = i - 1; j >= 0; --j )
            if ( d->vertices.at( j )->isSelected() ) {
                qreal l = d->vertices.at( j )->x() + 1.0;
                left = qMin( qMax( left, l ), right );
                break;
            }

        for ( int j = i + 1; j < n; ++j )
            if ( d->vertices.at( j )->isSelected() ) {
                qreal r = d->vertices.at( j )->x() - 1.0;
                right = qMax( left, qMin( r, right ) );
                break;
            }

        limits.setLeft( left );
        limits.setRight( right );

        d->vertices.at( i )->forceGeometricalConstraints( limits );
    }
}


QPointF medClutEditorTable::coordinateToValue( QPointF coord )
{
    qreal valRange  = d->rangeMax - d->rangeMin;

    QPointF value;
    value.setX( d->rangeMin + valRange * ( coord.x() / d->size.width() ) );
    value.setY( ( d->size.height() - coord.y() ) / d->size.height() );

    return value;
}

QPointF medClutEditorTable::valueToCoordinate( QPointF value )
{
    qreal valRange  = d->rangeMax - d->rangeMin;

    QPointF coord;
    if ( valRange > 0.0 ) {
	coord.setX( d->size.width() * ( value.x() - d->rangeMin ) / valRange );
	coord.setY( d->size.height() - d->size.height() * value.y() );
    }

    return coord;
}

void medClutEditorTable::addVertex( medClutEditorVertex *vertex,
                                    bool interpolate )
{
    d->vertices << vertex;

    if ( vertex->parentItem() != this )
        vertex->setParentItem( this );

    qSort( d->vertices.begin(), d->vertices.end(),
           medClutEditorVertex::LessThan );

    if (interpolate) {
        int i    = d->vertices.indexOf( vertex );
        int last = d->vertices.count() - 1;
        medClutEditorVertex * prev = i > 0    ? d->vertices[i-1] : NULL;
        medClutEditorVertex * next = i < last ? d->vertices[i+1] : NULL;
        vertex->interpolate( prev, next );
    }
}

QList< medClutEditorVertex * > medClutEditorTable::vertices()
{
    return d->vertices;
}

QList< const medClutEditorVertex * > medClutEditorTable::vertices() const
{
    QList< const medClutEditorVertex * > const_vertices;
    foreach (medClutEditorVertex * vertex, d->vertices)
	const_vertices << vertex;
    return const_vertices;
}

void medClutEditorTable::setSelectedAllVertices( bool isSelected )
{
    foreach (medClutEditorVertex * vertex, d->vertices)
        vertex->setSelected( isSelected );
}

void medClutEditorTable::deleteSelection()
{
    int nSelected = 0;
    foreach( medClutEditorVertex * vertex, d->vertices )
        if ( vertex->isSelected() )
            ++nSelected;

    if ( d->vertices.count() - nSelected < 2 ) {
        qDebug() << "Need at least two nodes, but only "
                 << ( d->vertices.count() - nSelected ) << " nodes"
                 << " would be left after deletion!  Not deleting any node.";
        return;
    }

    QList<medClutEditorVertex *> remaining;
    while ( !d->vertices.isEmpty() ) {
        medClutEditorVertex * vertex = d->vertices.takeFirst();
        if ( vertex->isSelected() )
            delete vertex;
        else
            remaining.append( vertex );
    }

    d->vertices = remaining;
}

void medClutEditorTable::deleteAllVertices()
{
    foreach( medClutEditorVertex * vertex, d->vertices )
	delete vertex;

    d->vertices.clear();
}

void medClutEditorTable::setColorOfSelection( const QColor & color )
{
    QColor newColor = QColorDialog::getColor( color, 0 );

    if ( newColor.isValid() )
      foreach( medClutEditorVertex * vertex, d->vertices )
        if ( vertex->isSelected() )
          vertex->setColor( newColor );
}

void medClutEditorTable::setColorOfSelection()
{
    QColor color;
    foreach( medClutEditorVertex * vertex, d->vertices )
        if ( vertex->isSelected() &&
             ( !color.isValid() || vertex->color().alpha() > color.alpha() ) )
            color = vertex->color();

    if ( color.isValid() )
        this->setColorOfSelection( color );
}

void medClutEditorTable::onDeleteVertex(medClutEditorVertex *vertex)
{
    if ( d->vertices.count() > 2 ) {
        d->vertices.removeAll( vertex );
        delete vertex;
    }
    else
        qDebug() << "Need at least two nodes, but only "
                 << ( d->vertices.count() - 1 ) << " nodes"
                 << " would be left after deletion!  Not deleting any node.";
}

void medClutEditorTable::setup(float min, float max, int size, int *table)
{
    for (int i = 0 ; i < d->vertices.count()-1; i++) {

        qreal x1 = qMax(static_cast<qreal>(min), d->vertices.at(i+0)->x());
        qreal x2 = qMin(static_cast<qreal>(max), d->vertices.at(i+1)->x());

        qreal r1 = d->vertices.at(i+0)->color().redF();
        qreal r2 = d->vertices.at(i+1)->color().redF();
        qreal g1 = d->vertices.at(i+0)->color().greenF();
        qreal g2 = d->vertices.at(i+1)->color().greenF();
        qreal b1 = d->vertices.at(i+0)->color().blueF();
        qreal b2 = d->vertices.at(i+1)->color().blueF();
        qreal a1 = this->coordinateToValue(d->vertices.at(i+0)->pos()).y();
        qreal a2 = this->coordinateToValue(d->vertices.at(i+1)->pos()).y();

        for (int x = static_cast< int >( x1 );
             x < static_cast< int >( x2 ); ++x ) {

            qreal xf = static_cast< qreal >( x );
            qreal r = ((xf-x1)/(x2-x1))*(r2-r1)+r1;
            qreal g = ((xf-x1)/(x2-x1))*(g2-g1)+g1;
            qreal b = ((xf-x1)/(x2-x1))*(b2-b1)+b1;
            qreal a = ((xf-x1)/(x2-x1))*(a2-a1)+a1;

            table[x-static_cast<int>(min)+(0*size)] = 255 * r;
            table[x-static_cast<int>(min)+(1*size)] = 255 * g;
            table[x-static_cast<int>(min)+(2*size)] = 255 * b;
            table[x-static_cast<int>(min)+(3*size)] = 255 * a;
        }
    }
}

void medClutEditorTable::getTransferFunction( QList<double> &scalars,
					      QList<QColor> &colors )
{
    scalars.clear();
    colors.clear();

    foreach (medClutEditorVertex * vertex, d->vertices) {
        colors  << vertex->color();
        scalars << static_cast< double >( vertex->x() );
    }
}

void medClutEditorTable::setTransferFunction( QList<double> &scalars,
					      QList<QColor> &colors )
{
    int size = qMin( scalars.count(), colors.count() );

    if ( size < 2 ) {
	qDebug() << "medClutEditorTable::setTransferFunction: Transfer function"
		 << " has less than two nodes.  Can't use it.";
	return;
    }

    this->deleteAllVertices();

    for ( int i = 0; i < size; i++ ) {
	QPointF value( scalars.at( i ), colors.at( i ).alphaF() );
	QPointF coord = this->valueToCoordinate( value );

	qDebug() << "value: " << value << ", coord: " << coord;
	d->vertices << new medClutEditorVertex( coord.x(), coord.y(),
						colors.at( i ), this );
    }

    qSort( d->vertices.begin(), d->vertices.end(),
           medClutEditorVertex::LessThan );
}

void medClutEditorTable::paint(QPainter *painter,
                               const QStyleOptionGraphicsItem *option,
                               QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    int n_points = d->vertices.size();
    QPointF *points = new QPointF[n_points + 2];
    // points[0]            = QPoint(0, vertices.first()->position().y());
    // points[n_points + 1] = QPoint(0, vertices.last()->position().y());
    points[0]            = QPointF(d->vertices.first()->x(), d->size.height());
    points[n_points + 1] = QPointF(d->vertices.last()->x(),  d->size.height());
    for (int i = 0 ; i < n_points; i++)
        points[i+1] = d->vertices.at(i)->pos();

    qreal xmin = d->vertices.first()->x();
    qreal xmax = d->vertices.last()->x();

    QLinearGradient linearGradient(xmin, 0.0, xmax, 0.0);
    foreach (medClutEditorVertex *vertex, d->vertices) {
        qreal position;
        position = ( vertex->x() - xmin ) / ( xmax - xmin );
        position = qMax( 0.0, position );
        position = qMin( position, 1.0 );
        QColor color = vertex->color();
        color.setAlphaF( d->displayAlpha * color.alphaF() );
        linearGradient.setColorAt(position, color);
    }

    QPen pen(Qt::lightGray, 2);
    painter->setPen(pen);
    painter->setBrush(linearGradient);
    painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter->drawPolygon(points, n_points + 2);

    delete[] points;
}

QRectF medClutEditorTable::boundingRect(void) const
{
    qreal xmin, xmax, ymin, ymax;
    xmin = d->size.width();
    xmax = 0.0;
    ymin = d->size.height();
    ymax = 0.0;

    foreach (medClutEditorVertex *vertex, d->vertices) {
        xmin = qMin(xmin, vertex->x());
        xmax = qMax(xmax, vertex->x());
        ymin = qMin(ymin, vertex->y());
        ymax = qMax(ymax, vertex->y());
    }

    return QRectF(xmin, ymin, xmax - xmin, qAbs(ymin));
}

void medClutEditorTable::changeDisplayAlpha( qreal step )
{
    d->displayAlpha = qMax( 0.0, qMin( d->displayAlpha + step, 1.0 ) );
    this->update();
}

void medClutEditorTable::resetDisplayAlpha()
{
    d->displayAlpha = 1.0;
    this->update();
}

// void medClutEditorTable::keyPressEvent(QKeyEvent *event)
// {
//     switch (event->key()) {
//     case Qt::Key_F: this->setZValue(this->zValue()+1); break;
//     case Qt::Key_B: this->setZValue(this->zValue()-1); break;
//     case Qt::Key_X: this->scene()->removeItem(this); break;
//     default: break;
//     }
//     qDebug() << event->key();
// }



// /////////////////////////////////////////////////////////////////
// medClutEditorHistogram
// /////////////////////////////////////////////////////////////////


class medClutEditorHistogramPrivate
{
public:
    QMap<qreal, qreal> values;
    QList<QPointF> scaledValues;
    qreal maxLogNum;
    QSizeF histSize;
};


medClutEditorHistogram::medClutEditorHistogram(QGraphicsItem *parent)
  : QGraphicsItem(parent)
  , d (new medClutEditorHistogramPrivate)
{
    d->histSize = QSizeF( 500.0, 300.0 );
    d->maxLogNum =  0.0 ;
    //this->setFlag(QGraphicsItem::ItemIsMovable, false);
    this->setZValue(-1000);
}

medClutEditorHistogram::~medClutEditorHistogram(void)
{
    delete d;
    d = NULL;
}

QRectF medClutEditorHistogram::boundingRect(void) const
{
    return QRectF( QPointF( 0.0, 0.0 ), d->histSize );
}

QSizeF medClutEditorHistogram::size(void) const
{
    return d->histSize;
}

void medClutEditorHistogram::setSize( QSizeF s )
{
    d->histSize = s;
    d->scaledValues.clear();
}

qreal medClutEditorHistogram::getRangeMin(void) const
{
    return d->values.begin().key();
}

qreal medClutEditorHistogram::getRangeMax(void) const
{
    QMap<qreal, qreal>::const_iterator end = d->values.end();
    --end;
    return end.key();
}

void medClutEditorHistogram::addValue(qreal intensity, qreal number)
{
    qreal logNum = 1000.0 * log10f( number + 1.0 );
    d->maxLogNum = qMax( logNum, d->maxLogNum );
    d->values.insert( intensity, logNum );
    d->scaledValues.clear();
}

QPointF medClutEditorHistogram::valueToCoordinate( QPointF value ) const
{
    qreal rangeMin  = this->getRangeMin();
    qreal valRange  = this->getRangeMax() - rangeMin;

    QPointF coord;
    coord.setX( d->histSize.width() * ( ( value.x() - rangeMin ) / valRange ) );
    coord.setY( d->histSize.height() -
                d->histSize.height() * ( value.y() / d->maxLogNum ) );

    return coord;
}

void medClutEditorHistogram::paint(QPainter *painter,
                                   const QStyleOptionGraphicsItem *option,
                                   QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    if ( d->scaledValues.count() != d->values.count() ) {
        qDebug() << "scaling histogram values";

        d->scaledValues.clear();
        for ( QMap<qreal, qreal>::iterator it = d->values.begin(),
                  end = d->values.end(); it != end; ++it )
            d->scaledValues << this->valueToCoordinate( QPointF( it.key(),
								 it.value() ) );
    }

    QPainterPath path;
    path.moveTo( QPointF( d->scaledValues.first().x(), d->histSize.height() ) );
    foreach ( QPointF point, d->scaledValues )
        path.lineTo( point );
    path.lineTo( QPointF( d->scaledValues.last().x(), d->histSize.height() ) );

    painter->setPen(QColor(0x61, 0x61, 0x61));
    painter->setBrush(QColor(0x2a, 0x2a, 0x2a));
    painter->drawPath(path);
}


void medClutEditorHistogram::mouseDoubleClickEvent(
    QGraphicsSceneMouseEvent * event )
{
    medClutEditorScene * scene =
        dynamic_cast< medClutEditorScene * >( this->scene() );
    medClutEditorTable * table = scene != NULL ? scene->table() : NULL;

    if ( table != NULL ) {
        const QPointF & p = event->pos();
        medClutEditorVertex * vertex =
            new medClutEditorVertex( p.x(), p.y(), Qt::yellow, table );
        table->addVertex( vertex, true );
    }
}

// /////////////////////////////////////////////////////////////////
// medClutEditorScene
// /////////////////////////////////////////////////////////////////

medClutEditorScene::medClutEditorScene(QObject *parent) : QGraphicsScene(parent)
{

}

medClutEditorScene::~medClutEditorScene(void)
{

}

medClutEditorTable * medClutEditorScene::table()
{
    foreach ( QGraphicsItem *item, this->items() )
      if ( medClutEditorTable * table =
           dynamic_cast<medClutEditorTable *>( item ) )
          return table;

    return NULL;
}

// /////////////////////////////////////////////////////////////////
// medClutEditorView
// /////////////////////////////////////////////////////////////////

medClutEditorView::medClutEditorView(QWidget *parent)
  : QGraphicsView(parent)
{
    this->setBackgroundBrush(QColor(0x00, 0x00, 0x00));
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setCacheMode(CacheBackground);
    this->setViewportUpdateMode(FullViewportUpdate);
    this->setRenderHints(QPainter::Antialiasing |
                         QPainter::SmoothPixmapTransform |
                         QPainter::TextAntialiasing);

    this->setFrameStyle(QFrame::NoFrame);
    this->setAttribute(Qt::WA_MacShowFocusRect, false);
    this->setDragMode(QGraphicsView::RubberBandDrag);
}

medClutEditorView::~medClutEditorView(void)
{
}

medClutEditorTable * medClutEditorView::table()
{
    medClutEditorScene * scene =
        dynamic_cast< medClutEditorScene * >( this->scene() );
    if ( scene == NULL )
        return NULL;

    return dynamic_cast< medClutEditorTable * >( scene->table() );
}

void medClutEditorView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);

    //qDebug() << this->sceneRect();
    this->fitInView(this->sceneRect(), Qt::KeepAspectRatio);
}

void medClutEditorView::wheelEvent ( QWheelEvent * event )
{
    double degrees = exp( 0.001 * static_cast< double >( event->delta() ) );
    qreal factor = qMax( 0.0, degrees );
    this->scale( factor, factor );
}

void medClutEditorView::keyPressEvent( QKeyEvent * event ) {
    bool withShift =
	static_cast<bool>( event->modifiers() & Qt::ShiftModifier );
    bool withCtrl =
	static_cast<bool>( event->modifiers() & Qt::ControlModifier );

    switch (event->key()) {
    case Qt::Key_Control:
        this->setDragMode(QGraphicsView::ScrollHandDrag); break;

    case Qt::Key_A:
    {
        medClutEditorTable * table = this->table();
        if ( table != NULL )
            table->setSelectedAllVertices( !withShift );
        break;
    }

    case Qt::Key_C:
    {
        medClutEditorTable * table = this->table();
        if ( table != NULL )
            table->setColorOfSelection();
        break;
    }

    case Qt::Key_Delete:
    {
        medClutEditorTable * table = this->table();
        if ( table != NULL )
            table->deleteSelection();
        break;
    }

    case Qt::Key_Return:
    case Qt::Key_Enter:
    {
        medClutEditor * editor = dynamic_cast<medClutEditor *>(this->parent());
        if ( editor != NULL )
            editor->applyTable();
    }

    case Qt::Key_Plus:
    case Qt::Key_Equal:
    {
        medClutEditorTable * table = this->table();
        if ( table != NULL )
	  table->changeDisplayAlpha( withCtrl ? 0.05 : 0.25 );

	break;
    }
    case Qt::Key_Minus:
    case Qt::Key_Underscore:
    {
        medClutEditorTable * table = this->table();
        if ( table != NULL )
	  table->changeDisplayAlpha( withCtrl ? -0.05 : -0.25 );

	break;
    }
    case Qt::Key_0:
    {
        medClutEditorTable * table = this->table();
        if ( table != NULL )
	  table->resetDisplayAlpha();

	break;
    }
    default:
        break;
    }
}

void medClutEditorView::keyReleaseEvent( QKeyEvent * event){
    if (event->key() == Qt::Key_Control)
        this->setDragMode(QGraphicsView::RubberBandDrag);
}

void medClutEditorView::mousePressEvent(QMouseEvent * event )
{
    if (event->button() == Qt::RightButton)
    {
        this->setDragMode(QGraphicsView::NoDrag);
    }
    QGraphicsView::mousePressEvent(event);
}

void medClutEditorView::mouseReleaseEvent(QMouseEvent * event )
{
    if (event->button() == Qt::RightButton)
    {
        this->setDragMode(QGraphicsView::RubberBandDrag);
    }
    QGraphicsView::mouseReleaseEvent(event);
}

// /////////////////////////////////////////////////////////////////
// medClutEditor
// /////////////////////////////////////////////////////////////////

class medClutEditorPrivate
{
public:
    QAction *newAction;
    QAction *deleteAction;
    QAction *colorAction;
    QAction *applyAction;
    QAction *loadTableAction;
    QAction *deleteTableAction;
    QAction *saveTableAction;
    medClutEditorScene *scene;
    medClutEditorView  *view;
    medClutEditorHistogram *histogram;

    dtkAbstractData *dtk_data;
    medAbstractView *med_view;
    QList <medClutEditorTable *> * tables;
};

medClutEditor::medClutEditor(QWidget *parent) : QWidget(parent)
{
    d = new medClutEditorPrivate;
    d->scene = new medClutEditorScene;
    d->view  = new medClutEditorView( this );
    d->view->setScene(d->scene);

    d->newAction         = new QAction("New table", this);
    d->loadTableAction   = new QAction("Load table", this);
    d->saveTableAction   = new QAction("SaveTable",this);
    d->deleteTableAction = new QAction("Delete table", this);
    d->colorAction       = new QAction("Edit color", this);
    d->deleteAction      = new QAction("Delete selection", this);
    d->applyAction       = new QAction("Apply", this);

    connect(d->newAction,    SIGNAL(triggered()),
            this,            SLOT(onNewTableAction()));
    connect(d->loadTableAction,    SIGNAL(triggered()),
            this,            SLOT(onLoadTableAction()));
    connect(d->saveTableAction,    SIGNAL(triggered()),
            this,            SLOT(onSaveTableAction()));
    connect(d->deleteTableAction, SIGNAL(triggered()),
            this,            SLOT(onDeleteTableAction()));
    connect(d->colorAction,  SIGNAL(triggered()),
            this,            SLOT(onColorAction()));
    connect(d->deleteAction, SIGNAL(triggered()),
            this,            SLOT(onDeleteAction()));
    connect(d->applyAction,  SIGNAL(triggered()),
            this,            SLOT(onApplyTablesAction()));

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(d->view);
    this->setLayout(layout);

    //get saved LUTs.
    d->tables = new QList<medClutEditorTable *>();
    medXMLToLUTReader reader = medXMLToLUTReader(d->tables);
    QString lutFileName = medStorage::dataLocation();
    lutFileName.append("/LUTs.xml");
    QFile file(lutFileName);
    if (file.open(QIODevice::ReadOnly))
    {
        reader.read(&file);
        file.close();
    }

}

medClutEditor::~medClutEditor(void)
{
    delete d->newAction;
    delete d->loadTableAction;
    delete d->saveTableAction;
    delete d->deleteTableAction;
    delete d->colorAction;
    delete d->deleteAction;
    delete d->applyAction;

    delete d->scene;
    delete d->view;
    delete d->tables; //TODO: delete all the tables as well.
    delete d;
}

void medClutEditor::setData(dtkAbstractData *data)
{
    if (dtkAbstractDataImage *image =
        dynamic_cast<dtkAbstractDataImage *>(data)) {

        d->histogram = new medClutEditorHistogram();
        int min_range = image->minRangeValue();
        int max_range = image->maxRangeValue();

        for ( int i = min_range; i <= max_range; ++i ) {
            qreal f = static_cast< qreal >(
                image->scalarValueCount( i - min_range ) );
            d->histogram->addValue( static_cast< qreal >( i ), f );
        }

        d->scene->addItem(d->histogram);
        this->initializeTable();

        d->dtk_data = image;
    }
}

void medClutEditor::setView(medAbstractView *view)
{
    d->med_view = view;

    if ( d->med_view != NULL ) {
	QList<double> scalars;
	QList<QColor> colors;
    
	d->med_view->getTransferFunctions( scalars, colors );
	medClutEditorTable * table = d->scene->table();
	table->setTransferFunction( scalars, colors );
    }
}

void medClutEditor::initializeTable(void)
{
    this->deleteTable();

    medClutEditorTable *lut = new medClutEditorTable("Unknown");
    lut->setSize( d->histogram->size() );
    lut->setRange( d->histogram->getRangeMin(), d->histogram->getRangeMax() );
    d->scene->addItem( lut );

    QRectF area = d->histogram->boundingRect();
    // int rangemin = d->histogram->getRangeMin();
    // int rangemax = d->histogram->getRangeMax();
    medClutEditorVertex * v1 =
        new medClutEditorVertex( area.left(), 0.8 * area.height(),
                                 Qt::blue, lut );
    medClutEditorVertex * v2 =
        new medClutEditorVertex( area.center().x(), area.center().y(),
                                 Qt::red, lut );
    medClutEditorVertex * v3 =
        new medClutEditorVertex( area.right(), 0.4 * area.height(),
                                 Qt::yellow, lut );
    // medClutEditorVertex *v4 = new medClutEditorVertex(260,  60, Qt::white);

    lut->addVertex(v1);
    lut->addVertex(v2);
    lut->addVertex(v3);
    // lut->addVertex(v4);
}

void medClutEditor::deleteTable(void)
{
    medClutEditorTable * table;
    while ( table = d->scene->table() ) {
        d->scene->removeItem( table );
        delete table;
    }
}

void medClutEditor::applyTable(void)
{
    // if (dtkAbstractDataImage *image =
    //     dynamic_cast<dtkAbstractDataImage *>(d->dtk_data)) {
    if ( d->med_view != NULL ) {

        QList<double> scalars;
        QList<QColor> colors;
	medClutEditorTable * table = d->scene->table();
	table->getTransferFunction(scalars, colors);
	// d->med_view->setColorLookupTable(scalars, colors);
	d->med_view->setTransferFunctions(scalars, colors);
    }
}


void medClutEditor::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        event->accept();

        QMenu menu("Color Lookup Table", this) ;
        menu.setWindowOpacity(0.8) ;
        menu.addAction(d->newAction);
        menu.addAction(d->loadTableAction);
        menu.addAction(d->saveTableAction);
        menu.addAction(d->deleteTableAction);
        menu.addSeparator();
        menu.addAction(d->colorAction);
        menu.addAction(d->deleteAction);
        menu.addSeparator();
        menu.addAction(d->applyAction);
        menu.exec(mapFrom(this, QCursor::pos()));
        //hack to get the rubber band back after right clicking on the view
        d->view->setDragMode(QGraphicsView::RubberBandDrag);
    }
}

void medClutEditor::onNewTableAction(void)
{
    this->initializeTable();
}

void medClutEditor::onColorAction(void)
{
    if ( medClutEditorTable * table = d->scene->table() )
        table->setColorOfSelection();
}

void medClutEditor::onDeleteAction(void)
{
    if ( medClutEditorTable * table = d->scene->table() )
        table->deleteSelection();
}

void medClutEditor::onApplyTablesAction(void)
{
    applyTable();
}

void medClutEditor::onLoadTableAction(void)
{
    QStringList titles;
    foreach (medClutEditorTable * table,*d->tables )
    {
        titles << table->title();
    }

    medLoadLUTDialog dialog (titles,this);
    // Show it and wait for Ok or Cancel
    if( dialog.exec() == QDialog::Accepted )
    {
        // We got Ok
       foreach (medClutEditorTable * table,*d->tables )
        {
           if ( table->title() == dialog.textValue())
           {
                //display this table
               d->scene->removeItem(d->scene->table());
               d->scene->addItem(table);
               d->scene->update();
           }
        }
    }

}

void medClutEditor::onSaveTableAction(void)
{

    if ( medClutEditorTable * table = d->scene->table() )
    {
        medSaveLUTDialog dialog (table->title(),this);
        // Show it and wait for Ok or Cancel
        if( dialog.exec() == QDialog::Accepted )
        {
            // We got Ok
            medClutEditorTable * tableToSave;
            if (table->title().compare(dialog.textValue()))
            {
                //copy the table
                tableToSave = new medClutEditorTable( * table );
                tableToSave->setTitle(dialog.textValue());
            }
            else
            {
                tableToSave = table;
            }

            QList<medClutEditorTable*>  l = *d->tables;
            l << tableToSave;
            medLUTToXMLWriter writer (l);
            //create file object in an existing storage place
            medStorage::mkpath(medStorage::dataLocation());
            QString fileName = medStorage::dataLocation();
            fileName.append("/LUTs.xml");
            QFile * file = new QFile();
            file->setFileName(fileName);
            if (file->open(QIODevice::WriteOnly))
            {
                writer.writeFile(file);
                file->close();
            }
            else
            {
                qDebug() << "can'open file " << fileName;
            }
            delete file;
        }
    }
}

void medClutEditor::onDeleteTableAction(void)
{

}
