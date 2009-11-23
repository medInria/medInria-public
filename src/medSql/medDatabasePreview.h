#ifndef MEDDATABASEPREVIEW_H
#define MEDDATABASEPREVIEW_H

#include <QtCore>
#include <QtGui>

class medDatabasePreviewItem;
class medDatabasePreviewPrivate;

class medDatabasePreview : public QFrame
{
    Q_OBJECT

public:
     medDatabasePreview(QWidget *parent = 0);
    ~medDatabasePreview(void);

    void setup(const QDir& dir);

    void setSelectionModel(QItemSelectionModel *model);

signals:
    void currentChanged(QModelIndex current, QItemSelectionModel::SelectionFlags);

public slots:
    void onPatientClicked(int id);
    void onStudyClicked(int id);
    void onSeriesClicked(int id);
    void onImageClicked(int id);

protected slots:
    void onSlideUp(void);
    void onSlideDw(void);

    void onMoveRt(void);
    void onMoveLt(void);
    void onMoveUp(void);
    void onMoveDw(void);
    void onMoveBg(void);

    void onHovered(medDatabasePreviewItem *item);

    void onCurrentChanged(QModelIndex index);

protected:
    medDatabasePreviewItem *setupPatient(const QDir& dir);
    medDatabasePreviewItem *setupStudy(const QDir& dir);
    medDatabasePreviewItem *setupSeries(const QDir& dir);

private:
    medDatabasePreviewPrivate *d;
};

#endif // MEDDATABASEPREVIEW_H
