/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractImageViewInteractor.h>

#include <itkDataImagePluginExport.h>

class v3dViewVectorFieldInteractorPrivate;

class ITKDATAIMAGEPLUGIN_EXPORT v3dViewVectorFieldInteractor : public medAbstractImageViewInteractor
{
    Q_OBJECT

public:
    v3dViewVectorFieldInteractor(medAbstractImageView* parent);
    ~v3dViewVectorFieldInteractor();

    virtual QString description() const;
    virtual QString identifier() const;
    virtual QStringList handled() const;

    static bool registered();

    virtual void setData(medAbstractData * data);

    virtual void moveToSliceAtPosition    (const QVector3D &position);
    virtual void moveToSlice  (int slice);

    virtual void windowLevel(double &window, double &level);

    double opacity() const;
    bool visibility() const;

    virtual QWidget* layerWidget();
    virtual QWidget* toolBoxWidget();
    virtual QWidget* toolBarWidget();
    virtual QList<medAbstractParameter*> parameters();

    QImage generateThumbnail(const QSize &size);
    void removeData();

public slots:

    void setScale(double scale);
    void setSampleRate(int sampleRate);
    void setColorMode(QString mode);
    void setProjection(bool enable);
    void setShowAxial(bool show);
    void setShowCoronal(bool show);
    void setShowSagittal(bool show);

    /** Change the position of the slices */
    void setPosition(const QVector3D& position);

    void setOpacity(double opacity);
    void setVisibility(bool visibility);
    void setWindowLevel(double &window, double &level);


protected:
    void setupParameters();


private:
    static QStringList dataHandled();

    void update();

private:
    v3dViewVectorFieldInteractorPrivate* d;
};
