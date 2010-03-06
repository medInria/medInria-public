/* medViewManager.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Dec 21 12:47:46 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Sat Mar  6 11:49:01 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 12
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDVIEWMANAGER_H
#define MEDVIEWMANAGER_H

#include <QtCore/QObject>

#include "medCoreExport.h"
#include "medDataIndex.h"

class dtkAbstractView;
class dtkAbstractViewFactory;

class medViewManagerPrivate;

class MEDCORE_EXPORT medViewManager : public QObject
{
    Q_OBJECT

public:
    static medViewManager *instance(void);

    void insert(const medDataIndex& index, dtkAbstractView *view);

    QList<dtkAbstractView *> views(const medDataIndex& index);
    QList<dtkAbstractView *> viewsForPatient(int id);
    QList<dtkAbstractView *> viewsForStudy  (int id);
    QList<dtkAbstractView *> viewsForSeries (int id);
    QList<dtkAbstractView *> viewsForImage  (int id);

    medDataIndex index(dtkAbstractView *view);

protected:
     medViewManager(void);
    ~medViewManager(void);

protected:
    static medViewManager *s_instance;

private:
    medViewManagerPrivate *d;
};

#endif // MEDVIEWMANAGER_H
