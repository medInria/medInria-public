#ifndef MEDDATABASEPREVIEWVIEW_H
#define MEDDATABASEPREVIEWVIEW_H

#include <QtCore>
#include <QtGui>

class medDatabasePreviewItem;
class medDatabasePreviewViewPrivate;
class medDatabasePreviewTooltip;

class medDatabasePreviewView : public QGraphicsView
{
    Q_OBJECT

public:
     medDatabasePreviewView(QWidget *parent = 0);
    ~medDatabasePreviewView(void);

    void setScene(QGraphicsScene *scene);

    void setPatientTooltip(medDatabasePreviewTooltip *tooltip);
    void setStudyTooltip(medDatabasePreviewTooltip *tooltip);
    void setSeriesTooltip(medDatabasePreviewTooltip *tooltip);
    void setImageTooltip(medDatabasePreviewTooltip *tooltip);

signals:
//    void patientClicked(int id);
//    void   studyClicked(int id);
//    void  seriesClicked(int id);
//    void   imageClicked(int id);

    void slideUp(void);
    void slideDw(void);
    void moveUp(void);
    void moveDw(void);
    void moveRt(void);
    void moveLt(void);

    void hovered(medDatabasePreviewItem *item);

protected slots:
    void onFrameChanged(int frame);

protected:
    void keyPressEvent(QKeyEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    medDatabasePreviewViewPrivate *d;
};

#endif // MEDDATABASEPREVIEWVIEW_H
