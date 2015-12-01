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
    medAbstractDWIMaskingProcessPresenter(medAbstractDWIMaskingProcess *parent = NULL);
    virtual ~medAbstractDWIMaskingProcessPresenter();

    virtual QWidget *buildToolBoxWidget();
    virtual medViewContainerSplitter *buildViewContainerSplitter();

    const QScopedPointer<medAbstractDWIMaskingProcessPresenterPrivate> d;

private slots:
    // TODO RDE - have to be moved later.
    void _importOutput(medAbstractJob::medJobExitStatus jobExitStatus);
    void _setInputFromContainer(medAbstractData *data);

signals:
    void _outputImported(medAbstractData *);
};

MED_DECLARE_PROCESS_PRESENTER_FACTORY(medAbstractDWIMaskingProcess, MEDWIDGETS_EXPORT)
