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

#include <vtkPointSet.h>

#include "v3dViewPluginExport.h"


class v3dViewMeshInteractorPrivate;

class medAbstractData;
class dtkAbstractView;
class vtkLookupTable;

class V3DVIEWPLUGIN_EXPORT v3dViewMeshInteractor: public medAbstractVtkViewInteractor
{
    Q_OBJECT

public:
    v3dViewMeshInteractor();
    virtual ~v3dViewMeshInteractor();

    virtual QString description() const;
    virtual QString identifier() const;
    virtual QStringList handled() const;

    virtual bool isDataTypeHandled(QString dataType) const;

    static bool registered();

    virtual void setData(medAbstractData * data);
    virtual void setView(dtkAbstractView * view);

    virtual void enable();
    virtual void disable();

    //! Override dtkAbstractViewInteractor.
    virtual bool isAutoEnabledWith ( medAbstractData * data );

    void setOpacity(int meshLayer, double value);
    double opacity(int meshLayer) const;

    void setVisibility(int meshLayer, bool visible);
    bool visibility(int meshLayer) const;

    void setEdgeVisibility(int meshLayer, bool visible);
    bool edgeVisibility(int meshLayer) const;

    void setColor(int meshLayer, QColor color);
    QColor color(int meshLayer) const;

    void setRenderingType(int meshLayer, const QString & type);
    QString renderingType(int meshLayer) const;

    void setAttribute(int meshLayer, const QString & attribute);
    QString attribute(int meshLayer) const;

    QStringList getAllAttributes(int meshLayer) const;

    void setLut(int meshLayer, const QString & lutName);
    QString lut(int meshLayer) const;

    QStringList getAllLUTs() const;

public slots:
    // Mandatory implementations from medVtkViewInteractor
    virtual void setOpacity(medAbstractData * data, double opacity);
    virtual double opacity(medAbstractData * data) const;

    virtual void setVisible(medAbstractData * data, bool visible);
    virtual bool isVisible(medAbstractData * data) const;



    void setEdgeVisibility(dtkAbstractData * data, bool visible);
    bool edgeVisibility(dtkAbstractData * data) const;

    void setColor(dtkAbstractData * data, QColor color);
    QColor color(dtkAbstractData * data) const;

    void setRenderingType(dtkAbstractData * data, const QString & type);
    QString renderingType(dtkAbstractData * data) const;

    void setAttribute(dtkAbstractData * data, const QString & attribute);
    QString attribute(dtkAbstractData * data) const;

    QStringList getAllAttributes(dtkAbstractData * data) const;

    void setLut(dtkAbstractData * data, const QString & lutName);
    QString lut(dtkAbstractData * data) const;

protected:
    void updatePipeline (unsigned int meshLayer = 0);
    void changeBounds ( vtkPointSet* pointSet);
    void setLut(int meshLayer, vtkLookupTable * lut);
    int getLayer(dtkAbstractData * data) const;

private:
    v3dViewMeshInteractorPrivate * d;
};

dtkAbstractViewInteractor * createV3dViewMeshInteractor();


