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
#include <medAbstractSingleFilterOperationProcess.h>

#include <medWidgetsExport.h>

class QWidget;
class QPushButton;

class medViewContainerSplitter;
class medAbstractData;

class medAbstractSingleFilterOperationProcessPresenterPrivate;
class MEDWIDGETS_EXPORT medAbstractSingleFilterOperationProcessPresenter : public medAbstractProcessPresenter
{
    Q_OBJECT

public:
    medAbstractSingleFilterOperationProcessPresenter(medAbstractSingleFilterOperationProcess *parent = NULL);
    virtual ~medAbstractSingleFilterOperationProcessPresenter();

    virtual QWidget *buildToolBoxWidget();
    virtual medViewContainerSplitter *buildViewContainerSplitter();

    const QScopedPointer<medAbstractSingleFilterOperationProcessPresenterPrivate> d;

private slots:
    // TODO RDE - have to be moved later.
    void _importOutput(medAbstractJob::medJobExitStatus jobExitStatus);
    void _setInputFromContainer(medAbstractData *data);

signals:
    void _outputImported(medAbstractData *);
};

