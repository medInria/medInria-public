/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QtCore>

#include "medGuiExport.h"

class medViewPoolPrivate;
class medAbstractView;

/**
 * @class medViewPool
 * @brief Stores a collection of views and ensures proper synchronization among them
 *
 * The role of the medViewPool is to ensure the correct synchronization among several
 * @medAbstractView. It is responsible for transmitting a change of property from
 * a view to the others, and to transmit view signals such as positionChanged() from
 * one view to the others.
 * A user only needs to call appendView() and removeView() and add/remove views to
 * the pool. The pool then manages the synchronization.
 **/
class MEDGUI_EXPORT medViewPool : public QObject
{
    Q_OBJECT

public:
             medViewPool (QObject *parent = 0);
    virtual ~medViewPool();    

    /**
       Returns the number of views in the pool.
     **/
    int count();

signals:
    void showInfo(const QString&,unsigned int timeout);
    void showError(const QString&,unsigned int timeout);

    void viewAppended (medAbstractView *view);
    void viewRemoved  (medAbstractView *view);
    
public slots:
    /**
       Append a view to the pool
     **/
    void appendView (medAbstractView *view);

    /**
       Remove a view from the pool
     **/
    void removeView (medAbstractView *view);

public slots:
    void onViewPropertySet (const QString &key, const QString &value);

    void onViewPositionChanged    (const QVector3D &position, bool propagate);
    void onViewCameraChanged      (const QVector3D &position, const QVector3D &viewup, const QVector3D &focal, double parallelScale, bool propagate);
    void onViewZoomChanged        (double zoom, bool propagate);
    void onViewPanChanged         (const QVector2D &pan, bool propagate);
    void onViewWindowingChanged   (double level, double window, bool propagate);

private:
    medViewPoolPrivate *d;
  
};


