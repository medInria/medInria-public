#ifndef MEDDATABASEPREVIEWITEM_H
#define MEDDATABASEPREVIEWITEM_H

#include <QtGui/QGraphicsPixmapItem>

class medDatabasePreviewItemPrivate;

class medDatabasePreviewItem : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
     medDatabasePreviewItem(const QString& path, QGraphicsItem *parent = 0);
    ~medDatabasePreviewItem(void);

    medDatabasePreviewItem *clone(void);

    QString path(void) const;

protected slots:
    void setImage(const QImage& image);

private:
    medDatabasePreviewItemPrivate *d;
};

#endif // MEDDATABASEPREVIEWITEM_H
