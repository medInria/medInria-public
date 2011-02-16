/* medViewPool.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Fri Feb 19 17:39:32 2010 (+0100)
 * Version: $Id$
 * Last-Updated: Mon Feb 22 21:14:38 2010 (+0100)
 *           By: Julien Wintz
 *     Update #: 9
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDVIEWPOOL_H
#define MEDVIEWPOOL_H

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
 *
 * Another role of the pool is to perform automatic registration of a view's data
 * onto a reference view, called the daddy. Only one daddy per pool is possible. When
 * the reg() signal is emitted from a view, the pool performsa registration from the
 * sender's data to the daddy's data and change the input of the sender with the result.
 * Original data are restored when the reg() signal is emitted a second time.
 * To be functional, a registration plugin must be loaded and compatible with the view's
 * data types.
 **/
class MEDGUI_EXPORT medViewPool : public QObject
{
    Q_OBJECT

public:
    medViewPool (void);
    ~medViewPool (void);

    /**
       Append a view to the pool
     **/
    void appendView (medAbstractView *view);

    /**
       Remove a view from the pool
     **/
    void removeView (medAbstractView *view);

    /**
       The daddy is the reference view, i.e., the target when performing automatic
       registration.
    **/
    medAbstractView *daddy (void);

    /**
       Returns the number of views in the pool.
     **/
    int count (void);

signals:
    void showInfo(QObject*,const QString&,unsigned int timeout);
    void showError(QObject*,const QString&,unsigned int timeout);
    
public slots:
    void onViewPropertySet (const QString &key, const QString &value);
    void onViewDaddy         (bool);
    void onViewReg           (bool);

    void onViewPositionChanged    (const QVector3D &position);
    void onViewCameraChanged      (const QVector3D &position, const QVector3D &viewup, const QVector3D &focal, double parallelScale);
    void onViewZoomChanged        (double zoom);
    void onViewPanChanged         (const QVector2D &pan);
    void onViewWindowingChanged   (double level, double window);

private:
    medViewPoolPrivate *d;
  
};

#endif
