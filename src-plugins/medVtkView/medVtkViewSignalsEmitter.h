/*=========================================================================

medInria

Copyright (c) INRIA 2013. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#pragma once

#include <QObject>
#include <QVector2D>
#include <medVtkViewPluginExport.h>

class medVtkView;

class MEDVTKVIEWPLUGIN_EXPORT medVtkViewSignalsEmitter : QObject
{
    Q_OBJECT

public:
    medVtkViewSignalsEmitter();

    void setView (medVtkView *view);

    void emitSliceChanged(int);
    void emitWindowLevelChanged(double window, double level, unsigned int layer);
    void emitZoomChanged(double);
    void emitPanChanged(const QVector2D&);
    void emitPositionViewedChanged(const QVector3D&);
    void emitCameraChanged(const QVector3D &position,
                       const QVector3D &viewup,
                       const QVector3D &focal,
                       double parallelScale);

signals:
    void orientationChanged();
    void sliceChanged(int);
    void windowLevelChanged(double window, double level, unsigned int layer);
    void zoomChanged(double);
    void panChanged(const QVector2D&);
    void positionViewedChanged(const QVector3D&);
    void cameraChanged(const QVector3D &position,
                       const QVector3D &viewup,
                       const QVector3D &focal,
                       double parallelScale);


private:
    medVtkView *m_view;


};


