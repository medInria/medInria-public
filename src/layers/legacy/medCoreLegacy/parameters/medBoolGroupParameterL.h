/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QBoxLayout>

#include <medAbstractParameterL.h>

#include <medCoreLegacyExport.h>

class QWidget;

class medBoolParameterL;

class medBoolGroupParameterLPrivate;
class MEDCORELEGACY_EXPORT medBoolGroupParameterL : public medAbstractGroupParameterL
{
    Q_OBJECT

public:
    medBoolGroupParameterL(QString name = "Unknown bool group parameter", QObject* parent = nullptr);
    virtual ~medBoolGroupParameterL();

    void setPushButtonDirection(QBoxLayout::Direction direction);
    void setRadioButtonDirection(QBoxLayout::Direction direction);
    void setCheckBoxDirection(QBoxLayout::Direction direction);

    void addParameter(medBoolParameterL* parameter);
    void removeBoolParameter(medBoolParameterL* parameter);

    QWidget* getPushButtonGroup();
    QWidget* getRadioButtonGroup();
    QWidget* getCheckBoxGroup();

    virtual QWidget* getWidget();
    virtual QList<medAbstractParameterL*> parametersCandidateToPool() const;

    virtual void trigger();

private slots:
    void removeInternPushButtonGroup();
    void removeInternRadioButtonGroup();
    void removeInternCheckBoxGroup();

private:
    medBoolGroupParameterLPrivate* d;

};
