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

class QWidget;
class QSpinBox;
class QSlider;


class medDoubleParameterPrivate;
class MEDGUI_EXPORT medDoubleParameter : public medAbstractDoubleParameter
{
    Q_OBJECT

public:
    medDoubleParameter(QString name = "Unknow double parameter");
    virtual ~medDoubleParameter();

    void setRange(double min, double max);
    QSpinBox* getSpinBox();

    virtual QWidget* getWidget();

protected:
    virtual void updateInternWigets();

private slots:
    void removeInternSpinBox();

private:
    medDoubleParameterPrivate* d;
};
