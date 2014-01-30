/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medGuiExport.h>

#include <QtCore>
#include <QtGui>

class medDatabasePreviewItem;
class medThumbnailContainerPrivate;
class medDataIndex;

/**
* @class medDeleteButton
* @brief A button showing a cross will be displayed on top of images that can be deleted.
**/
class MEDGUI_EXPORT medDeleteButton : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
    medDeleteButton();
    ~medDeleteButton();

signals:
    void clicked();

private slots:
    void mousePressEvent(QGraphicsSceneMouseEvent* event);
};

/**
* @class medThumbnailContainer
* @brief A container of thumbnails that can be dragged, dropped and deleted.
* The @medThumbnailContainer is a @QFrame that can contain thumbnail of images.
* The thumbnails can be dragged, dropped and deleted.
**/
class MEDGUI_EXPORT medThumbnailContainer : public QFrame
{
    Q_OBJECT

public:
    medThumbnailContainer(QList<medDataIndex>& previouslyContainedIndexes, QWidget *parent = 0);
    ~medThumbnailContainer();

    /** Removes all images in the scene. **/
    void reset();

    /** Allow/forbid dragging of thumbnails. **/
    void setAllowDragging(bool isDraggingAllowed);

    /** Allow/forbid dropping of thumbnails. **/
    void setAllowDropping(bool isDroppingAllowed);

    /** Allow/forbid deleting of thumbnails. **/
    void setAllowDeleting(bool isDeletingAllowed);

    /** Returns a list of the currently contained @medDataIndex. **/
    QList<medDataIndex> getContainedIndexes();

public slots:

    /** Adds the image for @seriesIndex to the container. **/
    void addSeriesItem(const medDataIndex& seriesIndex);

protected slots:

    /**
     * This slot is called when an item is dragged and dropped.
     * It adds the corresponding thumbnail to the container.
     */
    void onObjectDropped(const medDataIndex& index);

    /**
     * This slot is called when the cursor enters an item.
     * It updates the position and legend of the selector and
     * shows a delete button if appropriate.
     */
    void onItemHovered(medDatabasePreviewItem* item);

    /** Calls @onItemHovered if hover events are allowed at the moment. **/
    void onThumbnailHoverEntered(QGraphicsSceneHoverEvent* event, medDatabasePreviewItem* item);

    /** Simply hides the delete button. **/
    void onThumbnailHoverLeft(QGraphicsSceneHoverEvent* event, medDatabasePreviewItem* item);

    /**
     * This slot is called when the user clicks the delete button.
     * It removes the selected item from the container.
     */
    void onDeleteButtonClicked();

private slots:

    /** Unblocks hover events. **/
    void unblockHoverEvents();

    /** Shows the delete button if appropriate. **/
    void tryShowDeleteButton();

protected:

    /** This function reorders all the contained items after a resize event. **/
    void resizeEvent(QResizeEvent* resizeEvent);

private:

    /** Moves the thumbnail @item to the new position @newPos. **/
    void moveItem(medDatabasePreviewItem* targetItem, QPointF newPos);

    /** Moves the selector to the target item **/
    void moveSelectorToItem(medDatabasePreviewItem* targetItem);

    /** Updates the legend of the selector with the information of @index. **/
    void updateSelectorLegend(const medDataIndex& index);

    /** Called by @resizeEvent. It actually handles the new placement of the contained items. **/
    void handleResize(const QSize& size);

private:
    medThumbnailContainerPrivate *d;
};


