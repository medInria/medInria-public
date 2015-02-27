/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractImageViewInteractor.h>

#include <itkDataImagePluginExport.h>

class medVtkViewItkDataImageInteractorPrivate;

class medAbstractImageData;

class ITKDATAIMAGEPLUGIN_EXPORT medVtkViewItkDataImageInteractor: public medAbstractImageViewInteractor
{
    Q_OBJECT

public:
    medVtkViewItkDataImageInteractor(medAbstractView *parent);
    virtual ~medVtkViewItkDataImageInteractor();

    virtual QString description() const;
    virtual QString name() const;
    virtual QString identifier() const;
    static bool registered();

    virtual QWidget* buildToolBoxWidget();
    virtual QWidget* buildToolBarWidget();
    virtual QWidget* buildLayerWidget();
    virtual QList<medAbstractParameter*> linkableParameters();
    virtual QList<medBoolParameter*> mouseInteractionParameters();
    virtual void setInputData(medAbstractData * data);
    virtual QString lut() const;
    virtual QString preset() const;
    virtual QStringList handled() const;
    
    virtual void restoreParameters(QHash<QString,QString> parameters);

public slots:
    virtual void setOpacity (double opacity);
    void moveToSlice(int slice);
    void setPreset(QString value);
    virtual void setLut(QString lut);
    void setWindowLevel(QHash<QString, QVariant>);
    void enableWIndowLevel(bool enable);

    void setVisibility(bool);

    virtual void removeData();

    virtual void updateWidgets();

    virtual void setUpViewForThumbnail();

protected:
    void initParameters(medAbstractImageData* data);
    void initWindowLevelParameters(double* range);

private:
    static QStringList dataHandled();
    virtual void update();

    template <typename IMAGE>
    bool SetViewInput(const char* type, medAbstractData* data, int layer);

private slots:
    void updateSlicingParam();
    void updateImageViewInternalLayer();
    void setWindowLevelFromMinMax();

private:
    medVtkViewItkDataImageInteractorPrivate * d;

};

