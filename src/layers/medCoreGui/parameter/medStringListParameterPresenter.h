#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <QIcon>

#include <medAbstractParameterPresenter.h>
#include <medStringListParameter.h>

class QWidget;
class QComboBox;
class medStringListParameterPresenterPrivate;

class MEDCOREGUI_EXPORT medStringListParameterPresenter: public medAbstractParameterPresenter
{
    Q_OBJECT

public:
    medStringListParameterPresenter(medStringListParameter* parent);
    ~medStringListParameterPresenter() override;

    virtual medStringListParameter *parameter() const;

    QWidget *buildWidget() override;
    QComboBox* buildComboBox();


private:
    const QScopedPointer<medStringListParameterPresenterPrivate> d;
};