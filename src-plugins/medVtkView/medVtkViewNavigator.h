/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
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
             medVtkViewNavigator(medAbstractView* parent);
    virtual ~medVtkViewNavigator();

    virtual QString  identifier() const;
    static QString  s_identifier();
    static bool registered();
    virtual QStringList handled(void) const;
    virtual QString description() const;
    virtual QList<medAbstractParameter*> linkableParameters();
    virtual QList<medBoolParameter*> mouseInteractionParameters();


    virtual medImageView::Orientation orientation() const ;

public slots:
    void updateWidgets();

    virtual void setOrientation(medImageView::Orientation orientation);

    virtual void setZoom (double zoom);
    virtual void setPan (const QVector2D &pan);

    virtual void moveToPosition(const QVector3D &position);

    void setCamera(const QVector3D &position,
                           const QVector3D &viewup,
                           const QVector3D &focal,
                           double parallelScale);

signals:
    void orientationChanged();

protected:
    virtual QWidget* buildToolBoxWidget();
    virtual QWidget* buildToolBarWidget();
    
    virtual void restoreParameters(QHash<QString,QString> parameters);


    void cameraUp(double *coordinates) const;
    void cameraPosition(double *coordinates) const;
    void cameraFocalPoint(double *coordinates) const;
    /**
     * @brief cameraParallelScale return -1 if orientaion is not 3D.
     * @param parallelScale
     */
    void cameraParallelScale(double &parallelScale) const;

protected slots:
    void setCamera(QHash<QString, QVariant>);

    void setCameraUp(const QVector3D& viewup);
    void setCameraPosition(const QVector3D& position);
    void setCameraFocalPoint(const QVector3D& focal);
    void setCameraParallelScale(double parallelScale);

    void setAxial(bool axial);
    void setCoronal(bool coronal);
    void setSagittal(bool sagittal);
    void set3d(bool o3d);

    void showAxes(bool);
    void showRuler(bool);
    void showAnnotations(bool);
    void showScalarBar(bool);

    void enableZooming(bool);
    void enableSlicing(bool);
    void enablePanning(bool);
    void enableMeasuring(bool);

private slots:
    void changeOrientation(medImageView::Orientation orientation);

private:
    medVtkViewNavigatorPrivate *d;
};
