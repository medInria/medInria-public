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
#include <medGroupParameter.h>

class QWidget;
class QGroupBox;
class QVBoxLayout;

class medGroupParameterPresenterPrivate;

class MEDCOREGUI_EXPORT medGroupParameterPresenter : public medAbstractParameterPresenter
{
    Q_OBJECT

public:
    medGroupParameterPresenter(medGroupParameter *parent);
    medGroupParameterPresenter(const QString & newParameterId);
    ~medGroupParameterPresenter() override;

    virtual medGroupParameter* parameter() const;

    QWidget *   buildWidget() override;
    QGroupBox * buildGroupButton();

private:
    void addParam(medAbstractParameter * param, QVBoxLayout * pVLayout);

private:
    const QScopedPointer<medGroupParameterPresenterPrivate> d;
};
