#ifndef MEDDATABASENAVIGATORVIEW_H
#define MEDDATABASENAVIGATORVIEW_H

#include <QtCore>
#include <QtGui>

class medDatabaseNavigatorItem;
class medDatabaseNavigatorViewPrivate;
class medDatabaseNavigatorTooltip;

class medDatabaseNavigatorView : public QGraphicsView
{
    Q_OBJECT

public:
     medDatabaseNavigatorView(QWidget *parent = 0);
    ~medDatabaseNavigatorView(void);

    void setScene(QGraphicsScene *scene);

signals:
    void hovered(medDatabaseNavigatorItem *item);

protected:
    void mouseMoveEvent(QMouseEvent *event);

private:
    medDatabaseNavigatorViewPrivate *d;
};

#endif // MEDDATABASENAVIGATORVIEW_H
