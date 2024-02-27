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
#include <medVariantListParameter.h>

class QWidget;
class medCompositeParameterPresenterPrivate;


class MEDCOREGUI_EXPORT medVariantListParameterPresenter : public medAbstractParameterPresenter
{
    Q_OBJECT

public:
    medVariantListParameterPresenter(medVariantListParameter *parent);
    medVariantListParameterPresenter(const QString & newParameterId);
    virtual ~medVariantListParameterPresenter();

    virtual medVariantListParameter *parameter() const;

    QWidget * buildWidget() override;

private:
    const QScopedPointer<medCompositeParameterPresenterPrivate> d;
};
