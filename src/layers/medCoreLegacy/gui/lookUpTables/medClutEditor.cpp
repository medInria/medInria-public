/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractData.h>
#include <medAbstractImageData.h>
#include <dtkCoreSupport/dtkAbstractView.h>
#include <medAbstractImageView.h>
#include <medStorage.h>
#include <medClutEditor.h>

#include <math.h>
#include <limits>

#include <medLUTToXMLWriter.h>
#include <medXMLToLUTReader.h>
#include <medSaveLUTDialog.h>
#include <medLoadLUTDialog.h>

// /////////////////////////////////////////////////////////////////
// medClutEditorVertex
// /////////////////////////////////////////////////////////////////

class medClutEditorVertexPrivate
{
public:
    QPointF value;
    QColor color;

    QColor fgColor;
    QColor bgColor;

    qreal innerRadius;
    qreal outerRadius;

    bool isMoved;
    QPointF lastPos;
};

medClutEditorVertex::medClutEditorVertex(QPointF value, QPointF coord,
                                         QColor color, QGraphicsItem * parent)
: QObject(), QGraphicsItem(parent)
{
    d = new medClutEditorVertexPrivate;
    d->value = value;
    this->setPos( coord );
    // if ( medClutEditorScene * scene =
    //   dynamic_cast< medClutEditorScene * >( this->scene() ) )
    //  d->value = scene->coordinateToValue( this->pos() );
    this->setZValue(1);
    d->innerRadius =  5.0;
    d->outerRadius = 10.0;

    setAlpha();
    setColor( color );

    d->isMoved = false;

    this->setFlag(QGraphicsItem::ItemIsMovable, true);
    this->setFlag(QGraphicsItem::ItemIsSelectable, true);
}

medClutEditorVertex::medClutEditorVertex( const medClutEditorVertex & other,
                                         QGraphicsItem * parent)
: QObject(), QGraphicsItem( parent )
{
    if ( parent == NULL )
        this->setParentItem( other.parentItem() );

    d = new medClutEditorVertexPrivate;
    d->value = other.d->value;
    d->color   = other.d->color;
    d->fgColor = other.d->fgColor;
    d->bgColor = other.d->bgColor;
    d->innerRadius = other.d->innerRadius;
    d->outerRadius = other.d->outerRadius;

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

const QPointF & medClutEditorVertex::value() const
{
    return d->value;
}

void medClutEditorVertex::shiftValue( qreal amount, bool forceConstraints )
{
    if ( amount != 0.0 ) {
        medClutEditorTable * table =
            dynamic_cast< medClutEditorTable * >( this->parentItem() );
        forceConstraints = forceConstraints && table;

        medClutEditorScene * scene =
            dynamic_cast< medClutEditorScene * >( this->scene() );
        QPointF value = d->value;
        value.setX( value.x() + amount );

        if ( forceConstraints )
            table->initiateMoveSelection();
        this->setPos( scene->valueToCoordinate( value ) );
        if ( forceConstraints ) {
            table->constrainMoveSelection( this );
            table->finalizeMoveSelection();
        }

        this->updateValue();
    }
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

    setAlpha();
    painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter->setPen( this->isSelected() ?
                     QPen( Qt::lightGray, 3 ) : QPen( Qt::darkGray, 2 ) );
    painter->setBrush(d->bgColor);
    painter->drawEllipse( -d->outerRadius,    -d->outerRadius,
                          2 * d->outerRadius, 2 * d->outerRadius );

    painter->setPen( Qt::NoPen );
    painter->setBrush(d->fgColor);
    painter->drawEllipse( -d->innerRadius,    -d->innerRadius,
                          2 * d->innerRadius, 2 * d->innerRadius );
}

QRectF medClutEditorVertex::boundingRect(void) const
{
    return QRectF( QPointF( -d->outerRadius, -d->outerRadius ),
                   QPointF(  d->outerRadius,  d->outerRadius ) );
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

void medClutEditorVertex::setAlpha()
{
    qreal alpha = qMax( 0.0, qMin( d->value.y(), 1.0 ) );

    d->color.setAlphaF( alpha );
    d->bgColor.setAlphaF( alpha );
}

void medClutEditorVertex::updateCoordinates()
{
    if ( medClutEditorScene * scene =
         dynamic_cast< medClutEditorScene * >( this->scene() ) ) {
        this->setPos( scene->valueToCoordinate( d->value ) );
        this->update();
    }
}

void medClutEditorVertex::updateValue()
{
    if ( medClutEditorScene * scene =
         dynamic_cast< medClutEditorScene * >( this->scene() ) ) {
        d->value = scene->coordinateToValue( this->pos() );
        this->update();
    }

    this->setAlpha();
}

void medClutEditorVertex::initiateMove()
{
    d->isMoved = true;
    d->lastPos = this->pos();
}

void medClutEditorVertex::finalizeMove()
{
    d->isMoved = false;
}

void medClutEditorVertex::forceGeometricalConstraints( const QRectF & limits,
                                                       bool inManhattan )
{
    bool forced = false;

    if ( inManhattan ) {
        QPointF move = this->pos() - d->lastPos;
        if ( std::abs( move.x() ) > std::abs( move.y() ) ) {
            this->setY( d->lastPos.y() );
            forced = true;
        }
        if ( std::abs( move.x() ) < std::abs( move.y() ) ) {
            this->setX( d->lastPos.x() );
            forced = true;
        }
    }

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

    bool moved = forced || ( this->pos() - d->lastPos ) != QPointF( 0.0, 0.0 );
    if ( moved ) {
        this->updateValue();
        this->update();
    }
}

void medClutEditorVertex::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    this->QGraphicsItem::mouseMoveEvent( event );

    bool withShift =
        static_cast<bool>( event->modifiers() & Qt::ShiftModifier );

    if ( medClutEditorTable * table =
         dynamic_cast< medClutEditorTable * >( this->parentItem() ) ) {
        table->constrainMoveSelection( this, withShift );
        table->triggerVertexChanged();
    }
    // this->updateValue();
    // qDebug() << "[" << (long int) this << "] value: " << d->value;
    // qDebug() << "[" << (long int) this << "] coord: " << this->pos();
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
    else {
        this->QGraphicsItem::mousePressEvent(event);
        if ( medClutEditorTable * table =
             dynamic_cast< medClutEditorTable * >( this->parentItem() ) )
            table->initiateMoveSelection();
    }


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

void medClutEditorVertex::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    this->QGraphicsItem::mouseReleaseEvent(event);
    if ( medClutEditorTable * table =
         dynamic_cast< medClutEditorTable * >( this->parentItem() ) )
        table->finalizeMoveSelection();
}

// /////////////////////////////////////////////////////////////////
// medClutEditorTable
// /////////////////////////////////////////////////////////////////

class medClutEditorTablePrivate
{
public:
    QList<medClutEditorVertex *> vertices;
    QString title;
    qreal displayAlpha;
};


medClutEditorTable::medClutEditorTable(const QString & title,
                                       QGraphicsItem *parent)
  :QObject(), QGraphicsItem(parent)
{
    d = new medClutEditorTablePrivate;
    d->title = title;
    d->displayAlpha = 1.0;

    //this->setFlag(QGraphicsItem::ItemIsMovable, true);
    this->setFlag(QGraphicsItem::ItemIsFocusable, true);
    this->setZValue(0);
}

medClutEditorTable::medClutEditorTable(const medClutEditorTable & table)
  :QObject(),  QGraphicsItem( static_cast< QGraphicsItem *>( table.parentItem() ) )
// : medClutEditorTable (table.title(),table.parent())
{
    d = new medClutEditorTablePrivate;
    d->title = table.title();

    foreach ( const medClutEditorVertex * vertex, table.vertices())
    d->vertices << new medClutEditorVertex( * vertex );

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

QRectF medClutEditorTable::boundingRect(void) const
{
    if ( d->vertices.count() == 0 )
        return QRectF( 0.0, 0.0, 0.0, 0.0 );

    QRectF box( this->mapRectFromItem( d->vertices.first(),
                                       d->vertices.first()->boundingRect() ) );

    foreach ( medClutEditorVertex * vertex, d->vertices ) {
        box = box.united( this->mapRectFromItem( vertex,
                                                 vertex->boundingRect() ) );
    }

    return box;
}


void medClutEditorTable::range( qreal & min, qreal & max ) const
{
    if ( d->vertices.count() == 0 ) {
        min = max = 0.0;
        return;
    }

    min = d->vertices.first()->value().x();
    max = d->vertices.last()->value().x();
}

void medClutEditorTable::initiateMoveSelection()
{
    foreach (medClutEditorVertex * vertex, d->vertices)
        if ( vertex->isSelected() )
            vertex->initiateMove();
}

void medClutEditorTable::constrainMoveSelection( medClutEditorVertex * driver,
                                                 bool inManhattan )
{
    medClutEditorScene * scene =
        dynamic_cast< medClutEditorScene * >( this->scene() );
    QRectF box = scene->plotArea();

    foreach (medClutEditorVertex * vertex, d->vertices) {
        QRectF limits = box;
        if ( vertex->isSelected() ) {
            if ( vertex != driver ) {
                limits.setLeft(  vertex->x() - 1.0 );
                limits.setRight( vertex->x() + 1.0 );
            }
            vertex->forceGeometricalConstraints( limits, inManhattan );
            // vertex->updateValue();
        }
    }

    int n = d->vertices.count();
    for ( int i = 0 ; i < n; ++i ) {
        medClutEditorVertex * vertex = d->vertices.at( i );

        if ( vertex->isSelected() )
            continue;

        qreal left  = vertex->x() - 1.0;
        qreal right = vertex->x() + 1.0;

        for ( int j = i - 1; j >= 0; --j )
            if ( d->vertices.at( j )->isSelected() ) {
                left = d->vertices.at( j )->x() + static_cast<qreal>( i - j );
                break;
            }

        for ( int j = i + 1; j < n; ++j )
            if ( d->vertices.at( j )->isSelected() ) {
                right = d->vertices.at( j )->x() + static_cast<qreal>( i - j );
                break;
            }

        QRectF limits = box;
        limits.setLeft( left );
        limits.setRight( right );

        vertex->forceGeometricalConstraints( limits );
    }
    // emit vertexChanged();
}

void medClutEditorTable::finalizeMoveSelection()
{
    foreach (medClutEditorVertex * vertex, d->vertices)
        if ( vertex->isSelected() )
            vertex->finalizeMove();
            // emit vertexChanged();
}

void medClutEditorTable::updateCoordinates()
{
    this->prepareGeometryChange();
    foreach (medClutEditorVertex * vertex, d->vertices)
        vertex->updateCoordinates();
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

    emit vertexAdded();
}

QList< medClutEditorVertex * > & medClutEditorTable::vertices()
{
    return d->vertices;
}

const QList< medClutEditorVertex * > & medClutEditorTable::vertices() const
{
    return d->vertices;
}

// QList< const medClutEditorVertex * > medClutEditorTable::vertices() const
// {
//     QList< const medClutEditorVertex * > const_vertices;
//     foreach (medClutEditorVertex * vertex, d->vertices)
//      const_vertices << vertex;
//     return const_vertices;
// }

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

    emit vertexRemoved();
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
            if ( vertex->isSelected() ) {
                vertex->setColor( newColor );
                emit vertexChanged();
            }
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

// void medClutEditorTable::onDeleteVertex(medClutEditorVertex *vertex)
// {
//     if ( d->vertices.count() > 2 ) {
//         d->vertices.removeAll( vertex );
//         delete vertex;
//         emit vertexRemoved();
//     }
//     else
//         qDebug() << "Need at least two nodes, but only "
//                  << ( d->vertices.count() - 1 ) << " nodes"
//                  << " would be left after deletion!  Not deleting any node.";
// }

void medClutEditorTable::scaleWindowWidth( qreal factor )
{
    qreal min, max;
    this->range( min, max );
    qreal center = 0.5 * ( min + max );
    // factor *= 1e-4 * ( max - min );

    foreach (medClutEditorVertex * vertex, d->vertices) {
        qreal offset = vertex->value().x() - center;
        vertex->shiftValue( offset * ( factor - 1.0 ), false );
    }

    emit vertexChanged();
}

void medClutEditorTable::shiftWindowCenter( qreal amount )
{
    qreal min, max;
    this->range( min, max );
    qreal factor = 5e-4 * ( max - min );

    foreach (medClutEditorVertex * vertex, d->vertices)
        vertex->shiftValue( factor * amount, false );

    emit vertexChanged();
}

void medClutEditorTable::setup(float min, float max, int size, int *table)
{
    for (int i = 0 ; i < d->vertices.count()-1; i++) {

        QPointF value1 = d->vertices.at(i+0)->value();
        QPointF value2 = d->vertices.at(i+1)->value();
        qreal x1 = qMax(static_cast<qreal>(min), value1.x());
        qreal x2 = qMin(static_cast<qreal>(max), value2.x());

        qreal r1 = d->vertices.at(i+0)->color().redF();
        qreal r2 = d->vertices.at(i+1)->color().redF();
        qreal g1 = d->vertices.at(i+0)->color().greenF();
        qreal g2 = d->vertices.at(i+1)->color().greenF();
        qreal b1 = d->vertices.at(i+0)->color().blueF();
        qreal b2 = d->vertices.at(i+1)->color().blueF();
        qreal a1 = value1.y();
        qreal a2 = value2.y();
        // qreal a1 = this->coordinateToValue(d->vertices.at(i+0)->pos()).y();
        // qreal a2 = this->coordinateToValue(d->vertices.at(i+1)->pos()).y();

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
        QPointF value = vertex->value();
        scalars << static_cast< double >( value.x() );
    }
}

void medClutEditorTable::setTransferFunction( QList<double> &scalars,
                                              QList<QColor> &colors )
{
    medClutEditorScene * scene =
        dynamic_cast< medClutEditorScene * >( this->scene() );
    if ( scene == NULL )
        return;

    int size = qMin( scalars.count(), colors.count() );

    if ( size < 2 ) {
        qDebug() << "medClutEditorTable::setTransferFunction: Transfer function"
                 << " has less than two nodes.  Can't use it.";
        return;
    }

    this->deleteAllVertices();

    for ( int i = 0; i < size; i++ ) {
        QPointF value( scalars.at( i ), colors.at( i ).alphaF() );
        QPointF coord = scene->valueToCoordinate( value );

        // qDebug() << "value: " << value << ", coord: " << coord;
        d->vertices <<
            new medClutEditorVertex( value, coord, colors.at( i ), this );
    }

    qSort( d->vertices.begin(), d->vertices.end(),
           medClutEditorVertex::LessThan );

    scene->adjustRange();
}

void medClutEditorTable::simplifyTransferFunction()
{
    qreal threshold = 1.0;
    bool foundThreshold = false;

    d->vertices.first()->setSelected( false );
    d->vertices.last()->setSelected( false );

    for ( int i = 1, n = d->vertices.count() - 1; i < n; ++i ) {
        medClutEditorVertex * vertex = d->vertices.at( i );
        if ( vertex->isSelected() ) {
            qreal xp = d->vertices.at( i - 1 )->x();
            qreal xn = d->vertices.at( i + 1 )->x();
            qreal wn = ( vertex->x() - xp ) / ( xn - xp );
            qreal wp = 1.0 - wn;

            QColor prev = d->vertices.at( i - 1 )->color();
            QColor next = d->vertices.at( i + 1 )->color();

            QColor linear;
            linear.setRgbF( ( wp * prev.redF()   + wn * next.redF() ),
                            ( wp * prev.greenF() + wn * next.greenF() ),
                            ( wp * prev.blueF()  + wn * next.blueF() ),
                            ( wp * prev.alphaF() + wn * next.alphaF() ) );
            QColor c = vertex->color();
            qreal  r = linear.redF()   - c.redF();
            qreal  g = linear.greenF() - c.greenF();
            qreal  b = linear.blueF()  - c.blueF();
            qreal  a = linear.alphaF() - c.alphaF();
            qreal offset = sqrt( r * r + g * g + b * b + a * a );
            if ( !foundThreshold || offset > threshold ) {
                threshold = offset;
                foundThreshold = true;
            }
        }
    }

    if ( !foundThreshold ) {
        qDebug() << "Select at least one vertex (exept the first or last one)"
                 << " which is too similar to its neighbors.";
        return;
    }

    QList<medClutEditorVertex *> vertices( d->vertices );
    bool modified = true;
    while ( modified ) {
        modified = false;

        QList<medClutEditorVertex *> remaining;
        remaining.append( vertices.first() );
        for ( int i = 1, n = vertices.count() - 1; i < n; ++i ) {
            medClutEditorVertex * vertex = vertices.at( i );

            qreal xp = vertices.at( i - 1 )->x();
            qreal xn = vertices.at( i + 1 )->x();
            qreal wn = ( vertex->x() - xp ) / ( xn - xp );
            qreal wp = 1.0 - wn;

            QColor prev = vertices.at( i - 1 )->color();
            QColor next = vertices.at( i + 1 )->color();

            QColor linear;
            linear.setRgbF( ( wp * prev.redF()   + wn * next.redF() ),
                            ( wp * prev.greenF() + wn * next.greenF() ),
                            ( wp * prev.blueF()  + wn * next.blueF() ),
                            ( wp * prev.alphaF() + wn * next.alphaF() ) );
            QColor c = vertex->color();
            qreal  r = linear.redF()   - c.redF();
            qreal  g = linear.greenF() - c.greenF();
            qreal  b = linear.blueF()  - c.blueF();
            qreal  a = linear.alphaF() - c.alphaF();
            qreal offset = sqrt( r * r + g * g + b * b + a * a );

            if ( offset > threshold ) {
                remaining.append( vertex );
                vertex->setSelected( false );
            }
            else {
                vertex->setSelected( true );
                modified = true;
                vertex = vertices.at( ++i );
                vertex->setSelected( false );
                if ( i < n )
                    remaining.append( vertex );
            }
        }
        remaining.append( vertices.last() );
        vertices = remaining;
    }
    this->update();
}

void medClutEditorTable::paint(QPainter *painter,
                               const QStyleOptionGraphicsItem *option,
                               QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    medClutEditorScene * scene =
        dynamic_cast< medClutEditorScene * >( this->scene() );
    if ( scene == NULL )
        return;

    // QPointF origin = scene->valueToCoordinate( QPointF( 0.0, 0.0 ) );
    QRectF area = scene->plotArea();

    int n_points = d->vertices.size();
    QPointF *points = new QPointF[n_points + 2];
    points[0]            = QPointF( d->vertices.first()->x(), area.bottom() );
    points[n_points + 1] = QPointF( d->vertices.last()->x(),  area.bottom() );
    for ( int i = 0 ; i < n_points; i++ )
        points[i+1] = d->vertices.at(i)->pos();

    qreal xmin = d->vertices.first()->x();
    qreal xmax = d->vertices.last()->x();

    QLinearGradient linearGradient(xmin, 0.0, xmax, 0.0);
    foreach ( medClutEditorVertex * vertex, d->vertices ) {
        qreal position;
        position = ( vertex->x() - xmin ) / ( xmax - xmin );
        position = qMax( 0.0, position );
        position = qMin( position, 1.0 );
        QColor color = vertex->color();
        color.setAlphaF( d->displayAlpha * color.alphaF() );
        linearGradient.setColorAt(position, color);
    }

    QPen pen( Qt::darkGray, 1 );
    painter->setPen(pen);
    painter->setBrush(linearGradient);
    painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter->drawPolygon(points, n_points + 2);

    painter->setPen( QPen( Qt::gray, 0 ) );
    foreach ( medClutEditorVertex * vertex, d->vertices ) {
        if ( vertex->isSelected() ) {
            const QPointF & p = vertex->pos();
            painter->drawLine( p, QPointF( p.x(), area.bottom() ) );
            painter->drawLine( p, QPointF( area.left(), p.y() ) );

            QString annotation;
            annotation.setNum( vertex->color().alphaF(), 'g', 2 );
            painter->drawText( QPointF( area.left() - 40.0, p.y() + 5.0 ),
                               annotation );
            annotation.setNum( vertex->value().x(), 'f', 2 );
            painter->drawText( QPointF( p.x() - 20.0, area.bottom() + 20.0 ),
                               annotation );
        }
    }

    delete[] points;
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

void medClutEditorTable::triggerVertexChanged()
{
    emit vertexChanged();
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
    // QSizeF histSize;
};


medClutEditorHistogram::medClutEditorHistogram(QGraphicsItem *parent)
  : QGraphicsItem(parent)
  , d (new medClutEditorHistogramPrivate)
{
    // d->histSize = QSizeF( 500.0, 300.0 );
    d->maxLogNum = 0.0;
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
    medClutEditorScene * scene =
    dynamic_cast< medClutEditorScene * >( this->scene() );
    if ( scene ) {
        QPointF upperLeft =
        scene->valueToCoordinate( QPoint( this->getRangeMin(), 1.0 ) );
        QPointF lowerRight =
        scene->valueToCoordinate( QPoint( this->getRangeMax(), 0.0 ) );
        return QRectF( upperLeft, lowerRight );
    }

    return QRectF( 0.0, 0.0, 255.0, 255.0 );
}

void medClutEditorHistogram::range( qreal & min, qreal & max ) const
{
    min = this->getRangeMin();
    max = this->getRangeMax();
}

qreal medClutEditorHistogram::getRangeMin(void) const
{
    return d->values.size() > 0 ? d->values.begin().key() : 0.0;
}

qreal medClutEditorHistogram::getRangeMax(void) const
{
    qreal max = 0.0;
    if ( d->values.size() > 0 ) {
        QMap<qreal, qreal>::const_iterator end = d->values.end();
        --end;
        max = end.key();
    }
    return max;
}

void medClutEditorHistogram::updateCoordinates()
{
    this->prepareGeometryChange();
    d->scaledValues.clear();
}

const QMap< qreal, qreal > & medClutEditorHistogram::values()
{
    return d->values;
}

void medClutEditorHistogram::setValues(const QMap<qreal, qreal> & bins)
{
    d->values.clear();
    typedef QMap<qreal, qreal>::const_iterator iterator;
    for ( iterator it = bins.begin(), end = bins.end(); it != end; ++it ) {
        qreal logNum = 1000.0 * log10f( it.value() + 1.0 );
        d->maxLogNum = qMax( logNum, d->maxLogNum );
        d->values.insert( it.key(), logNum );
    }
    d->scaledValues.clear();

    medClutEditorScene * scene =
    dynamic_cast< medClutEditorScene * >( this->scene() );
    if ( scene != NULL )
        scene->adjustRange();
}

void medClutEditorHistogram::addValue(qreal intensity, qreal number)
{
    qreal logNum = 1000.0 * log10f( number + 1.0 );
    d->maxLogNum = qMax( logNum, d->maxLogNum );
    d->values.insert( intensity, logNum );
    d->scaledValues.clear();

    medClutEditorScene * scene =
        dynamic_cast< medClutEditorScene * >( this->scene() );
    if ( scene != NULL )
        scene->adjustRange();
}

void medClutEditorHistogram::paint(QPainter *painter,
                                   const QStyleOptionGraphicsItem *option,
                                   QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    medClutEditorScene * scene =
        dynamic_cast< medClutEditorScene * >( this->scene() );

    if ( scene            == NULL ||
         d->maxLogNum     == 0.0  ||
         d->values.size() == 0    )
        return;

    if ( d->scaledValues.count() != d->values.count() + 2 ) {
        d->scaledValues.clear();
        QMap<qreal, qreal>::iterator it = d->values.begin();
        d->scaledValues << scene->valueToCoordinate( QPointF( it.key(), 0.0 ) );
        for ( QMap<qreal, qreal>::iterator end = d->values.end();
              it != end; ++it )
            d->scaledValues << scene->valueToCoordinate(
                QPointF( it.key(), it.value() / d->maxLogNum ) );
        d->scaledValues <<
            scene->valueToCoordinate( QPointF( ( --it ).key(), 0.0 ) );
    }

    QPainterPath path;
    path.moveTo( d->scaledValues.first() );
    foreach ( QPointF point, d->scaledValues )
        path.lineTo( point );

    painter->setPen( QPen( QColor(0x61, 0x61, 0x61) ) );
    painter->setBrush( QColor(0x2a, 0x2a, 0x2a) );
    painter->drawPath( path );
}


void medClutEditorHistogram::mouseDoubleClickEvent(
    QGraphicsSceneMouseEvent * event )
{
    medClutEditorScene * scene =
        dynamic_cast< medClutEditorScene * >( this->scene() );
    medClutEditorTable * table = scene != NULL ? scene->table() : NULL;

    if ( table != NULL ) {
        const QPointF & p = event->pos();
        QPointF value = scene->coordinateToValue( p );
        medClutEditorVertex * vertex =
            new medClutEditorVertex( value, p, Qt::yellow, table );
        table->addVertex( vertex, true );
    }
}

// /////////////////////////////////////////////////////////////////
// medClutEditorScene
// /////////////////////////////////////////////////////////////////

class medClutEditorScenePrivate
{
public:
    QSizeF size;
    qreal leftMargin, rightMargin, topMargin, bottomMargin;
    qreal rangeMin, rangeMax;
};

medClutEditorScene::medClutEditorScene( QObject * parent )
  : QGraphicsScene( parent )
  , d( new medClutEditorScenePrivate )
{
    d = new medClutEditorScenePrivate;
    d->size = QSizeF( 500.0, 300.0 );
    d->leftMargin   = 50.0;
    d->rightMargin  = 10.0;
    d->topMargin    = 10.0;
    d->bottomMargin = 30.0;
    d->rangeMin = 0.0;
    d->rangeMax = d->size.width();
    this->setSceneRect( 0.0, 0.0, d->size.width(), d->size.height() );
}

medClutEditorScene::~medClutEditorScene(void)
{
    delete d;
}

medClutEditorView * medClutEditorScene::view()
{
    foreach ( QGraphicsView * view, this->views() )
        if ( medClutEditorView * v =
             dynamic_cast<medClutEditorView *>( view ) )
            return v;

    return NULL;
}

medClutEditorTable * medClutEditorScene::table()
{
    foreach ( QGraphicsItem *item, this->items() )
        if ( medClutEditorTable * table =
             dynamic_cast<medClutEditorTable *>( item ) )
            return table;

    return NULL;
}

medClutEditorHistogram * medClutEditorScene::histogram()
{
    foreach ( QGraphicsItem *item, this->items() )
        if ( medClutEditorHistogram * histogram =
             dynamic_cast<medClutEditorHistogram *>( item ) )
            return histogram;

    return NULL;
}

QRectF medClutEditorScene::plotArea()
{
    return this->sceneRect().adjusted( d->leftMargin,   d->topMargin,
                                       -d->rightMargin, -d->bottomMargin );
}

QPointF medClutEditorScene::coordinateToValue( QPointF coord )
{
    qreal  valRange = d->rangeMax - d->rangeMin;
    QRectF area     = this->plotArea();

    QPointF value;
    value.setX( d->rangeMin +
                valRange * ( ( coord.x() - d->leftMargin ) / area.width() ) );
    value.setY( ( area.height() - ( coord.y() - d->topMargin ) ) /
                area.height() );

    return value;
}

QPointF medClutEditorScene::valueToCoordinate( QPointF value )
{
    qreal  valRange = d->rangeMax - d->rangeMin;
    QRectF area     = this->plotArea();

    QPointF coord;
    if ( valRange > 0.0 ) {
        coord.setX( d->leftMargin +
                    area.width() * ( value.x() - d->rangeMin ) / valRange );
        coord.setY( d->topMargin + area.height() - area.height() * value.y() );
    }

    // qDebug() << "range: (" << d->rangeMin << ", " << d->rangeMax << ")";
    // qDebug() << "value: (" << value.x()   << ", " << value.y()   << ")";
    // qDebug() << "coord: (" << coord.x()   << ", " << coord.y()   << ")";

    return coord;
}

void medClutEditorScene::updateCoordinates()
{
    if ( medClutEditorTable * table = this->table() )
        table->updateCoordinates();
    if ( medClutEditorHistogram * histogram = this->histogram() )
        histogram->updateCoordinates();
}

void medClutEditorScene::setSize( QSizeF size )
{
    d->size = size;
    this->setSceneRect( 0.0, 0.0, d->size.width(), d->size.height() );
    this->updateCoordinates();
}

void medClutEditorScene::setRange( qreal min, qreal max )
{
    d->rangeMin = min;
    d->rangeMax = max;

    this->updateCoordinates();
}

void medClutEditorScene::adjustRange()
{
    qreal tableMin, tableMax, histMin, histMax;
    bool  tableFound = false, histFound = false;

    medClutEditorTable     * table     = this->table();
    medClutEditorHistogram * histogram = this->histogram();

    if ( table && table->vertices().count() > 0 ) {
        table->range( tableMin, tableMax );
        tableFound = true;
    }

    if ( histogram && histogram->values().count() > 0 ) {
        histogram->range( histMin, histMax );
        histFound = true;
    }

    if ( tableFound && histFound )
        this->setRange( qMin( tableMin, histMin ), qMax( tableMax, histMax ) );
    else if ( tableFound )
        this->setRange( tableMin, tableMax );
    else if ( histFound )
        this->setRange( histMin, histMax );

    if ( tableFound ) table->update();
    if ( histFound  ) histogram->update();
}

void medClutEditorScene::scaleRange( qreal factor )
{
    qreal center = 0.5 * ( d->rangeMin + d->rangeMax );

    this->setRange( factor * ( d->rangeMin - center ) + center,
                    factor * ( d->rangeMax - center ) + center );

    this->update();
}

void medClutEditorScene::shiftRange( qreal amount )
{
    qreal range  = d->rangeMax - d->rangeMin;
    qreal factor = 2e-4 * range;

    this->setRange( d->rangeMin + factor * amount,
                    d->rangeMax + factor * amount );

    this->update();
}


// /////////////////////////////////////////////////////////////////
// medClutEditorView
// /////////////////////////////////////////////////////////////////

medClutEditorView::medClutEditorView(QWidget *parent)
: QGraphicsView(parent)
{
    this->setBackgroundBrush(Qt::black);
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

    medClutEditorScene * scene =
    dynamic_cast< medClutEditorScene * >( this->scene() );
    if ( scene != NULL )
        scene->setSize( this->size() );
}

void medClutEditorView::wheelEvent( QWheelEvent * event )
{
    qreal shift = event->delta();
    qreal scale = exp( 0.001 * static_cast< double >( event->delta() ) );
    scale = qMax( 0.0, scale );

    bool withShift =
        static_cast<bool>( event->modifiers() & Qt::ShiftModifier );
    bool withCtrl =
        static_cast<bool>( event->modifiers() & Qt::ControlModifier );

    if ( !withCtrl )
    {
        medClutEditorScene * scene =
            dynamic_cast< medClutEditorScene * >( this->scene() );
        if ( scene != NULL )
        {
            if ( !withShift )
                scene->scaleRange( scale );
            else
                scene->shiftRange( shift );
        }
    }
    else {
        medClutEditorTable * table = this->table();
        if ( table != NULL )
        {
            if ( !withShift )
                table->scaleWindowWidth( scale );
            else
                table->shiftWindowCenter( shift );
        }
    }

    this->update();
}

void medClutEditorView::keyPressEvent( QKeyEvent * event ) {
    bool withShift =
        static_cast<bool>( event->modifiers() & Qt::ShiftModifier );
    bool withCtrl =
        static_cast<bool>( event->modifiers() & Qt::ControlModifier );

    switch (event->key()) {
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

    case Qt::Key_S:
    {
        medClutEditorTable * table = this->table();
        if ( table != NULL )
            table->simplifyTransferFunction();
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
    // QAction *deleteAction; // problem: right click deselects vertices
    // QAction *colorAction;  // problem: right click deselects vertices
    QAction *applyAction;
    QAction *loadTableAction;
    QAction *saveTableAction;
    // QAction *deleteTableAction;
    QAction *toggleDirectUpdateAction;
    medClutEditorScene *scene;
    medClutEditorView  *view;
    medClutEditorHistogram *histogram;

    medAbstractData *dtk_data;
    medAbstractImageView *med_view;
    QList <medClutEditorTable *> * tables;
};

medClutEditor::medClutEditor(QWidget *parent) : QWidget(parent)
{
    d = new medClutEditorPrivate;
    d->scene = new medClutEditorScene;
    d->view  = new medClutEditorView( this );
    d->view->setScene(d->scene);
    d->histogram = NULL;

    d->newAction                = new QAction("New table",    this);
    d->loadTableAction          = new QAction("Load table",   this);
    d->saveTableAction          = new QAction("Save table",   this);
    // d->deleteTableAction        = new QAction("Delete table", this);
    // d->colorAction              = new QAction("Edit color", this);
    // d->deleteAction             = new QAction("Delete selection", this);
    d->applyAction              = new QAction("Apply", this);
    d->toggleDirectUpdateAction = new QAction("Direct update", this);

    d->loadTableAction->setEnabled( false );
    d->saveTableAction->setEnabled( false );
    d->toggleDirectUpdateAction->setCheckable( true );
    d->toggleDirectUpdateAction->setChecked( true );

    connect(d->newAction,                SIGNAL(triggered()),
            this,                        SLOT(onNewTableAction()));
    connect(d->loadTableAction,          SIGNAL(triggered()),
            this,                        SLOT(onLoadTableAction()));
    connect(d->saveTableAction,          SIGNAL(triggered()),
            this,                        SLOT(onSaveTableAction()));
    // connect(d->deleteTableAction,        SIGNAL(triggered()),
    //         this,                        SLOT(onDeleteTableAction()));
    // connect(d->colorAction,              SIGNAL(triggered()),
    //         this,                        SLOT(onColorAction()));
    // connect(d->deleteAction,             SIGNAL(triggered()),
    //         this,                        SLOT(onDeleteAction()));
    connect(d->applyAction,              SIGNAL(triggered()),
            this,                        SLOT(onApplyTablesAction()));
    connect(d->toggleDirectUpdateAction, SIGNAL(triggered()),
            this,                        SLOT(onToggleDirectUpdateAction()));


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
    if (file.open(QIODevice::ReadOnly)) {
        reader.read(&file);
        file.close();
    }

}

medClutEditor::~medClutEditor(void)
{
    delete d->newAction;
    delete d->loadTableAction;
    delete d->saveTableAction;
    // delete d->deleteTableAction;
    // delete d->colorAction;
    // delete d->deleteAction;
    delete d->applyAction;
    delete d->toggleDirectUpdateAction;

    delete d->scene;
    delete d->view;
    delete d->tables; //TODO: delete all the tables as well.
    delete d;
}

void medClutEditor::setData(medAbstractData *data)
{
    if ( data == NULL ) {
        this->deleteTable();
        d->dtk_data = data;
    }

    if ( data == d->dtk_data )
        return;

    if (medAbstractImageData *image =
        dynamic_cast<medAbstractImageData *>(data)) {

        if ( d->histogram != NULL )
            delete d->histogram;

        d->histogram = new medClutEditorHistogram();
        d->scene->addItem( d->histogram );
        int min_range = image->minRangeValue();
        int max_range = image->maxRangeValue();
        QMap<qreal, qreal> bins;

        for ( int i = min_range; i <= max_range; ++i ) {
            qreal count = static_cast< qreal >(
                image->scalarValueCount( i - min_range ) );
            bins.insert( static_cast< qreal >( i ), count );
        }
        d->histogram->setValues( bins );

        this->initializeTable();

        d->dtk_data = image;
    }
}

void medClutEditor::setView(medAbstractImageView *view, bool force)
{
    medAbstractImageData * image =
        static_cast<medAbstractImageData *>(view->layerData(0));
    this->setData(image);

    if (!force && view == d->med_view)
        return;

    d->med_view = view;

    if (d->med_view != NULL)
    {
        QList<double> scalars;
        QList<QColor> colors;

        //TODO GPR: make this works
        //d->med_view->getTransferFunctions( scalars, colors );
        //medClutEditorTable * table = d->scene->table();
        //table->setTransferFunction( scalars, colors );
    }
}

void medClutEditor::initializeTable(void)
{
    this->deleteTable();

    medClutEditorTable *lut = new medClutEditorTable("Unknown");
    d->scene->addItem( lut );
    connect (lut, SIGNAL(vertexChanged()), this, SLOT(onVertexMoved()));
    connect (lut, SIGNAL(vertexAdded()),   this, SLOT(onVertexMoved()));
    connect (lut, SIGNAL(vertexRemoved()), this, SLOT(onVertexMoved()));

    QRectF  area = d->histogram->boundingRect();
    int rangemin = d->histogram->getRangeMin();
    int rangemax = d->histogram->getRangeMax();
    medClutEditorVertex * v1 =
        new medClutEditorVertex( QPointF( rangemin, 0.4 ),
                                 QPointF( area.left(), 0.6 * area.height() ),
                                 Qt::blue, lut );
    medClutEditorVertex * v2 =
        new medClutEditorVertex( QPointF( 0.5 * ( rangemin + rangemax ), 0.5 ),
                                 QPointF( area.center().x(), area.center().y()),
                                 Qt::red, lut );
    medClutEditorVertex * v3 =
        new medClutEditorVertex( QPointF( rangemax, 0.8 ),
                                 QPointF( area.right(), 0.2 * area.height() ),
                                 Qt::yellow, lut );

    lut->blockSignals(true);
    lut->addVertex(v1);
    lut->addVertex(v2);
    lut->addVertex(v3);
    lut->updateCoordinates();
    lut->blockSignals(false);
}

void medClutEditor::deleteTable(void)
{
    medClutEditorTable * table;
    while ( (table = d->scene->table()) ) {
        d->scene->removeItem( table );
        delete table;
    }
}

void medClutEditor::applyTable(void)
{
    // if (medAbstractImageData *image =
    //     dynamic_cast<medAbstractImageData *>(d->dtk_data)) {
    if ( d->med_view != NULL )
    {

        QList<double> scalars;
        QList<QColor> colors;
        medClutEditorTable * table = d->scene->table();
        table->getTransferFunction(scalars, colors);
        // d->med_view->setColorLookupTable(scalars, colors);

        //TODO GPR: make this works
        //d->med_view->setTransferFunctions(scalars, colors);
    }
}


void medClutEditor::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        event->accept();

        QMenu menu("Color Lookup Table", this);
        menu.setWindowOpacity(0.8);
        if  ( !d->toggleDirectUpdateAction->isChecked() )
            menu.addAction(d->applyAction);
        menu.addAction(d->toggleDirectUpdateAction);
        menu.addSeparator();
        menu.addAction(d->newAction);
        menu.addAction(d->loadTableAction);
        menu.addAction(d->saveTableAction);
        // menu.addAction(d->deleteTableAction);
        // menu.addSeparator();
        // menu.addAction(d->colorAction);
        // menu.addAction(d->deleteAction);
        menu.exec(mapFrom(this, QCursor::pos()));
        //hack to get the rubber band back after right clicking on the view
        d->view->setDragMode(QGraphicsView::RubberBandDrag);
    }
}

void medClutEditor::onNewTableAction(void)
{
    this->initializeTable();
    if ( medClutEditorTable * table = d->scene->table() )
        table->triggerVertexChanged();
}

// void medClutEditor::onColorAction(void)
// {
//     if ( medClutEditorTable * table = d->scene->table() )
//         table->setColorOfSelection();
// }

// void medClutEditor::onDeleteAction(void)
// {
//     if ( medClutEditorTable * table = d->scene->table() )
//         table->deleteSelection();
// }

void medClutEditor::onVertexMoved(void)
{
    if ( d->scene->table() )
    {
        if ( d->toggleDirectUpdateAction->isChecked() )
            this->applyTable();
    }
}

void medClutEditor::onApplyTablesAction(void)
{
    this->applyTable();
}

void medClutEditor::onToggleDirectUpdateAction(void)
{
    if  ( d->toggleDirectUpdateAction->isChecked() )
        this->applyTable();
}

void medClutEditor::onLoadTableAction(void)
{
    QStringList titles;
    foreach (medClutEditorTable * table,*d->tables ) {
        titles << table->title();
    }

    medLoadLUTDialog dialog (titles,this);
    // Show it and wait for Ok or Cancel
    if( dialog.exec() == QDialog::Accepted ) {
        // We got Ok
        foreach (medClutEditorTable * table,*d->tables ) {
            if ( table->title() == dialog.textValue()) {
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

    if ( medClutEditorTable * table = d->scene->table() ) {
        medSaveLUTDialog dialog (table->title(),this);
        // Show it and wait for Ok or Cancel
        if( dialog.exec() == QDialog::Accepted ) {
            // We got Ok
            medClutEditorTable * tableToSave;
            if (table->title().compare(dialog.textValue())) {
                //copy the table
                tableToSave = new medClutEditorTable( * table );
                tableToSave->setTitle(dialog.textValue());
            }
            else {
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
            if (file->open(QIODevice::WriteOnly)) {
                writer.writeFile(file);
                file->close();
            }
            else {
                qDebug() << "can'open file " << fileName;
            }
            delete file;
        }
    }
}

// void medClutEditor::onDeleteTableAction(void)
// {

// }
