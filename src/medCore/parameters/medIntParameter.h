/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractParameter.h>

#include <medCoreExport.h>

class QWidget;
class QSpinBox;
class QSlider;


class medIntParameterPrivate;
class MEDCORE_EXPORT medIntParameter : public medAbstractIntParameter
{
    Q_OBJECT

public:
    medIntParameter(QString name = "Unknow int parameter", QObject* parent = 0);
    virtual ~medIntParameter();

    void setRange(int min, int max);
    QSpinBox* getSpinBox();
    QSlider* getSlider();

    virtual QWidget* getWidget();

public slots:
    void setValue(int value);

protected:
    virtual void updateInternWigets();

private slots:
    void removeInternSpinBox();
    void removeInternSlider();

private:
    medIntParameterPrivate* d;
};
