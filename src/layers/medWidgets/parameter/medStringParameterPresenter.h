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

class MEDWIDGETS_EXPORT medStringParameterPresenter : public medAbstractParameterPresenter
{
    Q_OBJECT

public:
    medStringParameterPresenter(medStringParameter *parent);
    medStringParameterPresenter(const QString & newParameterId);
    ~medStringParameterPresenter() override;

    virtual medStringParameter *parameter() const;

    QWidget *buildWidget() override;
    QLineEdit *buildLineEdit();
<<<<<<< HEAD
    QLineEdit *buildLineEditOnFinish();
=======
    QDateEdit *buildDateEdit();
>>>>>>> [NEW] - Add C-Find Filters parameters in Browser Area and apply filters when  QtDcm is Requested

private:
    const QScopedPointer<medStringParameterPresenterPrivate> d;
};
