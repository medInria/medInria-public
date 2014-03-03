/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractVtkViewInteractor.h>

#include "v3dViewPluginExport.h"

class v3dViewVectorFieldInteractorPrivate;

class V3DVIEWPLUGIN_EXPORT v3dViewVectorFieldInteractor : public medAbstractVtkViewInteractor
{
    Q_OBJECT

public:
    v3dViewVectorFieldInteractor();
    ~v3dViewVectorFieldInteractor();

    virtual QString description() const;
    virtual QString identifier() const;
    virtual QStringList handled() const;

    virtual bool isDataTypeHandled(QString dataType) const;

    static bool registered();

    virtual void setData(dtkAbstractData *data);
    dtkAbstractData *data();

    virtual void setView(dtkAbstractView *view);
    dtkAbstractView *view();

    virtual void enable();
    virtual void disable();


    template <class ImageType>
    void test(ImageType *dataset);

    double getScale();
    int* getSampleRate();
    int getColorMode();
    bool getProjection();
    bool getShowAxial();
    bool getShowCoronal();
    bool getShowSagittal();

public slots:
    // Mandatory implementations from medVtkViewInteractor
    virtual void setOpacity(dtkAbstractData * data, double opacity);
    virtual double opacity(dtkAbstractData * data) const;

    virtual void setVisible(dtkAbstractData * data, bool visible);
    virtual bool isVisible(dtkAbstractData * data) const;

    void setScale(double scale);
    void setSampleRate(int sampleRate);
    void setColorMode(int mode);
    void setProjection(bool enable);
    void setShowAxial(bool show);
    void setShowCoronal(bool show);
    void setShowSagittal(bool show);

    /** Change position of the slices */
    void changePosition(const QVector3D& position, bool propagate);

private:
    v3dViewVectorFieldInteractorPrivate* d;
};

dtkAbstractViewInteractor *createV3dViewVectorFieldInteractor();
