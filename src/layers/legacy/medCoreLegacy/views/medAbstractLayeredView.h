/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractView.h>

#include <medCoreLegacyExport.h>

#include <dtkCoreSupport/dtkSmartPointer.h>

#include <medAbstractLayeredViewInteractor.h>
#include <medAbstractLayeredViewNavigator.h>
#include <medAbstractData.h>
#include <medAbstractInteractor.h>

class medAbstractData;
class medAbstractBoolParameterL;
class medStringListParameterL;
class medDataListParameterL;

class medAbstractLayeredViewPrivate;
class MEDCORELEGACY_EXPORT medAbstractLayeredView : public medAbstractView
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

    medAbstractBoolParameterL* visibilityParameter(unsigned int layer);
    medDataListParameterL *dataListParameter() const;

    virtual QList<medAbstractParameterL*> linkableParameters();
    virtual QList<medAbstractParameterL*> linkableParameters(unsigned int layer);

    virtual void resetCameraOnLayer(int layer);

    template <class Interactor>
    Interactor* getLayerInteractorOfType(int layerIndex);

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

    virtual QList<medAbstractParameterL*> interactorsParameters(unsigned int layer);

private slots:
    void updateDataListParameter(unsigned int layer);

private:
    medAbstractLayeredViewPrivate *d;
};

template <class Interactor>
Interactor* medAbstractLayeredView::getLayerInteractorOfType(int layerId)
{
    const QList<medAbstractInteractor*> interactors = layerInteractors(layerId);
    // get the correct interactor
    Interactor *interactor = nullptr;
    for (int i = 0; i < interactors.count(); ++i)
    {
        interactor = dynamic_cast<Interactor*>(interactors[i]);
        if (interactor)
        {
            return interactor;
        }
    }
    return nullptr;
}
