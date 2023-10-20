/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medTriggerParameter.h>
#include <medAbstractParameterPresenter.h>
#include <medWidgetsExport.h>

class QIcon;
class QPushButton;
class QSize;
class QWidget;

class medTriggerParameterPresenterPrivate;
class MEDWIDGETS_EXPORT medTriggerParameterPresenter : public medAbstractParameterPresenter
{
    Q_OBJECT

public:
    medTriggerParameterPresenter(medTriggerParameter *parent);
    ~medTriggerParameterPresenter() override;

    void setIcon(QIcon& icon);
    void setIconSize(QSize& size);

    QWidget* buildWidget() override;
    QPushButton* buildPushButton();

    medAbstractParameter *parameter() const override;
private:
    medTriggerParameterPresenterPrivate* d;
};
