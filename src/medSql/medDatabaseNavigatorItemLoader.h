#ifndef MEDDATABASENAVIGATORITEMLOADER_H
#define MEDDATABASENAVIGATORITEMLOADER_H

#include <QtCore>
#include <QtGui>

class medDatabaseNavigatorItemLoaderPrivate;

class medDatabaseNavigatorItemLoader : public QObject, public QRunnable
{
    Q_OBJECT

public:
     medDatabaseNavigatorItemLoader(const QString& path);
    ~medDatabaseNavigatorItemLoader(void);

    void run(void);

signals:
    void completed(const QImage& image);

private:
    medDatabaseNavigatorItemLoaderPrivate *d;
};

#endif // MEDDATABASENAVIGATORITEMLOADER_H
