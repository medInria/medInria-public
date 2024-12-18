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
#include <medStringParameter.h>

class QWidget;
class QLineEdit;
class QDateEdit;
class medStringParameterPresenterPrivate;

class MEDCOREGUI_EXPORT medStringParameterPresenter : public medAbstractParameterPresenter
{
    Q_OBJECT

public:
    medStringParameterPresenter(medStringParameter *parent);
    medStringParameterPresenter(const QString & newParameterId);
    ~medStringParameterPresenter() override;

    virtual medStringParameter *parameter() const;

    QWidget *buildWidget() override;
    QLineEdit *buildLineEdit();
    QLineEdit *buildLineEditOnFinish();
    QDateEdit *buildDateEdit();
    QLineEdit *buildLineEditPassword();
    QWidget   *buildLineEditPasswordEyes();
    QWidget   *buildLineEditFileImport();

private:
    const QScopedPointer<medStringParameterPresenterPrivate> d;
};
