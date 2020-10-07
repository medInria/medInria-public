#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractSingleFilterOperationProcessPresenter.h>
#include <medAbstractSingleFilterOperationDoubleProcess.h>

#include <medWidgetsExport.h>

class QWidget;
class QPushButton;

class medViewContainerSplitter;
class medAbstractData;

class medAbstractSingleFilterOperationDoubleProcessPresenterPrivate;
class MEDWIDGETS_EXPORT medAbstractSingleFilterOperationDoubleProcessPresenter : public medAbstractSingleFilterOperationProcessPresenter
{
    Q_OBJECT

public:
    medAbstractSingleFilterOperationDoubleProcessPresenter(medAbstractSingleFilterOperationDoubleProcess *parent = nullptr);
    virtual ~medAbstractSingleFilterOperationDoubleProcessPresenter();

    virtual QWidget *buildToolBoxWidget();

    const QScopedPointer<medAbstractSingleFilterOperationDoubleProcessPresenterPrivate> d;

};

