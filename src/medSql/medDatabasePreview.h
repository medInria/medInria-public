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

    void init(void);
    void reset(void);

signals:
    void patientClicked(int id);
    void   studyClicked(int id);
    void  seriesClicked(int id);
    void   imageClicked(int id);

public slots:
    void onPatientClicked(int id);
    void   onStudyClicked(int id);
    void  onSeriesClicked(int id);
    void   onImageClicked(int id);

protected slots:
    void onSlideUp(void);
    void onSlideDw(void);

    void onMoveRt(void);
    void onMoveLt(void);
    void onMoveUp(void);
    void onMoveDw(void);
    void onMoveBg(void);

    void onHovered(medDatabasePreviewItem *item);

private:
    medDatabasePreviewPrivate *d;
};

#endif // MEDDATABASEPREVIEW_H
