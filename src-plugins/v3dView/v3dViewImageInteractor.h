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

class v3dViewImageInteractorPrivate;

class dtkAbstractData;
class dtkAbstractView;

class V3DVIEWPLUGIN_EXPORT v3dViewImageInteractor: public medAbstractVtkViewInteractor
{
    Q_OBJECT

public:
    v3dViewImageInteractor();
    virtual ~v3dViewImageInteractor();

    virtual QString description() const;
    virtual QString identifier() const;
    virtual QStringList handled() const;

    virtual bool isDataTypeHandled(QString dataType) const;

    static bool registered();

    virtual void setData(dtkAbstractData * data);
    virtual void removeData(medAbstractData *data);
    virtual void setView(dtkAbstractView * view);

    virtual void enable();
    virtual void disable();

public slots:
    virtual void setOpacity(dtkAbstractData * data, double opacity);
    virtual double opacity(dtkAbstractData * data) const;
    
    virtual void setVisible(dtkAbstractData * data, bool visible);
    virtual bool isVisible(dtkAbstractData * data) const;

    virtual void setLUT(dtkAbstractData * data, QString lut);
    void setLUT(int layer, const QString &value);
    virtual QString LUT(dtkAbstractData * data) const;

    virtual void setPreset(dtkAbstractData * data, QString preset);
    void setPreset (int layer, const QString &value );
    virtual QString preset(dtkAbstractData * data) const;

private:
    int getLayer(dtkAbstractData * data) const;

    template <typename IMAGE>
    bool SetViewInput(const char* type,dtkAbstractData* data,const int layer);

private:
    v3dViewImageInteractorPrivate * d;
};

dtkAbstractViewInteractor * createv3dViewImageInteractor();


