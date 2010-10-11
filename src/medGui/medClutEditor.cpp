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

    void addValue(int intensity, int number);
    int getRangeMin();
    int getRangeMax();

public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    QRectF boundingRect(void) const;

private:
    QList<QPoint> values;
    QSizeF size;
};

medClutEditorHistogram::medClutEditorHistogram(QGraphicsItem *parent) :
        QGraphicsItem(parent),size()
{
    //this->setFlag(QGraphicsItem::ItemIsMovable, false);
    this->setZValue(-1000);
}

medClutEditorHistogram::~medClutEditorHistogram(void)
{

}

int medClutEditorHistogram::getRangeMin(void)
{
    return values[0].x();
}

int medClutEditorHistogram::getRangeMax(void)
{
    return values.back().x();
}

void medClutEditorHistogram::addValue(int intensity, int number)
{
    int x =0;
    if (!values.isEmpty() )
        x = intensity-values[0].x();
    int y = 200*-log10((double)1+number);
    if (-y>size.height()) size.setHeight(-y);

    if ((x>size.width()))
        size.setWidth(x);

    values << QPoint(intensity, y);
}

void medClutEditorHistogram::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QPainterPath path;
    path.moveTo(QPoint(values.first().x(), 0));
    foreach(QPoint point, values) path.lineTo(point);
    path.lineTo(QPoint(values.last().x(), 0));

    painter->setPen(QColor(0x61, 0x61, 0x61));
    painter->setBrush(QColor(0x2a, 0x2a, 0x2a));
    painter->drawPath(path);
}

QRectF medClutEditorHistogram::boundingRect(void) const
{
    if (!values.isEmpty())
    {
        //qDebug()<< "boundingRect Histogram" <<QRectF((double)(values[0].x()),(double)(values[0].y()),(double)(values.back().x()),(double)(values[0].y()));
        //qDebug()<<"boundingRect Histogram"<<QRectF(QPointF((double)(values[0].x()),-size.height()),size);
        return QRectF(QPointF((double)(values[0].x()),-size.height()),size);
    }
    else
    {
        qDebug() << "empty histogram";
        return QRectF(0, 0, 16, 16);
    }
}

// /////////////////////////////////////////////////////////////////
// medClutEditorScene
// /////////////////////////////////////////////////////////////////

class medClutEditorScene : public QGraphicsScene
{
public:
     medClutEditorScene(QObject *parent = 0);
    ~medClutEditorScene(void);
};

medClutEditorScene::medClutEditorScene(QObject *parent) : QGraphicsScene(parent)
{

}

medClutEditorScene::~medClutEditorScene(void)
{

}

// /////////////////////////////////////////////////////////////////
// medClutEditorView
// /////////////////////////////////////////////////////////////////

class medClutEditorView : public QGraphicsView
{
public:
     medClutEditorView(QWidget *parent = 0);
    ~medClutEditorView(void);

protected:
    void resizeEvent(QResizeEvent *event);
    void wheelEvent ( QWheelEvent * event );
    void keyReleaseEvent( QKeyEvent * event);
    void keyPressEvent( QKeyEvent * event);
};

medClutEditorView::medClutEditorView(QWidget *parent) : QGraphicsView(parent)
{
    this->setBackgroundBrush(QColor(0x00, 0x00, 0x00));
    this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    this->setCacheMode(CacheBackground);
    this->setViewportUpdateMode(FullViewportUpdate);
    this->setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform | QPainter::TextAntialiasing);

    this->setFrameStyle(QFrame::NoFrame);
    this->setAttribute(Qt::WA_MacShowFocusRect, false);
}

medClutEditorView::~medClutEditorView(void)
{

}

void medClutEditorView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);

    //qDebug() << this->sceneRect();
    this->fitInView(this->sceneRect(), Qt::KeepAspectRatio);
}

void medClutEditorView::wheelEvent ( QWheelEvent * event )
{
    double degrees =  exp(0.001*(double)(event->delta()));

    qreal factor = degrees;
    if (factor < 0 ) factor = 0 ;
    this->scale(factor,factor);
}

void medClutEditorView::keyPressEvent( QKeyEvent * event){
    if (event->key()==Qt::Key_Control)
        this->setDragMode(QGraphicsView::ScrollHandDrag);
}

void medClutEditorView::keyReleaseEvent( QKeyEvent * event){
    if (event->key()==Qt::Key_Control)
        this->setDragMode(QGraphicsView::NoDrag);
}



// /////////////////////////////////////////////////////////////////
// medClutEditor
// /////////////////////////////////////////////////////////////////

class medClutEditorPrivate
{   
public:
    QAction *newAction;
    QAction *clearAction;
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

    d->newAction = new QAction("New table", this);
    d->clearAction = new QAction("Clear tables", this);
    d->applyAction = new QAction("Apply", this);

    connect(d->newAction, SIGNAL(triggered()), this, SLOT(onNewTableAction()));
    connect(d->clearAction, SIGNAL(triggered()), this, SLOT(onClearTablesAction()));
    connect(d->applyAction, SIGNAL(triggered()), this, SLOT(onApplyTablesAction()));

    QHBoxLayout *layout = new QHBoxLayout;
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(d->view);
    this->setLayout(layout);
}

medClutEditor::~medClutEditor(void)
{
    delete d;
}

void medClutEditor::setData(dtkAbstractData *data)
{
    if (dtkAbstractDataImage *image =
	dynamic_cast<dtkAbstractDataImage *>(data)) {

        d->histogram = new medClutEditorHistogram;
        int min_range = image->minRangeValue();
        int max_range = image->maxRangeValue();

        for (int i = min_range ; i < max_range ; i++)
	    d->histogram->addValue(i, image->scalarValueCount(i-min_range));

        d->scene->addItem(d->histogram);

        foreach (QGraphicsItem *item, d->scene->items())
            if (medClutEditorTable *lut =
		dynamic_cast<medClutEditorTable *>(item))
	        lut->setAllowedArea( d->histogram->boundingRect() );

	// int log_max =
	//   static_cast<int>(log10((double)1+image->scalarValueMaxCount()));
       // d->scene->setSceneRect(min_range,           200 * -log_max,
//			       max_range-min_range, 200 *  log_max);

        //d->scene->setSceneRect(histogram->boundingRect());
        d->dtk_data = image;
    }
}

void medClutEditor::setView(medAbstractView *view)
{
    d->med_view = view;
}

void medClutEditor::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        QMenu menu("Color Lookup Table", this) ;
        menu.setWindowOpacity(0.8) ;
        menu.addAction(d->newAction) ;
        menu.addAction(d->clearAction) ;
        menu.addSeparator();
        menu.addAction(d->applyAction) ;
        menu.exec(mapFrom(this, QCursor::pos())) ;
    }
}

void medClutEditor::onNewTableAction(void)
{
    medClutEditorTable *lut = new medClutEditorTable;
    lut->setAllowedArea( d->histogram->boundingRect() );
    d->scene->addItem(lut);

    int rangemin = d->histogram->getRangeMin();
    int rangemax = d->histogram->getRangeMax();
    int upperbound = d->histogram->boundingRect().height();
    medClutEditorVertex *v1 =
      new medClutEditorVertex( rangemin, 300, Qt::blue, upperbound, lut );
    medClutEditorVertex *v2 =
      new medClutEditorVertex( (rangemax + rangemin) / 2, 500, Qt::red,
			       upperbound, lut );
    medClutEditorVertex *v3 =
      new medClutEditorVertex( rangemax, 550, Qt::yellow, upperbound, lut );
    // medClutEditorVertex *v4 = new medClutEditorVertex(260,  60, Qt::white);
    d->scene->addItem(v1);
    d->scene->addItem(v2);
    d->scene->addItem(v3);
    // d->scene->addItem(v4);
    
    lut->addVertex(v1);
    lut->addVertex(v2);
    lut->addVertex(v3);
    // lut->addVertex(v4);
}

void medClutEditor::onClearTablesAction(void)
{
    foreach(QGraphicsItem *item, d->scene->items())
        if(dynamic_cast<medClutEditorTable *>(item))
            d->scene->removeItem(item);
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



// /////////////////////////////////////////////////////////////////
// medClutEditorVertex
// /////////////////////////////////////////////////////////////////

class medClutEditorVertexPrivate
{
public:
    QColor fgColor;
    QColor bgColor;
    int upperBd;
    QAction *deleteAction;
    QAction *setColorAction;
    bool isSelectedForDeletion;

    medClutEditorVertex * prev;
    medClutEditorVertex * next;
};

medClutEditorVertex::medClutEditorVertex(int x, int y,
					 QColor color,
					 int upperBound,
					 QGraphicsItem * parent)
  : QGraphicsItem(parent)
{
    d = new medClutEditorVertexPrivate;
    d->fgColor = color;
    d->bgColor = QColor(0xc0, 0xc0, 0xc0,0x0);
    d->upperBd = upperBound;
    this->setPos(x, -y);
    setAlpha();
    this->setZValue(1);

    this->setFlag(QGraphicsItem::ItemIsMovable, true);

    d->deleteAction = new QAction("Delete", 0);
    d->setColorAction = new QAction("Edit Color", 0);

    
    connect(d->setColorAction, SIGNAL(triggered()),
	    this, SLOT(onSetColorAction()));

    if ( medClutEditorTable * table =
    	 dynamic_cast< medClutEditorTable * >( parent ) ) {
    	connect(d->deleteAction, SIGNAL(triggered()),
    		this, SLOT(onDeleteAction()));
    	connect(this, SIGNAL(deleteFromTable(medClutEditorVertex *)),
    		table, SLOT(onDeleteVertex(medClutEditorVertex *)));
    }

    d->isSelectedForDeletion = false;

    d->prev = NULL;
    d->next = NULL;
}

medClutEditorVertex::~medClutEditorVertex(void)
{
    delete d->deleteAction;
    delete d->setColorAction;
    delete d;
}

void medClutEditorVertex::setPrev( medClutEditorVertex * v )
{
    d->prev = v;
}
 
void medClutEditorVertex::setNext( medClutEditorVertex * v )
{
    d->next = v;
}

int medClutEditorVertex::upperBound()
{
    return d->upperBd;

}

void medClutEditorVertex::interpolate()
{
  if ( d->prev != NULL && d->next == NULL )
      d->fgColor = d->prev->color();
  else if ( d->prev == NULL && d->next != NULL )
      d->fgColor = d->next->color();
  else if ( d->prev != NULL && d->next != NULL ) {
      if ( ( d->prev->color().alpha() == 0 ) &&
	   ( d->next->color().alpha() > 0 ) )
	  d->fgColor = d->next->color();
      else if ( ( d->prev->color().alpha() > 0 ) &&
		( d->next->color().alpha() == 0 ) )
	  d->fgColor = d->prev->color();
      else if ( ( this->x() - d->prev->x() ) < ( d->next->x() - this->x() ) )
	  d->fgColor = d->prev->color();
      else	  
	  d->fgColor = d->next->color();
  }
}

void medClutEditorVertex::onSetColorAction()
{
    qDebug() << (long int) this;
    qDebug() << this->color();

    QColor color = QColorDialog::getColor(d->fgColor, 0);

    if(color.isValid()) {
	// int alpha = d->fgColor.alpha();
        d->fgColor = color;
	// d->fgColor.setAlpha( alpha );
        this->update();
    }
}

void medClutEditorVertex::onDeleteAction()
{
    emit deleteFromTable( this );
}

void medClutEditorVertex::paint(QPainter *painter,
				const QStyleOptionGraphicsItem *option,
				QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen( d->isSelectedForDeletion ? Qt::red : Qt::black);
    setAlpha();
    painter->setBrush(d->bgColor); painter->drawEllipse(-40, -40, 80, 80);
    painter->setBrush(d->fgColor); painter->drawEllipse(-25, -25, 50, 50);
}

QRectF medClutEditorVertex::boundingRect(void) const
{
    return QRectF(-40, -40, 80, 80);
}

QPoint medClutEditorVertex::position(void) const
{
    return QPoint(this->x(), this->y());
}

QColor medClutEditorVertex::color(void) const
{
    return d->fgColor;
}

void medClutEditorVertex::forceGeometricalConstraints()
{
    if ( medClutEditorTable * table =
	 dynamic_cast< medClutEditorTable * >( this->parentItem() ) ) {
	QRect & limits = table->allowedArea();
	if ( this->x() < limits.left() ) {
	    this->setX( limits.left() );
	    this->update();
	}
	if ( this->x() > limits.right() ) {
	    this->setX( limits.right() );
	    this->update();
	}
	if ( this->y() < limits.top() ) {
	    this->setY( limits.top() );
	    this->update();
	}
	if ( this->y() > limits.bottom() ) {
	    this->setY( limits.bottom() );
	    this->update();
	}
    }

    if ( d->prev != NULL && d->prev->x() > this->x() ) {
      	d->prev->setX( this->x() );
      	d->prev->forceGeometricalConstraints();
    }
    if ( d->next != NULL && d->next->x() < this->x() ) {
      	d->next->setX( this->x() );
      	d->next->forceGeometricalConstraints();
    }
}

void medClutEditorVertex::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    this->QGraphicsItem::mouseMoveEvent( event );

    forceGeometricalConstraints();
}

void medClutEditorVertex::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);

    if ( d->isSelectedForDeletion ) {
	d->deleteAction->trigger();
    }
    else {
	QColor color = QColorDialog::getColor(d->fgColor, 0);

	if(color.isValid()) {
	    d->fgColor = color;
	    this->update();
	}
    }
}

// void medClutEditorVertex::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
// {
//     Q_UNUSED(event);
// }

void medClutEditorVertex::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
	d->isSelectedForDeletion = !d->isSelectedForDeletion;
	this->update();
	event->accept();
        // QMenu menu("Edit Marker", 0) ;
        // menu.setWindowOpacity(0.8) ;
        // menu.addAction(d->setColorAction);
        // menu.addAction(d->deleteAction) ;
        // menu.exec(event->screenPos());
    }

    this->QGraphicsItem::mousePressEvent(event);
}

void medClutEditorVertex::setAlpha()
{
    float alpha = qAbs(this->y()/(float)d->upperBd);
    // int height = 0;
    // if ( medClutEditorTable * table =
    // 	 dynamic_cast< medClutEditorTable * >( this->parentItem() ) )
    //   height = table->allowedArea().height();
    // qDebug() << d->upperBd << " | " << height;
    if ( alpha > 1 ) alpha = 1;
    d->fgColor.setAlphaF(alpha);
}

// /////////////////////////////////////////////////////////////////
// medClutEditorTable
// /////////////////////////////////////////////////////////////////

class medClutEditorTablePrivate
{
 public:
    QRect limits;
    QList<medClutEditorVertex *> vertices;
};


medClutEditorTable::medClutEditorTable(QGraphicsItem *parent)
  : QGraphicsItem(parent)
{
    d = new medClutEditorTablePrivate;
    d->limits.setCoords( INT_MIN, INT_MIN, INT_MAX, INT_MAX );

    //this->setFlag(QGraphicsItem::ItemIsMovable, true);
    this->setFlag(QGraphicsItem::ItemIsFocusable, true);
    this->setZValue(0);
}

medClutEditorTable::~medClutEditorTable(void)
{
    delete d;
}

void medClutEditorTable::setAllowedArea( QRect rect )
{
    d->limits = rect;
}

void medClutEditorTable::setAllowedArea( QRectF rect )
{
    d->limits = QRect( rect.topLeft().toPoint(),
		    rect.bottomRight().toPoint() );
}

QRect & medClutEditorTable::allowedArea()
{
  return d->limits;
}

void medClutEditorTable::linkVertices()
{
    qSort(d->vertices.begin(), d->vertices.end(),
	  medClutEditorVertex::LessThan);

    medClutEditorVertex * lastVertex = NULL;
    foreach (medClutEditorVertex * vertex, d->vertices) {
	vertex->setPrev( lastVertex );
	if ( lastVertex != NULL )
	    lastVertex->setNext( vertex );

	lastVertex = vertex;
    }
    if ( lastVertex != NULL )
	lastVertex->setNext( NULL );
}

void medClutEditorTable::addVertex( medClutEditorVertex *vertex,
				    bool interpolate )
{
    d->vertices << vertex;

    vertex->setParentItem(this);

    linkVertices();

    if (interpolate)
	vertex->interpolate();
}

void medClutEditorTable::onDeleteVertex(medClutEditorVertex *vertex)
{
    qDebug() << (long int) vertex;
    qDebug() << vertex->color();

    d->vertices.removeAll( vertex );
    delete vertex;

    linkVertices();
}

void medClutEditorTable::setup(int min, int max, int size, int *table)
{
    for (int i = 0 ; i < d->vertices.count()-1 ; i++) {

	int x1 = qMax(min, d->vertices.at(i+0)->position().x());
	int x2 = qMin(max, d->vertices.at(i+1)->position().x());
        
	int r1 = d->vertices.at(i+0)->color().red();
	int r2 = d->vertices.at(i+1)->color().red();
	int g1 = d->vertices.at(i+0)->color().green();
	int g2 = d->vertices.at(i+1)->color().green();
	int b1 = d->vertices.at(i+0)->color().blue();
	int b2 = d->vertices.at(i+1)->color().blue();
	int a1 = d->vertices.at(i+0)->position().y();
	int a2 = d->vertices.at(i+1)->position().y();

	for (int x = x1 ; x < x2 ; x++) {

	    int r = ((double)(x-x1)/(double)(x2-x1))*(r2-r1)+r1;
	    int g = ((double)(x-x1)/(double)(x2-x1))*(g2-g1)+g1;
	    int b = ((double)(x-x1)/(double)(x2-x1))*(b2-b1)+b1;
	    int a = ((double)(x-x1)/(double)(x2-x1))*(a2-a1)+a1;

	    Q_UNUSED(a);

	    table[x-min+(0*size)] = r;
	    table[x-min+(1*size)] = g;
	    table[x-min+(2*size)] = b;
	    table[x-min+(3*size)] = 255;
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
        scalars << vertex->position().x();
    }
}

void medClutEditorTable::paint(QPainter *painter,
			       const QStyleOptionGraphicsItem *option,
			       QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    int n_points = d->vertices.size();
    QPoint *points = new QPoint[n_points + 2];
    // points[0]            = QPoint(0, vertices.first()->position().y());
    // points[n_points + 1] = QPoint(0, vertices.last()->position().y());
    points[0]            = QPoint(d->vertices.first()->position().x(), 0);
    points[n_points + 1] = QPoint(d->vertices.last()->position().x(),  0);
    for (int i = 0 ; i < n_points; i++) {
        points[i+1] = d->vertices.at(i)->position();
    }
    QPen pen(Qt::lightGray, 2);

    int xmin = d->vertices.first()->x();
    int xmax = d->vertices.last()->x();
    int ymin = INT_MAX; 
    foreach (medClutEditorVertex *vertex, d->vertices)
	ymin = qMin(ymin, (int)vertex->y());

    QLinearGradient linearGradient(xmin, 0, xmax, 0);
    foreach (medClutEditorVertex *vertex, d->vertices) {
        qreal position;
        position = (vertex->x()-xmin)/(xmax-xmin);
        position = qMax(0.0, position);
        position = qMin(position, 1.0);
        QColor color = vertex->color();
        color.setAlpha(128);
        linearGradient.setColorAt(position, color);
    }

    painter->setPen(pen);
    painter->setBrush(linearGradient);
    painter->setCompositionMode(QPainter::CompositionMode_SourceOver);
    painter->drawPolygon(points, n_points + 2);

    int posx = (xmax+xmin)/2;
    int posy = ymin/2;

    painter->setPen(Qt::lightGray);
    painter->setBrush(Qt::black);
    painter->drawRect(posx-20, posy-20, 40, 40);

    delete[] points;
}

QRectF medClutEditorTable::boundingRect(void) const
{
    int xmin, xmax, ymin, ymax;
    xmin = ymin = INT_MAX;
    xmax = ymax = INT_MIN;

    foreach (medClutEditorVertex *vertex, d->vertices) {
	xmin = qMin(xmin, vertex->position().x());
	xmax = qMax(xmax, vertex->position().x());
	ymin = qMin(ymin, vertex->position().y());
	ymax = qMax(ymax, vertex->position().y());
        // if (vertex->x() < xmin) xmin = vertex->x();
        // if (vertex->x() > xmax) xmax = vertex->x();
        // if (vertex->y() < ymin) ymin = vertex->y();
        // if (vertex->y() > ymax) ymax = vertex->y();
    }

    return QRectF(xmin, ymin, xmax-xmin, qAbs(ymin));
}

void medClutEditorTable::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_F: this->setZValue(this->zValue()+1); break;
    case Qt::Key_B: this->setZValue(this->zValue()-1); break;
    case Qt::Key_X: this->scene()->removeItem(this); break;
    default: break;
    }
    qDebug() << event->key();
}


void medClutEditorTable::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    addVertex( new medClutEditorVertex( event->pos().x(), -event->pos().y(),
					Qt::yellow,
					d->vertices[0]->upperBound(),
					this ),
	       true );
}
