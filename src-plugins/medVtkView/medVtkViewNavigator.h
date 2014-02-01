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


    virtual medImageView::Orientation orientaion() const ;
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

protected:
    void bounds(float& xmin, float& xmax, float& ymin, float& ymax, float& zmin, float& zmax);
    void cameraUp(double *coordinates);
    void cameraPosition(double *coordinates);
    void cameraFocalPoint(double *coordinates);
    void setCameraPosition(double x, double y, double z);
    void setCameraClippingRange(double nearRange, double farRange);

protected slots:
    void setAxial(bool axial);
    void setCoronal(bool coronal);
    void setSagittal(bool sagittal);
    void set3d(bool o3d);

    QString cameraProjectionMode();
    double cameraViewAngle();
    double cameraZoom();

private:
    void _prvt_setOrientation(medImageView::Orientation orientation);

private:
    medVtkViewNavigatorPrivate *d;
};
