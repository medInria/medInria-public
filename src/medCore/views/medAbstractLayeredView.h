/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "medAbstractView.h"

#include "medCoreExport.h"

#include <dtkCore/dtkSmartPointer.h>

#include<medAbstractLayeredViewInteractor.h>
#include<medAbstractLayeredViewNavigator.h>

class medAbstractData;

class medAbstractLayeredViewPrivate;
class MEDCORE_EXPORT medAbstractLayeredView : public medAbstractView
{
    Q_OBJECT

public:
    medAbstractLayeredView(QObject * parent = 0);
    virtual ~medAbstractLayeredView();

    virtual void addLayer(medAbstractData *data);
    virtual bool removeLayer(medAbstractData *data);
    virtual void removeLayer(unsigned int layer);
    virtual void insertLayer(unsigned int layer, medAbstractData *data);
    virtual void moveLayer(unsigned int fromLayer, unsigned int toLayer);
    virtual medAbstractData * data(unsigned int layer) const;
    virtual bool contains(medAbstractData * data) const;
    virtual unsigned int layersCount() const;


    /**
     * Set the visibility of the data on the corresponding layer
     */
    virtual void setVisibility (bool visibility, unsigned int layer);

    /**
     * Get the visibility of the data on the corresponding layer
     */
    virtual bool visibility(unsigned int layer) const;

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
    virtual medAbstractLayeredViewInteractor* primaryIntercator(medAbstractData* data) = 0;
    virtual QList<medAbstractIntercator*> extraIntercator(medAbstractData* data) = 0;
    virtual medAbstractLayeredViewInteractor* primaryIntercator(unsigned int layer) = 0;
    virtual QList<medAbstractIntercator*> extraIntercator(unsigned int layer) = 0;

    virtual medAbstractLayeredViewNavigator* primaryNavigator() = 0;
    virtual QList<medAbstractNavigator*> extraNavigator() = 0;

private:
    medAbstractLayeredViewPrivate *d;

public:

    /**
     * @brief implementationOf
     * @return Upper abstract class it derives from.
     * Do NOT reimplement it in non abstract class.
     * Used by the factory to kwnow what can be create.
     */
    static QString implementationOf()
    {
        return "medAbstractLayerdView";
    }
};
