/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractParameterL.h>

class QIcon;
class QPushButton;
class QSize;
class QWidget;

class medTriggerParameterLPrivate;
class MEDCORE_EXPORT medTriggerParameterL : public medAbstractTriggerParameterL
{
    Q_OBJECT

public:
    medTriggerParameterL(QString name = "Unknown trigger parameter", QObject* parent = nullptr);
    virtual ~medTriggerParameterL();

    void setButtonIcon(QIcon& icon);
    void setButtonIconSize(QSize& size);
    void setButtonText(QString text);
    QPushButton* getPushButton();

    virtual QWidget* getWidget();

private slots:
    void removeInternPushButton();

private:
    medTriggerParameterLPrivate* d;
};
