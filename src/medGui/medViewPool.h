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

class MEDGUI_EXPORT medViewPool : public QObject
{
    Q_OBJECT

public:
    medViewPool (void);
    ~medViewPool (void);

    void appendView (medAbstractView *view);
    void removeView (medAbstractView *view);

    medAbstractView *daddy (void);

    // void setViewProperty (const QString &key, const QString &value);

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
