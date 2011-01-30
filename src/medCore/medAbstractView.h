/* medAbstractView.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Thu Oct 21 19:54:36 2010 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Oct 26 10:46:04 2010 (+0200)
 *           By: Julien Wintz
 *     Update #: 9
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifndef MEDABSTRACTVIEW_H
#define MEDABSTRACTVIEW_H

#include <dtkCore/dtkAbstractView.h>

#include "medCoreExport.h"

class medAbstractViewPrivate;

class MEDCORE_EXPORT medAbstractView: public dtkAbstractView
{
    Q_OBJECT

public:
    medAbstractView(medAbstractView * parent = 0);
    medAbstractView(const medAbstractView& view);
    virtual ~medAbstractView(void){}

    virtual void setColorLookupTable(int min_range,
                                     int max_range,
                                     int size,
                                     const int & table);

    virtual void setColorLookupTable( QList<double> scalars,
				      QList<QColor> colors );
    virtual void setTransferFunctions( QList<double> scalars,
				       QList<QColor> colors );
    virtual void getTransferFunctions( QList<double> & scalars,
				       QList<QColor> & colors );

    virtual QWidget *receiverWidget(void);

signals:
    void closing       (void);
    void becomeDaddy   (bool);
    void reg           (bool);
    void lutChanged    (void);
    void fullScreen    (bool);

    // 2D
    void positionChanged  (const QVector3D &position);
    void zoomChanged      (double zoom);
    void panChanged       (const QVector2D &pan);
    void windowingChanged (double level, double window);

    // 3D
    void cameraChanged    (const QVector3D &position, const QVector3D &viewup, const QVector3D &focal, double parallelScale);

public slots:
    virtual void setLinkPosition (bool value);
    bool positionLinked (void) const;

    virtual void setLinkWindowing (bool value);
    bool windowingLinked (void) const;

    virtual void setLinkCamera (bool value);
    bool cameraLinked (void) const;
    
    virtual void setPosition    (const QVector3D &position);
    virtual void setZoom        (double zoom);
    virtual void setPan         (const QVector2D &pan);
    virtual void setWindowLevel (double level, double window);
    
    virtual void setCamera   (const QVector3D &position, const QVector3D &viewup, const QVector3D &focal, double parallelScale);
    
protected:
    void emitViewPositionChangedEvent(const QVector3D &position);
    void emitViewZoomChangedEvent(double zoom);
    void emitViewPanChangedEvent(const QVector2D &pan);
    void emitViewWindowingChangedEvent(double level, double window);
    void emitViewCameraChangedEvent(const QVector3D &position, const QVector3D &viewup, const QVector3D &focal, double parallelScale);

private:
    medAbstractViewPrivate *d;
};

#endif
