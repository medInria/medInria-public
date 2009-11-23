#ifndef MEDDATABASEPREVIEWSCENE_H
#define MEDDATABASEPREVIEWSCENE_H

#include <QtGui>

class medDatabasePreviewItemGroup;
class medDatabasePreviewScenePrivate;

class medDatabasePreviewScene : public QGraphicsScene
{
public:
     medDatabasePreviewScene(QObject *parent = 0);
    ~medDatabasePreviewScene(void);

    void addGroup(medDatabasePreviewItemGroup *group);

    void reset(void);

private:
    medDatabasePreviewScenePrivate *d;
};

#endif // MEDDATABASEPREVIEWSCENE_H
