/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medDatabasePreview.h"
#include <medDatabasePreviewController.h>
#include <medDatabasePreviewItem.h>
#include <medDatabasePreviewItemGroup.h>
#include <medDatabasePreviewScene.h>
#include <medDatabasePreviewSelector.h>
#include <medDatabasePreviewTooltip.h>
#include <medDatabasePreviewView.h>

#include <medAbstractDbController.h>
#include <medDataManager.h>
#include <medMetaDataKeys.h>
#include <medStorage.h>

#include <QtCore>
#include <QtGui>

#include <dtkCore/dtkGlobal.h>

class medDatabasePreviewPrivate
{
public:
    int level;

    medDataIndex currentIndex;

    medDatabasePreviewScene *scene;
    medDatabasePreviewView *view;
    medDatabasePreviewSelector *selector;

    medDatabasePreviewItemGroup *seriesGroup;
    medDatabasePreviewItemGroup *imageGroup;

    QPropertyAnimation *seriesAnimation;
    QPropertyAnimation *imageAnimation;
    QParallelAnimationGroup *animation;

    QPropertyAnimation *selectorPositionAnimation;
    QPropertyAnimation *selectorRectAnimation;
    QParallelAnimationGroup *selectorAnimation;

    medDatabasePreviewItem *target;
    int animationDuration;

    QPointF seriesGroupStartPos;
    QPointF imageGroupStartPos;

};

medDatabasePreview::medDatabasePreview(QWidget *parent) : QFrame(parent), d(new medDatabasePreviewPrivate)
{
    d->animationDuration = 500;

    d->level = 0;

    d->scene = new medDatabasePreviewScene(this);

    d->view = new medDatabasePreviewView(this);
    d->view->setScene(d->scene);

    d->selector = new medDatabasePreviewSelector;
    d->selector->setRect(QRectF(5, 5, 138, 138));

    d->scene->addItem(d->selector);

    d->seriesGroup = new medDatabasePreviewItemGroup;
    d->imageGroup = new medDatabasePreviewItemGroup;

    d->scene->addGroup(d->imageGroup);
    d->scene->addGroup(d->seriesGroup);

    d->seriesAnimation = NULL;
    d->imageAnimation = NULL;
    d->animation = NULL;

    d->imageGroupStartPos = d->imageGroup->pos();
    d->seriesGroupStartPos = d->seriesGroup->pos();

    d->selectorPositionAnimation = NULL;
    d->selectorRectAnimation = NULL;
    d->selectorAnimation = NULL;

    connect(d->view, SIGNAL(moveUp()), this, SLOT(onMoveUp()));
    connect(d->view, SIGNAL(moveDw()), this, SLOT(onMoveDw()));
    connect(d->view, SIGNAL(moveLt()), this, SLOT(onMoveLt()));
    connect(d->view, SIGNAL(moveRt()), this, SLOT(onMoveRt()));
    connect(d->view, SIGNAL(hovered(medDatabasePreviewItem*)), this, SLOT(onHovered(medDatabasePreviewItem*)));

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
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

    d->seriesGroup->clear();
    d->seriesGroup->setPos(d->seriesGroupStartPos);

    d->imageGroup->clear();
    d->imageGroup->setPos(d->imageGroupStartPos);

    d->level = 0;

    if (d->animation) {
        delete d->animation;
        d->animation = NULL;
    }

    if (d->seriesAnimation)
        d->seriesAnimation = NULL;

    if (d->imageAnimation)
        d->imageAnimation = NULL;
}

void medDatabasePreview::init(void)
{
    d->selector->hide();
}

void medDatabasePreview::onPatientClicked(const medDataIndex& id)
{
    d->seriesGroup->clear();
    d->imageGroup->clear();

    int firstSeId = -1;
    medAbstractDbController * db =  medDataManager::instance()->controllerForDataSource(id.dataSourceId());
    if ( db ) {

        QList<medDataIndex> studies = db->studies(id);
        for (QList<medDataIndex>::const_iterator studyIt( studies.begin() ); studyIt != studies.end(); ++studyIt ) {

            QList<medDataIndex> series = db->series(*studyIt);
            for (QList<medDataIndex>::const_iterator seriesIt( series.begin() ); seriesIt != series.end(); ++seriesIt ) {

                if ( firstSeId < 0)
                    firstSeId = (*seriesIt).seriesId();

                d->seriesGroup->addItem(new medDatabasePreviewItem(
                                            medDataIndex::makeSeriesIndex((*seriesIt).dataSourceId(), (*seriesIt).patientId(), (*seriesIt).studyId(), (*seriesIt).seriesId()) ) );
            }

        }
    }

    d->scene->setSceneRect(d->seriesGroup->boundingRect());

    if(d->level &&  !d->seriesGroup->childItems().isEmpty())
        this->onSlideDw();
    else
        moveToItem( d->seriesGroup->item(firstSeId) );
}

void medDatabasePreview::onStudyClicked(const medDataIndex& id)
{
    d->seriesGroup->clear();
    d->imageGroup->clear();

    int firstSeId = -1;
    medAbstractDbController * db =  medDataManager::instance()->controllerForDataSource(id.dataSourceId());
    if ( db ) {

        QList<medDataIndex> series = db->series(id);
        for (QList<medDataIndex>::const_iterator seriesIt( series.begin() ); seriesIt != series.end(); ++seriesIt ) {

            if ( firstSeId < 0)
                firstSeId = (*seriesIt).seriesId();

            d->seriesGroup->addItem(new medDatabasePreviewItem(
                                        medDataIndex::makeSeriesIndex((*seriesIt).dataSourceId(), (*seriesIt).patientId(), (*seriesIt).studyId(), (*seriesIt).seriesId()) ) );


        }
    }

    d->scene->setSceneRect(d->seriesGroup->boundingRect());

    if(d->level &&  !d->seriesGroup->childItems().isEmpty())
        this->onSlideDw();
    else
        moveToItem( d->seriesGroup->item(firstSeId) );
}

void medDatabasePreview::onSeriesClicked(const medDataIndex& id)
{
    medAbstractDbController * db =  medDataManager::instance()->controllerForDataSource(id.dataSourceId());

    d->seriesGroup->clear();

    if ( db ) {
        QList<medDataIndex> series = db->series(id);
        for (QList<medDataIndex>::const_iterator seriesIt( series.begin() ); seriesIt != series.end(); ++seriesIt ) {

            d->seriesGroup->addItem(new medDatabasePreviewItem(
                                        medDataIndex::makeSeriesIndex((*seriesIt).dataSourceId(), (*seriesIt).patientId(), (*seriesIt).studyId(), (*seriesIt).seriesId()) ) );
        }
    }

    // Image level
    int firstImageId(-1);

    d->imageGroup->clear();

    if ( db ) {
        QList<medDataIndex> images = db->images(id);
        for (QList<medDataIndex>::const_iterator imageIt( images.begin() ); imageIt != images.end(); ++imageIt ) {

            if (firstImageId < 0)
                firstImageId = (*imageIt).imageId();

            d->imageGroup->addItem(new medDatabasePreviewItem( medDataIndex(*imageIt ) ) );
        }

    }

    d->scene->setSceneRect(d->imageGroup->boundingRect());

    if(!d->level)
        onSlideUp();
    else
        moveToItem( d->imageGroup->item(firstImageId) );
}

void medDatabasePreview::onSlideUp(void)
{
    if(d->level)
        return;


    d->level++;

    qreal group_height  = medDatabasePreviewController::instance()->groupHeight();
    qreal group_spacing = medDatabasePreviewController::instance()->groupSpacing();

    if(!d->seriesAnimation)
        d->seriesAnimation = new QPropertyAnimation(d->seriesGroup, "pos");

    const QPointF slideOffset(0, group_height + group_spacing);

    d->seriesAnimation->setDuration(d->animationDuration);
    d->seriesAnimation->setStartValue(d->seriesGroup->pos());
    d->seriesAnimation->setEndValue(d->seriesGroupStartPos - slideOffset);
    d->seriesAnimation->setEasingCurve(QEasingCurve::OutBounce);

    if(!d->imageAnimation)
        d->imageAnimation = new QPropertyAnimation(d->imageGroup, "pos");


    d->imageAnimation->setDuration(d->animationDuration);
    d->imageAnimation->setStartValue(d->imageGroup->pos());
    d->imageAnimation->setEndValue(d->imageGroupStartPos - slideOffset);
    d->imageAnimation->setEasingCurve(QEasingCurve::OutBounce);

    if(!d->animation) {
        d->animation = new QParallelAnimationGroup(this);
        d->animation->addAnimation(d->seriesAnimation);
        d->animation->addAnimation(d->imageAnimation);
    }

    connect(d->animation, SIGNAL(finished()), this, SLOT(onMoveBg()));

    d->animation->start();
}

void medDatabasePreview::onSlideDw(void)
{
    if(!d->level)
        return;


    d->level--;

    qreal group_height  = medDatabasePreviewController::instance()->groupHeight();
    qreal group_spacing = medDatabasePreviewController::instance()->groupSpacing();

    if(!d->seriesAnimation)
        d->seriesAnimation = new QPropertyAnimation(d->seriesGroup, "pos");

    const QPointF slideOffset(0, group_height + group_spacing);

    d->seriesAnimation->setDuration(d->animationDuration);
    d->seriesAnimation->setStartValue(d->seriesGroup->pos());
    d->seriesAnimation->setEndValue(d->seriesGroupStartPos);
    d->seriesAnimation->setEasingCurve(QEasingCurve::OutBounce);

    if(!d->imageAnimation)
        d->imageAnimation = new QPropertyAnimation(d->imageGroup, "pos");

    d->imageAnimation->setDuration(d->animationDuration);
    d->imageAnimation->setStartValue(d->imageGroup->pos());
    d->imageAnimation->setEndValue(d->imageGroupStartPos);
    d->imageAnimation->setEasingCurve(QEasingCurve::OutBounce);

    if(!d->animation) {
        d->animation = new QParallelAnimationGroup(this);
        d->animation->addAnimation(d->seriesAnimation);
        d->animation->addAnimation(d->imageAnimation);
    }

    connect(d->animation, SIGNAL(finished()), this, SLOT(onMoveBg()));

    d->animation->start();
}

void medDatabasePreview::onMoveRt(void)
{
    qreal item_width = medDatabasePreviewController::instance()->itemWidth();
    qreal item_spacing = medDatabasePreviewController::instance()->itemSpacing();
    qreal query_offset = medDatabasePreviewController::instance()->queryOffset();

    QPoint selector_offset((medDatabasePreviewController::instance()->selectorWidth()  - medDatabasePreviewController::instance()->itemWidth())/-2,
                           (medDatabasePreviewController::instance()->selectorHeight() - medDatabasePreviewController::instance()->itemHeight())/-2);

    medDatabasePreviewItem *target = dynamic_cast<medDatabasePreviewItem *>(d->scene->itemAt(d->selector->pos() + QPointF(item_width + item_spacing + query_offset, query_offset)));

    if(!target)
        return;

    moveToItem( target );
}

void medDatabasePreview::onMoveLt(void)
{
    qreal item_width = medDatabasePreviewController::instance()->itemWidth();
    qreal item_spacing = medDatabasePreviewController::instance()->itemSpacing();
    qreal query_offset = medDatabasePreviewController::instance()->queryOffset();

    QPoint selector_offset((medDatabasePreviewController::instance()->selectorWidth()  - medDatabasePreviewController::instance()->itemWidth())/-2,
                           (medDatabasePreviewController::instance()->selectorHeight() - medDatabasePreviewController::instance()->itemHeight())/-2);

    medDatabasePreviewItem *target = dynamic_cast<medDatabasePreviewItem *>(d->scene->itemAt(d->selector->pos() + QPointF(-item_width - item_spacing + query_offset, query_offset)));

    moveToItem( target );
}

void medDatabasePreview::onMoveUp(void)
{
    if(d->level)
        this->onPatientClicked(d->currentIndex);
}

void medDatabasePreview::onMoveDw(void)
{
    if(!d->level)
        this->onSeriesClicked(d->currentIndex);
}

void medDatabasePreview::onMoveBg(void) // move to beginning of the current line
{
    if(!d->selector->isVisible())
        d->selector->show();

    medDatabasePreviewItem *target = NULL;

    target = dynamic_cast<medDatabasePreviewItem *>(d->scene->itemAt(40,40));

    if(!target) {
        d->selector->setPos(5, 5);
        return;
    }

    moveToItem( target );
}

void medDatabasePreview::moveToItem(medDatabasePreviewItem *target) // move to beginning of the current line
{

    if(!d->selector->isVisible())
        d->selector->show();

    if (!target)
    {
        d->selector->hide();
        return;
    }

    qreal selector_width = medDatabasePreviewController::instance()->selectorWidth();
    qreal item_width = medDatabasePreviewController::instance()->itemWidth();
    qreal item_height = medDatabasePreviewController::instance()->itemHeight();
    qreal item_spacing = medDatabasePreviewController::instance()->itemSpacing();
    qreal item_margins = selector_width - item_width;

    QPoint selector_offset((medDatabasePreviewController::instance()->selectorWidth()  - medDatabasePreviewController::instance()->itemWidth())/-2,
                           (medDatabasePreviewController::instance()->selectorHeight() - medDatabasePreviewController::instance()->itemHeight())/-2);

    d->currentIndex = target->dataIndex();

    target->ensureVisible(QRectF(0,0,item_width,item_height), medDatabasePreviewController::instance()->selectorWidth() + medDatabasePreviewController::instance()->itemSpacing(), 0);

    if ( d->currentIndex.isValidForImage() ) {
        d->selector->setText(QString(tr("Image id %1")).arg(d->currentIndex.imageId()));
    } else if (d->currentIndex.isValidForSeries() ) {
        medAbstractDbController * dbc = medDataManager::instance()->controllerForDataSource(d->currentIndex.dataSourceId());
        QString seriesName = dbc->metaData(d->currentIndex,medMetaDataKeys::SeriesDescription);
        if ( seriesName.isEmpty() )
            seriesName = QString(tr("Series id %1")).arg(d->currentIndex.seriesId());
        d->selector->setText(seriesName);
    } else {
        d->selector->setText(d->currentIndex.asString());
    }

    if(!d->selectorPositionAnimation)
        d->selectorPositionAnimation = new QPropertyAnimation(d->selector, "pos");

    d->selectorPositionAnimation->setDuration(100);
    d->selectorPositionAnimation->setStartValue(d->selector->pos());
    d->selectorPositionAnimation->setEndValue(target->scenePos() + selector_offset);
    d->selectorPositionAnimation->setEasingCurve(QEasingCurve::OutQuad);

    if(!d->selectorRectAnimation)
        d->selectorRectAnimation = new QPropertyAnimation(d->selector, "rect");

    d->selectorRectAnimation->setDuration(100);
    d->selectorRectAnimation->setStartValue(d->selector->rect());
    d->selectorRectAnimation->setEndValue(QRectF(0, 0, item_width + item_margins, item_height + item_margins + item_spacing));
    d->selectorRectAnimation->setEasingCurve(QEasingCurve::Linear);

    if(!d->selectorAnimation) {
        d->selectorAnimation = new QParallelAnimationGroup(this);
        d->selectorAnimation->addAnimation(d->selectorPositionAnimation);
        d->selectorAnimation->addAnimation(d->selectorRectAnimation);
    }

    d->selectorAnimation->start();
}

void medDatabasePreview::onHovered(medDatabasePreviewItem *item)
{

    qreal selector_width = medDatabasePreviewController::instance()->selectorWidth();
    qreal item_width = medDatabasePreviewController::instance()->itemWidth();
    qreal item_spacing = medDatabasePreviewController::instance()->itemSpacing();
    qreal item_margins = selector_width - item_width;

    d->currentIndex = item->dataIndex();

    QPoint selector_offset(-4, -4);

    if(qAbs(d->selector->pos().x() - item->scenePos().x()) < 20 && qAbs(d->selector->pos().y() - item->scenePos().y()) < 20)
        return;

    if ( d->currentIndex.isValidForImage() ) {
        d->selector->setText(QString(tr("Image id %1")).arg(d->currentIndex.imageId()));
    } else if (d->currentIndex.isValidForSeries() ) {
        medAbstractDbController * dbc = medDataManager::instance()->controllerForDataSource(d->currentIndex.dataSourceId());
        QString seriesName = dbc->metaData(d->currentIndex,medMetaDataKeys::SeriesDescription);
        d->selector->setText(seriesName);
        if ( seriesName.isEmpty() )
            seriesName = QString(tr("Series id %1")).arg(d->currentIndex.seriesId());
    } else {
        d->selector->setText(d->currentIndex.asString());
    }

    if(!d->selectorPositionAnimation)
        d->selectorPositionAnimation = new QPropertyAnimation(d->selector, "pos");

    d->selectorPositionAnimation->setDuration(100);
    d->selectorPositionAnimation->setStartValue(d->selector->pos());
    d->selectorPositionAnimation->setEndValue(item->scenePos() + selector_offset);
    d->selectorPositionAnimation->setEasingCurve(QEasingCurve::Linear);

    if(!d->selectorRectAnimation)
        d->selectorRectAnimation = new QPropertyAnimation(d->selector, "rect");

    d->selectorRectAnimation->setDuration(100);
    d->selectorRectAnimation->setStartValue(d->selector->rect());
    d->selectorRectAnimation->setEndValue(QRectF(item->boundingRect().x(), item->boundingRect().y(), item->boundingRect().width() + item_margins, item->boundingRect().height() + item_margins + item_spacing));
    d->selectorRectAnimation->setEasingCurve(QEasingCurve::Linear);

    if(!d->selectorAnimation) {
        d->selectorAnimation = new QParallelAnimationGroup(this);
        d->selectorAnimation->addAnimation(d->selectorPositionAnimation);
        d->selectorAnimation->addAnimation(d->selectorRectAnimation);
    }

    d->selectorAnimation->start();
}
