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

#include "medClutEditor.h"

#include <math.h>

// /////////////////////////////////////////////////////////////////
// medClutEditorHistogram
// /////////////////////////////////////////////////////////////////

class medClutEditorVertex : public QGraphicsItem
{
public:
     medClutEditorVertex(int x, int y, QColor color = Qt::yellow, QGraphicsItem *parent = 0);
    ~medClutEditorVertex(void);

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    QRectF boundingRect(void) const;
    QPoint position(void) const;
    QColor color(void) const;

protected:
    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);

private:
    QColor fgColor;
    QColor bgColor;
};

medClutEditorVertex::medClutEditorVertex(int x, int y, QColor color, QGraphicsItem *parent) : QGraphicsItem(parent)
{
    this->fgColor = color;
    this->bgColor = QColor(0xc0, 0xc0, 0xc0);

    this->setPos(x, -y);
    this->setZValue(1);

    this->setFlag(QGraphicsItem::ItemIsMovable, true);
}

medClutEditorVertex::~medClutEditorVertex(void)
{

}

void medClutEditorVertex::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    painter->setPen(Qt::black);
    painter->setBrush(bgColor); painter->drawEllipse(-40, -40, 80, 80);
    painter->setBrush(fgColor); painter->drawEllipse(-25, -25, 50, 50);
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
    return fgColor;
}

void medClutEditorVertex::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);

    QColor color = QColorDialog::getColor(fgColor, 0);

    if(color.isValid()) {
        this->fgColor = color;
        this->update();
    }
}

static bool medClutEditorVertexLessThan(const medClutEditorVertex *v1, const medClutEditorVertex *v2) {
    return (v1->position().x() < v2->position().x());
}

// /////////////////////////////////////////////////////////////////
// medClutEditorTable
// /////////////////////////////////////////////////////////////////

class medClutEditorTable : public QGraphicsItem
{
public:
     medClutEditorTable(QGraphicsItem *parent = 0);
    ~medClutEditorTable(void);

    void addVertex(medClutEditorVertex *vertex);

    void setup(int min, int max, int size, int *table);

public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    QRectF boundingRect(void) const;

    void keyPressEvent(QKeyEvent *event);

private:
    QList<medClutEditorVertex *> vertices;
};

medClutEditorTable::medClutEditorTable(QGraphicsItem *parent) : QGraphicsItem(parent)
{
    //this->setFlag(QGraphicsItem::ItemIsMovable, true);
    this->setFlag(QGraphicsItem::ItemIsFocusable, true);
    this->setZValue(0);
}

medClutEditorTable::~medClutEditorTable(void)
{

}

void medClutEditorTable::addVertex(medClutEditorVertex *vertex)
{
    vertices << vertex;

    vertex->setParentItem(this);

    qSort(vertices.begin(), vertices.end(), medClutEditorVertexLessThan);
}

void medClutEditorTable::setup(int min, int max, int size, int *table)
{
    for(int i = 0 ; i < vertices.count()-1 ; i++) {

        int x1 = qMax(min, (int)vertices.at(i+0)->position().x());
        int x2 = qMin(max, (int)vertices.at(i+1)->position().x());
        
        int r1 =  vertices.at(i+0)->color().red();
        int r2 =  vertices.at(i+1)->color().red();
        int g1 =  vertices.at(i+0)->color().green();
        int g2 =  vertices.at(i+1)->color().green();
        int b1 =  vertices.at(i+0)->color().blue();
        int b2 =  vertices.at(i+1)->color().blue();
        int a1 =  vertices.at(i+0)->y();
        int a2 =  vertices.at(i+1)->y();

        for(int x = x1 ; x < x2 ; x++) {

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

void medClutEditorTable::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    int n_points = vertices.size()+2;
	QPoint *points = new QPoint[n_points];
    points[0] = QPoint(vertices.first()->position().x(), 0);
    for(int i = 0 ; i < vertices.count() ; i++)
        points[i+1] = vertices.at(i)->position();
    points[vertices.count()+1] = QPoint(vertices.last()->position().x(), 0);

    QPen pen(Qt::lightGray, 2);

    int xmin = vertices.first()->x();
    int xmax = vertices.last()->x();
    int ymin = INT_MAX; 
    foreach(medClutEditorVertex *vertex, vertices) ymin = qMin(ymin, (int)vertex->y());

    QLinearGradient linearGradient(xmin, 0, xmax, 0);
    foreach(medClutEditorVertex *vertex, vertices) {
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
    painter->drawPolygon(points, n_points);

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

    foreach(medClutEditorVertex *vertex, vertices) {
        if(vertex->x() < xmin) xmin = vertex->x();
        if(vertex->x() > xmax) xmax = vertex->x();
        if(vertex->y() < ymin) ymin = vertex->y();
        if(vertex->y() > ymax) ymax = vertex->y();
    }

    return QRectF(xmin, ymin, xmax-xmin, qAbs(ymin));
}

void medClutEditorTable::keyPressEvent(QKeyEvent *event)
{
    switch(event->key()) {
    case Qt::Key_F: this->setZValue(this->zValue()+1); break;
    case Qt::Key_B: this->setZValue(this->zValue()-1); break;
    case Qt::Key_X: this->scene()->removeItem(this); break;
    default: break;
    }
}

// /////////////////////////////////////////////////////////////////
// medClutEditorHistogram
// /////////////////////////////////////////////////////////////////

class medClutEditorHistogram : public QGraphicsItem
{
public:
     medClutEditorHistogram(QGraphicsItem *parent = 0);
    ~medClutEditorHistogram(void);

    void addValue(int intensity, int number);

public:
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

    QRectF boundingRect(void) const;

private:
    QList<QPoint> values;
};

medClutEditorHistogram::medClutEditorHistogram(QGraphicsItem *parent) : QGraphicsItem(parent)
{
    this->setFlag(QGraphicsItem::ItemIsMovable, false);
    this->setZValue(-1000);
}

medClutEditorHistogram::~medClutEditorHistogram(void)
{

}

void medClutEditorHistogram::addValue(int intensity, int number)
{
    values << QPoint(intensity, 200*-log10((double)1+number));
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
    return QRectF(0, 0, 16, 16);
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

    this->fitInView(this->sceneRect(), Qt::KeepAspectRatio);
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

    dtkAbstractData *dtk_data;
    dtkAbstractView *dtk_view;
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

}

void medClutEditor::setData(dtkAbstractData *data)
{
    if(dtkAbstractDataImage *image = dynamic_cast<dtkAbstractDataImage *>(data)) {
        medClutEditorHistogram *histogram = new medClutEditorHistogram;
        int min_range = image->minRangeValue();
        int max_range = image->maxRangeValue();
	qDebug() << "min: " << min_range << "\nmax: " << max_range;


        for(int i = min_range ; i < max_range ; i++)
            histogram->addValue(i, image->scalarValueCount(i-min_range));

        d->scene->addItem(histogram);
        d->scene->setSceneRect(
            min_range,           200*-log10((double)1+image->scalarValueMaxCount()),
            max_range-min_range, 200* log10((double)1+image->scalarValueMaxCount()));
        d->dtk_data = image;
    }
}

void medClutEditor::setView(dtkAbstractView *view)
{
    d->dtk_view = view;
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
    medClutEditorVertex *v1 = new medClutEditorVertex(-1024, 300, Qt::blue);
    medClutEditorVertex *v2 = new medClutEditorVertex( 1379, 500, Qt::red);
    medClutEditorVertex *v3 = new medClutEditorVertex(  130, 550, Qt::yellow);
    // medClutEditorVertex *v4 = new medClutEditorVertex(260,  60, Qt::white);
    d->scene->addItem(v1);
    d->scene->addItem(v2);
    d->scene->addItem(v3);
    // d->scene->addItem(v4);
    
    medClutEditorTable *lut = new medClutEditorTable;
    lut->addVertex(v1);
    lut->addVertex(v2);
    lut->addVertex(v3);
    // lut->addVertex(v4);
    d->scene->addItem(lut);
}

void medClutEditor::onClearTablesAction(void)
{
    foreach(QGraphicsItem *item, d->scene->items())
        if(dynamic_cast<medClutEditorTable *>(item))
            d->scene->removeItem(item);
}

void medClutEditor::onApplyTablesAction(void)
{
    if(dtkAbstractDataImage *image = dynamic_cast<dtkAbstractDataImage *>(d->dtk_data)) {
        
        int min_range = image->minRangeValue();
        int max_range = image->maxRangeValue();
        int size = max_range-min_range;

        int *table = new int[4*size];
        
        for(int i = 0 ; i < size ; i++) {
            table[0*size+i] = ((double)i/(double)size)*255;
            table[1*size+i] = ((double)i/(double)size)*255;
            table[2*size+i] = ((double)i/(double)size)*255;
            table[3*size+i] = 0;
        }
        
        foreach(QGraphicsItem *item, d->scene->items())
            if(medClutEditorTable *lut = dynamic_cast<medClutEditorTable *>(item))
                lut->setup(min_range, max_range, size, table);
        
        // d->dtk_view->setColorLookupTable(min_range, max_range, max_range-min_range, table);
    }
}
