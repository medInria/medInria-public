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
};

medDatabasePreview::medDatabasePreview(QWidget *parent) : QFrame(parent), d(new medDatabasePreviewPrivate)
{
    d->level = 0;

    d->scene = new medDatabasePreviewScene(this);

    d->view = new medDatabasePreviewView(this);
    d->view->setScene(d->scene);

    d->selector = new medDatabasePreviewSelector;

    d->scene->addItem(d->selector);

    d->patient_group = NULL;
    d->study_group = NULL;
    d->series_group = NULL;
    d->image_group = NULL;

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

//    medDatabasePreviewController::instance()->setOrientation(Qt::Vertical);

//    medDatabasePreviewController::instance()->setItemSize( 72,  72);
//    medDatabasePreviewController::instance()->setItemSize( 96,  96);
    medDatabasePreviewController::instance()->setItemSize(128, 128);

//    this->setup(QDir("/Users/jwintz/Desktop/thumbs 128"));

    medDatabasePreviewController::instance()->orientation() == Qt::Horizontal
        ? this->setFixedHeight(medDatabasePreviewController::instance()->groupHeight() + medDatabasePreviewController::instance()->itemSpacing() + 36) // 26 pixels for the scroller
        : this->setFixedWidth(medDatabasePreviewController::instance()->groupWidth() + medDatabasePreviewController::instance()->itemSpacing() + 36); // 26 pixels for the scroller

    QTimer::singleShot(100, this, SLOT(onMoveBg()));
}

medDatabasePreview::~medDatabasePreview(void)
{
    delete d;

    d = NULL;
}

void medDatabasePreview::setup(const QDir& dir)
{
    medDatabasePreviewItemGroup *group = new medDatabasePreviewItemGroup;

    foreach(QString path, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
        group->addItem(setupPatient(QDir(dir.absoluteFilePath(path))));

    d->scene->addGroup(group);

    d->patient_group = group;
}

void medDatabasePreview::setSelectionModel(QItemSelectionModel *model)
{
    connect(model, SIGNAL(currentChanged(QModelIndex,QModelIndex)), this, SLOT(onCurrentChanged(QModelIndex)));
    connect(this, SIGNAL(currentChanged(QModelIndex, QItemSelectionModel::SelectionFlags)), model, SLOT(setCurrentIndex(QModelIndex, QItemSelectionModel::SelectionFlags)));
}

medDatabasePreviewItem *medDatabasePreview::setupPatient(const QDir& dir)
{
    medDatabasePreviewItemGroup *group = new medDatabasePreviewItemGroup;

    foreach(QString path, dir.entryList(QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot))
        group->addItem(setupStudy(QDir(dir.absoluteFilePath(path))));

    d->scene->addGroup(group);

    d->study_group = group;

    return dynamic_cast<medDatabasePreviewItem *>(((QGraphicsItem *)group)->children().first())->clone();
}

medDatabasePreviewItem *medDatabasePreview::setupStudy(const QDir& dir)
{
    medDatabasePreviewItemGroup *group = new medDatabasePreviewItemGroup;

    foreach(QString path, dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot))
        group->addItem(setupSeries(QDir(dir.absoluteFilePath(path))));

    d->scene->addGroup(group);

    d->series_group = group;

    return dynamic_cast<medDatabasePreviewItem *>(((QGraphicsItem *)group)->children().first())->clone();
}

medDatabasePreviewItem *medDatabasePreview::setupSeries(const QDir& dir)
{
    medDatabasePreviewItemGroup *group = new medDatabasePreviewItemGroup;

    foreach(QString path, dir.entryList(QDir::Files | QDir::NoDotAndDotDot))
        group->addItem(new medDatabasePreviewItem(dir.absoluteFilePath(path)));

    d->scene->addGroup(group);

    d->image_group = group;

    return dynamic_cast<medDatabasePreviewItem *>(((QGraphicsItem *)group)->children().first())->clone();
}

void medDatabasePreview::onPatientClicked(int patientId)
{
    // Clear preview state

    d->scene->reset();

    d->level = 0;

    if (d->animation) {
        delete d->animation;
        d->animation = NULL;
    }

    if (d->patient_animation) {
//        delete d->patient_animation;
        d->patient_animation = NULL;
    }

    if (d->study_animation) {
//        delete d->study_animation;
        d->study_animation = NULL;
    }

    if (d->series_animation) {
//        delete d->series_animation;
        d->series_animation = NULL;
    }

    if (d->image_animation) {
//        delete d->image_animation;
        d->image_animation = NULL;
    }

    // Database query

    QSqlQuery query(*(medDatabaseController::instance()->database())); QVariant id;

    // Retrieve patient information

    QString patientName;
    QString patientThumbnail;

    query.prepare("SELECT name, thumbnail FROM patient WHERE id = :id");
    query.bindValue(":id", patientId);
    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NOCOLOR;

    if(query.first()) {
        patientName = query.value(0).toString();
        patientThumbnail = query.value(1).toString();

        if (d->patient_group)
            delete d->patient_group;

        d->patient_group = new medDatabasePreviewItemGroup;
        d->patient_group->addItem(new medDatabasePreviewItem(patientThumbnail));
    }

    // Retrieve studies information

    QVariant studyId;
    QVariant studyName;
    QVariant studyThumbnail;

    query.prepare("SELECT id, name, thumbnail FROM study WHERE patient = :patient");
    query.bindValue(":patient", patientId);
    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NOCOLOR;

    if(query.first()) {
        studyId = query.value(0);
        studyName = query.value(1);
        studyThumbnail = query.value(2);

        if (d->study_group)
            delete d->study_group;

        d->study_group = new medDatabasePreviewItemGroup;
        d->study_group->addItem(new medDatabasePreviewItem(studyThumbnail.toString()));
    }

    // Retrieve series information

    QVariant seriesId;
    QVariant seriesName;
    QVariant seriesThumbnail;

    query.prepare("SELECT id, name, thumbnail FROM series WHERE study = :study");
    query.bindValue(":study", studyId);
    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NOCOLOR;

    if(query.first()) {
        seriesId = query.value(0);
        seriesName = query.value(1);
        seriesThumbnail = query.value(2);

        if (d->series_group)
            delete d->series_group;

        d->series_group = new medDatabasePreviewItemGroup;
        d->series_group->addItem(new medDatabasePreviewItem(seriesThumbnail.toString()));
    }

    // Retrieve images information

    QVariant imageId;
    QVariant imageName;
    QVariant imageThumbnail;

    query.prepare("SELECT id, name, thumbnail FROM image WHERE series = :series");
    query.bindValue(":series", seriesId);
    if(!query.exec())
        qDebug() << DTK_COLOR_FG_RED << query.lastError() << DTK_NOCOLOR;

    if (d->image_group)
        delete d->image_group;

    d->image_group = new medDatabasePreviewItemGroup;

    while(query.next()) {
        imageId = query.value(0);
        imageName = query.value(1);
        imageThumbnail = query.value(2);

        d->image_group->addItem(new medDatabasePreviewItem(imageThumbnail.toString()));
    }

    d->scene->addGroup(d->image_group);
    d->scene->addGroup(d->series_group);
    d->scene->addGroup(d->study_group);
    d->scene->addGroup(d->patient_group);

    QTimer::singleShot(100, this, SLOT(onMoveBg()));
}

void medDatabasePreview::onStudyClicked(int id)
{
    qDebug() << __func__ << id;
}

void medDatabasePreview::onSeriesClicked(int id)
{
    qDebug() << __func__ << id;
}

void medDatabasePreview::onImageClicked(int id)
{
    qDebug() << __func__ << id;
}

void medDatabasePreview::onSlideUp(void)
{
    if(d->level > 2)
        return;

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

    medDatabasePreviewItem *target = dynamic_cast<medDatabasePreviewItem *>(d->scene->itemAt(d->selector->scenePos() + QPointF(d->selector->scenePos().x() * (-1) + query_offset, query_offset)));

    if(!target)
        return;

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

void medDatabasePreview::onCurrentChanged(QModelIndex index)
{
    qDebug() << __func__ << index;
}
