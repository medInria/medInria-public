/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractImageView.h>
#include <medVtkViewPluginExport.h>

class medVtkViewPrivate;
class medAbstractData;
class medAbstractImageData;
class medVtkViewObserver;
class medBoolParameterL;

class MEDVTKVIEWPLUGIN_EXPORT medVtkView : public medAbstractImageView
{
    Q_OBJECT

public:

    medVtkView(QObject* parent = 0);
    virtual ~medVtkView();

    virtual QString  identifier() const;
    static QString  s_identifier();
    static bool registered();
    virtual medViewBackend * backend() const;
    virtual QString description() const;
    virtual QWidget *viewWidget();
    virtual QPointF mapWorldToDisplayCoordinates(const QVector3D & worldVec );
    virtual QVector3D mapDisplayToWorldCoordinates(const QPointF & scenePoint );
    virtual QVector3D viewCenter();
    virtual QVector3D viewPlaneNormal();
    virtual QVector3D viewUp();
    virtual bool is2D();
    virtual qreal sliceThickness();
    virtual qreal scale();

    medBoolParameterL* rubberBandZoomParameter() const;

    virtual void setOffscreenRendering(bool isOffscreen);

    /**
     * @brief resetCameraOnLayer changes the camera focus onto the
     * bounding box's centroid of parameter layer's data.
     * @param layer index
     */
    virtual void resetCameraOnLayer(int layer);

public slots:
    virtual void reset();
    virtual void render();
    virtual void showHistogram(bool checked);

private slots:
    void displayDataInfo(uint layer);
    void changeCurrentLayer();

    void buildMouseInteractionParamPool(uint layer);
    void saveMouseInteractionSettings(bool parameterEnabled);
    void resetKeyboardInteractionModifier();

    virtual void enableRubberBandZoom(bool);

protected:
    virtual QImage buildThumbnail(const QSize &size);

private:
    medVtkViewPrivate *d;
    friend class medVtkViewObserver;
};

