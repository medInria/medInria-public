/*=========================================================================

medInria

Copyright (c) INRIA 2013. All rights reserved.
See LICENSE.txt for details.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkAbstractViewInteractor.h>

#include <medCoreExport.h>

#include <QWidget>

#include <medAbstractView.h>
#include <medAbstractData.h>
#include <medImageViewEnum.h>
#include <medDataEnum.h>

class medAbstractParameter;
class medBoolParameter;

class medAbstractInteractorPrivate;
class MEDCORE_EXPORT medAbstractInteractor : public dtkAbstractViewInteractor
{
    Q_OBJECT

public:
             medAbstractInteractor(medAbstractView* parent);
    virtual ~medAbstractInteractor();

public:
    virtual QString description() const = 0;

    virtual void setData(medAbstractData *data);
    virtual medAbstractData *data() const;
    
    virtual QWidget* toolBoxWidget();
    virtual QWidget* toolBarWidget();
    virtual QWidget* layerWidget();

    virtual QList<medAbstractParameter*> linkableParameters() = 0;
    virtual QList<medBoolParameter*> mouseInteractionParameters() = 0;

public slots:
    virtual void updateWidgets(){}

protected:
    /**
    * @brief buildToolBoxWidget reimplement it to construct and return the widget displayed in the
    * view settings toolBox when the container of the parent view is single selected.
    * @return
    */
    virtual QWidget* buildToolBoxWidget() = 0;
    /**
    * @brief buildToolBoxWidget reimplement it to construct and return the widget displayed in the
    * toolbar of the container where the parent view is dsiplayed.
    * @return
    */
    virtual QWidget* buildToolBarWidget() = 0;
    /**
    * @brief buildToolBoxWidget reimplement it to construct and return the widget displayed in the
    * layer toolbox when the container of the parent view is selected.
    * @return
    */
    virtual QWidget* buildLayerWidget() = 0;

private slots:
    void removeInternToolBoxWidget();
    void removeInternToolBarWidget();
    void removeInternLayerWidget();


private:
    medAbstractInteractorPrivate *d;

/*=========================================================================
                         NOT IMPLEMENTED ON PURPOSE
*=========================================================================*/
private:
    using dtkAbstractViewInteractor::setView;
    using dtkAbstractViewInteractor::view;
    using dtkAbstractViewInteractor::setData;
//    using dtkAbstractViewInteractor::data;

    using dtkAbstractViewInteractor::output;
    using dtkAbstractViewInteractor::predraw;
    using dtkAbstractViewInteractor::draw;
    using dtkAbstractViewInteractor::postdraw;
    using dtkAbstractViewInteractor::enabled;
    using dtkAbstractViewInteractor::enable;
    using dtkAbstractViewInteractor::disable;
};
