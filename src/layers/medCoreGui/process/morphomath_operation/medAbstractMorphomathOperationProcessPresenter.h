#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractMorphomathOperationProcess.h>
#include <medAbstractProcessPresenter.h>
#include <medProcessPresenterFactory.h>
#include <medCoreGuiExport.h>

class QWidget;
class QPushButton;

class medViewContainerSplitter;
class medAbstractData;

class medAbstractMorphomathOperationProcessPresenterPrivate;
class MEDCOREGUI_EXPORT medAbstractMorphomathOperationProcessPresenter : public medAbstractProcessPresenter
{
    Q_OBJECT

public:
    medAbstractMorphomathOperationProcessPresenter(medAbstractMorphomathOperationProcess *parent = nullptr);
    virtual ~medAbstractMorphomathOperationProcessPresenter();

    virtual QWidget *buildToolBoxWidget();
    virtual medViewContainerSplitter *buildViewContainerSplitter();

    virtual medAbstractMorphomathOperationProcess* process() const;

    const QScopedPointer<medAbstractMorphomathOperationProcessPresenterPrivate> d;

private slots:
    void _importOutput(medAbstractJob::medJobExitStatus jobExitStatus);
    void _setInputFromContainer(medAbstractData *data);

signals:
    void _outputImported(medAbstractData *);
};
MED_DECLARE_PROCESS_PRESENTER_FACTORY(medAbstractMorphomathOperationProcess, MEDCOREGUI_EXPORT)
MED_DECLARE_PROCESS_PRESENTER_CREATOR(medAbstractMorphomathOperationProcess, medAbstractMorphomathOperationProcess)
