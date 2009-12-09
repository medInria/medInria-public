#ifndef MEDDATABASENAVIGATOR_H
#define MEDDATABASENAVIGATOR_H

#include <QtCore>
#include <QtGui>

class medDatabaseNavigatorItem;
class medDatabaseNavigatorPrivate;

class medDatabaseNavigator : public QFrame
{
    Q_OBJECT

public:
     medDatabaseNavigator(QWidget *parent = 0);
    ~medDatabaseNavigator(void);

    void reset(void);

signals:
    void patientClicked(int id);
    void   studyClicked(int id);
    void  seriesClicked(int id);
    void   imageClicked(int id);

public slots:
    void onPatientClicked(int id);
    void   onStudyClicked(int id);
    void  onSeriesClicked(int id);
    void   onImageClicked(int id);

protected slots:
    void onHovered(medDatabaseNavigatorItem *item);

private:
    medDatabaseNavigatorPrivate *d;
};

#endif // MEDDATABASENAVIGATOR_H
