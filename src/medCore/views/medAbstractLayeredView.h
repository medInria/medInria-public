/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
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

class medAbstractLayeredViewPrivate;
class MEDCORE_EXPORT medAbstractLayeredView : public medAbstractView
{
    Q_OBJECT

public:
    medAbstractLayeredView(QObject * parent = 0);
    virtual ~medAbstractLayeredView();

    void addLayer(medAbstractData *data);
    void removeData(medAbstractData *data);
    void removeLayer(unsigned int layer);
    void insertLayer(unsigned int layer, medAbstractData *data);
    void moveLayer(unsigned int fromLayer, unsigned int toLayer);

    medAbstractData * layerData(unsigned int layer) const;
    QList<dtkSmartPointer<medAbstractData> > dataList() const;

    bool contains(medAbstractData * data) const;
    bool contains(QString identifier) const;

    unsigned int layersCount() const;
    unsigned int layer(medAbstractData * data);

    void setCurrentLayer(unsigned int layer);
    unsigned int currentLayer() const;

    medAbstractBoolParameter* visibilityParameter(unsigned int layer);

    medStringListParameter *layerLinkParameter(unsigned int layer);

    QList <medAbstractInteractor*> interactors(unsigned int layer);

    virtual QWidget* navigatorWidget();

public slots:
    void removeLayer();

signals:
    void layerAdded(unsigned int layer);
    void layerRemoved(unsigned int layer);
    void currentLayerChanged() const;

protected:
    virtual medAbstractLayeredViewInteractor * primaryInteractor(medAbstractData* data);
    virtual QList<medAbstractInteractor *> extraInteractors(medAbstractData* data);
    virtual medAbstractLayeredViewInteractor * primaryInteractor(unsigned int layer);
    virtual QList<medAbstractInteractor *> extraInteractors(unsigned int layer);

    virtual medAbstractLayeredViewNavigator * primaryNavigator();
    virtual QList<medAbstractNavigator *> extraNavigators();

    virtual bool initialiseInteractors(medAbstractData* data);
    virtual bool initialiseNavigators();
    virtual void removeInteractors(medAbstractData *data);

    virtual QList<medAbstractParameter*> interactorsParameters(unsigned int layer);

protected slots:
    virtual void setUpViewForThumbnail();
    virtual void linkLayer(QString);
    virtual void unlinkLayer(unsigned int layer = -1);

 private:
    medStringListParameter* createLinkParameter();

private:
    medAbstractLayeredViewPrivate *d;

};
