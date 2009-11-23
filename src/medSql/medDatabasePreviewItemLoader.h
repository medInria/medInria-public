#ifndef MEDDATABASEPREVIEWITEMLOADER_H
#define MEDDATABASEPREVIEWITEMLOADER_H

#include <QtCore>
#include <QtGui>

class medDatabasePreviewItemLoaderPrivate;

class medDatabasePreviewItemLoader : public QObject, public QRunnable
{
    Q_OBJECT

public:
     medDatabasePreviewItemLoader(const QString& path);
    ~medDatabasePreviewItemLoader(void);

    void run(void);

signals:
    void completed(const QImage& image);

private:
    medDatabasePreviewItemLoaderPrivate *d;
};

#endif // MEDDATABASEPREVIEWITEMLOADER_H
