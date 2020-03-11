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
#include <medAbstractMaskImageProcess.h>
#include <medProcessPresenterFactory.h>
#include <medWidgetsExport.h>

class QWidget;
class QPushButton;

class medViewContainerSplitter;
class medAbstractData;

class medAbstractMaskImageProcessPresenterPrivate;
class MEDWIDGETS_EXPORT medAbstractMaskImageProcessPresenter : public medAbstractProcessPresenter
{
    Q_OBJECT

public:
    medAbstractMaskImageProcessPresenter(medAbstractMaskImageProcess *parent = nullptr);
    virtual ~medAbstractMaskImageProcessPresenter();

    virtual QWidget *buildToolBoxWidget();
    virtual medViewContainerSplitter *buildViewContainerSplitter();

    const QScopedPointer<medAbstractMaskImageProcessPresenterPrivate> d;

private slots:
    void _importOutput(medAbstractJob::medJobExitStatus jobExitStatus);
    void _setInputFromContainer(medAbstractData *data);
    void _setMaskFromContainer(medAbstractData *data);

signals:
    void _outputImported(medAbstractData *);
};

MED_DECLARE_PROCESS_PRESENTER_FACTORY(medAbstractMaskImageProcess, MEDWIDGETS_EXPORT)
