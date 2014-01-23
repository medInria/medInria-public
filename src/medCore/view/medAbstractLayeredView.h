/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkSmartPointer.h>
#include "medCoreExport.h"
#include "medAbstractView.h"

class medAbstractData;
class medAbstractLayeredViewPrivate;


class MEDCORE_EXPORT medAbstractLayeredView : public medAbstractView
{
    Q_OBJECT

public:
    medAbstractLayeredView(medAbstractView * parent);

    virtual void addLayer(medAbstractData *data);
    virtual bool removeLayer(medAbstractData *data);
    virtual void removeLayerAt(unsigned int layer);
    virtual void insertLayer(unsigned int layer, medAbstractData *data);
    virtual void moveLayer(unsigned int fromLayer, unsigned int toLayer);
    virtual medAbstractData * dataAtLayer(unsigned int layer) const;
    virtual bool contains(medAbstractData * data) const;
    virtual unsigned int layersCount() const;


    /**
     * Set the visibility of the data on the corresponding layer
     */
    virtual void setVisibility (bool visibility, int layer);

    /**
     * Get the visibility of the data on the corresponding layer
     */
    virtual bool visibility(int layer) const;


    /**
     * Get the current layer. The current layer is used to determine which layer will receive
     * property changed.
     */
    virtual int currentLayer() const;


signals:

    /**
     * This signal is emitted when the visibility of a layer has changed.
     */
    void visibilityChanged(bool visibility, int layer);

    /**
     *  This signal is emitted when the user adds a data to the view
     */
    //TODO: to simplify
    void dataAdded (int layer);
    void dataAdded (dtkAbstractData* data);
    void dataAdded (dtkAbstractData* data, int layer);
    void dataRemoved (int layer);
    void dataRemoved(dtkAbstractData* data,int layer);


public slots:
    /**
     * Set the current layer. The current layer is used to determine which layer will receive
     * property changed.
     */
    virtual void setCurrentLayer(int layer);

protected:
    virtual void addLayer_impl(int layer) = 0;
    virtual bool removeLayer_impl(medAbstractData *data) = 0;
    virtual void removeLayerAt_impl(unsigned int layer) = 0;
    virtual void insertLayer_impl(unsigned int layer, medAbstractData *data) = 0;
    virtual void moveLayer_impl(unsigned int fromLayer, unsigned int toLayer) = 0;

protected slots:
    virtual void setCurrentLayer_impl(int layer) = 0;

private:
    medAbstractLayeredViewPrivate *d;
};
