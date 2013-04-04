/* medViewManager.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Mon Dec 21 12:47:46 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Wed Mar 17 18:39:17 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 17
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#pragma once

#include <QtCore/QObject>

#include "medCoreExport.h"
#include "medDataIndex.h"

class medAbstractView;
class medAbstractViewFactory;

class medViewManagerPrivate;

class MEDCORE_EXPORT medViewManager : public QObject
{
    Q_OBJECT

public:
    static medViewManager *instance();

    void insert(const medDataIndex& index, medAbstractView *view);
    void remove(const medDataIndex& index, medAbstractView *view);
    void remove(const medDataIndex& index);

    QList<medAbstractView *> views(const medDataIndex& index);
    QList<medAbstractView *> viewsForPatient(int id);
    QList<medAbstractView *> viewsForStudy  (int id);
    QList<medAbstractView *> viewsForSeries (int id);
    QList<medAbstractView *> viewsForImage  (int id);

    QList<medDataIndex> indices(medAbstractView *view) const; 

protected:
     medViewManager();
    ~medViewManager();

protected:
    static medViewManager *s_instance;

private:
    medViewManagerPrivate *d;
};


