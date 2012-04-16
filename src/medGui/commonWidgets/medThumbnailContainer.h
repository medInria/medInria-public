#ifndef MEDTHUMBNAILCONTAINER_H
#define MEDTHUMBNAILCONTAINER_H

#include "medGuiExport.h"

#include <QtCore>
#include <QtGui>

class medDatabasePreviewItem;
class medThumbnailContainerPrivate;
class medDataIndex;

class MEDGUI_EXPORT medDeleteButton : public QObject, public QGraphicsRectItem
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


class MEDGUI_EXPORT medThumbnailContainer : public QFrame
{
    Q_OBJECT

public:
    medThumbnailContainer(QList<medDataIndex>& previouslyContainedIndexes, QWidget *parent = 0);
    ~medThumbnailContainer();

    void init();
    void reset();

    void setAllowDragging(bool isDraggingAllowed);
    void setAllowDropping(bool isDroppingAllowed);
    void setAllowDeleting(bool isDeletingAllowed);

    QList<medDataIndex> getContainedIndexes();

public slots:
    void addSeriesItem(const medDataIndex& seriesIndex);

protected slots:
    void onObjectDropped(const medDataIndex& index);
    void onItemHovered(medDatabasePreviewItem* item);
    void onThumbnailHoverEntered(QGraphicsSceneHoverEvent* event, medDatabasePreviewItem* item);
    void onThumbnailHoverLeft(QGraphicsSceneHoverEvent* event, medDatabasePreviewItem* item);
    void onDeleteButtonClicked();
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
