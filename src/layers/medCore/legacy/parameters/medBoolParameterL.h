#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractParameterL.h>

class QCheckBox;
class QPushButton;
class QRadioButton;
class QSize;
class QIcon;

class medBoolParameterLPrivate;
class MEDCORE_EXPORT medBoolParameterL : public medAbstractBoolParameterL
{
    Q_OBJECT

public:
    medBoolParameterL(QString name = "Unknown bool parameter", QObject* parent = nullptr);
    virtual ~medBoolParameterL();

    void setIcon(QIcon icon);
    void setIconSize(QSize size);
    void setText(QString text);
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
    medBoolParameterLPrivate* d;
};
