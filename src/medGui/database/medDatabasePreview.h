#pragma once

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QImage>


class medDataIndex;
class QLabel;


class medDatabasePreviewStaticScenePrivate;
class medDatabasePreviewStaticScene: public QGraphicsScene
{
    Q_OBJECT
public:
    medDatabasePreviewStaticScene(QObject *parent = NULL);
    virtual ~medDatabasePreviewStaticScene();

    void addImage(const medDataIndex &index);
    void setImage(const medDataIndex &index);
    medDataIndex& currentDataIndex() const;

    void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
signals:
    void openRequest(const medDataIndex& index);

private :
    medDatabasePreviewStaticScenePrivate *d;
};


class medDatabasePreviewDynamicScenePrivate;
class medDatabasePreviewDynamicScene: public medDatabasePreviewStaticScene
{
    Q_OBJECT
public:
    medDatabasePreviewDynamicScene(QMap<QString, medDataIndex> &seriesDescriptionDataIndexMap,
                                 QObject *parent = NULL);
    virtual ~medDatabasePreviewDynamicScene();
    void previewMouseMoveEvent(QMouseEvent *event, int width);

private:
    medDatabasePreviewDynamicScenePrivate *d;

};


class medDatabasePreviewPrivate;
class medDatabasePreview: public QGraphicsView
{
    Q_OBJECT
public :
    medDatabasePreview(QWidget *parent = NULL);
    virtual ~medDatabasePreview();

    enum medDataType
    {
        PATIENT,
        STUDY,
        SERIES
    };

public slots:
    void showSeriesPreview(const medDataIndex &index);
    void showStudyPreview(const medDataIndex &index);
    void showPatientPreview(const medDataIndex &index);

    QLabel* label() const;


protected:
    void resizeEvent(QResizeEvent *event);
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

signals:
    void openRequest(const medDataIndex& index);

private:
    medDatabasePreviewPrivate *d;
};



