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

public slots:
    void mousePressEvent(QGraphicsSceneMouseEvent* event);
};


class MEDGUI_EXPORT medThumbnailContainer : public QFrame
{
    Q_OBJECT

public:
    medThumbnailContainer(QWidget *parent = 0);
    ~medThumbnailContainer(void);

    void init(void);
    void reset(void);

    void setAllowDragging(bool isDraggingAllowed);
    void setAllowDropping(bool isDroppingAllowed);
    void setAllowDeleting(bool isDeletingAllowed);
    void setColumnsCount(int columnsCount);
    void setRowsCount(int rowsCount);

    QList<medDataIndex> getContainedIndexes();

public slots:

    void addSeriesItem(const medDataIndex&);

protected slots:

    void onObjectDropped (const medDataIndex& index);
    void onHoverEntered(medDatabasePreviewItem* item);
    void onSelectorReachedThumbnail();
    void onThumbnailHoverEntered(QGraphicsSceneHoverEvent* event, medDatabasePreviewItem* item);
    void onThumbnailHoverLeft(QGraphicsSceneHoverEvent* event, medDatabasePreviewItem* item);
    void onDeleteButtonClicked();

private:

    void moveItem(medDatabasePreviewItem *target, QPointF pos);
    void moveSelectorToItem(medDatabasePreviewItem *target);
    void showDeleteButton();
    void updateSelectorLegend(const medDataIndex& index);
    void print();

private:
    medThumbnailContainerPrivate *d;
};

#endif // MEDTHUMBNAILCONTAINER_H
