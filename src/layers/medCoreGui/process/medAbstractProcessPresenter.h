#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QObject>

#include <medAbstractProcess.h>
#include <medProcessPresenterFactory.h>
#include <medWidgetsExport.h>

class QWidget;
class QPushButton;
class medViewContainerSplitter;

class medAbstractProcessPresenterPrivate;
class MEDWIDGETS_EXPORT medAbstractProcessPresenter : public QObject
{
    Q_OBJECT

public:
    medAbstractProcessPresenter(medAbstractProcess *parent = nullptr);
    virtual ~medAbstractProcessPresenter();

    virtual medAbstractProcess *process() const = 0;

    virtual QWidget *buildToolBoxWidget() = 0;
    virtual medViewContainerSplitter *buildViewContainerSplitter() = 0;

    QPushButton *buildRunButton();
    QPushButton *buildCancelButton();

protected slots:
    void _runProcessFromThread();

private:
    const QScopedPointer<medAbstractProcessPresenterPrivate> d;
};
MED_DECLARE_PROCESS_PRESENTER_FACTORY(medAbstractProcess, MEDWIDGETS_EXPORT)
