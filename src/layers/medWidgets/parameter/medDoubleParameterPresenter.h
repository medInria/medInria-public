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

#include <medDoubleParameter.h>

class QWidget;
class QDoubleSpinBox;

class medDoubleParameterPresenterPrivate;
class MEDWIDGETS_EXPORT medDoubleParameterPresenter : public medAbstractParameterPresenter
{
    Q_OBJECT

public:
    medDoubleParameterPresenter(medDoubleParameter *parent);
    medDoubleParameterPresenter(const QString & newParameterId);
    virtual ~medDoubleParameterPresenter();

    virtual medDoubleParameter *parameter() const;

    virtual QWidget *buildWidget();
    QDoubleSpinBox *buildSpinBox();

    void setSingleStep(double step);
    double singleStep() const;

    void setDecimals(int precision);
    int decimals() const;

signals:
    void _singleStepChanged(double step);
    void _decimalsChanged(int precision);

private:
    const QScopedPointer<medDoubleParameterPresenterPrivate> d;
};
