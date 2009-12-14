#ifndef MEDDATABASEPREVIEWITEM_H
#define MEDDATABASEPREVIEWITEM_H

#include <QtGui/QGraphicsPixmapItem>

class medDatabasePreviewItemPrivate;

class medDatabasePreviewItem : public QObject, public QGraphicsPixmapItem
{
    Q_OBJECT

public:
     medDatabasePreviewItem(int patientId = -1, int studyId = -1, int seriesId = -1, int imageId = -1, const QString& path = QString(), QGraphicsItem *parent = 0);
    ~medDatabasePreviewItem(void);

    medDatabasePreviewItem *clone(void);

    int patientId(void) const;
    int   studyId(void) const;
    int  seriesId(void) const;
    int   imageId(void) const;

    QString path(void) const;

signals:
    void patientClicked(int id);
    void   studyClicked(int id);
    void  seriesClicked(int id);
    void   imageClicked(int id);

protected slots:
    void setImage(const QImage& image);

private:
    medDatabasePreviewItemPrivate *d;
};

#endif // MEDDATABASEPREVIEWITEM_H
