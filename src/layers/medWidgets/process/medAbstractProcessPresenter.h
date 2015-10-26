/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QObject>

#include <medWidgetsExport.h>
#include <medAbstractProcess.h>

class QWidget;
class QPushButton;

class medAbstractProcessPresenterPrivate;
class MEDWIDGETS_EXPORT medAbstractProcessPresenter : public QObject
{
    Q_OBJECT

public:
    medAbstractProcessPresenter(medAbstractProcess *parent = NULL);
    virtual ~medAbstractProcessPresenter();

    virtual medAbstractProcess *process() const = 0;

    virtual QWidget *buildToolBoxWidget() = 0;
    QPushButton *buildRunButton();

protected slots:
    void _runProcessFromThread();

private:
    const QScopedPointer<medAbstractProcessPresenterPrivate> d;
};

