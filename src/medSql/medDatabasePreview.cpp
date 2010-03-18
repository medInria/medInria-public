/* medDatabasePreview.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:42:18 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Mar 17 11:06:21 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 25
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

    int current_patient_id;
    int current_study_id;
    int current_series_id;
    int current_image_id;

    medDatabasePreviewScene *scene;
    medDatabasePreviewView *view;
    medDatabasePreviewSelector *selector;

    medDatabasePreviewItemGroup *patient_group;
    medDatabasePreviewItemGroup *study_group;
    medDatabasePreviewItemGroup *series_group;
    medDatabasePreviewItemGroup *image_group;

    QPropertyAnimation *patient_animation;
    QPropertyAnimation *study_animation;
    QPropertyAnimation *series_animation;
    QPropertyAnimation *image_animation;
    QParallelAnimationGroup *animation;

    QPropertyAnimation *selector_position_animation;
    QPropertyAnimation *selector_rect_animation;
    QParallelAnimationGroup *selector_animation;

    medDatabasePreviewTooltip *tooltip_patient;
    medDatabasePreviewTooltip *tooltip_study;
    medDatabasePreviewTooltip *tooltip_series;
    medDatabasePreviewTooltip *tooltip_image;

    QPropertyAnimation *tooltip_patient_animation;
    QPropertyAnimation *tooltip_study_animation;
    QPropertyAnimation *tooltip_series_animation;
    QPropertyAnimation *tooltip_image_animation;

    QItemSelectionModel *model;

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

    d->current_patient_id = -1;
    d->current_study_id = -1;
    d->current_series_id = -1;
    d->current_image_id = -1;

    d->patient_group = new medDatabasePreviewItemGroup;
    d->study_group = new medDatabasePreviewItemGroup;
    d->series_group = new medDatabasePreviewItemGroup;
    d->image_group = new medDatabasePreviewItemGroup;

    d->scene->addGroup(d->image_group);
    d->scene->addGroup(d->series_group);
    d->scene->addGroup(d->study_group);
    d->scene->addGroup(d->patient_group);

    d->patient_animation = NULL;
    d->study_animation = NULL;
    d->series_animation = NULL;
    d->image_animation = NULL;
    d->animation = NULL;

    d->selector_position_animation = NULL;
    d->selector_rect_animation = NULL;
    d->selector_animation = NULL;

    d->tooltip_patient = new medDatabasePreviewTooltip(QPixmap(":/pixmaps/database_preview_tooltip_patient.tiff"));
    d->tooltip_patient->setOpacity(0.0);

    d->tooltip_study = new medDatabasePreviewTooltip(QPixmap(":/pixmaps/database_preview_tooltip_study.tiff"));
    d->tooltip_study->setOpacity(0.0);

    d->tooltip_series = new medDatabasePreviewTooltip(QPixmap(":/pixmaps/database_preview_tooltip_series.tiff"));
    d->tooltip_series->setOpacity(0.0);

    d->tooltip_image = new medDatabasePreviewTooltip(QPixmap(":/pixmaps/database_preview_tooltip_image.tiff"));
    d->tooltip_image->setOpacity(0.0);

    d->scene->addItem(d->tooltip_patient);
    d->scene->addItem(d->tooltip_study);
    d->scene->addItem(d->tooltip_series);
    d->scene->addItem(d->tooltip_image);

    d->view->setPatientTooltip(d->tooltip_patient);
    d->view->setStudyTooltip(d->tooltip_study);
    d->view->setSeriesTooltip(d->tooltip_series);
    d->view->setImageTooltip(d->tooltip_image);

    d->tooltip_patient_animation = new QPropertyAnimation(d->tooltip_patient, "opacity");
    d->tooltip_patient_animation->setDuration(500);
    d->tooltip_patient_animation->setKeyValueAt(0.0, 0.0);
    d->tooltip_patient_animation->setKeyValueAt(0.5, 1.0);
    d->tooltip_patient_animation->setKeyValueAt(1.0, 0.0);
    d->tooltip_patient_animation->setEasingCurve(QEasingCurve::OutInExpo);

    d->tooltip_study_animation = new QPropertyAnimation(d->tooltip_study, "opacity");
    d->tooltip_study_animation->setDuration(500);
    d->tooltip_study_animation->setStartValue(0.0);
    d->tooltip_study_animation->setKeyValueAt(0.5, 1.0);
    d->tooltip_study_animation->setEndValue(0.0);
    d->tooltip_study_animation->setEasingCurve(QEasingCurve::OutInExpo);

    d->tooltip_series_animation = new QPropertyAnimation(d->tooltip_series, "opacity");
    d->tooltip_series_animation->setDuration(500);
    d->tooltip_series_animation->setStartValue(0.0);
    d->tooltip_series_animation->setKeyValueAt(0.5, 1.0);
    d->tooltip_series_animation->setEndValue(0.0);
    d->tooltip_series_animation->setEasingCurve(QEasingCurve::OutInExpo);

    d->tooltip_image_animation = new QPropertyAnimation(d->tooltip_image, "opacity");
    d->tooltip_image_animation->setDuration(500);
    d->tooltip_image_animation->setStartValue(0.0);
    d->tooltip_image_animation->setKeyValueAt(0.5, 1.0);
    d->tooltip_image_animation->setEndValue(0.0);
    d->tooltip_image_animation->setEasingCurve(QEasingCurve::OutInExpo);

    connect(d->view, SIGNAL(slideUp()), this, SLOT(onSlideUp()));
    connect(d->view, SIGNAL(slideDw()), this, SLOT(onSlideDw()));
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

    if (d->patient_animation)
        d->patient_animation = NULL;

    if (d->study_animation)
        d->study_animation = NULL;

    if (d->series_animation)
        d->series_animation = NULL;

    if (d->image_animation)
        d->image_animation = NULL;
}

void medDatabasePreview::init(void)
{
    QSqlQuery query(*(medDatabaseController::instance()->database()));

    // Retrieve patient information

    QVariant patientId;
    QVariant patientName;
    QVariant patientThumbnail;

    query.prepare("SELECT id, name, thumbnail FROM patient");

    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

    d->patient_group->clear();

    while(query.next()) {
        patientId = query.value(0);
        patientName = query.value(1);
        patientThumbnail = query.value(2);

        d->patient_group->addItem(new medDatabasePreviewItem(patientId.toInt(), -1, -1, -1, patientThumbnail.toString()));
    }
}

void medDatabasePreview::onPatientClicked(int id)
{
    QSqlQuery query(*(medDatabaseController::instance()->database()));

    // Retrieve studies information

    QVariant studyId;
    QVariant studyName;
    QVariant studyThumbnail;

    query.prepare("SELECT id, name, thumbnail FROM study WHERE patient = :patient");
    query.bindValue(":patient", id);
    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

    d->study_group->clear();
    d->series_group->clear();
    d->image_group->clear();

    while(query.next()) {
        studyId = query.value(0);
        studyName = query.value(1);
        studyThumbnail = query.value(2);

        d->study_group->addItem(new medDatabasePreviewItem(id, studyId.toInt(), -1, -1, studyThumbnail.toString()));
    }

    d->current_patient_id = id;
    d->current_study_id = -1;
    d->current_series_id = -1;
    d->current_image_id = -1;

    while(d->level > 0) this->onSlideDw();
    while(d->level < 0) this->onSlideUp();

    onMoveBg();
}

void medDatabasePreview::onStudyClicked(int id)
{
    // Retrieve series information

    QSqlQuery query(*(medDatabaseController::instance()->database()));

    QVariant patientId;

    query.prepare("SELECT patient FROM study WHERE id = :id");
    query.bindValue(":id", id);
    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

    if(query.first())
        patientId = query.value(0);

    QVariant seriesId;
    QVariant seriesName;
    QVariant seriesThumbnail;

    query.prepare("SELECT id, name, thumbnail FROM series WHERE study = :study");
    query.bindValue(":study", id);
    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NO_COLOR;

    d->series_group->clear();
    d->image_group->clear();

    while(query.next()) {
        seriesId = query.value(0);
        seriesName = query.value(1);
        seriesThumbnail = query.value(2);

        d->series_group->addItem(new medDatabasePreviewItem(patientId.toInt(), id, seriesId.toInt(), -1, seriesThumbnail.toString()));
    }

    d->current_study_id = id;
    d->current_series_id = -1;
    d->current_image_id = -1;

    while(d->level > 1) this->onSlideDw();
    while(d->level < 1) this->onSlideUp();

    onMoveBg();
}

void medDatabasePreview::onSeriesClicked(int id)
{
    // Retrieve images information

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

    d->current_series_id = id;
    d->current_image_id = -1;

    while(d->level > 2) this->onSlideDw();
    while(d->level < 2) this->onSlideUp();

    onMoveBg();
}

void medDatabasePreview::onImageClicked(int id)
{
    d->current_image_id = id;

    while(d->level > 3) this->onSlideDw();
    while(d->level < 3) this->onSlideUp();

    onMoveBg();
}

void medDatabasePreview::onSlideUp(void)
{
    if(d->level > 2)
        return;

    qreal selector_width = medDatabasePreviewController::instance()->selectorWidth();
    qreal item_width = medDatabasePreviewController::instance()->itemWidth();
    qreal item_height = medDatabasePreviewController::instance()->itemHeight();
    qreal item_spacing = medDatabasePreviewController::instance()->itemSpacing();
    qreal item_margins = selector_width - item_width;
    qreal query_offset = medDatabasePreviewController::instance()->queryOffset();

    if(medDatabasePreviewItem *target = dynamic_cast<medDatabasePreviewItem *>(d->scene->itemAt(d->selector->pos() + QPointF(query_offset, query_offset)))) {

        if (d->level == 0)
            this->onPatientClicked(target->patientId());

        if (d->level == 1)
            this->onStudyClicked(target->studyId());

        if (d->level == 2)
            this->onSeriesClicked(target->seriesId());
    }

    d->level++;

    qreal group_height  = medDatabasePreviewController::instance()->groupHeight();
    qreal group_spacing = medDatabasePreviewController::instance()->groupSpacing();

    if(!d->patient_animation)
        d->patient_animation = new QPropertyAnimation(d->patient_group, "pos");

    d->patient_animation->setDuration(250);
    d->patient_animation->setStartValue(d->patient_group->pos());
    d->patient_animation->setEndValue(d->patient_group->pos() - QPointF(0, group_height + group_spacing));
    d->patient_animation->setEasingCurve(QEasingCurve::OutBounce);

    if(!d->study_animation)
        d->study_animation = new QPropertyAnimation(d->study_group, "pos");

    d->study_animation->setDuration(250);
    d->study_animation->setStartValue(d->study_group->pos());
    d->study_animation->setEndValue(d->study_group->pos() - QPointF(0, group_height + group_spacing));
    d->study_animation->setEasingCurve(QEasingCurve::OutBounce);

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
        d->animation->addAnimation(d->patient_animation);
        d->animation->addAnimation(d->study_animation);
        d->animation->addAnimation(d->series_animation);
        d->animation->addAnimation(d->image_animation);
    }

    connect(d->animation, SIGNAL(finished()), this, SLOT(onMoveBg()));

    d->animation->start();

    switch(d->level) {
    case 0: d->tooltip_patient_animation->start(); break;
    case 1: d->tooltip_study_animation->start(); break;
    case 2: d->tooltip_series_animation->start(); break;
    case 3: d->tooltip_image_animation->start(); break;
    default: break;
    }
}

void medDatabasePreview::onSlideDw(void)
{
    if(d->level < 1)
        return;

    qreal selector_width = medDatabasePreviewController::instance()->selectorWidth();
    qreal item_width = medDatabasePreviewController::instance()->itemWidth();
    qreal item_height = medDatabasePreviewController::instance()->itemHeight();
    qreal item_spacing = medDatabasePreviewController::instance()->itemSpacing();
    qreal item_margins = selector_width - item_width;
    qreal query_offset = medDatabasePreviewController::instance()->queryOffset();

    if(medDatabasePreviewItem *target = dynamic_cast<medDatabasePreviewItem *>(d->scene->itemAt(d->selector->pos() + QPointF(query_offset, query_offset)))) {

        if (d->level == 0)
            this->onPatientClicked(target->patientId());

        if (d->level == 1)
            this->onStudyClicked(target->studyId());

        if (d->level == 2)
            this->onSeriesClicked(target->seriesId());
    }

    d->level--;

    qreal group_height  = medDatabasePreviewController::instance()->groupHeight();
    qreal group_spacing = medDatabasePreviewController::instance()->groupSpacing();

    if(!d->patient_animation)
        d->patient_animation = new QPropertyAnimation(d->patient_group, "pos");

    d->patient_animation->setDuration(250);
    d->patient_animation->setStartValue(d->patient_group->pos());
    d->patient_animation->setEndValue(d->patient_group->pos() + QPointF(0, group_height + group_spacing));
    d->patient_animation->setEasingCurve(QEasingCurve::OutBounce);

    if(!d->study_animation)
        d->study_animation = new QPropertyAnimation(d->study_group, "pos");

    d->study_animation->setDuration(250);
    d->study_animation->setStartValue(d->study_group->pos());
    d->study_animation->setEndValue(d->study_group->pos() + QPointF(0, group_height + group_spacing));
    d->study_animation->setEasingCurve(QEasingCurve::OutBounce);

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
        d->animation->addAnimation(d->patient_animation);
        d->animation->addAnimation(d->study_animation);
        d->animation->addAnimation(d->series_animation);
        d->animation->addAnimation(d->image_animation);
    }

    if(d->animation->state() == QAbstractAnimation::Running)
        return;

    connect(d->animation, SIGNAL(finished()), this, SLOT(onMoveBg()));

    d->animation->start();

    switch(d->level) {
    case 0: d->tooltip_patient_animation->start(); break;
    case 1: d->tooltip_study_animation->start(); break;
    case 2: d->tooltip_series_animation->start(); break;
    case 3: d->tooltip_image_animation->start(); break;
    default: break;
    }
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

    if(target->patientId() >= 0)
        emit patientClicked(target->patientId());

    if(target->studyId() >= 0)
        emit studyClicked(target->studyId());

    if(target->seriesId() >= 0)
        emit seriesClicked(target->seriesId());

    if(target->imageId() >= 0)
        emit imageClicked(target->imageId());

    // Scroll view if needed

    target->ensureVisible(target->boundingRect(), medDatabasePreviewController::instance()->selectorWidth() + medDatabasePreviewController::instance()->itemSpacing(), 0);

    // Update selector

    d->selector->setText(target->path());

    // Selector position animation

    if(!d->selector_position_animation)
        d->selector_position_animation = new QPropertyAnimation(d->selector, "pos");

//    if(d->selector_position_animation->state() == QAbstractAnimation::Running)
//        return;

    d->selector_position_animation->setDuration(100);
    d->selector_position_animation->setStartValue(d->selector->pos());
    d->selector_position_animation->setEndValue(target->scenePos() + selector_offset);
    d->selector_position_animation->setEasingCurve(QEasingCurve::OutQuad);

    // Selector rect animation

    if(!d->selector_rect_animation)
        d->selector_rect_animation = new QPropertyAnimation(d->selector, "rect");

//    if(d->selector_rect_animation->state() == QAbstractAnimation::Running)
//        return;

    d->selector_rect_animation->setDuration(100);
    d->selector_rect_animation->setStartValue(d->selector->rect());
    d->selector_rect_animation->setEndValue(QRectF(target->boundingRect().x(), target->boundingRect().y(), target->boundingRect().width() + item_margins, target->boundingRect().height() + item_spacing + item_margins));
    d->selector_rect_animation->setEasingCurve(QEasingCurve::Linear);

    // Selector Parallel animation

    if(!d->selector_animation) {
        d->selector_animation = new QParallelAnimationGroup(this);
        d->selector_animation->addAnimation(d->selector_position_animation);
        d->selector_animation->addAnimation(d->selector_rect_animation);
    }

//    if(d->selector_animation->state() == QAbstractAnimation::Running)
//        return;

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

    if(target->patientId() >= 0)
        emit patientClicked(target->patientId());

    if(target->studyId() >= 0)
        emit studyClicked(target->studyId());

    if(target->seriesId() >= 0)
        emit seriesClicked(target->seriesId());

    if(target->imageId() >= 0)
        emit imageClicked(target->imageId());

    // Scroll view if needed

    target->ensureVisible(target->boundingRect(), medDatabasePreviewController::instance()->selectorWidth() + medDatabasePreviewController::instance()->itemSpacing(), 0);

    // Update selector

    d->selector->setText(target->path());
    
    // Selector position animation

    if(!d->selector_position_animation)
        d->selector_position_animation = new QPropertyAnimation(d->selector, "pos");

//    if(d->selector_position_animation->state() == QAbstractAnimation::Running)
//        return;

    d->selector_position_animation->setDuration(100);
    d->selector_position_animation->setStartValue(d->selector->pos());
    d->selector_position_animation->setEndValue(target->scenePos() + selector_offset);
    d->selector_position_animation->setEasingCurve(QEasingCurve::OutQuad);

    // Selector rect animation

    if(!d->selector_rect_animation)
        d->selector_rect_animation = new QPropertyAnimation(d->selector, "rect");

//    if(d->selector_rect_animation->state() == QAbstractAnimation::Running)
//        return;

    d->selector_rect_animation->setDuration(100);
    d->selector_rect_animation->setStartValue(d->selector->rect());
    d->selector_rect_animation->setEndValue(QRectF(target->boundingRect().x(), target->boundingRect().y(), target->boundingRect().width() + item_margins, target->boundingRect().height() + item_margins + item_spacing));
    d->selector_rect_animation->setEasingCurve(QEasingCurve::Linear);

    // Selector Parallel animation

    if(!d->selector_animation) {
        d->selector_animation = new QParallelAnimationGroup(this);
        d->selector_animation->addAnimation(d->selector_position_animation);
        d->selector_animation->addAnimation(d->selector_rect_animation);
    }

//    if(d->selector_animation->state() == QAbstractAnimation::Running)
//        return;

    d->selector_animation->start();
}

void medDatabasePreview::onMoveUp(void)
{
    qreal selector_width = medDatabasePreviewController::instance()->selectorWidth();
    qreal item_width = medDatabasePreviewController::instance()->itemWidth();
    qreal item_height = medDatabasePreviewController::instance()->itemHeight();
    qreal item_spacing = medDatabasePreviewController::instance()->itemSpacing();
    qreal item_margins = selector_width - item_width;
    qreal query_offset = medDatabasePreviewController::instance()->queryOffset();

    QPoint selector_offset((medDatabasePreviewController::instance()->selectorWidth()  - medDatabasePreviewController::instance()->itemWidth())/-2,
                           (medDatabasePreviewController::instance()->selectorHeight() - medDatabasePreviewController::instance()->itemHeight())/-2);

    medDatabasePreviewItem *target = qgraphicsitem_cast<medDatabasePreviewItem *>(d->scene->itemAt(d->selector->pos() + QPointF(query_offset, -item_height - item_spacing + query_offset)));

    if(!target)
        target = dynamic_cast<medDatabasePreviewItem *>(d->scene->itemAt(d->selector->pos() + QPointF(d->selector->pos().x() * - 1 + query_offset, -item_height - item_spacing + query_offset)));

    if(!target)
        return;

    if(target->patientId() >= 0)
        emit patientClicked(target->patientId());

    if(target->studyId() >= 0)
        emit studyClicked(target->studyId());

    if(target->seriesId() >= 0)
        emit seriesClicked(target->seriesId());

    if(target->imageId() >= 0)
        emit imageClicked(target->imageId());

    // Update selector

    d->selector->setText(target->path());

    // Selector position animation

    if(!d->selector_position_animation)
        d->selector_position_animation = new QPropertyAnimation(d->selector, "pos");

//    if(d->selector_position_animation->state() == QAbstractAnimation::Running)
//        return;

    d->selector_position_animation->setDuration(100);
    d->selector_position_animation->setStartValue(d->selector->pos());
    d->selector_position_animation->setEndValue(target->scenePos() + selector_offset);
    d->selector_position_animation->setEasingCurve(QEasingCurve::OutQuad);

    // Selector rect animation

    if(!d->selector_rect_animation)
        d->selector_rect_animation = new QPropertyAnimation(d->selector, "rect");

//    if(d->selector_rect_animation->state() == QAbstractAnimation::Running)
//        return;

    d->selector_rect_animation->setDuration(100);
    d->selector_rect_animation->setStartValue(d->selector->rect());
    d->selector_rect_animation->setEndValue(QRectF(target->boundingRect().x(), target->boundingRect().y(), target->boundingRect().width() + item_margins, target->boundingRect().height() + item_margins + item_spacing));
    d->selector_rect_animation->setEasingCurve(QEasingCurve::Linear);

    // Selector Parallel animation

    if(!d->selector_animation) {
        d->selector_animation = new QParallelAnimationGroup(this);
        d->selector_animation->addAnimation(d->selector_position_animation);
        d->selector_animation->addAnimation(d->selector_rect_animation);
    }

//    if(d->selector_animation->state() == QAbstractAnimation::Running)
//        return;

    d->selector_animation->start();
}

void medDatabasePreview::onMoveDw(void)
{
    qreal selector_width = medDatabasePreviewController::instance()->selectorWidth();
    qreal item_width = medDatabasePreviewController::instance()->itemWidth();
    qreal item_height = medDatabasePreviewController::instance()->itemHeight();
    qreal item_spacing = medDatabasePreviewController::instance()->itemSpacing();
    qreal item_margins = selector_width - item_width;
    qreal query_offset = medDatabasePreviewController::instance()->queryOffset();

    QPoint selector_offset((medDatabasePreviewController::instance()->selectorWidth()  - medDatabasePreviewController::instance()->itemWidth())/-2,
                           (medDatabasePreviewController::instance()->selectorHeight() - medDatabasePreviewController::instance()->itemHeight())/-2);

    medDatabasePreviewItem *target = dynamic_cast<medDatabasePreviewItem *>(d->scene->itemAt(d->selector->pos() + QPointF(query_offset, item_height - item_spacing + query_offset)));

    if(!target)
        return;

    if(target->patientId() >= 0)
        emit patientClicked(target->patientId());

    if(target->studyId() >= 0)
        emit studyClicked(target->studyId());

    if(target->seriesId() >= 0)
        emit seriesClicked(target->seriesId());

    if(target->imageId() >= 0)
        emit imageClicked(target->imageId());

    // Update selector

    d->selector->setText(target->path());

    // Selector position animation

    if(!d->selector_position_animation)
        d->selector_position_animation = new QPropertyAnimation(d->selector, "pos");

//    if(d->selector_position_animation->state() == QAbstractAnimation::Running)
//        return;

    d->selector_position_animation->setDuration(100);
    d->selector_position_animation->setStartValue(d->selector->pos());
    d->selector_position_animation->setEndValue(target->scenePos() + selector_offset);
    d->selector_position_animation->setEasingCurve(QEasingCurve::OutQuad);

    // Selector rect animation

    if(!d->selector_rect_animation)
        d->selector_rect_animation = new QPropertyAnimation(d->selector, "rect");

//    if(d->selector_rect_animation->state() == QAbstractAnimation::Running)
//        return;

    d->selector_rect_animation->setDuration(100);
    d->selector_rect_animation->setStartValue(d->selector->rect());
    d->selector_rect_animation->setEndValue(QRectF(target->boundingRect().x(), target->boundingRect().y(), target->boundingRect().width() + item_margins, target->boundingRect().height() + item_margins + item_spacing));
    d->selector_rect_animation->setEasingCurve(QEasingCurve::Linear);

    // Selector Parallel animation

    if(!d->selector_animation) {
        d->selector_animation = new QParallelAnimationGroup(this);
        d->selector_animation->addAnimation(d->selector_position_animation);
        d->selector_animation->addAnimation(d->selector_rect_animation);
    }

//    if(d->selector_animation->state() == QAbstractAnimation::Running)
//        return;

    d->selector_animation->start();
}

void medDatabasePreview::onMoveBg(void) // move to beginning of the current line
{
    qreal selector_width = medDatabasePreviewController::instance()->selectorWidth();
    qreal item_width = medDatabasePreviewController::instance()->itemWidth();
    qreal item_height = medDatabasePreviewController::instance()->itemHeight();
    qreal item_spacing = medDatabasePreviewController::instance()->itemSpacing();
    qreal item_margins = selector_width - item_width;
    qreal query_offset = medDatabasePreviewController::instance()->queryOffset();

    QPoint selector_offset((medDatabasePreviewController::instance()->selectorWidth()  - medDatabasePreviewController::instance()->itemWidth())/-2,
                           (medDatabasePreviewController::instance()->selectorHeight() - medDatabasePreviewController::instance()->itemHeight())/-2);

    medDatabasePreviewItem *target = NULL;

    if(d->level == 0 && d->current_patient_id >= 0)
        target = d->patient_group->item(d->current_patient_id);
    else if(d->level == 1 && d->current_study_id >= 0)
        target = d->study_group->item(d->current_study_id);
    else if(d->level == 2 && d->current_series_id >= 0)
        target = d->series_group->item(d->current_series_id);
    else if(d->level == 3 && d->current_image_id >= 0)
        target = d->image_group->item(d->current_image_id);
    else
        target = dynamic_cast<medDatabasePreviewItem *>(d->scene->itemAt(d->selector->scenePos() + QPointF(d->selector->scenePos().x() * (-1) + query_offset, query_offset)));

    if(!target)
        return;

    if(target->patientId() >= 0)
        emit patientClicked(target->patientId());

    if(target->studyId() >= 0)
        emit studyClicked(target->studyId());

    if(target->seriesId() >= 0)
        emit seriesClicked(target->seriesId());

    if(target->imageId() >= 0)
        emit imageClicked(target->imageId());

    // Scroll view if needed

    target->ensureVisible(target->boundingRect(), medDatabasePreviewController::instance()->selectorWidth() + medDatabasePreviewController::instance()->itemSpacing(), 0);

    // Update selector

    d->selector->setText(target->path());

    // Selector position animation

    if(!d->selector_position_animation)
        d->selector_position_animation = new QPropertyAnimation(d->selector, "pos");

//    if(d->selector_position_animation->state() == QAbstractAnimation::Running)
//        return;

    d->selector_position_animation->setDuration(100);
    d->selector_position_animation->setStartValue(d->selector->pos());
    d->selector_position_animation->setEndValue(target->scenePos() + selector_offset);
    d->selector_position_animation->setEasingCurve(QEasingCurve::OutQuad);

    // Selector rect animation

    if(!d->selector_rect_animation)
        d->selector_rect_animation = new QPropertyAnimation(d->selector, "rect");

//    if(d->selector_rect_animation->state() == QAbstractAnimation::Running)
//        return;

    d->selector_rect_animation->setDuration(100);
    d->selector_rect_animation->setStartValue(d->selector->rect());
    d->selector_rect_animation->setEndValue(QRectF(target->boundingRect().x(), target->boundingRect().y(), target->boundingRect().width() + item_margins, target->boundingRect().height() + item_margins + item_spacing));
    d->selector_rect_animation->setEasingCurve(QEasingCurve::Linear);

    // Selector Parallel animation

    if(!d->selector_animation) {
        d->selector_animation = new QParallelAnimationGroup(this);
        d->selector_animation->addAnimation(d->selector_position_animation);
        d->selector_animation->addAnimation(d->selector_rect_animation);
    }

//    if(d->selector_animation->state() == QAbstractAnimation::Running)
//        return;

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

//    QPoint selector_offset((medDatabasePreviewController::instance()->selectorWidth()  - medDatabasePreviewController::instance()->itemWidth())/-2,
//                           (medDatabasePreviewController::instance()->selectorHeight() - medDatabasePreviewController::instance()->itemHeight())/-2);

    QPoint selector_offset(-4, -4);

    if(qAbs(d->selector->pos().x() - item->scenePos().x()) < 20 && qAbs(d->selector->pos().y() - item->scenePos().y()) < 20)
        return;

    if(item->patientId() >= 0)
        emit patientClicked(item->patientId());

    if(item->studyId() >= 0)
        emit studyClicked(item->studyId());

    if(item->seriesId() >= 0)
        emit seriesClicked(item->seriesId());

    if(item->imageId() >= 0)
        emit imageClicked(item->imageId());

    // Update selector

    d->selector->setText(item->path());

    // Selector position animation

    if(!d->selector_position_animation)
        d->selector_position_animation = new QPropertyAnimation(d->selector, "pos");

//    if(d->selector_position_animation->state() == QAbstractAnimation::Running)
//        return;

    d->selector_position_animation->setDuration(100);
    d->selector_position_animation->setStartValue(d->selector->pos());
    d->selector_position_animation->setEndValue(item->scenePos() + selector_offset);
    d->selector_position_animation->setEasingCurve(QEasingCurve::Linear);

    // Selector rect animation

    if(!d->selector_rect_animation)
        d->selector_rect_animation = new QPropertyAnimation(d->selector, "rect");

//    if(d->selector_rect_animation->state() == QAbstractAnimation::Running)
//        return;

    d->selector_rect_animation->setDuration(100);
    d->selector_rect_animation->setStartValue(d->selector->rect());
    d->selector_rect_animation->setEndValue(QRectF(item->boundingRect().x(), item->boundingRect().y(), item->boundingRect().width() + item_margins, item->boundingRect().height() + item_margins + item_spacing));
    d->selector_rect_animation->setEasingCurve(QEasingCurve::Linear);

    // Selector Parallel animation

    if(!d->selector_animation) {
        d->selector_animation = new QParallelAnimationGroup(this);
        d->selector_animation->addAnimation(d->selector_position_animation);
        d->selector_animation->addAnimation(d->selector_rect_animation);
    }

//    if(d->selector_animation->state() == QAbstractAnimation::Running)
//        return;

    d->selector_animation->start();
}
