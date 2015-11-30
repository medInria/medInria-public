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
#include <medAbstractDiffusionModelEstimationProcess.h>

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
    medAbstractDiffusionModelEstimationProcessPresenter(medAbstractDiffusionModelEstimationProcess *parent = NULL);
    virtual ~medAbstractDiffusionModelEstimationProcessPresenter();

    virtual QWidget *buildToolBoxWidget();
    virtual medViewContainerSplitter *buildViewContainerSplitter();

    const QScopedPointer<medAbstractDiffusionModelEstimationProcessPresenterPrivate> d;

private slots:
    // TODO RDE - have to be moved later.
    void _importOutput(medAbstractJob::medJobExitStatus jobExitStatus);

    void _setInputFromContainer(medAbstractData *data);
    void _setMaskFromContainer(medAbstractData *mask);

signals:
    void _outputImported(medAbstractData *);
};

