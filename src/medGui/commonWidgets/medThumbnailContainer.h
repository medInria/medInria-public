#ifndef MEDTHUMBNAILCONTAINER_H
#define MEDTHUMBNAILCONTAINER_H

#include "medGuiExport.h"

#include <QtCore>
#include <QtGui>

class medDatabasePreviewItem;
class medThumbnailContainerPrivate;
class medDataIndex;

class MEDGUI_EXPORT medThumbnailContainer : public QFrame
{
    Q_OBJECT

public:
    medThumbnailContainer(QWidget *parent = 0);
    ~medThumbnailContainer(void);

    void init(void);
    void reset(void);

signals:
    void seriesClicked(const medDataIndex& id);
    void  imageClicked(const medDataIndex& id);

//public slots:
//    void onPatientClicked(const medDataIndex& id);
//    void onStudyClicked(const medDataIndex& id);
//    void onSeriesClicked(const medDataIndex& id);

protected slots:
    void onSlideUp(void);
    void onSlideDw(void);
    void onMoveRt(void);
    void onMoveLt(void);
    void onMoveUp(void);
    void onMoveDw(void);
    void onMoveBg(void);
    void onHovered(medDatabasePreviewItem *item);

    void onObjectDropped (const medDataIndex& index);

protected:
    void moveToItem(medDatabasePreviewItem *target);

private:
    medThumbnailContainerPrivate *d;
};

#endif // MEDTHUMBNAILCONTAINER_H
