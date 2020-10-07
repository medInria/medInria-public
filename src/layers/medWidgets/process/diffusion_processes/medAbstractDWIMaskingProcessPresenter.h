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
#include <medAbstractDWIMaskingProcess.h>
#include <medProcessPresenterFactory.h>
#include <medWidgetsExport.h>

class QWidget;
class QPushButton;

class medViewContainerSplitter;
class medAbstractData;

class medAbstractDWIMaskingProcessPresenterPrivate;
class MEDWIDGETS_EXPORT medAbstractDWIMaskingProcessPresenter : public medAbstractProcessPresenter
{
    Q_OBJECT

public:
    medAbstractDWIMaskingProcessPresenter(medAbstractDWIMaskingProcess *parent = nullptr);
    virtual ~medAbstractDWIMaskingProcessPresenter();

    void setUseRunControls(bool useRun);
    bool useRunControls();

    virtual QWidget *buildToolBoxWidget();
    virtual medViewContainerSplitter *buildViewContainerSplitter();

    const QScopedPointer<medAbstractDWIMaskingProcessPresenterPrivate> d;

private slots:
    void _importOutput(medAbstractJob::medJobExitStatus jobExitStatus);
    void _setInputFromContainer(medAbstractData *data);

signals:
    void _outputImported(medAbstractData *);
};

MED_DECLARE_PROCESS_PRESENTER_FACTORY(medAbstractDWIMaskingProcess, MEDWIDGETS_EXPORT)
