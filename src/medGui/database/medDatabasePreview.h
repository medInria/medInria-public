#pragma once

#include <QGraphicsView>
#include <QImage>

class medDatabasePreviewPrivate;
class medDataIndex;
class QLabel;


/**
 * @brief
 */
class medDatabasePreview: public QGraphicsView
{
    Q_OBJECT
public :
    medDatabasePreview(QWidget *parent = NULL);
    virtual ~medDatabasePreview();

public slots:
    void showSeriesThumbnail(const medDataIndex &index);
    void showStudyThumbnail(const medDataIndex &index);
    void showPatientThumbnail(const medDataIndex &index);

    void setImage(const QImage &image);
    void setIphotoImage(const QImage &image);
    void addImage(const QImage &image);
    QLabel* label();

protected:
    void resizeEvent(QResizeEvent *event);
    void mouseMoveEvent (QMouseEvent * event);
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);

private:
    medDatabasePreviewPrivate *d;
};


