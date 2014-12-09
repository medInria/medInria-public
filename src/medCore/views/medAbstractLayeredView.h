/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractView.h>

#include <medCoreExport.h>

#include <dtkCore/dtkSmartPointer.h>

#include <medAbstractLayeredViewInteractor.h>
#include <medAbstractLayeredViewNavigator.h>
#include <medAbstractData.h>
#include <medAbstractInteractor.h>

class medAbstractData;
class medAbstractBoolParameter;
class medStringListParameter;
class medDataIndexListParameter;

class medAbstractLayeredViewPrivate;
class MEDCORE_EXPORT medAbstractLayeredView : public medAbstractView
{
    Q_OBJECT

public:
    medAbstractLayeredView(QObject * parent = 0);
    virtual ~medAbstractLayeredView();

    virtual void addLayer(medAbstractData *data);
    virtual void removeData(medAbstractData *data);
    void removeLayer(unsigned int layer);
    void insertLayer(unsigned int layer, medAbstractData *data);

    medAbstractData * layerData(unsigned int layer) const;
    QList<medDataIndex> dataList() const;

    bool contains(medAbstractData * data) const;
    bool contains(QString identifier) const;

    unsigned int layersCount() const;
    unsigned int layer(medAbstractData * data);

    void setCurrentLayer(unsigned int layer);
    unsigned int currentLayer() const;

    QList <medAbstractInteractor*> interactors();
    QList <medAbstractInteractor*> layerInteractors(unsigned int layer);
    QList<medAbstractNavigator*> navigators();

    medAbstractBoolParameter* visibilityParameter(unsigned int layer);
    medDataIndexListParameter *dataListParameter() const;

    virtual QList<medAbstractParameter*> linkableParameters();
    virtual QList<medAbstractParameter*> linkableParameters(unsigned int layer);

public slots:
    void setDataList(QList<medDataIndex> dataList);
    void removeLayer();

signals:
    void layerAdded(unsigned int layer);
    void layerRemoved(unsigned int layer);
    void layerRemoved(medAbstractData*);
    void currentLayerChanged() const;

protected:
    virtual medAbstractLayeredViewInteractor * primaryInteractor(medAbstractData* data);
    virtual QList<medAbstractInteractor *> extraInteractors(medAbstractData* data);
    virtual medAbstractLayeredViewInteractor * primaryInteractor(unsigned int layer);
    virtual QList<medAbstractInteractor *> extraInteractors(unsigned int layer);
    virtual medAbstractLayeredViewInteractor * primaryInteractor();
    virtual QList<medAbstractInteractor *> extraInteractors();

    virtual medAbstractLayeredViewNavigator * primaryNavigator();
    virtual QList<medAbstractNavigator *> extraNavigators();

    virtual bool initialiseInteractors(medAbstractData* data);
    virtual bool initialiseNavigators();
    virtual void removeInteractors(medAbstractData *data);

    virtual QList<medAbstractParameter*> interactorsParameters(unsigned int layer);  

private slots:
    void updateDataListParameter(unsigned int layer);

private:
    medAbstractLayeredViewPrivate *d;
};
