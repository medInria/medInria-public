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

#include <medGuiExport.h>

#include <QBoxLayout>

class QWidget;

class medBoolParameter;

class medBoolGroupParameterPrivate;
class MEDGUI_EXPORT medBoolGroupParameter : public medAbstractGroupParameter
{
    Q_OBJECT

public:
    medBoolGroupParameter(QString name = "Unknow bool group parameter", QObject* parent = 0);
    virtual ~medBoolGroupParameter();

    void setPushButtonDirection(QBoxLayout::Direction direction);
    void setRadioButtonDirection(QBoxLayout::Direction direction);
    void setCheckBoxDirection(QBoxLayout::Direction direction);

    void addParameter(medBoolParameter* parameter);
    void removeBoolParameter(medBoolParameter* parameter);

    QWidget* getPushButtonGroup();
    QWidget* getRadioButtonGroup();
    QWidget* getCheckBoxGroup();

    virtual QWidget* getWidget();
    virtual QList<medAbstractParameter*> parametersCandidateToPool() const;


private slots:
    void _prvt_removeInternPushButtonGroup();
    void _prvt_removeInternRadioButtonGroup();
    void _prvt_removeInternCheckBoxGroup();

private:
    medBoolGroupParameterPrivate* d;

};
