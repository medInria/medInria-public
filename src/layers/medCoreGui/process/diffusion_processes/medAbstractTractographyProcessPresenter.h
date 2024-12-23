#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractProcessPresenter.h>
#include <medAbstractTractographyProcess.h>
#include <medProcessPresenterFactory.h>
#include <medCoreGuiExport.h>

class QWidget;
class QPushButton;

class medViewContainerSplitter;
class medAbstractFibersData;
class medAbstractData;

class medAbstractTractographyProcessPresenterPrivate;
class MEDCOREGUI_EXPORT medAbstractTractographyProcessPresenter : public medAbstractProcessPresenter
{
    Q_OBJECT

public:
    medAbstractTractographyProcessPresenter(medAbstractTractographyProcess *parent = nullptr);
    virtual ~medAbstractTractographyProcessPresenter();

    virtual QWidget *buildToolBoxWidget();
    virtual medViewContainerSplitter *buildViewContainerSplitter();

    const QScopedPointer<medAbstractTractographyProcessPresenterPrivate> d;

private slots:
    void _importOutput(medAbstractJob::medJobExitStatus jobExitStatus);

    void _setInputFromContainer(medAbstractData *data);

signals:
    void _outputImported(medAbstractData *);
};

MED_DECLARE_PROCESS_PRESENTER_FACTORY(medAbstractTractographyProcess, MEDCOREGUI_EXPORT)
