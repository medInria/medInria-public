/* medDatabaseNavigator.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:38:25 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Dec 15 09:38:29 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 1
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

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
