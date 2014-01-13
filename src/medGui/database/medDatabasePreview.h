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
    void update(const medDataIndex &index);
    void setImage(const QImage &image);
    QLabel* label();

protected:
    void resizeEvent(QResizeEvent *event);

private:
    medDatabasePreviewPrivate *d;
};


