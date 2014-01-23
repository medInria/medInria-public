/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractParameter.h>

#include "medGuiExport.h"

class QCheckBox;
class QPushButton;
class QRadioButton;
class QSize;
class QIcon;

class medBoolParameterPrivate;
class MEDGUI_EXPORT medBoolParameter : public medAbstractBoolParameter
{
    Q_OBJECT

public:
    medBoolParameter(QString name = "Unknow bool parameter");
    virtual ~medBoolParameter();

    void setIcon(QIcon& icon);
    void setIconSize(QSize& size);
    void setText(QString& text);
    QPushButton* getPushButton();
    QRadioButton* getRadioButton();
    QCheckBox* getCheckBox();

    virtual QWidget* getWidget();

protected:
    virtual void updateInternWigets();

private slots:
    void removeInternPushButton();
    void removeInternRadioButton();
    void removeInternCheckBox();

private:
    medBoolParameterPrivate* d;
};
