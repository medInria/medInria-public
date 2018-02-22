/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractProcessPresenter.h>
#include <medDiffusionModelEstimationMetaProcess.h>
#include <medProcessPresenterFactory.h>

#include <medWidgetsExport.h>

class QWidget;
class QPushButton;

class medViewContainerSplitter;
class medAbstractData;

class medDiffusionModelEstimationMetaProcessPresenterPrivate;
class MEDWIDGETS_EXPORT medDiffusionModelEstimationMetaProcessPresenter : public medAbstractProcessPresenter
{
    Q_OBJECT

public:
    medDiffusionModelEstimationMetaProcessPresenter(medDiffusionModelEstimationMetaProcess *parent = NULL);
    virtual ~medDiffusionModelEstimationMetaProcessPresenter();

    virtual QWidget *buildToolBoxWidget();
    virtual medViewContainerSplitter *buildViewContainerSplitter();

    virtual medDiffusionModelEstimationMetaProcess *process() const;

    const QScopedPointer<medDiffusionModelEstimationMetaProcessPresenterPrivate> d;

private slots:
    // TODO RDE - have to be moved later.
    void _importOutput(medAbstractJob::medJobExitStatus jobExitStatus);

    void _setInputFromContainer(medAbstractData *data);

    void chooseDWIMaskingPlugin(int processIndex);
    void chooseEstimationPlugin(int processIndex);

signals:
    void _outputImported(medAbstractData *);
};
