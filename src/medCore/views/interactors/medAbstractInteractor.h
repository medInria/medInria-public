/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCoreSupport/dtkAbstractViewInteractor.h>

#include <medCoreExport.h>

#include <QWidget>

#include <medAbstractView.h>
#include <medAbstractData.h>
#include <medImageViewEnum.h>

class medAbstractParameter;
class medBoolParameter;

class medAbstractInteractorPrivate;

/**
 * @class medAbstractInteractor
 * @brief Base class for interactor types in medInria
 * A medAbstractInteractor belongs to a medAbstractView and is responsible for managing
 * all the interactions for a type of medAbstractData in the medAbstractView.
 **/
class MEDCORE_EXPORT medAbstractInteractor : public dtkAbstractViewInteractor
{
    Q_OBJECT

public:
             medAbstractInteractor(medAbstractView* parent);
    virtual ~medAbstractInteractor();

public:
    virtual QString description() const = 0;

    virtual void setInputData(medAbstractData *data);
    virtual medAbstractData *inputData() const;
    
    virtual QWidget* toolBoxWidget();
    virtual QWidget* toolBarWidget();
    virtual QWidget* layerWidget();

    virtual QList<medAbstractParameter*> linkableParameters() = 0;
    virtual QList<medBoolParameter*> mouseInteractionParameters() = 0;

public slots:
    virtual void updateWidgets();

protected:
    virtual QWidget* buildToolBoxWidget() = 0;
    virtual QWidget* buildToolBarWidget() = 0;
    virtual QWidget* buildLayerWidget() = 0;

private slots:

private:
    medAbstractInteractorPrivate *d;

/*=========================================================================
                         NOT IMPLEMENTED ON PURPOSE
*=========================================================================*/
private:
    using dtkAbstractViewInteractor::setView;
    using dtkAbstractViewInteractor::view;
    using dtkAbstractViewInteractor::setData;
    using dtkAbstractViewInteractor::data;

    using dtkAbstractViewInteractor::output;
    using dtkAbstractViewInteractor::predraw;
    using dtkAbstractViewInteractor::draw;
    using dtkAbstractViewInteractor::postdraw;
    using dtkAbstractViewInteractor::enabled;
    using dtkAbstractViewInteractor::enable;
    using dtkAbstractViewInteractor::disable;
};
