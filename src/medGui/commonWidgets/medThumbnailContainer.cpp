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
    bool showDelBt;
    bool blockHoverEvents;

    QGraphicsScene* scene;
    medDatabasePreviewView* view;

    medDatabasePreviewSelector *selector;

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
    d->blockHoverEvents = false;

    d->scene = new QGraphicsScene(this);
    d->scene->setBackgroundBrush(QColor(0x41, 0x41, 0x41));

    d->view = new medDatabasePreviewView();
    d->view->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    d->view->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    d->view->setAcceptWheelEvent(true);

    d->view->setScene(d->scene);

    d->selector = new medDatabasePreviewSelector;
    d->selector->setRect(QRectF(5, 5, 138, 138));

    d->scene->addItem(d->selector);

    connect(d->view, SIGNAL(objectDropped (const medDataIndex&)), this, SLOT(onObjectDropped (const medDataIndex&)));

    d->view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    QVBoxLayout *layout = new QVBoxLayout();
    layout->setContentsMargins(10, 0, 10, 10);
    layout->setSpacing(0);
    layout->addWidget(d->view);
    this->setLayout(layout);


    d->selector_position_animation = NULL;
    d->selector_rect_animation = NULL;
    d->selector_animation = NULL;

    d->del = new medDeleteButton();
    d->scene->addItem(d->del);
    connect(d->del, SIGNAL(clicked()), this, SLOT(onDeleteButtonClicked()));

    qreal item_width   = medDatabasePreviewController::instance()->itemWidth(); //128
    qreal item_height   = medDatabasePreviewController::instance()->itemHeight(); //128
    qreal item_spacing = medDatabasePreviewController::instance()->itemSpacing(); //10

//    this->setMinimumHeight(2 * (item_height + item_spacing) + item_spacing + 36);
//    this->setMinimumWidth(d->columns * (item_width + item_spacing) + item_spacing + 36);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
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

    d->current_item = NULL;
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
    bool isContainedAlready = false;
    foreach(medDatabasePreviewItem* it, d->containedItems)
        isContainedAlready |= it->dataIndex() == index;

    if(!isContainedAlready)
    {
        medDatabasePreviewItem* item = new medDatabasePreviewItem( medDataIndex::makeSeriesIndex(index.dataSourceId(), index.patientId(), index.studyId(), index.seriesId()) );
        item->allowDrag(d->canDrag);
        item->setAcceptHoverEvents(true);

        connect(item, SIGNAL(hoverEntered(QGraphicsSceneHoverEvent*, medDatabasePreviewItem*)), this, SLOT(onThumbnailHoverEntered(QGraphicsSceneHoverEvent*, medDatabasePreviewItem*)) );
        connect(item, SIGNAL(hoverLeft(QGraphicsSceneHoverEvent*, medDatabasePreviewItem*)), this, SLOT(onThumbnailHoverLeft(QGraphicsSceneHoverEvent*, medDatabasePreviewItem*)) );

        qreal item_width   = medDatabasePreviewController::instance()->itemWidth();
        qreal item_height   = medDatabasePreviewController::instance()->itemHeight();
        qreal item_vertical_spacing = medDatabasePreviewController::instance()->itemSpacing();
        qreal item_horizontal_spacing = item_vertical_spacing + 6;

        int left_margin = 10;
        int top_margin = 10;

        int items_count = d->containedItems.count();
        int items_in_last_row = items_count % d->columns;

        qreal space_taken_by_items_already_in_row =  items_in_last_row * (item_width + item_vertical_spacing);
        qreal pos_x = left_margin + space_taken_by_items_already_in_row;

        int full_columns_count = items_count / d->columns;
        qreal space_taken_by_items_on_top =  full_columns_count * (item_height + item_horizontal_spacing);
        qreal pos_y = top_margin + space_taken_by_items_on_top;

        item->setPos(pos_x,  pos_y);
        d->scene->addItem(item);

        d->containedItems << item;

        moveSelectorToItem( item );

        //print();
    }
}

void medThumbnailContainer::onObjectDropped (const medDataIndex& index)
{
    if(d->canDrop)
        addSeriesItem(index);
}

void medThumbnailContainer::moveItem(medDatabasePreviewItem *target, QPointF pos)
{
    if (!target)
        return;

    d->blockHoverEvents = true;

    QPropertyAnimation* ani = new QPropertyAnimation(target, "pos");

    ani->setDuration(200);
    ani->setStartValue(target->pos());
    ani->setEndValue(pos);
    ani->setEasingCurve(QEasingCurve::OutQuad);

    connect(ani, SIGNAL(finished()), this, SLOT(unblockHoverEvents()));

    ani->start();
}

void medThumbnailContainer::unblockHoverEvents()
{
    d->blockHoverEvents = false;
}

void medThumbnailContainer::moveSelectorToItem(medDatabasePreviewItem *target)
{
    if (!target)
        return;

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

    medDataIndex current_index = target->dataIndex();

    target->ensureVisible(QRectF(0,0,item_width,item_height), selector_width + item_spacing, 0);

    updateSelectorLegend(current_index);

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

    medDataIndex current_index = item->dataIndex();
    d->current_item = item;

    QPoint selector_offset((medDatabasePreviewController::instance()->selectorWidth()  - medDatabasePreviewController::instance()->itemWidth())/-2,
                           (medDatabasePreviewController::instance()->selectorHeight() - medDatabasePreviewController::instance()->itemHeight())/-2);

    // if the selector is already here do nothing
    if(qAbs(d->selector->pos().x() - item->scenePos().x()) < 20 && qAbs(d->selector->pos().y() - item->scenePos().y()) < 20)
    {
        showDeleteButton();
        return;
    }


    updateSelectorLegend(current_index);

    if(d->selector->isVisible())
    {
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
    else
    {
        d->selector->setPos(item->scenePos() + selector_offset);
        d->selector->setRect(QRectF(item->boundingRect().x(), item->boundingRect().y(), item->boundingRect().width() + item_margins, item->boundingRect().height() + item_margins + item_spacing));
        d->selector->show();
        showDeleteButton();
    }
}

void medThumbnailContainer::updateSelectorLegend(const medDataIndex& index)
{
    if ( index.isValidForImage() )
    {
        d->selector->setText(QString(tr("Image id %1")).arg(index.imageId()));
    }
    else if (index.isValidForSeries() )
    {
        medAbstractDbController * dbc = medDataManager::instance()->controllerForDataSource(index.dataSourceId());
        QString seriesName = dbc->metaData(index, medMetaDataKeys::SeriesDescription);

        if ( seriesName.isEmpty() )
            seriesName = QString(tr("Series id %1")).arg(index.seriesId());

        d->selector->setText(seriesName);
    }
    else
    {
        d->selector->setText(index.asString());
    }
}

void medThumbnailContainer::onSelectorReachedThumbnail()
{
    if(d->canDelete)
        showDeleteButton();
}

void medThumbnailContainer::onThumbnailHoverEntered(QGraphicsSceneHoverEvent* event, medDatabasePreviewItem* item)
{
    if(d->blockHoverEvents)
        return;

    if(item)
    {
        // this flag is necessary bc the button is made visible after the animation
        // hence it can be turned on even after it was turned off, due to the delay
        d->showDelBt = true;
        onHoverEntered(item);
    }
}

void medThumbnailContainer::onThumbnailHoverLeft(QGraphicsSceneHoverEvent* event, medDatabasePreviewItem* item)
{
//    if(d->blockHoverEvents)
//        return;

    d->showDelBt = false;
    d->del->hide();
}

void medThumbnailContainer::showDeleteButton()
{
    if(!d->canDelete || !d->showDelBt)
        return;

    QPointF newPos = QPointF(d->selector->pos().rx() + 88, d->selector->pos().ry() + 8);
    d->del->setPos(newPos);
    d->del->show();
}

void medThumbnailContainer::onDeleteButtonClicked()
{
    if(d->current_item) {

        // if it is the last item or the only one
        // we just delete it and hide the selector

        if(d->containedItems.last()->dataIndex() == d->current_item->dataIndex() || d->containedItems.count() == 1)
        {
            d->scene->removeItem(d->current_item);
            d->del->hide();

            d->containedItems.removeOne(d->current_item);

            d->selector->hide();
//            d->selector->setPos(QPointF(0,0));
            d->current_item = NULL;
        }
        else
        {
            // we have at least 2 and we are not deleting the last one
            QPointF deletedItemPosition = d->current_item->pos();
            d->scene->removeItem(d->current_item);

            int i2 = d->containedItems.indexOf(d->current_item, 0);

            d->containedItems.removeOne(d->current_item);

            medDatabasePreviewItem* last = d->containedItems.last();

            moveItem(last, deletedItemPosition);
            updateSelectorLegend(last->dataIndex());

            d->current_item = last;

            if(d->containedItems.count() > 1)
            {
                d->containedItems.move(d->containedItems.count() - 1, i2);
            }
        }
    }
}

void medThumbnailContainer::print()
{
    qDebug() << d->containedItems.count();

    QStringList pepe;
    foreach(medDatabasePreviewItem* it, d->containedItems)
    {
        medDataIndex index = it->dataIndex();
        QString lala;
        if ( index.isValidForImage() )
        {
            lala = QString(tr("Image id %1")).arg(index.imageId());
        }
        else if (index.isValidForSeries() )
        {
            medAbstractDbController * dbc = medDataManager::instance()->controllerForDataSource(index.dataSourceId());
            QString seriesName = dbc->metaData(index, medMetaDataKeys::SeriesDescription);

            if ( seriesName.isEmpty() )
                seriesName = QString(tr("Series id %1")).arg(index.seriesId());

            lala = seriesName;
        }
        else
        {
            lala = index.asString();
        }

        pepe << lala;
    }
    qDebug() << pepe;
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

QList<medDataIndex> medThumbnailContainer::getContainedIndexes()
{
    QList<medDataIndex> pepe = *(new QList<medDataIndex>()); //TODO dejo el new o lo  saco?
    foreach(medDatabasePreviewItem* it, d->containedItems)
    {
        medDataIndex index = it->dataIndex();
        pepe << index;
    }

    return pepe;
}
