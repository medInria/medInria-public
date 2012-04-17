#ifndef MEDTHUMBNAILCONTAINER_H
#define MEDTHUMBNAILCONTAINER_H

#include "medGuiExport.h"

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
    void onObjectDropped(const medDataIndex& index);
    void onItemHovered(medDatabasePreviewItem* item);
    void onThumbnailHoverEntered(QGraphicsSceneHoverEvent* event, medDatabasePreviewItem* item);
    void onThumbnailHoverLeft(QGraphicsSceneHoverEvent* event, medDatabasePreviewItem* item);
    void onDeleteButtonClicked();

private slots:
    void unblockHoverEvents();
    void tryShowDeleteButton();

protected:
    void paintEvent(QPaintEvent* paintEvent);
    void resizeEvent(QResizeEvent* resizeEvent);

private:
    void moveItem(medDatabasePreviewItem* targetItem, QPointF newPos);
    void moveSelectorToItem(medDatabasePreviewItem* targetItem);
    void updateSelectorLegend(const medDataIndex& index);
    void handleResize(const QSize& size);

private:
    medThumbnailContainerPrivate *d;
};

#endif // MEDTHUMBNAILCONTAINER_H
