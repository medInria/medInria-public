/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <limits>
#include <math.h>

#include <dtkCoreSupport/dtkAbstractView.h>

#include <medAbstractData.h>
#include <medAbstractImageData.h>
#include <medAbstractImageView.h>
#include <medClutEditor.h>
#include <medLoadLUTDialog.h>
#include <medLUTToXMLWriter.h>
#include <medSaveLUTDialog.h>
#include <medStorage.h>
#include <medXMLToLUTReader.h>

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

    QWidgetAction *setValueAction;
    QAction *setColorAction;
    QDoubleSpinBox* setValueSpinBox;
};

medClutEditorVertex::medClutEditorVertex(QPointF value, QPointF coord,
                                         QColor color, QGraphicsItem * parent)
    : QGraphicsObject(parent)
{
    d = new medClutEditorVertexPrivate;
    d->value = value;
    this->setPos( coord );
    this->setZValue(1);
    d->innerRadius =  5.0;
    d->outerRadius = 10.0;

    setAlpha();
    setColor( color );

    d->isMoved = false;

    this->setFlag(QGraphicsItem::ItemIsMovable, true);
    this->setFlag(QGraphicsItem::ItemIsSelectable, true);

    // Set Value Action
    d->setValueAction = new QWidgetAction(this);
    QLabel* setValueLabel = new QLabel("Set value ");
    d->setValueSpinBox = new QDoubleSpinBox;
    d->setValueSpinBox->setMaximum(10000);
    d->setValueSpinBox->setMinimum(-10000);
    d->setValueSpinBox->setValue(static_cast<double>(this->value().x()));
    connect(d->setValueSpinBox, SIGNAL(editingFinished()), this, SLOT(setValue()));
    QHBoxLayout *setValueLayout = new QHBoxLayout;
    setValueLayout->addWidget(setValueLabel);
    setValueLayout->addWidget(d->setValueSpinBox);
    QWidget *setValueWidget = new QWidget;
    setValueWidget->setLayout(setValueLayout);
    d->setValueAction->setDefaultWidget(setValueWidget);

    // Set Color Action
    d->setColorAction = new QAction("Set color", this);
    connect(d->setColorAction, SIGNAL(triggered()), this, SLOT(showColorSelection()));
}

medClutEditorVertex::medClutEditorVertex( const medClutEditorVertex & other,
                                          QGraphicsItem * parent)
    : QGraphicsObject(parent)
{
    if ( parent == nullptr )
    {
        this->setParentItem( other.parentItem() );
    }

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
    if ( amount != 0.0 )
    {
        medClutEditorTable *table =
                dynamic_cast< medClutEditorTable * >( this->parentItem() );
        forceConstraints = forceConstraints && table;

        medClutEditorScene *scene =
                dynamic_cast< medClutEditorScene * >( this->scene() );
        QPointF value = d->value;
        value.setX( value.x() + amount );

        if ( forceConstraints )
        {
            table->initiateMoveSelection();
        }
        this->setPos( scene->valueToCoordinate( value ) );
        if ( forceConstraints )
        {
            table->constrainMoveSelection( this );
            table->finalizeMoveSelection();
        }

        this->updateValue();
    }
}

void medClutEditorVertex::setValue()
{
    //editingFinished is emitted when we press Enter AND when the spinBox loses focus
    if (d->setValueSpinBox->hasFocus())
    {
        d->setValueSpinBox->blockSignals(true);
        double newValue = d->setValueSpinBox->value();
        double value = this->value().x();
        float amount = static_cast<float>(newValue - value);
        this->shiftValue(amount);
        if ( medClutEditorTable *table =
             dynamic_cast< medClutEditorTable * >( this->parentItem() ) )
        {
            table->triggerVertexSet();
        }
        d->setValueSpinBox->blockSignals(false);
    }
}

void medClutEditorVertex::interpolate( medClutEditorVertex * prev,
                                       medClutEditorVertex * next )
{
    if ( prev != nullptr && next == nullptr )
    {
        setColor( prev->color() );
    }
    else if ( prev == nullptr && next != nullptr )
    {
        setColor( next->color() );
    }
    else if ( prev != nullptr && next != nullptr )
    {
        if ( ( prev->color().alpha() == 0 ) &&
             ( next->color().alpha() > 0 ) )
        {
            setColor( next->color() );
        }
        else if ( ( prev->color().alpha() > 0 ) &&
                  ( next->color().alpha() == 0 ) )
        {
            setColor( prev->color() );
        }
        else if ( ( this->x() - prev->x() ) < ( next->x() - this->x() ) )
        {
            setColor( prev->color() );
        }
        else
        {
            setColor( next->color() );
        }
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

void medClutEditorVertex::showColorSelection()
{
    if ( medClutEditorTable *table =
         dynamic_cast< medClutEditorTable * >( this->parentItem() ) )
    {
        table->setColorOfSelection( d->color );
    }
}

QAction* medClutEditorVertex::getColorAction()
{
    return d->setColorAction;
}

void medClutEditorVertex::setAlpha()
{
    qreal alpha = qMax( 0.0, qMin( d->value.y(), 1.0 ) );

    d->color.setAlphaF( alpha );
    d->bgColor.setAlphaF( alpha );
}

void medClutEditorVertex::updateCoordinates()
{
    if ( medClutEditorScene *scene =
         dynamic_cast< medClutEditorScene * >( this->scene() ) )
    {
        this->setPos( scene->valueToCoordinate( d->value ) );
        this->update();
    }
}

void medClutEditorVertex::updateValue()
{
    if ( medClutEditorScene *scene =
         dynamic_cast< medClutEditorScene * >( this->scene() ) )
    {
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

    if ( inManhattan )
    {
        QPointF move = this->pos() - d->lastPos;
        if ( std::abs( move.x() ) > std::abs( move.y() ) )
        {
            this->setY( d->lastPos.y() );
            forced = true;
        }
        if ( std::abs( move.x() ) < std::abs( move.y() ) )
        {
            this->setX( d->lastPos.x() );
            forced = true;
        }
    }

    if ( this->x() < limits.left() )
    {
        this->setX( limits.left() );
        forced = true;
    }
    else if ( this->x() > limits.right() )
    {
        this->setX( limits.right() );
        forced = true;
    }
    if ( this->y() < limits.top() )
    {
        this->setY( limits.top() );
        forced = true;
    }
    else if ( this->y() > limits.bottom() )
    {
        this->setY( limits.bottom() );
        forced = true;
    }

    bool moved = forced || ( this->pos() - d->lastPos ) != QPointF( 0.0, 0.0 );
    if ( moved )
    {
        this->updateValue();
        this->update();
    }
}

void medClutEditorVertex::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    this->QGraphicsItem::mouseMoveEvent( event );

    bool withShift =
            static_cast<bool>( event->modifiers() & Qt::ShiftModifier );

    if ( medClutEditorTable *table =
         dynamic_cast< medClutEditorTable * >( this->parentItem() ) )
    {
        table->constrainMoveSelection( this, withShift );
        table->triggerVertexMoving();
    }

    d->setValueSpinBox->setValue(static_cast<double>(this->value().x()));

    this->updateValue();
}

void medClutEditorVertex::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);

    showColorSelection();
}

void medClutEditorVertex::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::RightButton)
    {
        this->setSelected( true );
        this->update();
        event->accept();
        QMenu menu("Edit Marker", 0) ;
        menu.setWindowOpacity(0.8) ;
        menu.addAction(d->setValueAction);
        menu.addAction(d->setColorAction);
        menu.exec(event->screenPos());
    }
    else
    {
        this->QGraphicsItem::mousePressEvent(event);
        if ( medClutEditorTable *table =
             dynamic_cast< medClutEditorTable * >( this->parentItem() ) )
        {
            table->initiateMoveSelection();
        }
    }
}

void medClutEditorVertex::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    this->QGraphicsItem::mouseReleaseEvent(event);
    if ( medClutEditorTable *table =
         dynamic_cast< medClutEditorTable * >( this->parentItem() ) )
    {
        table->finalizeMoveSelection();
        table->triggerVertexSet();
    }
}

// /////////////////////////////////////////////////////////////////
// medClutEditorTable
// /////////////////////////////////////////////////////////////////

class medClutEditorTablePrivate
{
public:
    QList<medClutEditorVertex *> verticesToDisplay, principalVertices;
    //difference between these 2 lists occurs only in Discrete Mode
    QString title;
    qreal displayAlpha;
    bool discreteMode, LUTinverted;
};

medClutEditorTable::medClutEditorTable(const QString & title,
                                       QGraphicsItem *parent)
    : QGraphicsObject(parent)
{
    d = new medClutEditorTablePrivate;
    d->title = title;
    d->displayAlpha = 0.25;
    d->discreteMode = false;
    d->LUTinverted = false;

    this->setFlag(QGraphicsItem::ItemIsFocusable, true);
    this->setZValue(0);
}

medClutEditorTable::medClutEditorTable(const medClutEditorTable & table)
    : QGraphicsObject(static_cast< QGraphicsItem *>(table.parentItem()))
{
    d = new medClutEditorTablePrivate;
    d->title = table.title();
    d->discreteMode = false;

    for( const medClutEditorVertex * vertex : table.vertices())
    {
        d->principalVertices << new medClutEditorVertex( * vertex );
    }

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

void medClutEditorTable::setDiscreteMode(bool value)
{
    d->discreteMode = value;
    updateVerticesToDisplay();
    emit switchMode();
}

void medClutEditorTable::invertLUT(bool value)
{
    d->LUTinverted = value;
    updateVerticesToDisplay();
    emit switchMode(); //to update 3D view
}

QHash<medClutEditorVertex *, medClutEditorVertex *> *
medClutEditorTable::calculateCoupledVertices(QList<medClutEditorVertex *>list)
{
    QHash<medClutEditorVertex *, medClutEditorVertex *> *hash = new QHash<medClutEditorVertex *, medClutEditorVertex *>();
    float epsilon = static_cast<float>(0.0001);

    for (int i=0; i<list.size(); i++)
    {
        if( i==0 )
        {
            hash->insert(list.at(i), nullptr); //first one coupled with nullptr
        }
        else
        {
            QPointF value, position;
            medClutEditorVertex *coupledVertex;

            if(!d->LUTinverted)
            {
                value = QPointF(list.at(i)->value().x(), list.at(i-1)->value().y()); //same abscissa, ordinate of the previous vertex
                position = QPointF(list.at(i)->pos().x()-epsilon, list.at(i-1)->pos().y());
                coupledVertex = new medClutEditorVertex(value, position, list.at(i-1)->color()); //color of the previous
            }
            else
            {
                value = QPointF(list.at(i-1)->value().x(), list.at(i)->value().y()); //
                position = QPointF(list.at(i-1)->pos().x()+epsilon, list.at(i)->pos().y());
                coupledVertex = new medClutEditorVertex(value, position, list.at(i)->color()); //color of the previous
            }
            if(coupledVertex && !hash->contains(list.at(i)))
            {
                hash->insert(list.at(i), coupledVertex);
            }
        }
    }
    return hash;
}

void medClutEditorTable::updateVerticesToDisplay()
{
    if(d->discreteMode)
    {
        QHash<medClutEditorVertex *, medClutEditorVertex *> *hash = this->calculateCoupledVertices(d->principalVertices);
        if(hash)
        {
            d->verticesToDisplay.clear(); //reset list of vertices to display

            QHash<medClutEditorVertex *, medClutEditorVertex *>::const_iterator i = hash->constBegin();
            while(i != hash->constEnd())
            {
                d->verticesToDisplay << i.key();
                if(i.value())
                {
                    d->verticesToDisplay << i.value();
                }
                i++;
            }
        }
        else
        {
            d->verticesToDisplay = d->principalVertices;
        }
    }
    else
    {
        d->verticesToDisplay = d->principalVertices;
    }

    std::sort(d->verticesToDisplay.begin(), d->verticesToDisplay.end(), medClutEditorVertex::LessThan);
    this->update();
}

QRectF medClutEditorTable::boundingRect(void) const
{
    if ( d->principalVertices.count() == 0 )
    {
        return QRectF( 0.0, 0.0, 0.0, 0.0 );
    }

    QRectF box( this->mapRectFromItem(  d->principalVertices.first(),
                                        d->principalVertices.first()->boundingRect() ) );

    for( medClutEditorVertex *vertex : d->principalVertices )
    {
        box = box.united( this->mapRectFromItem( vertex,
                                                 vertex->boundingRect() ) );
    }

    return box;
}


void medClutEditorTable::range( qreal & min, qreal & max ) const
{
    if ( d->principalVertices.count() == 0 )
    {
        min = max = 0.0;
        return;
    }

    min = d->principalVertices.first()->value().x();
    max = d->principalVertices.last()->value().x();
}

void medClutEditorTable::initiateMoveSelection()
{
    for(medClutEditorVertex *vertex : d->principalVertices)
    {
        if ( vertex->isSelected() )
        {
            vertex->initiateMove();
        }
    }
}

void medClutEditorTable::constrainMoveSelection( medClutEditorVertex * driver,
                                                 bool inManhattan )
{
    medClutEditorScene * scene =
            dynamic_cast< medClutEditorScene * >( this->scene() );
    QRectF box = scene->plotArea();

    for(medClutEditorVertex *vertex : d->principalVertices)
    {
        QRectF limits = box;
        if ( vertex->isSelected() )
        {
            if ( vertex != driver )
            {
                limits.setLeft(  vertex->x() - 1.0 );
                limits.setRight( vertex->x() + 1.0 );
            }
            vertex->forceGeometricalConstraints( limits, inManhattan );
        }
    }

    int n = d->principalVertices.count();
    for ( int i = 0 ; i < n; ++i )
    {
        medClutEditorVertex *vertex = d->principalVertices.at( i );

        if ( vertex->isSelected() )
        {
            continue;
        }

        qreal left  = vertex->x() - 1.0;
        qreal right = vertex->x() + 1.0;

        for ( int j = i - 1; j >= 0; --j )
        {
            if ( d->principalVertices.at( j )->isSelected() )
            {
                left = d->principalVertices.at( j )->x() + static_cast<qreal>( i - j );
                break;
            }
        }

        for ( int j = i + 1; j < n; ++j )
        {
            if ( d->principalVertices.at( j )->isSelected() )
            {
                right = d->principalVertices.at( j )->x() + static_cast<qreal>( i - j );
                break;
            }
        }

        QRectF limits = box;
        limits.setLeft( left );
        limits.setRight( right );

        vertex->forceGeometricalConstraints( limits );
    }
}

void medClutEditorTable::finalizeMoveSelection()
{
    for(medClutEditorVertex * vertex : d->principalVertices)
    {
        if ( vertex->isSelected() )
        {
            vertex->finalizeMove();
        }
    }
}

void medClutEditorTable::updateCoordinates()
{
    this->prepareGeometryChange();
    for(medClutEditorVertex * vertex : d->principalVertices)
    {
        vertex->updateCoordinates();
    }
}

void medClutEditorTable::addVertex( medClutEditorVertex *vertex, bool interpolate )
{
    d->principalVertices<< vertex;
    d->verticesToDisplay<< vertex;

    if ( vertex->parentItem() != this )
    {
        vertex->setParentItem( this );
    }

    std::sort(d->principalVertices.begin(), d->principalVertices.end(), medClutEditorVertex::LessThan);

    if (interpolate)
    {
        int i    = d->principalVertices.indexOf( vertex );
        int last = d->principalVertices.count() - 1;
        medClutEditorVertex * prev = i > 0    ? d->principalVertices[i-1] : nullptr;
        medClutEditorVertex * next = i < last ? d->principalVertices[i+1] : nullptr;
        vertex->interpolate( prev, next );
    }
    updateVerticesToDisplay();

    emit vertexAdded();
}

QList< medClutEditorVertex * > & medClutEditorTable::vertices()
{
    return d->principalVertices;
}

const QList< medClutEditorVertex * > & medClutEditorTable::vertices() const
{
    return d->principalVertices;
}

void medClutEditorTable::setSelectedAllVertices( bool isSelected )
{
    for(medClutEditorVertex * vertex : d->verticesToDisplay)
    {
        vertex->setSelected( isSelected );
    }
}

void medClutEditorTable::deleteSelection()
{
    int nSelected = 0;
    for( medClutEditorVertex *vertex : d->principalVertices )
    {
        if ( vertex->isSelected() )
        {
            ++nSelected;
        }
    }

    if ( d->principalVertices.count() - nSelected < 2 )
    {
        qDebug() << "Need at least two nodes, but only "
                 << ( d->principalVertices.count() - nSelected ) << " nodes"
                 << " would be left after deletion!  Not deleting any node.";
        return;
    }

    QList<medClutEditorVertex *> remaining;
    while ( !d->principalVertices.isEmpty() )
    {
        medClutEditorVertex *vertex = d->principalVertices.takeFirst();
        if ( vertex->isSelected() )
        {
            delete vertex;
        }
        else
        {
            remaining.append( vertex );
        }
    }

    d->principalVertices = remaining;
    updateVerticesToDisplay();

    emit vertexRemoved();
}

void medClutEditorTable::deleteAllVertices()
{
    for( medClutEditorVertex *vertex : d->verticesToDisplay )
    {
        delete vertex;
    }

    d->verticesToDisplay.clear();
    d->principalVertices.clear();
}

void medClutEditorTable::setColorOfSelection( const QColor & color )
{
    QColor newColor = QColorDialog::getColor( color, 0 );

    if ( newColor.isValid() )
    {
        for( medClutEditorVertex * vertex : d->principalVertices )
        {
            if ( vertex->isSelected() )
            {
                vertex->setColor( newColor );
                updateVerticesToDisplay();
                emit vertexSet();
            }
        }
    }
}

void medClutEditorTable::setColorOfSelection()
{
    QColor color;
    for( medClutEditorVertex * vertex : d->principalVertices )
    {
        if ( vertex->isSelected() &&
             ( !color.isValid() || vertex->color().alpha() > color.alpha() ) )
        {
            color = vertex->color();
        }
    }

    if ( color.isValid() )
    {
        this->setColorOfSelection( color );
    }
}

void medClutEditorTable::scaleWindowWidth( qreal factor )
{
    qreal min, max;
    this->range( min, max );
    qreal center = 0.5 * ( min + max );

    for(medClutEditorVertex * vertex : d->principalVertices)
    {
        qreal offset = vertex->value().x() - center;
        vertex->shiftValue( offset * ( factor - 1.0 ), false );
    }

    emit vertexMoving();
}

void medClutEditorTable::shiftWindowCenter( qreal amount )
{
    qreal min, max;
    this->range( min, max );
    qreal factor = 5e-4 * ( max - min );

    for(medClutEditorVertex * vertex : d->principalVertices)
    {
        vertex->shiftValue( factor * amount, false );
    }

    emit vertexMoving();
}

void medClutEditorTable::setup(float min, float max, int size, int *table)
{
    Q_UNUSED(min);
    Q_UNUSED(max);
    Q_UNUSED(size);
    Q_UNUSED(table);
}

void medClutEditorTable::getTransferFunction( QList<double> &scalars,
                                              QList<QColor> &colors )
{
    scalars.clear();
    colors.clear();

    for(medClutEditorVertex *vertex : d->principalVertices)
    {
        if(!vertex)
        {
            return;
        }
        colors  << vertex->color();
        QPointF value = vertex->value();
        scalars << static_cast< double >( value.x() );
    }
}

void medClutEditorTable::setTransferFunction( QList<double> &scalars,
                                              QList<QColor> &colors )
{
    medClutEditorScene *scene =
            dynamic_cast< medClutEditorScene * >( this->scene() );
    if ( scene == nullptr )
    {
        return;
    }

    int size = qMin( scalars.count(), colors.count() );

    if ( size < 2 )
    {
        qDebug() << "medClutEditorTable::setTransferFunction: Transfer function"
                 << " has less than two nodes.  Can't use it.";
        return;
    }

    this->deleteAllVertices();

    for ( int i = 0; i < size; i++ )
    {
        QPointF value( scalars.at( i ), colors.at( i ).alphaF() );
        QPointF coord = scene->valueToCoordinate( value );

        this->addVertex(new medClutEditorVertex( value, coord, colors.at( i ), this ));
    }

    std::sort(d->principalVertices.begin(), d->principalVertices.end(), medClutEditorVertex::LessThan);

    scene->adjustRange();
}

void medClutEditorTable::simplifyTransferFunction()
{
    qreal threshold = 1.0;
    bool foundThreshold = false;

    d->principalVertices.first()->setSelected( false );
    d->principalVertices.last()->setSelected( false );

    for ( int i = 1, n = d->verticesToDisplay.count() - 1; i < n; ++i )
    {
        medClutEditorVertex *vertex = d->verticesToDisplay.at( i );
        if ( vertex->isSelected() )
        {
            qreal xp = d->principalVertices.at( i - 1 )->x();
            qreal xn = d->principalVertices.at( i + 1 )->x();
            qreal wn = ( vertex->x() - xp ) / ( xn - xp );
            qreal wp = 1.0 - wn;

            QColor prev = d->principalVertices.at( i - 1 )->color();
            QColor next = d->principalVertices.at( i + 1 )->color();

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

    if ( !foundThreshold )
    {
        qDebug() << "Select at least one vertex (except the first or last one)"
                 << " which is too similar to its neighbors.";
        return;
    }

    QList<medClutEditorVertex *> vertices( d->principalVertices );
    bool modified = true;
    while ( modified )
    {
        modified = false;

        QList<medClutEditorVertex *> remaining;
        remaining.append( vertices.first() );
        for ( int i = 1, n = vertices.count() - 1; i < n; ++i )
        {
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

            if ( offset > threshold )
            {
                remaining.append( vertex );
                vertex->setSelected( false );
            }
            else
            {
                vertex->setSelected( true );
                modified = true;
                vertex = vertices.at( ++i );
                vertex->setSelected( false );
                if ( i < n )
                {
                    remaining.append( vertex );
                }
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
    if ( scene == nullptr )
    {
        return;
    }

    QRectF area = scene->plotArea();

    int n_points = d->verticesToDisplay.size();
    if(n_points == 0)
    {
        return;
    }
    QPointF *points = new QPointF[n_points + 2];
    points[0]            = QPointF( d->verticesToDisplay.first()->x(), area.bottom() );
    points[n_points + 1] = QPointF( d->verticesToDisplay.last()->x(),  area.bottom() );
    for ( int i = 0 ; i < n_points; i++ )
    {
        points[i+1] = d->verticesToDisplay.at(i)->pos();
    }

    qreal xmin = d->verticesToDisplay.first()->x();
    qreal xmax = d->verticesToDisplay.last()->x();

    QLinearGradient linearGradient(xmin, 0.0, xmax, 0.0);
    for( medClutEditorVertex * vertex : d->verticesToDisplay )
    {
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
    for( medClutEditorVertex * vertex : d->verticesToDisplay )
    {
        if ( vertex->isSelected() )
        {
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

void medClutEditorTable::triggerVertexMoving()
{
    updateVerticesToDisplay();
    emit vertexMoving();
}

void medClutEditorTable::triggerVertexSet()
{
    updateVerticesToDisplay();
    emit vertexSet();
}

// /////////////////////////////////////////////////////////////////
// medClutEditorHistogram
// /////////////////////////////////////////////////////////////////

class medClutEditorHistogramPrivate
{
public:
    QMap<qreal, qreal> values;
    QList<QPointF> scaledValues;
    qreal maxLogNum;
};

medClutEditorHistogram::medClutEditorHistogram(QGraphicsItem *parent)
    : QGraphicsItem(parent)
    , d (new medClutEditorHistogramPrivate)
{
    d->maxLogNum = 0.0;
    this->setZValue(-1000);
}

medClutEditorHistogram::~medClutEditorHistogram(void)
{
    delete d;
    d = nullptr;
}

QRectF medClutEditorHistogram::boundingRect(void) const
{
    medClutEditorScene * scene =
            dynamic_cast< medClutEditorScene * >( this->scene() );
    if ( scene )
    {
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
    if ( d->values.size() > 0 )
    {
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
    for ( iterator it = bins.begin(), end = bins.end(); it != end; ++it )
    {
        qreal logNum = 1000.0 * log10f( it.value() + 1.0 );
        d->maxLogNum = qMax( logNum, d->maxLogNum );
        d->values.insert( it.key(), logNum );
    }
    d->scaledValues.clear();

    medClutEditorScene * scene =
            dynamic_cast< medClutEditorScene * >( this->scene() );
    if ( scene != nullptr )
    {
        scene->adjustRange();
    }
}

void medClutEditorHistogram::addValue(qreal intensity, qreal number)
{
    qreal logNum = 1000.0 * log10f( number + 1.0 );
    d->maxLogNum = qMax( logNum, d->maxLogNum );
    d->values.insert( intensity, logNum );
    d->scaledValues.clear();

    medClutEditorScene * scene =
            dynamic_cast< medClutEditorScene * >( this->scene() );
    if ( scene != nullptr )
    {
        scene->adjustRange();
    }
}

void medClutEditorHistogram::paint(QPainter *painter,
                                   const QStyleOptionGraphicsItem *option,
                                   QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    medClutEditorScene * scene =
            dynamic_cast< medClutEditorScene * >( this->scene() );

    if ( scene            == nullptr ||
         d->maxLogNum     == 0.0  ||
         d->values.size() == 0    )
    {
        return;
    }

    if ( d->scaledValues.count() != d->values.count() + 2 )
    {
        d->scaledValues.clear();
        QMap<qreal, qreal>::iterator it = d->values.begin();
        d->scaledValues << scene->valueToCoordinate( QPointF( it.key(), 0.0 ) );
        for ( QMap<qreal, qreal>::iterator end = d->values.end(); it != end; ++it )
        {
            d->scaledValues << scene->valueToCoordinate(
                                   QPointF( it.key(), it.value() / d->maxLogNum ) );
        }
        d->scaledValues << scene->valueToCoordinate( QPointF( ( --it ).key(), 0.0 ) );
    }

    QPainterPath path;
    path.moveTo( d->scaledValues.first() );
    for( QPointF point : d->scaledValues )
    {
        path.lineTo( point );
    }

    painter->setPen( QPen( QColor(0x61, 0x61, 0x61) ) );
    painter->setBrush( QColor(0x2a, 0x2a, 0x2a) );
    painter->drawPath( path );
}

void medClutEditorHistogram::mouseDoubleClickEvent(
        QGraphicsSceneMouseEvent * event )
{
    medClutEditorScene * scene =
            dynamic_cast< medClutEditorScene * >( this->scene() );
    medClutEditorTable * table = scene != nullptr ? scene->table() : nullptr;

    if ( table != nullptr )
    {
        const QPointF & p = event->pos();
        QPointF value = scene->coordinateToValue( p );
        medClutEditorVertex * vertex =
                new medClutEditorVertex( value, p, Qt::blue, table );
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
    for( QGraphicsView * view : this->views() )
    {
        if ( medClutEditorView * v =
             dynamic_cast<medClutEditorView *>( view ) )
        {
            return v;
        }
    }

    return nullptr;
}

medClutEditorTable * medClutEditorScene::table()
{
    for( QGraphicsItem *item : this->items() )
    {
        if ( medClutEditorTable * table =
             dynamic_cast<medClutEditorTable *>( item ) )
        {
            return table;
        }
    }

    return nullptr;
}

medClutEditorHistogram * medClutEditorScene::histogram()
{
    for( QGraphicsItem *item : this->items() )
    {
        if ( medClutEditorHistogram * histogram =
             dynamic_cast<medClutEditorHistogram *>( item ) )
        {
            return histogram;
        }
    }

    return nullptr;
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
    if ( valRange > 0.0 )
    {
        coord.setX( d->leftMargin +
                    area.width() * ( value.x() - d->rangeMin ) / valRange );
        coord.setY( d->topMargin + area.height() - area.height() * value.y() );
    }

    return coord;
}

void medClutEditorScene::updateCoordinates()
{
    if ( medClutEditorTable * table = this->table() )
    {
        table->updateCoordinates();
    }
    if ( medClutEditorHistogram * histogram = this->histogram() )
    {
        histogram->updateCoordinates();
    }
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

    if ( table && table->vertices().count() > 0 )
    {
        table->range( tableMin, tableMax );
        tableFound = true;
    }

    if ( histogram && histogram->values().count() > 0 )
    {
        histogram->range( histMin, histMax );
        histFound = true;
    }

    if ( tableFound && histFound )
    {
        this->setRange( qMin( tableMin, histMin ), qMax( tableMax, histMax ) );
    }
    else if ( tableFound )
    {
        this->setRange( tableMin, tableMax );
    }
    else if ( histFound )
    {
        this->setRange( histMin, histMax );
    }

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
    if ( scene == nullptr )
    {
        return nullptr;
    }

    return dynamic_cast< medClutEditorTable * >( scene->table() );
}

void medClutEditorView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);

    medClutEditorScene * scene =
            dynamic_cast< medClutEditorScene * >( this->scene() );
    if ( scene != nullptr )
    {
        scene->setSize( this->size() );
        if(scene->table())
        {
            scene->table()->updateVerticesToDisplay();
        }
    }
}

void medClutEditorView::wheelEvent( QWheelEvent * event )
{
    Q_UNUSED(event);
}

void medClutEditorView::keyPressEvent( QKeyEvent * event ) {
    bool withShift =
            static_cast<bool>( event->modifiers() & Qt::ShiftModifier );
    bool withCtrl =
            static_cast<bool>( event->modifiers() & Qt::ControlModifier );

    switch (event->key())
    {
        case Qt::Key_A:
        {
            medClutEditorTable * table = this->table();
            if ( table != nullptr )
            {
                table->setSelectedAllVertices( !withShift );
            }
            break;
        }

        case Qt::Key_C:
        {
            medClutEditorTable * table = this->table();
            if ( table != nullptr )
            {
                table->setColorOfSelection();
            }
            break;
        }

        case Qt::Key_S:
        {
            medClutEditorTable * table = this->table();
            if ( table != nullptr )
            {
                table->simplifyTransferFunction();
            }
            break;
        }

        case Qt::Key_Delete:
        {
            medClutEditorTable * table = this->table();
            if ( table != nullptr )
            {
                table->deleteSelection();
            }
            break;
        }

        case Qt::Key_Plus:
        case Qt::Key_Equal:
        {
            medClutEditorTable * table = this->table();
            if ( table != nullptr )
            {
                table->changeDisplayAlpha( withCtrl ? 0.05 : 0.25 );
            }
            break;
        }
        case Qt::Key_Minus:
        case Qt::Key_Underscore:
        {
            medClutEditorTable * table = this->table();
            if ( table != nullptr )
            {
                table->changeDisplayAlpha( withCtrl ? -0.05 : -0.25 );
            }
            break;
        }
        case Qt::Key_0:
        {
            medClutEditorTable * table = this->table();
            if ( table != nullptr )
            {
                table->resetDisplayAlpha();
            }
            break;
        }
        default:
        {
            break;
        }
    }
}

void medClutEditorView::keyReleaseEvent( QKeyEvent * event){
    if (event->key() == Qt::Key_Control)
    {
        this->setDragMode(QGraphicsView::RubberBandDrag);
    }
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
