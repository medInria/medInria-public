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
#include <medCoreLegacyExport.h>

class QWidget;
class QDoubleSpinBox;
class QSlider;


class medDoubleParameterLPrivate;
class MEDCORELEGACY_EXPORT medDoubleParameterL : public medAbstractDoubleParameterL
{
    Q_OBJECT

public:
    medDoubleParameterL(QString name = "Unknown double parameter", QObject* parent = nullptr);
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
    void removeInternLabel();

    void setSliderIntValue(int value);

private:
    int convertToInt(double value);

    medDoubleParameterLPrivate* d;
};
