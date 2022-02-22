#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractParameterPresenter.h>
#include <medIntParameter.h>

class QWidget;
class QSpinBox;
class QProgressBar;
class medSlider;
class medIntParameterPresenterPrivate;

class MEDWIDGETS_EXPORT medIntParameterPresenter : public medAbstractParameterPresenter
{
    Q_OBJECT

public:
    medIntParameterPresenter(medIntParameter *parent);
    medIntParameterPresenter(const QString & newParameterId);
    ~medIntParameterPresenter() override;

    medIntParameter *parameter() const;

    QWidget *buildWidget() override;
    QSpinBox *buildSpinBox();
    QProgressBar *buildProgressBar();
    medSlider * buildSlider();


    void setSingleStep(int step);
    int singleStep() const;

signals:
    void singleStepChanged(int step);

private:
    const QScopedPointer<medIntParameterPresenterPrivate> d;
};
