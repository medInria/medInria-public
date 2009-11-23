#ifndef MEDDATABASEPREVIEWARROW_H
#define MEDDATABASEPREVIEWARROW_H

#include <QtGui>

class medDatabasePreviewArrowPrivate;

class medDatabasePreviewArrow : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
     medDatabasePreviewArrow(QPixmap pixmap, QPixmap pressed, QGraphicsItem *parent = 0);
    ~medDatabasePreviewArrow(void);

signals:
    void clicked(void);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    medDatabasePreviewArrowPrivate *d;
};

#endif // MEDDATABASEPREVIEWARROW_H
