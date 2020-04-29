#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractImageViewInteractor.h>

#include <itkDataImagePluginExport.h>

class medVtkViewItkVectorFieldInteractorPrivate;

class ITKDATAIMAGEPLUGIN_EXPORT medVtkViewItkVectorFieldInteractor : public medAbstractImageViewInteractor
{
    Q_OBJECT

public:
    medVtkViewItkVectorFieldInteractor(medAbstractView* parent);
    ~medVtkViewItkVectorFieldInteractor();

    QString description() const override;
    QString identifier() const override;
    QStringList handled() const override;

    static bool registered();

    void setInputData(medAbstractData * data) override;

    QWidget* buildLayerWidget() override;
    QWidget* buildToolBoxWidget() override;
    QWidget* buildToolBarWidget() override;

    QList<medAbstractParameterL*> linkableParameters() override;
    QList<medBoolParameterL*> mouseInteractionParameters() override;

    void removeData() override;

public slots:

    void setScale(double scale);
    void setSampleRate(int sampleRate);
    void setColorMode(QString mode);
    void setProjection(bool enable);
    void setShowAxial(bool show);
    void setShowCoronal(bool show);
    void setShowSagittal(bool show);
    virtual void moveToSlice  (int slice);

    /** Change the position of the slices */
    void setPosition(const QVector3D& position);

    void setOpacity(double opacity);
    void setVisibility(bool visibility);
    void setWindowLevel(QHash<QString, QVariant>);

    virtual void setUpViewForThumbnail();

    void updateWidgets() override;

protected:
    void setupParameters();

private:
    static QStringList dataHandled();
    void update();

private slots:
    void updateSlicingParam();
    void updatePlaneVisibility();

private:
    medVtkViewItkVectorFieldInteractorPrivate* d;
};
