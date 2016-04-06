/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractParameterPresenter.h>

#include <medIntParameter.h>

class QWidget;
class QSpinBox;
class QProgressBar;

class medIntParameterPresenterPrivate;
class MEDWIDGETS_EXPORT medIntParameterPresenter : public medAbstractParameterPresenter
{
    Q_OBJECT

public:
    medIntParameterPresenter(medIntParameter *parent);
    medIntParameterPresenter(const QString & newParameterId);
    virtual ~medIntParameterPresenter();

    virtual medIntParameter *parameter() const;

    virtual QWidget *buildWidget();
    QSpinBox *buildSpinBox();
    QProgressBar *buildProgressBar();

    void setSingleStep(int step);
    int singleStep() const;

signals:
    void singleStepChanged(int step);

private:
    const QScopedPointer<medIntParameterPresenterPrivate> d;
};
