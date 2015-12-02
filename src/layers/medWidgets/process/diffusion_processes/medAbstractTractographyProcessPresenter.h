/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractProcessPresenter.h>
#include <medAbstractTractographyProcess.h>
#include <medProcessPresenterFactory.h>

#include <medWidgetsExport.h>

class QWidget;
class QPushButton;

class medViewContainerSplitter;
class medAbstractFibersData;
class medAbstractData;

class medAbstractTractographyProcessPresenterPrivate;
class MEDWIDGETS_EXPORT medAbstractTractographyProcessPresenter : public medAbstractProcessPresenter
{
    Q_OBJECT

public:
    medAbstractTractographyProcessPresenter(medAbstractTractographyProcess *parent = NULL);
    virtual ~medAbstractTractographyProcessPresenter();

    virtual QWidget *buildToolBoxWidget();
    virtual medViewContainerSplitter *buildViewContainerSplitter();

    const QScopedPointer<medAbstractTractographyProcessPresenterPrivate> d;

private slots:
    // TODO RDE - have to be moved later.
    void _importOutput(medAbstractJob::medJobExitStatus jobExitStatus);

    void _setInputFromContainer(medAbstractData *data);

signals:
    void _outputImported(medAbstractData *);
};

MED_DECLARE_PROCESS_PRESENTER_FACTORY(medAbstractTractographyProcess, MEDWIDGETS_EXPORT)
