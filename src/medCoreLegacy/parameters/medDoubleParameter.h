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
class QDoubleSpinBox;
class QSlider;


class medDoubleParameterPrivate;
class MEDCORE_EXPORT medDoubleParameter : public medAbstractDoubleParameter
{
    Q_OBJECT

public:
    medDoubleParameter(QString name = "Unknow double parameter", QObject* parent = 0);
    virtual ~medDoubleParameter();

    void setRange(double min, double max);
    void setSingleStep(double step);
    void setDecimals(unsigned int decimals);

    QDoubleSpinBox* getSpinBox();
    QSlider* getSlider();
    QLabel* getValueLabel();

    virtual QWidget* getWidget();

public slots:
    void setValue(double value);

protected:
    virtual void updateInternWigets();

private slots:
    void removeInternSpinBox();
    void removeInternSlider();

    void setIntValue(int);

private:
    int convertToInt(double value);

private:
    medDoubleParameterPrivate* d;
};
