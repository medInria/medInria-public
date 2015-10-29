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

#include <medBoolParameter.h>

class QWidget;
class QCheckBox;
class QAbstractButton;

class medBoolParameterPresenterPrivate;
class MEDWIDGETS_EXPORT medBoolParameterPresenter : public medAbstractParameterPresenter
{
    Q_OBJECT

public:
    medBoolParameterPresenter(medBoolParameter *parent);
    medBoolParameterPresenter(const QString & newParameterId);
    virtual ~medBoolParameterPresenter();

    virtual medBoolParameter *parameter() const;

    virtual QWidget *buildWidget();
    QCheckBox *buildCheckBox();

private:
    void _connectButton(QAbstractButton *button);

private:
    const QScopedPointer<medBoolParameterPresenterPrivate> d;
};
