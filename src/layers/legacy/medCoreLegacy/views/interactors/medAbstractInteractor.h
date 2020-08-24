#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <dtkCoreSupport/dtkAbstractViewInteractor.h>

#include <QWidget>

#include <medAbstractView.h>
#include <medAbstractData.h>
#include <medCoreLegacyExport.h>
#include <medImageViewEnum.h>

class medAbstractParameterL;
class medBoolParameterL;

class medAbstractInteractorPrivate;

/**
 * @class medAbstractInteractor
 * @brief Base class for interactor types
 * A medAbstractInteractor belongs to a medAbstractView and is responsible for managing
 * all the interactions for a type of medAbstractData in the medAbstractView.
 **/
class MEDCORELEGACY_EXPORT medAbstractInteractor : public dtkAbstractViewInteractor
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

    virtual QList<medAbstractParameterL*> linkableParameters() = 0;
    virtual QList<medBoolParameterL*> mouseInteractionParameters() = 0;

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
