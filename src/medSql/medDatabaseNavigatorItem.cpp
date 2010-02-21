/* medDatabaseNavigatorItem.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:39:35 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Sun Feb 21 13:39:16 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 10
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medDatabaseNavigatorItem.h"
#include "medDatabaseNavigatorItemLoader.h"

#include <QtCore>
#include <QtGui>

// /////////////////////////////////////////////////////////////////
// medDatabaseNavigatorItemLabel
// /////////////////////////////////////////////////////////////////

medDatabaseNavigatorItemLabel::medDatabaseNavigatorItemLabel(QGraphicsItem *parent) : QObject(), QGraphicsPixmapItem(QPixmap(":/pixmaps/database_navigator_item_label.png"), parent)
{

}

medDatabaseNavigatorItemLabel::~medDatabaseNavigatorItemLabel(void)
{

}

void medDatabaseNavigatorItemLabel::setText(const QString& text)
{
    this->text = text;
}

void medDatabaseNavigatorItemLabel::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QGraphicsPixmapItem::paint(painter, option, widget);

    painter->save();
    painter->setPen(Qt::white);
    painter->setFont(QFont("Helvetica", 11, 1, 0));
    painter->drawText(option->rect.translated(0, 7), Qt::AlignRight | Qt::TextSingleLine, text);
    painter->restore();
}

// /////////////////////////////////////////////////////////////////
// medDatabaseNavigatorItemCount
// /////////////////////////////////////////////////////////////////

medDatabaseNavigatorItemCount::medDatabaseNavigatorItemCount(QGraphicsItem *parent) : QObject(), QGraphicsPixmapItem(QPixmap(":/pixmaps/database_navigator_item_count.png"), parent)
{
    this->count = 0;
}

medDatabaseNavigatorItemCount::~medDatabaseNavigatorItemCount(void)
{

}

void medDatabaseNavigatorItemCount::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QGraphicsPixmapItem::paint(painter, option, widget);

    painter->save();
    painter->setPen(Qt::white);
    painter->setFont(QFont("Helvetica", 11, 1, 0));
    painter->drawText(option->rect.translated(0, 7), Qt::AlignHCenter, QString::number(count));
    painter->restore();
}

// /////////////////////////////////////////////////////////////////
// medDatabaseNavigatorItemChecker
// /////////////////////////////////////////////////////////////////

class medDatabaseNavigatorItemCheckerPrivate
{
public:
    bool      checked;
    QPixmap unchecked_pixmap;
    QPixmap   checked_pixmap;
};

medDatabaseNavigatorItemChecker::medDatabaseNavigatorItemChecker(QGraphicsItem *parent) : QObject(), QGraphicsPixmapItem(parent), d(new medDatabaseNavigatorItemCheckerPrivate)
{
    d->unchecked_pixmap = QPixmap(":/pixmaps/database_navigator_item_checker_unchecked.png");
    d->checked_pixmap   = QPixmap(":/pixmaps/database_navigator_item_checker_checked.png");
    d->checked = false;

    this->setPixmap(d->unchecked_pixmap);
}

medDatabaseNavigatorItemChecker::~medDatabaseNavigatorItemChecker(void)
{
    delete d;

    d = NULL;
}

void medDatabaseNavigatorItemChecker::check(void)
{
    this->setChecked(true);
}

void medDatabaseNavigatorItemChecker::uncheck(void)
{
    this->setChecked(false);
}

void medDatabaseNavigatorItemChecker::setChecked(bool value)
{
    d->checked = value;

    if (d->checked) {
        this->setPixmap(d->checked_pixmap);
        emit checked();
    } else {
        this->setPixmap(d->unchecked_pixmap);
        emit unchecked();
    }
}

void medDatabaseNavigatorItemChecker::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    d->checked = !d->checked;

    if (d->checked) {
        this->setPixmap(d->checked_pixmap);
        emit checked();
    } else {
        this->setPixmap(d->unchecked_pixmap);
        emit unchecked();
    }
}

// /////////////////////////////////////////////////////////////////
// medDatabaseNavigatorItem
// /////////////////////////////////////////////////////////////////

// -- TODO
//
// Passing database indexes as arguments to the constructor is more than
// sufficiant.
//
// Attributes such as path to thumbnail, count and text should be retreived
// from the database.

class medDatabaseNavigatorItemPrivate
{
public:
    int patientId;
    int studyId;
    int seriesId;
    int imageId;

    QString path;

    medDatabaseNavigatorItemCount *item_count;
    medDatabaseNavigatorItemChecker *item_checker_single;
    medDatabaseNavigatorItemChecker *item_checker_multi;
    medDatabaseNavigatorItemLabel *item_label;

    QPropertyAnimation *item_count_fading_animation;
    QPropertyAnimation *item_checker_single_fading_animation;
    QPropertyAnimation *item_checker_multi_fading_animation;
    QPropertyAnimation *item_label_fading_animation;
    QParallelAnimationGroup *fading_animation;
};

medDatabaseNavigatorItem::medDatabaseNavigatorItem(int patientId, int studyId, int seriesId, int imageId, const QString &path, QGraphicsItem *parent) : QObject(), QGraphicsPixmapItem(QPixmap(":/pixmap/thumbnail_default.tiff"), parent), d(new medDatabaseNavigatorItemPrivate)
{
    d->patientId = patientId;
    d->studyId = studyId;
    d->seriesId = seriesId;
    d->imageId = imageId;

    d->path = path;

    d->item_count = new medDatabaseNavigatorItemCount(this);
    d->item_count->setPos(5, 5);
//    d->item_count->setVisible(false);
    d->item_count->setOpacity(0.0);

    d->item_checker_single= new medDatabaseNavigatorItemChecker(this);
    d->item_checker_single->setPos(98, 5);
//    d->item_checker_single->setVisible(false);
    d->item_checker_single->setOpacity(0.0);

    d->item_checker_multi = new medDatabaseNavigatorItemChecker(this);
    d->item_checker_multi->setPos(98, 30);
//    d->item_checker_multi->setVisible(false);
    d->item_checker_multi->setOpacity(0.0);

    d->item_label = new medDatabaseNavigatorItemLabel(this);
    d->item_label->setPos(5, 98);
//    d->item_label->setVisible(false);
    d->item_label->setOpacity(0.0);
    d->item_label->setText(path);

    connect(d->item_checker_single, SIGNAL(  checked()), this, SLOT(  checked()));
    connect(d->item_checker_single, SIGNAL(unchecked()), this, SLOT(unchecked()));
    connect(d->item_checker_single, SIGNAL(  checked()), d->item_checker_multi, SLOT(check()));

    // Setup animation

    d->item_count_fading_animation = new QPropertyAnimation(d->item_count, "opacity", this);
    d->item_count_fading_animation->setDuration(150);

    d->item_checker_single_fading_animation = new QPropertyAnimation(d->item_checker_single, "opacity", this);
    d->item_checker_single_fading_animation->setDuration(150);

    d->item_checker_multi_fading_animation = new QPropertyAnimation(d->item_checker_multi, "opacity", this);
    d->item_checker_multi_fading_animation->setDuration(150);

    d->item_label_fading_animation = new QPropertyAnimation(d->item_label, "opacity", this);
    d->item_label_fading_animation->setDuration(150);

    d->fading_animation = new QParallelAnimationGroup(this);
    d->fading_animation->addAnimation(d->item_count_fading_animation);
    d->fading_animation->addAnimation(d->item_checker_single_fading_animation);
    d->fading_animation->addAnimation(d->item_checker_multi_fading_animation);
    d->fading_animation->addAnimation(d->item_label_fading_animation);

    // Load thumbnail

    medDatabaseNavigatorItemLoader *loader = new medDatabaseNavigatorItemLoader(path);

    connect(loader, SIGNAL(completed(const QImage&)), this, SLOT(setImage(const QImage&)));

    QThreadPool::globalInstance()->start(loader);

    this->setAcceptHoverEvents(true);
}

medDatabaseNavigatorItem::~medDatabaseNavigatorItem(void)
{
    delete d;

    d = NULL;
}

medDatabaseNavigatorItem *medDatabaseNavigatorItem::clone(void)
{
    return new medDatabaseNavigatorItem(d->patientId, d->studyId, d->seriesId, d->imageId, d->path);
}

void medDatabaseNavigatorItem::setup(void)
{
    // Retrieve attributes from the database
}

int medDatabaseNavigatorItem::patientId(void) const
{
    return d->patientId;
}

int medDatabaseNavigatorItem::studyId(void) const
{
    return d->studyId;
}

int medDatabaseNavigatorItem::seriesId(void) const
{
    return d->seriesId;
}

int medDatabaseNavigatorItem::imageId(void) const
{
    return d->imageId;
}

QString medDatabaseNavigatorItem::path(void) const
{
    return d->path;
}

void medDatabaseNavigatorItem::checked(void)
{
    if(d->imageId >= 0)
        emit imageClicked(d->imageId);

    else if(d->seriesId >= 0)
        emit seriesClicked(d->seriesId);

    else if(d->studyId >= 0)
        emit studyClicked(d->studyId);

    else if(d->patientId >= 0)
        emit patientClicked(d->patientId);
}

void medDatabaseNavigatorItem::unchecked(void)
{

}

void medDatabaseNavigatorItem::setImage(const QImage& image)
{
    this->setPixmap(QPixmap::fromImage(image));
}

void medDatabaseNavigatorItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    d->item_count_fading_animation->setStartValue(0.0);
    d->item_count_fading_animation->setEndValue(1.0);
//    d->item_count_fading_animation->setEasingCurve(QEasingCurve::InQuart);

    d->item_checker_single_fading_animation->setStartValue(0.0);
    d->item_checker_single_fading_animation->setEndValue(1.0);
//    d->item_checker_single_fading_animation->setEasingCurve(QEasingCurve::InQuart);

    d->item_checker_multi_fading_animation->setStartValue(0.0);
    d->item_checker_multi_fading_animation->setEndValue(1.0);
//    d->item_checker_multi_fading_animation->setEasingCurve(QEasingCurve::InQuart);

    d->item_label_fading_animation->setStartValue(0.0);
    d->item_label_fading_animation->setEndValue(1.0);
//    d->item_label_fading_animation->setEasingCurve(QEasingCurve::InQuart);

    d->fading_animation->start();
}

void medDatabaseNavigatorItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    d->item_count_fading_animation->setStartValue(1.0);
    d->item_count_fading_animation->setEndValue(0.0);
//    d->item_count_fading_animation->setEasingCurve(QEasingCurve::OutQuart);

    d->item_checker_single_fading_animation->setStartValue(1.0);
    d->item_checker_single_fading_animation->setEndValue(0.0);
//    d->item_checker_single_fading_animation->setEasingCurve(QEasingCurve::OutQuart);

    d->item_checker_multi_fading_animation->setStartValue(1.0);
    d->item_checker_multi_fading_animation->setEndValue(0.0);
//    d->item_checker_multi_fading_animation->setEasingCurve(QEasingCurve::OutQuart);

    d->item_label_fading_animation->setStartValue(1.0);
    d->item_label_fading_animation->setEndValue(0.0);
//    d->item_label_fading_animation->setEasingCurve(QEasingCurve::OutQuart);

    d->fading_animation->start();
}

void medDatabaseNavigatorItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    Q_UNUSED(event);
}

void medDatabaseNavigatorItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QMimeData *data = new QMimeData;
    data->setImageData(this->pixmap());
    
    QDrag *drag = new QDrag(this->scene()->views().first());
    drag->setMimeData(data);
    drag->setPixmap(this->pixmap());
    drag->setHotSpot(QPoint(drag->pixmap().width()/2, drag->pixmap().height()/2));
    drag->start();
}
