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
#include <medDoubleParameter.h>

class QWidget;
class QDoubleSpinBox;
class QProgressBar;
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
    QProgressBar *buildProgressBar();

    void setSingleStep(double step);
    double singleStep() const;

    void setDecimals(int precision);
    int decimals() const;

private:
    double _valueFromPercent(int percent);
    int _percentFromValue(double value);
private slots:
    void _setValueFromPercent(int percent);
    void _emitValueChangedInPercent(double value);

signals:
    void singleStepChanged(double step);
    void decimalsChanged(int precision);
    void valueChanged(int percent);

private:
    const QScopedPointer<medDoubleParameterPresenterPrivate> d;
};
