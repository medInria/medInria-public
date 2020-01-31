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
#include <medAbstractDiffusionModelEstimationProcess.h>
#include <medProcessPresenterFactory.h>
#include <medWidgetsExport.h>

class QWidget;
class QPushButton;

class medViewContainerSplitter;
class medAbstractData;

class medAbstractDiffusionModelEstimationProcessPresenterPrivate;
class MEDWIDGETS_EXPORT medAbstractDiffusionModelEstimationProcessPresenter : public medAbstractProcessPresenter
{
    Q_OBJECT

public:
    medAbstractDiffusionModelEstimationProcessPresenter(medAbstractDiffusionModelEstimationProcess *parent = nullptr);
    virtual ~medAbstractDiffusionModelEstimationProcessPresenter();

    virtual QWidget *buildToolBoxWidget();
    virtual medViewContainerSplitter *buildViewContainerSplitter();

    void setUseRunControls(bool useRun);
    bool useRunControls();

private:
    const QScopedPointer<medAbstractDiffusionModelEstimationProcessPresenterPrivate> d;

private slots:
    void _importOutput(medAbstractJob::medJobExitStatus jobExitStatus);

    void _setInputFromContainer(medAbstractData *data);

    void setInputGradientFile();
    void setInputBValuesFile();

signals:
    void _outputImported(medAbstractData *);
};

MED_DECLARE_PROCESS_PRESENTER_FACTORY(medAbstractDiffusionModelEstimationProcess, MEDWIDGETS_EXPORT)
