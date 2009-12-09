#include "medDatabaseController.h"
#include "medDatabaseNavigator.h"
#include "medDatabaseNavigatorController.h"
#include "medDatabaseNavigatorItem.h"
#include "medDatabaseNavigatorItemGroup.h"
#include "medDatabaseNavigatorScene.h"
#include "medDatabaseNavigatorSelector.h"
#include "medDatabaseNavigatorTooltip.h"
#include "medDatabaseNavigatorView.h"

#include <QtCore>
#include <QtGui>

#include <dtkCore/dtkGlobal.h>

class medDatabaseNavigatorPrivate
{
public:
    medDatabaseNavigatorScene *scene;
    medDatabaseNavigatorView *view;
    medDatabaseNavigatorSelector *selector;

    QPropertyAnimation *selector_position_animation;
    QPropertyAnimation *selector_rect_animation;
    QParallelAnimationGroup *selector_animation;
};

medDatabaseNavigator::medDatabaseNavigator(QWidget *parent) : QFrame(parent), d(new medDatabaseNavigatorPrivate)
{
    d->scene = new medDatabaseNavigatorScene(this);

    d->view = new medDatabaseNavigatorView(this);
    d->view->setScene(d->scene);

    d->selector = new medDatabaseNavigatorSelector;
    d->selector_position_animation = NULL;
    d->selector_rect_animation = NULL;
    d->selector_animation = NULL;
    d->scene->addItem(d->selector);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(d->view);

    medDatabaseNavigatorController::instance()->setOrientation(Qt::Vertical);
    medDatabaseNavigatorController::instance()->setItemSize(128, 128);
//    medDatabaseNavigatorController::instance()->orientation() == Qt::Horizontal
//        ? this->setFixedHeight(medDatabaseNavigatorController::instance()->groupHeight() + medDatabaseNavigatorController::instance()->itemSpacing() + 36) // 26 pixels for the scroller
//        : this->setFixedWidth(medDatabaseNavigatorController::instance()->groupWidth() + medDatabaseNavigatorController::instance()->itemSpacing() + 36); // 26 pixels for the scroller

    connect(d->view, SIGNAL(hovered(medDatabaseNavigatorItem*)), this, SLOT(onHovered(medDatabaseNavigatorItem*)));
}

medDatabaseNavigator::~medDatabaseNavigator(void)
{
    delete d;

    d = NULL;
}

void medDatabaseNavigator::reset(void)
{
    d->scene->reset();
}

void medDatabaseNavigator::onPatientClicked(int patientId)
{
    this->reset();

    QSqlQuery patientQuery(*(medDatabaseController::instance()->database())); QVariant id;

    // Retrieve patient information

    QString patientName;
    QString patientThumbnail;

    patientQuery.prepare("SELECT name, thumbnail FROM patient WHERE id = :id");
    patientQuery.bindValue(":id", patientId);
    if(!patientQuery.exec())
        qDebug() << DTK_COLOR_FG_RED << patientQuery.lastError() << DTK_NOCOLOR;

    if(patientQuery.first())
        patientName = patientQuery.value(0).toString();

    // Retrieve studies information

    QSqlQuery studyQuery(*(medDatabaseController::instance()->database()));

    QVariant studyId;
    QVariant studyName;
    QVariant studyThumbnail;

    studyQuery.prepare("SELECT id, name, thumbnail FROM study WHERE patient = :patient");
    studyQuery.bindValue(":patient", patientId);
    if(!studyQuery.exec())
        qDebug() << DTK_COLOR_FG_RED << studyQuery.lastError() << DTK_NOCOLOR;

    while(studyQuery.next()) {
        studyId = studyQuery.value(0);
        studyName = studyQuery.value(1);
        studyThumbnail = studyQuery.value(2);

        medDatabaseNavigatorItemGroup *group = new medDatabaseNavigatorItemGroup;

        group->setName(studyName.toString());

        // Retrieve series information

        QSqlQuery seriesQuery(*(medDatabaseController::instance()->database()));

        QVariant seriesId;
        QVariant seriesName;
        QVariant seriesThumbnail;

        seriesQuery.prepare("SELECT id, name, thumbnail FROM series WHERE study = :study");
        seriesQuery.bindValue(":study", studyId);
        if(!seriesQuery.exec())
            qDebug() << DTK_COLOR_FG_RED << seriesQuery.lastError() << DTK_NOCOLOR;

        while(seriesQuery.next()) {
            seriesId = seriesQuery.value(0);
            seriesName = seriesQuery.value(1);
            seriesThumbnail = seriesQuery.value(2);

            medDatabaseNavigatorItem *item = new medDatabaseNavigatorItem(id.toInt(), studyId.toInt(), seriesId.toInt(), -1, seriesThumbnail.toString());

            connect(item, SIGNAL(patientClicked(int)), this, SIGNAL(patientClicked(int)));
            connect(item, SIGNAL(studyClicked(int)), this, SIGNAL(studyClicked(int)));
            connect(item, SIGNAL(seriesClicked(int)), this, SIGNAL(seriesClicked(int)));
            connect(item, SIGNAL(imageClicked(int)), this, SIGNAL(imageClicked(int)));

            group->addItem(item);
        }

        d->scene->addGroup(group);
    }
}

void medDatabaseNavigator::onStudyClicked(int id)
{
    qDebug() << __func__ << id;
}

void medDatabaseNavigator::onSeriesClicked(int id)
{
    qDebug() << __func__ << id;
}

void medDatabaseNavigator::onImageClicked(int id)
{
    qDebug() << __func__ << id;
}

void medDatabaseNavigator::onHovered(medDatabaseNavigatorItem *item)
{
    qreal selector_width = medDatabaseNavigatorController::instance()->selectorWidth();
    qreal item_width = medDatabaseNavigatorController::instance()->itemWidth();
    qreal item_height = medDatabaseNavigatorController::instance()->itemHeight();
    qreal item_spacing = medDatabaseNavigatorController::instance()->itemSpacing();
    qreal item_margins = selector_width - item_width;
    qreal query_offset = medDatabaseNavigatorController::instance()->queryOffset();

    QPoint selector_offset(-4, -4);

//    QPoint selector_offset((medDatabaseNavigatorController::instance()->selectorWidth()  - medDatabaseNavigatorController::instance()->itemWidth())/-2,
//                           (medDatabaseNavigatorController::instance()->selectorHeight() - medDatabaseNavigatorController::instance()->itemHeight())/-2);

    if(qAbs(d->selector->pos().x() - item->scenePos().x()) < 20 && qAbs(d->selector->pos().y() - item->scenePos().y()) < 20)
        return;

    // Update selector

    d->selector->setText(item->path());

    // Selector position animation

    if(!d->selector_position_animation)
        d->selector_position_animation = new QPropertyAnimation(d->selector, "pos");

    d->selector_position_animation->setDuration(100);
    d->selector_position_animation->setStartValue(d->selector->pos());
    d->selector_position_animation->setEndValue(item->scenePos() + selector_offset);
    d->selector_position_animation->setEasingCurve(QEasingCurve::Linear);

    // Selector rect animation

    if(!d->selector_rect_animation)
        d->selector_rect_animation = new QPropertyAnimation(d->selector, "rect");

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

    d->selector_animation->start();
}
