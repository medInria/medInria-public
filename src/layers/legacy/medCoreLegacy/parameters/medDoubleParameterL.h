/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2018. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractParameterL.h>

#include <medCoreLegacyExport.h>

class QWidget;
class QDoubleSpinBox;
class QSlider;


class medDoubleParameterLPrivate;
class MEDCORELEGACY_EXPORT medDoubleParameterL : public medAbstractDoubleParameterL
{
    Q_OBJECT

public:
    medDoubleParameterL(QString name = "Unknown double parameter", QObject* parent = 0);
    virtual ~medDoubleParameterL();

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
    medDoubleParameterLPrivate* d;
};
