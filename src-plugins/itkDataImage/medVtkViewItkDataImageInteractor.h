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
    medVtkViewItkDataImageInteractor(medAbstractView *parent);
    virtual ~medVtkViewItkDataImageInteractor();

    virtual QString description() const;
    virtual QString identifier() const;
    static bool registered();

    virtual QWidget* buildToolBoxWidget();
    virtual QWidget* buildToolBarWidget();
    virtual QWidget* buildLayerWidget();
    virtual QList<medAbstractParameter*> parameters();
    virtual void setData(medAbstractData * data);
    virtual QString lut() const;
    virtual QString preset() const;
    virtual void setWindowLevel (double &window, double &level);
    virtual void windowLevel(double &window, double &level);
    virtual bool visibility() const;
    virtual double opacity() const;
    virtual QStringList handled() const;

public slots:
    virtual void setOpacity (int opacity);
    virtual void setOpacity (double opacity);
    void setPreset(QString value);
    virtual void setLut(QString lut);
    void moveToSlice(int slice);
    virtual void setVisibility(bool visibility);
    void setWindow(double window);
    void setLevel(double level);
    void setWindowLevel(QList<QVariant>);
    virtual void removeData();

    virtual void updateWidgets();

    virtual void setUpViewForThumbnail();

protected:
    void initParameters(medAbstractImageData* data);

private:
    static QStringList dataHandled();
    virtual void update();

    template <typename IMAGE>
    bool SetViewInput(const char* type, medAbstractData* data, int layer);

private slots:
    void updateWindowLevelParam(double window, double level, unsigned int layer);
    void updateImageViewInternalLayer();

private:
    medVtkViewItkDataImageInteractorPrivate * d;
};

