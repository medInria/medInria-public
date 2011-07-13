/* medDatabaseNavigator.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:38:25 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Thu May 13 20:07:04 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 4
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

#include "medGuiExport.h"

class medDatabaseNavigatorItem;
class medDatabaseNavigatorPrivate;
class medDataIndex;

class MEDGUI_EXPORT medDatabaseNavigator : public QFrame
{
    Q_OBJECT

public:
     medDatabaseNavigator(QWidget *parent = 0);
    ~medDatabaseNavigator(void);

    void reset(void);

    Qt::Orientation orientation (void) const;
    
signals:
    void itemClicked(const medDataIndex& id);

public slots:
    void onItemClicked(const medDataIndex& id);

    void   setOrientation(Qt::Orientation orientation);

protected:
    void onPatientClicked(const medDataIndex& id);
    void   onStudyClicked(const medDataIndex& id);
    void  onSeriesClicked(const medDataIndex& id);
    void   onImageClicked(const medDataIndex& id);

private:
    medDatabaseNavigatorPrivate *d;
};

#endif // MEDDATABASENAVIGATOR_H
