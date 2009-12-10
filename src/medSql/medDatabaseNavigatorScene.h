#ifndef MEDDATABASENAVIGATORSCENE_H
#define MEDDATABASENAVIGATORSCENE_H

#include <QtGui>

class medDatabaseNavigatorItemGroup;
class medDatabaseNavigatorScenePrivate;

class medDatabaseNavigatorScene : public QGraphicsScene
{
public:
     medDatabaseNavigatorScene(QObject *parent = 0);
    ~medDatabaseNavigatorScene(void);

    void addGroup(medDatabaseNavigatorItemGroup *group);

    void reset(void);

private:
    medDatabaseNavigatorScenePrivate *d;
};

#endif // MEDDATABASENAVIGATORSCENE_H
