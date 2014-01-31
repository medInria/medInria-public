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

class medAbstractData;

class medAbstractLayeredViewPrivate;
class MEDCORE_EXPORT medAbstractLayeredView : public medAbstractView
{
    Q_OBJECT

public:
    medAbstractLayeredView(QObject * parent = 0);
    virtual ~medAbstractLayeredView();

    void addLayer(medAbstractData *data);
    bool removeData(medAbstractData *data);
    void removeLayer(unsigned int layer);
    void insertLayer(unsigned int layer, medAbstractData *data);
    void moveLayer(unsigned int fromLayer, unsigned int toLayer);
    medAbstractData * data(unsigned int layer) const;
    bool contains(medAbstractData * data) const;
    unsigned int layersCount() const;
    unsigned int layer(medAbstractData * data);


    /**
     * Set the visibility of the data on the corresponding layer
     */
    void setVisibility (bool visibility, unsigned int layer);

    /**
     * Get the visibility of the data on the corresponding layer
     */
    bool visibility(unsigned int layer) ;

    QList<dtkSmartPointer<medAbstractData> > data() const;

signals:

    /**
     * This signal is emitted when the visibility of a layer has changed.
     */
    void visibilityChanged(bool visibility, unsigned int layer);

    /**
     *  This signal is emitted when the user adds a data to the view
     */
    void dataAdded (int layer);
    void dataAdded (medAbstractData* data);
    void dataAdded (medAbstractData* data, unsigned int layer);
    void dataRemoved (int layer);
    void dataRemoved(medAbstractData* data, unsigned int layer);

protected:
    virtual medAbstractLayeredViewInteractor * primaryInteractor(medAbstractData* data);
    virtual QList<medAbstractInteractor *> extraInteractors(medAbstractData* data);
    virtual medAbstractLayeredViewInteractor * primaryInteractor(unsigned int layer);
    virtual QList<medAbstractInteractor *> extraInteractors(unsigned int layer);

    virtual medAbstractLayeredViewNavigator * primaryNavigator();
    virtual QList<medAbstractNavigator *> extraNavigators();

    virtual void initialiseInteractors(medAbstractData* data);
    virtual void initialiseNavigators();
    virtual void removeInteractors(medAbstractData *data);

private:
    medAbstractLayeredViewPrivate *d;

};
