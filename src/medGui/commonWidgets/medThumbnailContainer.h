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

protected slots:
    void onObjectDropped (const medDataIndex& index);
    void onHoverEntered(medDatabasePreviewItem* item);
    void onSelectorReachedThumbnail();
    void onThumbnailHoverEntered(QGraphicsSceneHoverEvent* event);
    void onThumbnailHoverLeft(QGraphicsSceneHoverEvent* event);
    void onDeleteButtonClicked();

protected:
    void moveToItem(medDatabasePreviewItem *target);
    void showDeleteButton();

private:
    medThumbnailContainerPrivate *d;
};

#endif // MEDTHUMBNAILCONTAINER_H
