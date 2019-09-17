/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractParameterL.h>
#include <medCoreLegacyExport.h>
#include <medSliderL.h>

class QWidget;
class QSpinBox;
class QSlider;


class medIntParameterLPrivate;
class MEDCORELEGACY_EXPORT medIntParameterL : public medAbstractIntParameterL
{
    Q_OBJECT

public:
    medIntParameterL(QString name = "Unknown int parameter", QObject* parent = nullptr);
    virtual ~medIntParameterL();

    void setRange(int min, int max);
    QSpinBox* getSpinBox();
    medSliderL *getSlider();

    virtual QWidget* getWidget();

public slots:
    void setValue(int value);

protected:
    virtual void updateInternWigets();

private slots:
    void removeInternSpinBox();
    void removeInternSlider();

private:
    medIntParameterLPrivate* d;
};
