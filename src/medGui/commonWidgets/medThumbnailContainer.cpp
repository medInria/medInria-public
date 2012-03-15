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


medDeleteButton::medDeleteButton() : QGraphicsRectItem()
{
    QRectF rec = QRectF(0, 0, 35, 35);
    setRect(rec);
    setPen(Qt::SolidLine);
    setBrush(Qt::red);
    setZValue(100); // this item should always be on top of all the others
}

medDeleteButton::~medDeleteButton(void)
{
}


void medDeleteButton::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    emit clicked();
}

class medThumbnailContainerPrivate
{
public:

    int columns;
    int rows;
    bool canDrag;
    bool canDrop;
    bool canDelete;

    QList<medDataIndex> containedIndexes;

    QGraphicsScene* scene;
    medDatabasePreviewView* view;

    medDatabasePreviewSelector *selector;

    medDataIndex current_index;
    medDatabasePreviewItem* current_item;

    QPropertyAnimation *selector_position_animation;
    QPropertyAnimation *selector_rect_animation;
    QParallelAnimationGroup *selector_animation;

    medDeleteButton* del;

    QList<medDatabasePreviewItem*> containedItems;
};

medThumbnailContainer::medThumbnailContainer(QWidget *parent) : QFrame(parent), d(new medThumbnailContainerPrivate)
{
    d->columns = 4;

    d->scene = new QGraphicsScene(this);
    d->scene->setBackgroundBrush(QColor(0x41, 0x41, 0x41));

    d->view = new medDatabasePreviewView(this);
    d->view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    d->view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

    d->view->setScene(d->scene);

    d->selector = new medDatabasePreviewSelector;
    d->selector->setRect(QRectF(5, 5, 138, 138));

    d->scene->addItem(d->selector);

    connect(d->view, SIGNAL(objectDropped (const medDataIndex&)), this, SLOT(onObjectDropped (const medDataIndex&)));

//    connect(d->view, SIGNAL(hovered(medDatabasePreviewItem*)), this, SLOT(onHovered(medDatabasePreviewItem*)));

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 0, 10, 10);
    layout->setSpacing(0);
    layout->addWidget(d->view);

    d->selector_position_animation = NULL;
    d->selector_rect_animation = NULL;
    d->selector_animation = NULL;

    d->del = new medDeleteButton();
//    QObject* pp = dynamic_cast<QObject*>(d->del);
//    if(!pp)
//        qDebug() << "FUCK MY LIFE";
    d->scene->addItem(d->del);
    connect(d->del, SIGNAL(clicked()), this, SLOT(onDeleteButtonClicked()));

    qreal item_width   = medDatabasePreviewController::instance()->itemWidth(); //128
    qreal item_height   = medDatabasePreviewController::instance()->itemHeight(); //128
    qreal item_spacing = medDatabasePreviewController::instance()->itemSpacing(); //10

    this->setFixedHeight(2 * (item_height + item_spacing) + item_spacing + 36); // 36 pixels for the scroller
    this->setFixedWidth(d->columns * (item_width + item_spacing) + item_spacing + 36); // 36 pixels for the scroller

    this->init();
}

medThumbnailContainer::~medThumbnailContainer(void)
{
    delete d;

    d = NULL;
}

void medThumbnailContainer::reset(void)
{
    foreach(medDatabasePreviewItem* item, d->containedItems)
            d->scene->removeItem(item);

    d->containedIndexes.clear();

//    d->current_index = NULL;
//    d->current_item = NULL;

    d->containedItems.clear();
}

void medThumbnailContainer::init(void)
{
    d->selector->hide();
    d->del->hide();
}

void medThumbnailContainer::addSeriesItem(const medDataIndex& index)
{
    if (!index.isValidForSeries())
            return;

    // we need to check if the image is already here
    if(!d->containedIndexes.contains(index))
    {
        medDatabasePreviewItem* item = new medDatabasePreviewItem( medDataIndex::makeSeriesIndex(index.dataSourceId(), index.patientId(), index.studyId(), index.seriesId()) );
        item->allowDrag(d->canDrag);
        item->setAcceptHoverEvents(true);

        connect(item, SIGNAL(hoverEntered(QGraphicsSceneHoverEvent*)), this, SLOT(onThumbnailHoverEntered(QGraphicsSceneHoverEvent*)) );
        connect(item, SIGNAL(hoverLeft(QGraphicsSceneHoverEvent*)), this, SLOT(onThumbnailHoverLeft(QGraphicsSceneHoverEvent*)) );

        qreal item_width   = medDatabasePreviewController::instance()->itemWidth();
        qreal item_height   = medDatabasePreviewController::instance()->itemHeight();
        qreal item_spacing = medDatabasePreviewController::instance()->itemSpacing();

        int left_margin = 10;
        int top_margin = 10;

        int items_count = d->containedIndexes.count();
        int items_in_last_row = items_count % d->columns;

        qreal space_taken_by_items_already_in_row =  items_in_last_row * (item_width + item_spacing);
        qreal pos_x = left_margin + space_taken_by_items_already_in_row;

        int full_columns_count = items_count / d->columns;
        qreal space_taken_by_items_on_top =  full_columns_count * (item_height + item_spacing);
        qreal pos_y = top_margin + space_taken_by_items_on_top;

        item->setPos(pos_x,  pos_y);

        qDebug() << "__x__ : " << pos_x << "__y__ : " << pos_y;

        d->scene->addItem(item);

        d->containedIndexes << index;
        d->containedItems << item;

        //d->series_group->addItem(item);

        moveToItem( item );
    }
}

void medThumbnailContainer::onObjectDropped (const medDataIndex& index)
{
    if(d->canDrop)
        addSeriesItem(index);
}

void medThumbnailContainer::moveToItem(medDatabasePreviewItem *target)
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

    target->ensureVisible(QRectF(0,0,item_width,item_height), selector_width + item_spacing, 0);

    if ( d->current_index.isValidForImage() )
    {
        d->selector->setText(QString(tr("Image id %1")).arg(d->current_index.imageId()));
    }
    else if (d->current_index.isValidForSeries() )
    {
        medAbstractDbController * dbc = medDataManager::instance()->controllerForDataSource(d->current_index.dataSourceId());
        QString seriesName = dbc->metaData(d->current_index, medMetaDataKeys::SeriesDescription);

        if ( seriesName.isEmpty() )
            seriesName = QString(tr("Series id %1")).arg(d->current_index.seriesId());

        d->selector->setText(seriesName);
    }
    else
    {
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

void medThumbnailContainer::onHoverEntered(medDatabasePreviewItem *item)
{
    qreal selector_width = medDatabasePreviewController::instance()->selectorWidth();
    qreal item_width = medDatabasePreviewController::instance()->itemWidth();
    qreal item_height = medDatabasePreviewController::instance()->itemHeight();
    qreal item_spacing = medDatabasePreviewController::instance()->itemSpacing();
    qreal item_margins = selector_width - item_width;
    qreal query_offset = medDatabasePreviewController::instance()->queryOffset();

    d->current_index = item->dataIndex();
    d->current_item = item;

    QPoint selector_offset(-4, -4);

    if(qAbs(d->selector->pos().x() - item->scenePos().x()) < 20 && qAbs(d->selector->pos().y() - item->scenePos().y()) < 20)
    {
        if(d->canDelete)
            showDeleteButton();
        return;
    }

    if ( d->current_index.isValidForImage() )
    {
        d->selector->setText(QString(tr("Image id %1")).arg(d->current_index.imageId()));
    }
    else if (d->current_index.isValidForSeries() )
    {
        medAbstractDbController * dbc = medDataManager::instance()->controllerForDataSource(d->current_index.dataSourceId());
        QString seriesName = dbc->metaData(d->current_index,medMetaDataKeys::SeriesDescription);
        d->selector->setText(seriesName);

        if ( seriesName.isEmpty() )
            seriesName = QString(tr("Series id %1")).arg(d->current_index.seriesId());
    }
    else
    {
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

    connect(d->selector_animation, SIGNAL(finished()), this, SLOT(onSelectorReachedThumbnail()));

    d->selector_animation->start();
}

void medThumbnailContainer::onSelectorReachedThumbnail()
{
    if(d->canDelete)
        showDeleteButton();
}

void medThumbnailContainer::onThumbnailHoverEntered(QGraphicsSceneHoverEvent* event)
{
    medDatabasePreviewItem* target = dynamic_cast<medDatabasePreviewItem*>(d->scene->itemAt(event->scenePos()));

    if(target)
        onHoverEntered(target);
}

void medThumbnailContainer::onThumbnailHoverLeft(QGraphicsSceneHoverEvent* event)
{
    d->del->hide();
}

void medThumbnailContainer::showDeleteButton()
{
    if(!d->canDelete)
        return;

    QPointF newPos = QPointF(d->selector->pos().rx() + 88, d->selector->pos().ry() + 8);
    d->del->setPos(newPos);
    d->del->show();
}

void medThumbnailContainer::onDeleteButtonClicked()
{
    if(d->current_item) {
        d->scene->removeItem(d->current_item);
        d->del->hide();
        d->containedIndexes.removeOne(d->current_index);
        d->containedItems.removeOne(d->current_item);

        if(d->containedIndexes.count() > 0)
        {
            // TODO let's move to any item, then we do it better
            medDatabasePreviewItem* target = d->containedItems[0];
            moveToItem(target);
        }
        else
        {
            d->selector->hide();
            d->selector->setPos(QPointF(0,0));
        }

    }

}

void medThumbnailContainer::setAllowDragging(bool isDraggingAllowed)
{
    d->canDrag = isDraggingAllowed;
}

void medThumbnailContainer::setAllowDropping(bool isDroppingAllowed)
{
    d->canDrop = isDroppingAllowed;
    d->view->setAcceptDrops(isDroppingAllowed);
}

void medThumbnailContainer::setAllowDeleting(bool isDeletingAllowed)
{
    d->canDelete = isDeletingAllowed;
}

void medThumbnailContainer::setColumnsCount(int columnsCount)
{
    d->columns = columnsCount;
}

void medThumbnailContainer::setRowsCount(int rowsCount)
{
    d->rows = rowsCount;
}
