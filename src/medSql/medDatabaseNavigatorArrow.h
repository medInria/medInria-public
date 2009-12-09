#ifndef MEDDATABASENAVIGATORARROW_H
#define MEDDATABASENAVIGATORARROW_H

#include <QtGui>

class medDatabaseNavigatorArrowPrivate;

class medDatabaseNavigatorArrow : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
     medDatabaseNavigatorArrow(QPixmap pixmap, QPixmap pressed, QGraphicsItem *parent = 0);
    ~medDatabaseNavigatorArrow(void);

signals:
    void clicked(void);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
    medDatabaseNavigatorArrowPrivate *d;
};

#endif // MEDDATABASENAVIGATORARROW_H
