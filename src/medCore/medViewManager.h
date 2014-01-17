/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QtCore/QObject>

#include "medCoreExport.h"
#include <medDataIndex.h>

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

signals:
    //emitted when a new view is opened
    void viewOpened();

private:
    medViewManagerPrivate *d;
};


