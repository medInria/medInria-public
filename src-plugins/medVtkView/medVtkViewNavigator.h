/*=========================================================================

medInria

Copyright (c) INRIA 2013. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractImageViewNavigator.h>
#include <medImageViewEnum.h>

#include <medVtkViewPluginExport.h>

class medAbstractImageView;

class medVtkViewNavigatorPrivate;
class MEDVTKVIEWPLUGIN_EXPORT medVtkViewNavigator : public medAbstractImageViewNavigator
{
    Q_OBJECT

public:
             medVtkViewNavigator(medAbstractImageView* parent);
    virtual ~medVtkViewNavigator();

    virtual QString  identifier() const;
    static QString  s_identifier();

    static bool registered();
    virtual QStringList handled(void) const;


    virtual QString description() const;

    virtual QWidget* widgetForToolBox() const;
    virtual QWidget* widgetForToolBar() const;

    virtual QList<medAbstractParameter*> parameters();


    virtual medImageView::Orientation orientation() const ;
    virtual void camera(QVector3D &position,
                        QVector3D &viewup,
                        QVector3D &focal,
                        double &parallelScale) const;
    virtual QVector3D positionBeingViewed() const;


    virtual void setOrientation(medImageView::Orientation orientation);
    virtual void setCamera(const QVector3D &position,
                           const QVector3D &viewup,
                           const QVector3D &focal,
                           double parallelScale);

    virtual double zoom() const;
    virtual QVector2D pan() const;


public slots:
     virtual void setZoom (double zoom);
     virtual void setPan (const QVector2D &pan);

signals:
    void orientationChanged();

protected:
    void bounds(float& xmin, float& xmax, float& ymin, float& ymax, float& zmin, float& zmax) const;
    void cameraUp(double *coordinates) const;
    void cameraPosition(double *coordinates) const;
    void cameraFocalPoint(double *coordinates) const;
    void cameraParallelScale(double &parallelScale) const;
    void setCameraPosition(double x, double y, double z);
    void setCameraClippingRange(double nearRange, double farRange);
    QString cameraProjectionMode();
    double cameraViewAngle();
    double cameraZoom();

protected slots:
    void setAxial(bool axial);
    void setCoronal(bool coronal);
    void setSagittal(bool sagittal);
    void set3d(bool o3d);
    void setCamera(QList<QVariant>);
    void setCameraPosition(const QVector3D& position);
    void setCameraUp(const QVector3D& viewup);
    void setCameraFocalPoint(const QVector3D& focal);
    void setCameraParallelScale(double parallelScale);
    void updateCameraParam(const QVector3D& postion,const QVector3D& viewUp,const QVector3D& focal,double parallelScale);
    void showAxes(bool);
    void showRuler(bool);
    void showAnnotations(bool);
    void showScalarBar(bool);

private:
    void changeOrientation(medImageView::Orientation orientation);

private:
    medVtkViewNavigatorPrivate *d;
};
