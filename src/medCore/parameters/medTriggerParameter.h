/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractParameter.h>

#include <medCoreExport.h>

class QIcon;
class QPushButton;
class QSize;
class QWidget;

class medTriggerParameterPrivate;
class MEDCORE_EXPORT medTriggerParameter : public medAbstractTriggerParameter
{
    Q_OBJECT

public:
    medTriggerParameter(QString name = "Unknow trigger parameter", QObject* parent = 0);
    virtual ~medTriggerParameter();

    void setButtonIcon(QIcon& icon);
    void setButtonIconSize(QSize& size);
    void setButtonText(QString text);
    QPushButton* getPushButton();

    virtual QWidget* getWidget();
private slots:
    void removeInternPushButton();

private:
    medTriggerParameterPrivate* d;
};
