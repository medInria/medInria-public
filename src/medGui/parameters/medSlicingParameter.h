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

class QWidget;
class QSpinBox;
class QSlider;


class medSlicingParameterPrivate;
class MEDGUI_EXPORT medSlicingParameter : public medAbstractDoubleParameter
{
    Q_OBJECT

public:
    medSlicingParameter(QString name = "Unknow slicing parameter", QObject* parent = 0);
    virtual ~medSlicingParameter();

    int slice() const;
    double position() const;

    QSpinBox* getSpinBox();
    QSlider* getSlider();

    virtual QWidget* getWidget();

    int mapPositionToSLice(const double& position);
    double mapSLiceToPosition (int slice);

public slots:
    void setPositionRange(double posMin, double posMax);
    void setNumberOfSLice(int numberOfSLice);
    void setSlice(int slice);
    void setSlice(double position);

protected:
    virtual void updateInternWigets();


private slots:
    void _prvt_removeInternSpinBox();

private:
    medSlicingParameterPrivate* d;
};
