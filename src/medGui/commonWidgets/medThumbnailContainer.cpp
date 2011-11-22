#include "medThumbnailContainer.h"
#include "medDatabasePreviewController.h"
#include "medDatabasePreviewItem.h"
#include "medDatabasePreviewItemGroup.h"
#include "medDatabasePreviewScene.h"
#include "medDatabasePreviewSelector.h"
#include "medDatabasePreviewTooltip.h"
#include "medDatabasePreviewView.h"

#include <medAbstractDbController.h>
#include <medDataManager.h>
#include <medMetaDataKeys.h>
#include <medDataIndex.h>
#include <medStorage.h>

#include <QtCore>
#include <QtGui>

#include <dtkCore/dtkGlobal.h>

class medThumbnailContainerPrivate
{
public:
    int level;

    medDataIndex current_index;

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
    int animationDuration;

    QPointF series_groupStartPos;
    QPointF image_groupStartPos;

   QList<medDataIndex>  containedIndexes;

};

medThumbnailContainer::medThumbnailContainer(QWidget *parent) : QFrame(parent), d(new medThumbnailContainerPrivate)
{
    d->animationDuration = 500;

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

    d->image_groupStartPos = d->image_group->pos();
    d->series_groupStartPos = d->series_group->pos();

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

    //TODO solve this...
//    medDatabasePreviewController::instance()->orientation() == Qt::Horizontal
//        ? this->setFixedHeight(medDatabasePreviewController::instance()->groupHeight() + medDatabasePreviewController::instance()->itemSpacing() + 36) // 26 pixels for the scroller
//        : this->setFixedWidth(medDatabasePreviewController::instance()->groupWidth() + medDatabasePreviewController::instance()->itemSpacing() + 36); // 26 pixels for the scroller


//    setAcceptDrops(true);
    d->view->setAcceptDrops(true);

    connect(d->view, SIGNAL(objectDropped (const medDataIndex&)), this, SLOT(onObjectDropped (const medDataIndex&)));

    this->init();
}

medThumbnailContainer::~medThumbnailContainer(void)
{
    delete d;

    d = NULL;
}

void medThumbnailContainer::reset(void)
{
    d->scene->reset();
    d->series_group->setPos(d->series_groupStartPos);
    d->image_group->setPos(d->image_groupStartPos);

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

void medThumbnailContainer::init(void)
{
    d->selector->hide();
}

//void medThumbnailContainer::onPatientClicked(const medDataIndex& id)
//{
//    d->series_group->clear();
//    d->image_group->clear();
//
//    int firstSeId = -1;
//    medAbstractDbController * db =  medDataManager::instance()->controllerForDataSource(id.dataSourceId());
//    if ( db ) {
//
//        QList<medDataIndex> studies = db->studies(id);
//        for (QList<medDataIndex>::const_iterator studyIt( studies.begin() ); studyIt != studies.end(); ++studyIt ) {
//
//            QList<medDataIndex> series = db->series(*studyIt);
//            for (QList<medDataIndex>::const_iterator seriesIt( series.begin() ); seriesIt != series.end(); ++seriesIt ) {
//
//                if ( firstSeId < 0)
//                    firstSeId = (*seriesIt).seriesId();
//
//                d->series_group->addItem(new medDatabasePreviewItem(
//                    medDataIndex::makeSeriesIndex((*seriesIt).dataSourceId(), (*seriesIt).patientId(), (*seriesIt).studyId(), (*seriesIt).seriesId()) ) );
//            }
//
//        }
//    }
//
//    d->scene->setSceneRect(d->series_group->boundingRect());
//
//    if(d->level)
//        this->onSlideDw();
//    else
//        moveToItem( d->series_group->item(firstSeId) );
//}
//
//void medThumbnailContainer::onStudyClicked(const medDataIndex& id)
//{
//    d->series_group->clear();
//    d->image_group->clear();
//
//    int firstSeId = -1;
//    medAbstractDbController * db =  medDataManager::instance()->controllerForDataSource(id.dataSourceId());
//    if ( db ) {
//
//        QList<medDataIndex> series = db->series(id);
//            for (QList<medDataIndex>::const_iterator seriesIt( series.begin() ); seriesIt != series.end(); ++seriesIt ) {
//
//                if ( firstSeId < 0)
//                    firstSeId = (*seriesIt).seriesId();
//
//                d->series_group->addItem(new medDatabasePreviewItem(
//                    medDataIndex::makeSeriesIndex((*seriesIt).dataSourceId(), (*seriesIt).patientId(), (*seriesIt).studyId(), (*seriesIt).seriesId()) ) );
//            }
//    }
//
//    d->scene->setSceneRect(d->series_group->boundingRect());
//
//    if(d->level)
//        this->onSlideDw();
//    else
//        moveToItem( d->series_group->item(firstSeId) );
//}
//
//void medThumbnailContainer::onSeriesClicked(const medDataIndex& id)
//{
//    medAbstractDbController * db =  medDataManager::instance()->controllerForDataSource(id.dataSourceId());
//
//    d->series_group->clear();
//
//    if ( db ) {
//        QList<medDataIndex> series = db->series(id);
//        for (QList<medDataIndex>::const_iterator seriesIt( series.begin() ); seriesIt != series.end(); ++seriesIt ) {
//
//            d->series_group->addItem(new medDatabasePreviewItem(
//                medDataIndex::makeSeriesIndex((*seriesIt).dataSourceId(), (*seriesIt).patientId(), (*seriesIt).studyId(), (*seriesIt).seriesId()) ) );
//        }
//    }
//
//    // Image level
//    int firstImageId(-1);
//
//    d->image_group->clear();
//
//    if ( db ) {
//        QList<medDataIndex> images = db->images(id);
//        for (QList<medDataIndex>::const_iterator imageIt( images.begin() ); imageIt != images.end(); ++imageIt ) {
//
//            if (firstImageId < 0)
//                firstImageId = (*imageIt).imageId();
//
//            d->image_group->addItem(new medDatabasePreviewItem( medDataIndex(*imageIt ) ) );
//        }
//
//    }
//
//    d->scene->setSceneRect(d->image_group->boundingRect());
//
//    if(!d->level)
//        onSlideUp();
//    else
//        moveToItem( d->image_group->item(firstImageId) );
//}

void medThumbnailContainer::onSlideUp(void)
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

    const QPointF slideOffset(0, group_height + group_spacing);

    d->series_animation->setDuration(d->animationDuration);
    d->series_animation->setStartValue(d->series_group->pos());
    d->series_animation->setEndValue(d->series_groupStartPos - slideOffset);
    d->series_animation->setEasingCurve(QEasingCurve::OutBounce);

    if(!d->image_animation)
        d->image_animation = new QPropertyAnimation(d->image_group, "pos");


    d->image_animation->setDuration(d->animationDuration);
    d->image_animation->setStartValue(d->image_group->pos());
    d->image_animation->setEndValue(d->image_groupStartPos - slideOffset);
    d->image_animation->setEasingCurve(QEasingCurve::OutBounce);

    if(!d->animation) {
        d->animation = new QParallelAnimationGroup(this);
        d->animation->addAnimation(d->series_animation);
        d->animation->addAnimation(d->image_animation);
    }

    connect(d->animation, SIGNAL(finished()), this, SLOT(onMoveBg()));

    d->animation->start();
}

void medThumbnailContainer::onSlideDw(void)
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

    const QPointF slideOffset(0, group_height + group_spacing);

    d->series_animation->setDuration(d->animationDuration);
    d->series_animation->setStartValue(d->series_group->pos());
    d->series_animation->setEndValue(d->series_groupStartPos);
    d->series_animation->setEasingCurve(QEasingCurve::OutBounce);

    if(!d->image_animation)
        d->image_animation = new QPropertyAnimation(d->image_group, "pos");

    d->image_animation->setDuration(d->animationDuration);
    d->image_animation->setStartValue(d->image_group->pos());
    d->image_animation->setEndValue(d->image_groupStartPos);
    d->image_animation->setEasingCurve(QEasingCurve::OutBounce);

    if(!d->animation) {
        d->animation = new QParallelAnimationGroup(this);
        d->animation->addAnimation(d->series_animation);
        d->animation->addAnimation(d->image_animation);
    }

    connect(d->animation, SIGNAL(finished()), this, SLOT(onMoveBg()));

    d->animation->start();
}

void medThumbnailContainer::onMoveRt(void)
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
    
    moveToItem( target );
}

void medThumbnailContainer::onMoveLt(void)
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

    moveToItem( target );
}

void medThumbnailContainer::onMoveUp(void)
{
//    if(d->level)
//        this->onPatientClicked(d->current_index);
}

void medThumbnailContainer::onMoveDw(void)
{
//    if(!d->level)
//        this->onSeriesClicked(d->current_index);
}

void medThumbnailContainer::onMoveBg(void) // move to beginning of the current line
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

void medThumbnailContainer::moveToItem(medDatabasePreviewItem *target) // move to beginning of the current line
{

    if(!d->selector->isVisible())
        d->selector->show();

    if (!target)
        return;
    
    qreal selector_width = medDatabasePreviewController::instance()->selectorWidth();
    qreal item_width = medDatabasePreviewController::instance()->itemWidth();
    qreal item_height = medDatabasePreviewController::instance()->itemHeight();
    qreal item_spacing = medDatabasePreviewController::instance()->itemSpacing();
    qreal item_margins = selector_width - item_width;
    qreal query_offset = medDatabasePreviewController::instance()->queryOffset();
    
    QPoint selector_offset((medDatabasePreviewController::instance()->selectorWidth()  - medDatabasePreviewController::instance()->itemWidth())/-2,
                           (medDatabasePreviewController::instance()->selectorHeight() - medDatabasePreviewController::instance()->itemHeight())/-2);
    
    d->current_index = target->dataIndex();
    
    target->ensureVisible(QRectF(0,0,item_width,item_height), medDatabasePreviewController::instance()->selectorWidth() + medDatabasePreviewController::instance()->itemSpacing(), 0);

    if ( d->current_index.isValidForImage() ) {
        d->selector->setText(QString(tr("Image id %1")).arg(d->current_index.imageId()));
    } else if (d->current_index.isValidForSeries() ) {
        medAbstractDbController * dbc = medDataManager::instance()->controllerForDataSource(d->current_index.dataSourceId());
        QString seriesName = dbc->metaData(d->current_index,medMetaDataKeys::SeriesDescription);
        if ( seriesName.isEmpty() )
            seriesName = QString(tr("Series id %1")).arg(d->current_index.seriesId());
        d->selector->setText(seriesName);
    } else {
        d->selector->setText(d->current_index.asString());
    }

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
    d->selector_rect_animation->setEndValue(QRectF(0, 0, item_width + item_margins, item_height + item_margins + item_spacing));
    d->selector_rect_animation->setEasingCurve(QEasingCurve::Linear);
    
    if(!d->selector_animation) {
        d->selector_animation = new QParallelAnimationGroup(this);
        d->selector_animation->addAnimation(d->selector_position_animation);
        d->selector_animation->addAnimation(d->selector_rect_animation);
    }
    
    d->selector_animation->start();
}

void medThumbnailContainer::onHovered(medDatabasePreviewItem *item)
{
    
    qreal selector_width = medDatabasePreviewController::instance()->selectorWidth();
    qreal item_width = medDatabasePreviewController::instance()->itemWidth();
    qreal item_height = medDatabasePreviewController::instance()->itemHeight();
    qreal item_spacing = medDatabasePreviewController::instance()->itemSpacing();
    qreal item_margins = selector_width - item_width;
    qreal query_offset = medDatabasePreviewController::instance()->queryOffset();

    d->current_index = item->dataIndex();

    QPoint selector_offset(-4, -4);

    if(qAbs(d->selector->pos().x() - item->scenePos().x()) < 20 && qAbs(d->selector->pos().y() - item->scenePos().y()) < 20)
        return;

    if ( d->current_index.isValidForImage() ) {
        d->selector->setText(QString(tr("Image id %1")).arg(d->current_index.imageId()));
    } else if (d->current_index.isValidForSeries() ) {
        medAbstractDbController * dbc = medDataManager::instance()->controllerForDataSource(d->current_index.dataSourceId());
        QString seriesName = dbc->metaData(d->current_index,medMetaDataKeys::SeriesDescription);
        d->selector->setText(seriesName);
        if ( seriesName.isEmpty() )
            seriesName = QString(tr("Series id %1")).arg(d->current_index.seriesId());
    } else {
        d->selector->setText(d->current_index.asString());
    }

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


void medThumbnailContainer::onObjectDropped (const medDataIndex& index)
{
//        d->series_group->clear();
//        d->image_group->clear();

        if (!index.isValidForSeries())
            return;

//        int firstSeId = -1;
//        medAbstractDbController * db =  medDataManager::instance()->controllerForDataSource(index.dataSourceId());
//        if ( db ) {
//
//            QList<medDataIndex> series = db->series(index);
//                for (QList<medDataIndex>::const_iterator seriesIt( series.begin() ); seriesIt != series.end(); ++seriesIt ) {
//
//                    if ( firstSeId < 0)
//                        firstSeId = (*seriesIt).seriesId();
//
//                    d->series_group->addItem(new medDatabasePreviewItem(
//                        medDataIndex::makeSeriesIndex((*seriesIt).dataSourceId(), (*seriesIt).patientId(), (*seriesIt).studyId(), (*seriesIt).seriesId()) ) );
//                }
//        }

        // we need to check if the image is already here
        if(!d->containedIndexes.contains(index))
        {
            d->containedIndexes << index;


            medDatabasePreviewItem* item = new medDatabasePreviewItem( medDataIndex::makeSeriesIndex(index.dataSourceId(), index.patientId(), index.studyId(), index.seriesId()) );
            item->allowDrag(false);

            d->series_group->addItem(item);

            d->scene->setSceneRect(d->series_group->boundingRect());

            if(d->level)
                this->onSlideDw();
            else
                moveToItem( d->series_group->item(index.seriesId()) );
        }
}

