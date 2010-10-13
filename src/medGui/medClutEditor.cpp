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
#include "medClutEditor.h"

#include <math.h>
#include <climits>

// /////////////////////////////////////////////////////////////////
// medClutEditorHistogram
// /////////////////////////////////////////////////////////////////

class medClutEditorHistogram : public QGraphicsItem
{
public:
     medClutEditorHistogram(QGraphicsItem *parent = 0);
    ~medClutEditorHistogram(void);

    void addValue(qreal intensity, qreal number);
    qreal getRangeMin();
    qreal getRangeMax();
    QPointF valueToCoordinate( QPointF value );

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
	       QWidget *widget = 0);

    QRectF boundingRect(void) const;

private:
    QMap<qreal, qreal> values;

    qreal maxLogNum;
    QSizeF size;
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
};







// /////////////////////////////////////////////////////////////////
// medClutEditorVertex
// /////////////////////////////////////////////////////////////////

class medClutEditorVertexPrivate
{
public:
    QColor fgColor;
    QColor bgColor;
};

medClutEditorVertex::medClutEditorVertex(qreal x, qreal y, QColor color,
					 QGraphicsItem * parent)
  : QGraphicsItem(parent)
{
    d = new medClutEditorVertexPrivate;
    d->fgColor = color;
    d->bgColor = QColor(0xc0, 0xc0, 0xc0, 0x0);
    this->setPos(x, y);
    setAlpha();
    this->setZValue(1);

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
      d->fgColor = prev->color();
  else if ( prev == NULL && next != NULL )
      d->fgColor = next->color();
  else if ( prev != NULL && next != NULL ) {
      if ( ( prev->color().alpha() == 0 ) &&
	   ( next->color().alpha() > 0 ) )
	  d->fgColor = next->color();
      else if ( ( prev->color().alpha() > 0 ) &&
		( next->color().alpha() == 0 ) )
	  d->fgColor = prev->color();
      else if ( ( this->x() - prev->x() ) < ( next->x() - this->x() ) )
	  d->fgColor = prev->color();
      else	  
	  d->fgColor = next->color();
  }
}

void medClutEditorVertex::paint(QPainter *painter,
				const QStyleOptionGraphicsItem *option,
				QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    setAlpha();
    painter->setPen( this->isSelected() ? Qt::red : Qt::black);
    painter->setBrush(d->bgColor); painter->drawEllipse(-40, -40, 80, 80);
    painter->setBrush(d->fgColor); painter->drawEllipse(-25, -25, 50, 50);
}

QRectF medClutEditorVertex::boundingRect(void) const
{
    return QRectF(-40.0, -40.0, 80.0, 80.0);
}

QColor medClutEditorVertex::color(void) const
{
    return d->fgColor;
}

void medClutEditorVertex::setColor( QColor color )
{
    color.setAlpha( d->fgColor.alpha() );
    if ( d->fgColor != color ) {
	d->fgColor = color;
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
	table->setColorOfSelection( d->fgColor );
}

// void medClutEditorVertex::mousePressEvent(QGraphicsSceneMouseEvent *event)
// {
//     if (event->button() == Qt::RightButton) {
// 	this->setSelected( !this->isSelected() );
// 	this->update();
// 	event->accept();
//         // QMenu menu("Edit Marker", 0) ;
//         // menu.setWindowOpacity(0.8) ;
//         // menu.addAction(d->setColorAction);
//         // menu.addAction(d->deleteAction) ;
//         // menu.exec(event->screenPos());
//     }

//     this->QGraphicsItem::mousePressEvent(event);
// }

void medClutEditorVertex::setAlpha()
{
    qreal alpha = 1.0;
    if ( medClutEditorTable * table =
	 dynamic_cast< medClutEditorTable * >( this->parentItem() ) )
	alpha = table->coordinateToValue( this->pos() ).y();

    d->fgColor.setAlphaF( qMin( alpha, 1.0 ) );
}

// /////////////////////////////////////////////////////////////////
// medClutEditorTable
// /////////////////////////////////////////////////////////////////

class medClutEditorTablePrivate
{
 public:
    QRectF size;
    qreal rangeMin;
    qreal rangeMax;
    QList<medClutEditorVertex *> vertices;
};


medClutEditorTable::medClutEditorTable(QGraphicsItem *parent)
  : QGraphicsItem(parent)
{
    d = new medClutEditorTablePrivate;
    d->size.setCoords( 0.0, 0.0, 500.0, 300.0 );

    //this->setFlag(QGraphicsItem::ItemIsMovable, true);
    this->setFlag(QGraphicsItem::ItemIsFocusable, true);
    this->setZValue(0);
}

medClutEditorTable::~medClutEditorTable(void)
{
    qDebug() << __func__;

    delete d;
}

void medClutEditorTable::setSize( const QRectF & size )
{
    d->size = size;
}

const QRectF & medClutEditorTable::size() const
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
	QRectF limits = size();
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

void medClutEditorTable::setSelectedAllVertices( bool isSelected )
{
    foreach (medClutEditorVertex * vertex, d->vertices)
	vertex->setSelected( isSelected );
}

void medClutEditorTable::deleteSelection()
{
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
	if ( vertex->isSelected() && vertex->color().alpha() > color.alpha() )
	    color = vertex->color();

    if ( color.isValid() )
	this->setColorOfSelection( color );
}

void medClutEditorTable::onDeleteVertex(medClutEditorVertex *vertex)
{
    qDebug() << (long int) vertex;
    qDebug() << vertex->color();

    d->vertices.removeAll( vertex );
    delete vertex;
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

void medClutEditorTable::setupTransferFunction( QList<double> &scalars,
						QList<QColor> &colors )
{
    scalars.clear();
    colors.clear();

    foreach (medClutEditorVertex * vertex, d->vertices) {
        colors  << vertex->color();
        scalars << static_cast< double >( vertex->x() );
    }
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
        // color.setAlphaF( 0.5 );
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
    xmin = d->size.right();
    xmax = d->size.left();
    ymin = d->size.top();
    ymax = d->size.bottom();

    foreach (medClutEditorVertex *vertex, d->vertices) {
	xmin = qMin(xmin, vertex->x());
	xmax = qMax(xmax, vertex->x());
	ymin = qMin(ymin, vertex->y());
	ymax = qMax(ymax, vertex->y());
    }

    return QRectF(xmin, ymin, xmax - xmin, qAbs(ymin));
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

medClutEditorHistogram::medClutEditorHistogram(QGraphicsItem *parent)
  : QGraphicsItem(parent)
  , size( 500.0, 300.0 )
  , maxLogNum( 0.0 )
{
    //this->setFlag(QGraphicsItem::ItemIsMovable, false);
    this->setZValue(-1000);
}

medClutEditorHistogram::~medClutEditorHistogram(void)
{
}

qreal medClutEditorHistogram::getRangeMin(void)
{
    return values.begin().key();
}

qreal medClutEditorHistogram::getRangeMax(void)
{
    QMap<qreal, qreal>::iterator end = values.end();
    --end;
    return end.key();
}

void medClutEditorHistogram::addValue(qreal intensity, qreal number)
{
    qreal logNum = 1000.0 * log10f( number + 1.0 );
    maxLogNum = qMax( logNum, maxLogNum );
    values.insert( intensity, logNum );
}

QPointF medClutEditorHistogram::valueToCoordinate( QPointF value )
{
    qreal rangeMin  = this->getRangeMin();
    qreal valRange  = this->getRangeMax() - rangeMin;

    QPointF coord;
    coord.setX( size.width() * ( ( value.x() - rangeMin ) / valRange ) );
    coord.setY( size.height() - size.height() * ( value.y() / maxLogNum ) );

    return coord;
}

void medClutEditorHistogram::paint(QPainter *painter,
				   const QStyleOptionGraphicsItem *option,
				   QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QList<QPointF> scaledValues;
    for ( QMap<qreal, qreal>::iterator it = values.begin(), end = values.end();
	  it != end; ++it )
	scaledValues <<
	    this->valueToCoordinate( QPointF( it.key(), it.value() ) );

    QPainterPath path;
    path.moveTo( QPointF( scaledValues.first().x(), size.height() ) );
    foreach ( QPointF point, scaledValues )
	path.lineTo( point );
    path.lineTo( QPointF( scaledValues.last().x(), size.height() ) );

    painter->setPen(QColor(0x61, 0x61, 0x61));
    painter->setBrush(QColor(0x2a, 0x2a, 0x2a));
    painter->drawPath(path);
}

QRectF medClutEditorHistogram::boundingRect(void) const
{
    return QRectF( QPointF( 0.0, 0.0 ), size );
    // if (!values.isEmpty())
    //     return QRectF(QPointF((double)(values[0].x()),-size.height()),size);
    // else {
    //     qDebug() << "empty histogram";
    //     return QRectF(0, 0, 16, 16);
    // }
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

medClutEditorView::medClutEditorView(QWidget *parent) : QGraphicsView(parent)
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
    switch (event->key()) {
    case Qt::Key_Control:
	this->setDragMode(QGraphicsView::ScrollHandDrag); break;
    case Qt::Key_A:
    {
	medClutEditorTable * table = this->table();
	if ( table != NULL )
	    table->setSelectedAllVertices(
		!static_cast<bool>( event->modifiers() & Qt::ShiftModifier ) );
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
	if ( medClutEditorTable * table = this->table() )
	    table->deleteSelection();
	break;
    default:
	break;
    }
}

void medClutEditorView::keyReleaseEvent( QKeyEvent * event){
    if (event->key() == Qt::Key_Control)
        this->setDragMode(QGraphicsView::NoDrag);
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

    medClutEditorScene *scene;
    medClutEditorView  *view;
    medClutEditorHistogram *histogram;

    dtkAbstractData *dtk_data;
    medAbstractView *med_view;
};

medClutEditor::medClutEditor(QWidget *parent) : QWidget(parent)
{
    d = new medClutEditorPrivate;
    d->scene = new medClutEditorScene;
    d->view  = new medClutEditorView;
    d->view->setScene(d->scene);

    d->newAction    = new QAction("New table", this);
    d->colorAction  = new QAction("Edit color", this);
    d->deleteAction = new QAction("Delete selection", this);
    d->applyAction  = new QAction("Apply", this);

    connect(d->newAction,      SIGNAL(triggered()),
	    this,              SLOT(onNewTableAction()));
    connect(d->colorAction, SIGNAL(triggered()),
    	    this,              SLOT(onColorAction()));
    connect(d->deleteAction,   SIGNAL(triggered()),
    	    this,              SLOT(onDeleteAction()));
    connect(d->applyAction,    SIGNAL(triggered()),
	    this,              SLOT(onApplyTablesAction()));

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(d->view);
    this->setLayout(layout);
}

medClutEditor::~medClutEditor(void)
{
    delete d->newAction;
    delete d->colorAction;
    delete d->deleteAction;
    delete d->applyAction;

    delete d->scene;
    delete d->view;
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
}

void medClutEditor::initializeTable(void)
{
    this->deleteTable();

    medClutEditorTable *lut = new medClutEditorTable;
    lut->setSize( d->histogram->boundingRect() );
    lut->setRange( d->histogram->getRangeMin(), d->histogram->getRangeMax() );
    d->scene->addItem( lut );

    QRectF area = d->histogram->boundingRect();
    // int rangemin = d->histogram->getRangeMin();
    // int rangemax = d->histogram->getRangeMax();
    medClutEditorVertex * v1 =
	new medClutEditorVertex( area.left(), 0.2 * area.height(),
				 Qt::blue, lut );
    medClutEditorVertex * v2 =
	new medClutEditorVertex( area.center().x(), area.center().y(),
				 Qt::red, lut );
    medClutEditorVertex * v3 =
	new medClutEditorVertex( area.right(), 0.6 * area.height(),
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

void medClutEditor::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
	event->accept();

        QMenu menu("Color Lookup Table", this) ;
        menu.setWindowOpacity(0.8) ;
        menu.addAction(d->newAction);
        menu.addSeparator();
	menu.addAction(d->colorAction);
	menu.addAction(d->deleteAction);
        menu.addSeparator();
        menu.addAction(d->applyAction);
        menu.exec(mapFrom(this, QCursor::pos()));
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
    if (dtkAbstractDataImage *image =
	dynamic_cast<dtkAbstractDataImage *>(d->dtk_data)) {
        
//        int min_range = image->minRangeValue();
//        int max_range = image->maxRangeValue();
//        int size = max_range-min_range;

//        int *table = new int[4*size];
        
//        for(int i = 0 ; i < size ; i++) {
//            table[0*size+i] = ((double)i/(double)size)*255;
//            table[1*size+i] = ((double)i/(double)size)*255;
//            table[2*size+i] = ((double)i/(double)size)*255;
//            table[3*size+i] = 0;
//        }
        
//        foreach(QGraphicsItem *item, d->scene->items())
//            if(medClutEditorTable *lut = dynamic_cast<medClutEditorTable *>(item))
//            {
//                //lut->setup(min_range, max_range, size, table);

//            }
//        // d->dtk_view->setColorLookupTable(min_range, max_range, max_range-min_range, table);

        QList<double> scalars;
        QList<QColor> colors;
        foreach (QGraphicsItem *item, d->scene->items())
            if (medClutEditorTable *lut =
		dynamic_cast<medClutEditorTable *>(item)) {
                lut->setupTransferFunction(scalars,colors);
                d->med_view->setColorLookupTable(scalars,colors);
            }

    }
}
