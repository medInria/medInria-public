/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medThumbnailContainer.h>

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
#include <medDataIndex.h>
#include <medStorage.h>

#include <QtCore>
#include <QtGui>

#include <dtkCore/dtkGlobal.h>


medDeleteButton::medDeleteButton() : QGraphicsPixmapItem()
{
    const QPixmap pixmap(":pixmaps/delete.png");
    this->setPixmap(pixmap);
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

    int amountOfColumns;
    bool canDrag;
    bool canDrop;
    bool canDelete;
    bool showDeleteButton;
    bool blockHoverEnterEvent;
    bool firstTimeResizeEventIsCalled;

    QGraphicsScene* scene;
    medDatabasePreviewView* view;
    medDatabasePreviewSelector *selector;
    medDeleteButton* deleteButton;

    QPropertyAnimation *selectorPositionAnimation;
    QPropertyAnimation *selectorRectAnimation;
    QParallelAnimationGroup *selectorAnimation;

    medDatabasePreviewItem* currentSelectedItem;
    QList<medDataIndex> previouslyContainedIndexes;
    QList<medDatabasePreviewItem*> containedItems;
};

medThumbnailContainer::medThumbnailContainer(QList<medDataIndex>& previouslyContainedIndexes, QWidget* parent) : QFrame(parent), d(new medThumbnailContainerPrivate)
{
    d->previouslyContainedIndexes = previouslyContainedIndexes;

    d->amountOfColumns = 4;
    d->blockHoverEnterEvent = false;
    d->firstTimeResizeEventIsCalled = true;

    d->scene = new QGraphicsScene(this);
    d->scene->setBackgroundBrush(QColor(0x41, 0x41, 0x41));

    d->view = new medDatabasePreviewView();
    d->view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    d->view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    d->view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    d->view->setAcceptWheelEvent(true);
    d->view->setScene(d->scene);

    d->selector = new medDatabasePreviewSelector;
    d->scene->addItem(d->selector);

    connect(d->view, SIGNAL(objectDropped (const medDataIndex&)), this, SLOT(onObjectDropped (const medDataIndex&)));

    // a layout is needed for proper resizing
    QVBoxLayout *viewLayout = new QVBoxLayout();
    viewLayout->setContentsMargins(10, 8, 10, 13);
    viewLayout->setSpacing(0);
    viewLayout->addWidget(d->view);
    this->setLayout(viewLayout);

    d->selectorPositionAnimation = NULL;
    d->selectorRectAnimation = NULL;
    d->selectorAnimation = NULL;

    d->deleteButton = new medDeleteButton();
    d->scene->addItem(d->deleteButton);
    connect(d->deleteButton, SIGNAL(clicked()), this, SLOT(onDeleteButtonClicked()));

    qreal item_width = medDatabasePreviewController::instance()->itemWidth(); //128
    qreal item_height = medDatabasePreviewController::instance()->itemHeight(); //128


    this->setMinimumHeight(item_height);
    this->setMinimumWidth(item_width);
    this->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    this->setFrameStyle(QFrame::NoFrame);
    this->setAttribute(Qt::WA_MacShowFocusRect, false);

    d->selector->hide();
    d->deleteButton->hide();
}

medThumbnailContainer::~medThumbnailContainer()
{
    delete d;

    d = NULL;
}

void medThumbnailContainer::reset()
{
    // remove all images from the scene
    foreach(medDatabasePreviewItem* item, d->containedItems)
            d->scene->removeItem(item);

    d->currentSelectedItem = NULL;
    d->containedItems.clear();
}

void medThumbnailContainer::resizeEvent(QResizeEvent* resizeEvent)
{
//    qDebug() << "medThumbnailContainer Resize Event";

    // no resize if the widget was only made taller, as the number of columns won't change
    if( resizeEvent->oldSize().width() == resizeEvent->size().width() )
        return;

    // the first time the resize event is called is when the widget is being shown
    // for the first time. if so we want to add the previously selected images
    // (those passed to the constructor), otherwise we save the images we have now,
    // we calculate the new number of columns, and we put them back
    if( d->firstTimeResizeEventIsCalled )
    {
        d->firstTimeResizeEventIsCalled = false;
    }
    else
    {
        d->previouslyContainedIndexes = getContainedIndexes();
    }

    handleResize(resizeEvent->size());
}

void medThumbnailContainer::handleResize(const QSize& size)
{
    reset();

    qreal item_width = medDatabasePreviewController::instance()->itemWidth();
    qreal item_vertical_spacing = medDatabasePreviewController::instance()->itemSpacing();
    qreal item_horizontal_spacing = item_vertical_spacing + 6;

    int columns = size.width() / (int)(item_width + item_horizontal_spacing);

    if (columns == 0)
        columns = 1;

    d->amountOfColumns = columns;

    foreach(medDataIndex index, d->previouslyContainedIndexes)
    {
        addSeriesItem(index);
    }
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
        item->setAllowDragging(d->canDrag);
        item->setAcceptHoverEvents(true);

        connect(item, SIGNAL(hoverEntered(QGraphicsSceneHoverEvent*, medDatabasePreviewItem*)), this, SLOT(onThumbnailHoverEntered(QGraphicsSceneHoverEvent*, medDatabasePreviewItem*)) );
        connect(item, SIGNAL(hoverLeft(QGraphicsSceneHoverEvent*, medDatabasePreviewItem*)), this, SLOT(onThumbnailHoverLeft(QGraphicsSceneHoverEvent*, medDatabasePreviewItem*)) );

        qreal item_width = medDatabasePreviewController::instance()->itemWidth();
        qreal item_height = medDatabasePreviewController::instance()->itemHeight();
        qreal item_vertical_spacing = medDatabasePreviewController::instance()->itemSpacing();
        qreal item_horizontal_spacing = medDatabasePreviewController::instance()->itemSpacing() + 6;

        int left_margin = 10;
        int top_margin = 10;

        int items_count = d->containedItems.count();
        int items_in_last_row = items_count % d->amountOfColumns;

        qreal space_taken_by_items_already_in_row = items_in_last_row * (item_width + item_vertical_spacing);
        qreal pos_x = left_margin + space_taken_by_items_already_in_row;

        int number_of_full_columns = items_count / d->amountOfColumns;
        qreal space_taken_by_items_on_top =  number_of_full_columns * (item_height + item_horizontal_spacing);
        qreal pos_y = top_margin + space_taken_by_items_on_top;

        item->setPos(pos_x,  pos_y);
        d->scene->addItem(item);

        d->containedItems << item;

        moveSelectorToItem( item );
    }
}

void medThumbnailContainer::onObjectDropped (const medDataIndex& index)
{
    if(d->canDrop)
        addSeriesItem(index);
}

void medThumbnailContainer::moveItem(medDatabasePreviewItem* targetItem, QPointF newPos)
{
    if (!targetItem)
        return;

    // hover events shall be blocked otherwise the user can quickly move the mouse and disturb the current movement
    d->blockHoverEnterEvent = true;

    QPropertyAnimation* position_animation = new QPropertyAnimation(targetItem, "pos");

    position_animation->setDuration(200);
    position_animation->setStartValue(targetItem->pos());
    position_animation->setEndValue(newPos);
    position_animation->setEasingCurve(QEasingCurve::OutQuad);

    connect(position_animation, SIGNAL(finished()), this, SLOT(unblockHoverEvents()));

    position_animation->start();
}

void medThumbnailContainer::unblockHoverEvents()
{
    d->blockHoverEnterEvent = false;
}

void medThumbnailContainer::moveSelectorToItem(medDatabasePreviewItem* targetItem)
{
    if (!targetItem)
        return;

    if(!d->selector->isVisible())
        d->selector->show();

    qreal selector_width = medDatabasePreviewController::instance()->selectorWidth();
    qreal item_width = medDatabasePreviewController::instance()->itemWidth();
    qreal item_height = medDatabasePreviewController::instance()->itemHeight();
    qreal item_spacing = medDatabasePreviewController::instance()->itemSpacing();
    qreal item_margins = selector_width - item_width;

    QPoint selector_offset((medDatabasePreviewController::instance()->selectorWidth()  - medDatabasePreviewController::instance()->itemWidth())/-2,
                           (medDatabasePreviewController::instance()->selectorHeight() - medDatabasePreviewController::instance()->itemHeight())/-2);

    targetItem->ensureVisible(QRectF(0,0,item_width,item_height), selector_width + item_spacing, 0);

    updateSelectorLegend(targetItem->dataIndex());

    if(!d->selectorPositionAnimation)
        d->selectorPositionAnimation = new QPropertyAnimation(d->selector, "pos");

    d->selectorPositionAnimation->setDuration(100);
    d->selectorPositionAnimation->setStartValue(d->selector->pos());
    d->selectorPositionAnimation->setEndValue(targetItem->scenePos() + selector_offset);
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

void medThumbnailContainer::onItemHovered(medDatabasePreviewItem* item)
{
    qreal selector_width = medDatabasePreviewController::instance()->selectorWidth();
    qreal item_width = medDatabasePreviewController::instance()->itemWidth();
    qreal item_spacing = medDatabasePreviewController::instance()->itemSpacing();
    qreal item_margins = selector_width - item_width;

    d->currentSelectedItem = item;

    QPoint selector_offset((medDatabasePreviewController::instance()->selectorWidth()  - medDatabasePreviewController::instance()->itemWidth())/-2,
                           (medDatabasePreviewController::instance()->selectorHeight() - medDatabasePreviewController::instance()->itemHeight())/-2);

    // if the selector is already here don't move it
    if(qAbs(d->selector->pos().x() - item->scenePos().x()) < 20 && qAbs(d->selector->pos().y() - item->scenePos().y()) < 20)
    {
        tryShowDeleteButton();
        return;
    }

    updateSelectorLegend(item->dataIndex());

    // if the selector is visible, and not already where we want, we animate the movement
    if(d->selector->isVisible())
    {
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

        connect(d->selectorAnimation, SIGNAL(finished()), this, SLOT(tryShowDeleteButton()));

        d->selectorAnimation->start();
    }
    else
    {
        // but if it is not visible, we just update its position and show it
        d->selector->setPos(item->scenePos() + selector_offset);
        d->selector->setRect(QRectF(item->boundingRect().x(), item->boundingRect().y(), item->boundingRect().width() + item_margins, item->boundingRect().height() + item_margins + item_spacing));
        d->selector->show();
        tryShowDeleteButton();
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
        medAbstractDbController* dbc = medDataManager::instance()->controllerForDataSource(index.dataSourceId());
        QString seriesName = dbc->metaData(index, medMetaDataKeys::SeriesDescription);

        if (seriesName.isEmpty())
            seriesName = QString(tr("Series id %1")).arg(index.seriesId());

        d->selector->setText(seriesName);
    }
    else
    {
        d->selector->setText(index.asString());
    }
}

void medThumbnailContainer::onThumbnailHoverEntered(QGraphicsSceneHoverEvent* event, medDatabasePreviewItem* item)
{
    if(d->blockHoverEnterEvent)
        return;

    if(item)
    {
        // this flag is necessary because the button is made visible after the animation
        // hence it can be turned on even after it was turned off, due to the delay
        // for example, without this flag, it can happen that the user quickly enters and leaves a thumbnail
        // but the animation finishes after the mouse left and shows the delete button anyway
        d->showDeleteButton = true;
        onItemHovered(item);
    }
}

void medThumbnailContainer::onThumbnailHoverLeft(QGraphicsSceneHoverEvent* event, medDatabasePreviewItem* item)
{
    // read @onThumbnailHoverEntered to understand this flag
    d->showDeleteButton = false;
    d->deleteButton->hide();
}

void medThumbnailContainer::tryShowDeleteButton()
{
    if(!d->canDelete || !d->showDeleteButton)
        return;

    QPointF newPos = QPointF(d->selector->pos().rx() + 88, d->selector->pos().ry() + 8);
    d->deleteButton->setPos(newPos);
    d->deleteButton->show();
}

void medThumbnailContainer::onDeleteButtonClicked()
{
    // we will delete the item and try to fill the hole it left (if that's the case)
    // with the last item in the container (for simplicity)

    if(d->currentSelectedItem)
    {
        // if it is the last item or the only one
        // we just delete it and hide the selector

        if(d->containedItems.last()->dataIndex() == d->currentSelectedItem->dataIndex() || d->containedItems.count() == 1)
        {
            d->scene->removeItem(d->currentSelectedItem);
            d->deleteButton->hide();

            d->containedItems.removeOne(d->currentSelectedItem);

            d->selector->hide();
            d->currentSelectedItem = NULL;
        }
        else
        {
            // we have at least 2 items and we are not deleting the last one

            QPointF deletedItemPosition = d->currentSelectedItem->pos();
            d->scene->removeItem(d->currentSelectedItem);

            int indexOfDeletedItem = d->containedItems.indexOf(d->currentSelectedItem, 0);

            d->containedItems.removeOne(d->currentSelectedItem);

            medDatabasePreviewItem* lastItem = d->containedItems.last();

            moveItem(lastItem, deletedItemPosition);
            updateSelectorLegend(lastItem->dataIndex());

            d->currentSelectedItem = lastItem;

            if(d->containedItems.count() > 1)
                d->containedItems.move(d->containedItems.count() - 1, indexOfDeletedItem);
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

QList<medDataIndex> medThumbnailContainer::getContainedIndexes()
{
    QList<medDataIndex> getContainedIndexes;
    foreach(medDatabasePreviewItem* item, d->containedItems)
    {
        getContainedIndexes << item->dataIndex();
    }

    return getContainedIndexes;
}
