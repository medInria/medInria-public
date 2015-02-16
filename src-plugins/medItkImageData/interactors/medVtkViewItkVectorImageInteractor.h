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

#include <medItkImageExport.h>

class medVtkViewItkVectorImageInteractorPrivate;

class MEDITKIMAGEPLUGIN_EXPORT medVtkViewItkVectorImageInteractor : public medAbstractImageViewInteractor
{
    Q_OBJECT

public:
    medVtkViewItkVectorImageInteractor(medAbstractView* parent);
    ~medVtkViewItkVectorImageInteractor();

    virtual QString description() const;
    virtual QString identifier() const;
    virtual QStringList handled() const;

    static bool registered();

    virtual void setInputData(medAbstractData * data);

    virtual QWidget* buildLayerWidget();
    virtual QWidget* buildToolBoxWidget();
    virtual QWidget* buildToolBarWidget();

    virtual QList<medAbstractParameter*> linkableParameters();
    virtual QList<medBoolParameter*> mouseInteractionParameters();

    virtual void removeData();

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

    virtual void updateWidgets();


protected:
    void setupParameters();

private:
    static QStringList dataHandled();
    void update();

private slots:
    void updateSlicingParam();
    void updatePlaneVisibility();


private:
    medVtkViewItkVectorImageInteractorPrivate* d;
};
