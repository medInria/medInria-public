/* medDatabasePreview.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:42:18 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Sat Mar 20 20:06:00 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 118
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medDatabaseController.h"
#include "medDatabasePreview.h"
#include "medDatabasePreviewController.h"
#include "medDatabasePreviewItem.h"
#include "medDatabasePreviewItemGroup.h"
#include "medDatabasePreviewScene.h"
#include "medDatabasePreviewSelector.h"
#include "medDatabasePreviewTooltip.h"
#include "medDatabasePreviewView.h"

#include <QtCore>
#include <QtGui>

#include <dtkCore/dtkGlobal.h>

class medDatabasePreviewPrivate
{
public:
    int level;

    int current_index_patient;
    int current_index_study;
    int current_index_series;
    int current_index_image;

    medDatabasePreviewScene *scene;
    medDatabasePreviewView *view;
    medDatabasePreviewSelector *selector;

    medDatabasePreviewItemGroup *series_group;
    medDatabasePreviewItemGroup *image_group;

    QPropertyAnimation *series_animation;
    QPropertyAnimation *image_animation;
    QParallelAnimationGroup *animation;

    QPropertyAnimation *selector_position_animation;
    QPropertyAnimation *selector_rect_animation;
    QParallelAnimationGroup *selector_animation;

    medDatabasePreviewItem *target;
};

medDatabasePreview::medDatabasePreview(QWidget *parent) : QFrame(parent), d(new medDatabasePreviewPrivate)
{
    d->level = 0;

    d->scene = new medDatabasePreviewScene(this);

    d->view = new medDatabasePreviewView(this);
    d->view->setScene(d->scene);

    d->selector = new medDatabasePreviewSelector;
    d->selector->setRect(QRectF(5, 5, 138, 138));

    d->scene->addItem(d->selector);

    d->series_group = new medDatabasePreviewItemGroup;
    d->image_group = new medDatabasePreviewItemGroup;

    d->scene->addGroup(d->image_group);
    d->scene->addGroup(d->series_group);

    d->series_animation = NULL;
    d->image_animation = NULL;
    d->animation = NULL;

    d->selector_position_animation = NULL;
    d->selector_rect_animation = NULL;
    d->selector_animation = NULL;

    connect(d->view, SIGNAL(moveUp()), this, SLOT(onMoveUp()));
    connect(d->view, SIGNAL(moveDw()), this, SLOT(onMoveDw()));
    connect(d->view, SIGNAL(moveLt()), this, SLOT(onMoveLt()));
    connect(d->view, SIGNAL(moveRt()), this, SLOT(onMoveRt()));
    connect(d->view, SIGNAL(hovered(medDatabasePreviewItem*)), this, SLOT(onHovered(medDatabasePreviewItem*)));

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 0, 10, 10);
    layout->setSpacing(0);
    layout->addWidget(d->view);

    medDatabasePreviewController::instance()->orientation() == Qt::Horizontal
        ? this->setFixedHeight(medDatabasePreviewController::instance()->groupHeight() + medDatabasePreviewController::instance()->itemSpacing() + 36) // 26 pixels for the scroller
        : this->setFixedWidth(medDatabasePreviewController::instance()->groupWidth() + medDatabasePreviewController::instance()->itemSpacing() + 36); // 26 pixels for the scroller

    this->init();
}

medDatabasePreview::~medDatabasePreview(void)
{
    delete d;

    d = NULL;
}

void medDatabasePreview::reset(void)
{
    d->scene->reset();

    d->level = 0;

    if (d->animation) {
        delete d->animation;
        d->animation = NULL;
    }

    if (d->series_animation)
        d->series_animation = NULL;

    if (d->image_animation)
        d->image_animation = NULL;
}

void medDatabasePreview::init(void)
{
    d->selector->hide();
}

void medDatabasePreview::onPatientClicked(int id)
{
    d->series_group->clear();
    d->image_group->clear();

    QSqlQuery stQuery(*(medDatabaseController::instance()->database()));
    stQuery.prepare("SELECT * FROM study WHERE patient = :id");
    stQuery.bindValue(":id", id);
    if(!stQuery.exec())
        qDebug() << DTK_COLOR_FG_RED << stQuery.lastError() << DTK_NO_COLOR;
    
    while(stQuery.next()) {

        QVariant stId = stQuery.value(0);

        QSqlQuery seQuery(*(medDatabaseController::instance()->database()));
        seQuery.prepare("SELECT id, name, thumbnail FROM series WHERE study = :id");
        seQuery.bindValue(":id", stId);
        if(!seQuery.exec())
            qDebug() << DTK_COLOR_FG_RED << seQuery.lastError() << DTK_NO_COLOR;

        while(seQuery.next()) {
            
            QVariant   seId  = seQuery.value(0);
            QVariant seName  = seQuery.value(1);
            QVariant seThumb = seQuery.value(2);
            
            d->series_group->addItem(new medDatabasePreviewItem(id, stId.toInt(), seId.toInt(), -1, seThumb.toString()));
        }
    }

    if(d->level)
        this->onSlideDw();
}

void medDatabasePreview::onSeriesClicked(int id)
{
    QSqlQuery query(*(medDatabaseController::instance()->database()));

    QVariant studyId;

    query.prepare("SELECT study FROM series WHERE id = :id");
    query.bindValue(":id", id);
    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

    if(query.first())
        studyId = query.value(0);

    QVariant patientId;

    query.prepare("SELECT patient FROM study WHERE id = :id");
    query.bindValue(":id", studyId);
    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

    if(query.first())
        patientId = query.value(0);

    QVariant imageId;
    QVariant imageName;
    QVariant imageThumbnail;

    query.prepare("SELECT id, name, thumbnail FROM image WHERE series = :series");
    query.bindValue(":series", id);
    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

    d->image_group->clear();

    while(query.next()) {
        imageId = query.value(0);
        imageName = query.value(1);
        imageThumbnail = query.value(2);

        d->image_group->addItem(new medDatabasePreviewItem(patientId.toInt(), studyId.toInt(), id, imageId.toInt(), imageThumbnail.toString()));
    }

    if(!d->level)
        onSlideUp();
}

void medDatabasePreview::onSlideUp(void)
{
    if(d->level)
        return;

    qreal selector_width = medDatabasePreviewController::instance()->selectorWidth();
    qreal item_width = medDatabasePreviewController::instance()->itemWidth();
    qreal item_height = medDatabasePreviewController::instance()->itemHeight();
    qreal item_spacing = medDatabasePreviewController::instance()->itemSpacing();
    qreal item_margins = selector_width - item_width;
    qreal query_offset = medDatabasePreviewController::instance()->queryOffset();

    d->level++;

    qreal group_height  = medDatabasePreviewController::instance()->groupHeight();
    qreal group_spacing = medDatabasePreviewController::instance()->groupSpacing();

    if(!d->series_animation)
        d->series_animation = new QPropertyAnimation(d->series_group, "pos");

    d->series_animation->setDuration(250);
    d->series_animation->setStartValue(d->series_group->pos());
    d->series_animation->setEndValue(d->series_group->pos() - QPointF(0, group_height + group_spacing));
    d->series_animation->setEasingCurve(QEasingCurve::OutBounce);

    if(!d->image_animation)
        d->image_animation = new QPropertyAnimation(d->image_group, "pos");

    d->image_animation->setDuration(250);
    d->image_animation->setStartValue(d->image_group->pos());
    d->image_animation->setEndValue(d->image_group->pos() - QPointF(0, group_height + group_spacing));
    d->image_animation->setEasingCurve(QEasingCurve::OutBounce);

    if(!d->animation) {
        d->animation = new QParallelAnimationGroup(this);
        d->animation->addAnimation(d->series_animation);
        d->animation->addAnimation(d->image_animation);
    }

    connect(d->animation, SIGNAL(finished()), this, SLOT(onMoveBg()));

    d->animation->start();
}

void medDatabasePreview::onSlideDw(void)
{
    if(!d->level)
        return;

    qreal selector_width = medDatabasePreviewController::instance()->selectorWidth();
    qreal item_width = medDatabasePreviewController::instance()->itemWidth();
    qreal item_height = medDatabasePreviewController::instance()->itemHeight();
    qreal item_spacing = medDatabasePreviewController::instance()->itemSpacing();
    qreal item_margins = selector_width - item_width;
    qreal query_offset = medDatabasePreviewController::instance()->queryOffset();

    d->level--;

    qreal group_height  = medDatabasePreviewController::instance()->groupHeight();
    qreal group_spacing = medDatabasePreviewController::instance()->groupSpacing();

    if(!d->series_animation)
        d->series_animation = new QPropertyAnimation(d->series_group, "pos");

    d->series_animation->setDuration(250);
    d->series_animation->setStartValue(d->series_group->pos());
    d->series_animation->setEndValue(d->series_group->pos() + QPointF(0, group_height + group_spacing));
    d->series_animation->setEasingCurve(QEasingCurve::OutBounce);

    if(!d->image_animation)
        d->image_animation = new QPropertyAnimation(d->image_group, "pos");

    d->image_animation->setDuration(250);
    d->image_animation->setStartValue(d->image_group->pos());
    d->image_animation->setEndValue(d->image_group->pos() + QPointF(0, group_height + group_spacing));
    d->image_animation->setEasingCurve(QEasingCurve::OutBounce);

    if(!d->animation) {
        d->animation = new QParallelAnimationGroup(this);
        d->animation->addAnimation(d->series_animation);
        d->animation->addAnimation(d->image_animation);
    }

    connect(d->animation, SIGNAL(finished()), this, SLOT(onMoveBg()));

    d->animation->start();
}

void medDatabasePreview::onMoveRt(void)
{
    qreal selector_width = medDatabasePreviewController::instance()->selectorWidth();
    qreal item_width = medDatabasePreviewController::instance()->itemWidth();
    qreal item_height = medDatabasePreviewController::instance()->itemHeight();
    qreal item_spacing = medDatabasePreviewController::instance()->itemSpacing();
    qreal item_margins = selector_width - item_width;
    qreal query_offset = medDatabasePreviewController::instance()->queryOffset();

    QPoint selector_offset((medDatabasePreviewController::instance()->selectorWidth()  - medDatabasePreviewController::instance()->itemWidth())/-2,
                           (medDatabasePreviewController::instance()->selectorHeight() - medDatabasePreviewController::instance()->itemHeight())/-2);

    medDatabasePreviewItem *target = dynamic_cast<medDatabasePreviewItem *>(d->scene->itemAt(d->selector->pos() + QPointF(item_width + item_spacing + query_offset, query_offset)));

    if(!target)
        return;

    d->current_index_patient = target->patientId();
    d->current_index_study   = target->studyId();
    d->current_index_series  = target->seriesId();
    d->current_index_image   = target->imageId();

    target->ensureVisible(target->boundingRect(), medDatabasePreviewController::instance()->selectorWidth() + medDatabasePreviewController::instance()->itemSpacing(), 0);

    d->selector->setText(target->path());

    if(!d->selector_position_animation)
        d->selector_position_animation = new QPropertyAnimation(d->selector, "pos");

    d->selector_position_animation->setDuration(100);
    d->selector_position_animation->setStartValue(d->selector->pos());
    d->selector_position_animation->setEndValue(target->scenePos() + selector_offset);
    d->selector_position_animation->setEasingCurve(QEasingCurve::OutQuad);

    if(!d->selector_rect_animation)
        d->selector_rect_animation = new QPropertyAnimation(d->selector, "rect");

    d->selector_rect_animation->setDuration(100);
    d->selector_rect_animation->setStartValue(d->selector->rect());
    d->selector_rect_animation->setEndValue(QRectF(target->boundingRect().x(), target->boundingRect().y(), target->boundingRect().width() + item_margins, target->boundingRect().height() + item_spacing + item_margins));
    d->selector_rect_animation->setEasingCurve(QEasingCurve::Linear);

    if(!d->selector_animation) {
        d->selector_animation = new QParallelAnimationGroup(this);
        d->selector_animation->addAnimation(d->selector_position_animation);
        d->selector_animation->addAnimation(d->selector_rect_animation);
    }

    d->selector_animation->start();
}

void medDatabasePreview::onMoveLt(void)
{
    qreal selector_width = medDatabasePreviewController::instance()->selectorWidth();
    qreal item_width = medDatabasePreviewController::instance()->itemWidth();
    qreal item_height = medDatabasePreviewController::instance()->itemHeight();
    qreal item_spacing = medDatabasePreviewController::instance()->itemSpacing();
    qreal item_margins = selector_width - item_width;
    qreal query_offset = medDatabasePreviewController::instance()->queryOffset();

    QPoint selector_offset((medDatabasePreviewController::instance()->selectorWidth()  - medDatabasePreviewController::instance()->itemWidth())/-2,
                           (medDatabasePreviewController::instance()->selectorHeight() - medDatabasePreviewController::instance()->itemHeight())/-2);

    medDatabasePreviewItem *target = dynamic_cast<medDatabasePreviewItem *>(d->scene->itemAt(d->selector->pos() + QPointF(-item_width - item_spacing + query_offset, query_offset)));

    if(!target)
        return;

    d->current_index_patient = target->patientId();
    d->current_index_study   = target->studyId();
    d->current_index_series  = target->seriesId();
    d->current_index_image   = target->imageId();

    target->ensureVisible(target->boundingRect(), medDatabasePreviewController::instance()->selectorWidth() + medDatabasePreviewController::instance()->itemSpacing(), 0);

    d->selector->setText(target->path());
    
    if(!d->selector_position_animation)
        d->selector_position_animation = new QPropertyAnimation(d->selector, "pos");

    d->selector_position_animation->setDuration(100);
    d->selector_position_animation->setStartValue(d->selector->pos());
    d->selector_position_animation->setEndValue(target->scenePos() + selector_offset);
    d->selector_position_animation->setEasingCurve(QEasingCurve::OutQuad);

    if(!d->selector_rect_animation)
        d->selector_rect_animation = new QPropertyAnimation(d->selector, "rect");

    d->selector_rect_animation->setDuration(100);
    d->selector_rect_animation->setStartValue(d->selector->rect());
    d->selector_rect_animation->setEndValue(QRectF(target->boundingRect().x(), target->boundingRect().y(), target->boundingRect().width() + item_margins, target->boundingRect().height() + item_margins + item_spacing));
    d->selector_rect_animation->setEasingCurve(QEasingCurve::Linear);

    if(!d->selector_animation) {
        d->selector_animation = new QParallelAnimationGroup(this);
        d->selector_animation->addAnimation(d->selector_position_animation);
        d->selector_animation->addAnimation(d->selector_rect_animation);
    }

    d->selector_animation->start();
}

void medDatabasePreview::onMoveUp(void)
{
    if(d->level)
        this->onPatientClicked(d->current_index_patient);
}

void medDatabasePreview::onMoveDw(void)
{
    if(!d->level)
        this->onSeriesClicked(d->current_index_series);
}

void medDatabasePreview::onMoveBg(void) // move to beginning of the current line
{
    if(!d->selector->isVisible())
        d->selector->show();

    qreal selector_width = medDatabasePreviewController::instance()->selectorWidth();
    qreal item_width = medDatabasePreviewController::instance()->itemWidth();
    qreal item_height = medDatabasePreviewController::instance()->itemHeight();
    qreal item_spacing = medDatabasePreviewController::instance()->itemSpacing();
    qreal item_margins = selector_width - item_width;
    qreal query_offset = medDatabasePreviewController::instance()->queryOffset();

    QPoint selector_offset((medDatabasePreviewController::instance()->selectorWidth()  - medDatabasePreviewController::instance()->itemWidth())/-2,
                           (medDatabasePreviewController::instance()->selectorHeight() - medDatabasePreviewController::instance()->itemHeight())/-2);

    medDatabasePreviewItem *target = NULL;

    target = dynamic_cast<medDatabasePreviewItem *>(d->scene->itemAt(10, 10));

    if(!target)
        return;

    d->current_index_patient = target->patientId();
    d->current_index_study   = target->studyId();
    d->current_index_series  = target->seriesId();
    d->current_index_image   = target->imageId();

    target->ensureVisible(target->boundingRect(), medDatabasePreviewController::instance()->selectorWidth() + medDatabasePreviewController::instance()->itemSpacing(), 0);

    d->selector->setText(target->path());

    if(!d->selector_position_animation)
        d->selector_position_animation = new QPropertyAnimation(d->selector, "pos");

    d->selector_position_animation->setDuration(100);
    d->selector_position_animation->setStartValue(d->selector->pos());
    d->selector_position_animation->setEndValue(target->scenePos() + selector_offset);
    d->selector_position_animation->setEasingCurve(QEasingCurve::OutQuad);

    if(!d->selector_rect_animation)
        d->selector_rect_animation = new QPropertyAnimation(d->selector, "rect");

    d->selector_rect_animation->setDuration(100);
    d->selector_rect_animation->setStartValue(d->selector->rect());
    d->selector_rect_animation->setEndValue(QRectF(target->boundingRect().x(), target->boundingRect().y(), target->boundingRect().width() + item_margins, target->boundingRect().height() + item_margins + item_spacing));
    d->selector_rect_animation->setEasingCurve(QEasingCurve::Linear);

    if(!d->selector_animation) {
        d->selector_animation = new QParallelAnimationGroup(this);
        d->selector_animation->addAnimation(d->selector_position_animation);
        d->selector_animation->addAnimation(d->selector_rect_animation);
    }

    d->selector_animation->start();
}

void medDatabasePreview::onHovered(medDatabasePreviewItem *item)
{
    qreal selector_width = medDatabasePreviewController::instance()->selectorWidth();
    qreal item_width = medDatabasePreviewController::instance()->itemWidth();
    qreal item_height = medDatabasePreviewController::instance()->itemHeight();
    qreal item_spacing = medDatabasePreviewController::instance()->itemSpacing();
    qreal item_margins = selector_width - item_width;
    qreal query_offset = medDatabasePreviewController::instance()->queryOffset();

    d->current_index_patient = item->patientId();
    d->current_index_study   = item->studyId();
    d->current_index_series  = item->seriesId();
    d->current_index_image   = item->imageId();

    QPoint selector_offset(-4, -4);

    if(qAbs(d->selector->pos().x() - item->scenePos().x()) < 20 && qAbs(d->selector->pos().y() - item->scenePos().y()) < 20)
        return;

    d->selector->setText(item->path());

    if(!d->selector_position_animation)
        d->selector_position_animation = new QPropertyAnimation(d->selector, "pos");

    d->selector_position_animation->setDuration(100);
    d->selector_position_animation->setStartValue(d->selector->pos());
    d->selector_position_animation->setEndValue(item->scenePos() + selector_offset);
    d->selector_position_animation->setEasingCurve(QEasingCurve::Linear);

    if(!d->selector_rect_animation)
        d->selector_rect_animation = new QPropertyAnimation(d->selector, "rect");

    d->selector_rect_animation->setDuration(100);
    d->selector_rect_animation->setStartValue(d->selector->rect());
    d->selector_rect_animation->setEndValue(QRectF(item->boundingRect().x(), item->boundingRect().y(), item->boundingRect().width() + item_margins, item->boundingRect().height() + item_margins + item_spacing));
    d->selector_rect_animation->setEasingCurve(QEasingCurve::Linear);

    if(!d->selector_animation) {
        d->selector_animation = new QParallelAnimationGroup(this);
        d->selector_animation->addAnimation(d->selector_position_animation);
        d->selector_animation->addAnimation(d->selector_rect_animation);
    }

    d->selector_animation->start();
}
