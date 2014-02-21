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

class medVtkViewItkDataImageInteractorPrivate;

//class dtkAbstractData;
//class dtkAbstractView;
class medAbstractImageData;

class ITKDATAIMAGEPLUGIN_EXPORT medVtkViewItkDataImageInteractor: public medAbstractImageViewInteractor
{
    Q_OBJECT

public:
    medVtkViewItkDataImageInteractor(medAbstractImageView *parent);
    virtual ~medVtkViewItkDataImageInteractor();

    virtual QString description() const;
    virtual QString identifier() const;
    static bool registered();

    virtual QWidget* toolBarWidget();
    virtual QWidget* toolBoxWidget();
    virtual QWidget* layerWidget();
    virtual QList<medAbstractParameter*> parameters();
    virtual void setData(medAbstractData * data);
    virtual QString lut() const;
    virtual QString preset() const;
    virtual void setWindowLevel (double &window, double &level);
    virtual void windowLevel(double &window, double &level);
    virtual bool visibility() const;
    virtual double opacity() const;
    virtual QStringList handled() const;

    virtual QImage& generateThumbnail(const QSize &size);

public slots:
    virtual void setOpacity (int opacity);
    virtual void setOpacity (double opacity);
    void setPreset(QString value);
    virtual void setLut(QString lut);
    void moveToSlice(int slice);
    virtual void moveToSliceAtPosition(const QVector3D &position);
    virtual void setVisibility(bool visibility);
    void setWindow(double window);
    void setLevel(double level);
    virtual void removeData();

    virtual void updateWidgets();

protected:
    void initParameters(medAbstractImageData* data);
private:
    static QStringList dataHandled();
    virtual void update();

    template <typename IMAGE>
    bool SetViewInput(const char* type, medAbstractData* data, const int layer);

private slots:
    void updateWindowLevelParam(double window, double level);

private:
    medVtkViewItkDataImageInteractorPrivate * d;
};

