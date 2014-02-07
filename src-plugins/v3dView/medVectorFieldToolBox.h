/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "medToolBox.h"
#include "medGuiExport.h"

class medVectorFieldToolBoxPrivate;

class MEDGUI_EXPORT medVectorFieldToolBox : public medToolBox
{
    Q_OBJECT

public:
    medVectorFieldToolBox(QWidget *parent);
    ~medVectorFieldToolBox();   

    void update (dtkAbstractView *view);

    static bool registered();

public slots:
    void setScaleFactor();
    void setSampleRate (int sampleRate);
    void setColorMode(int mode);
    void setProjection(bool enable);
    void setShowAxial(int checkBoxState);
    void setShowCoronal(int checkBoxState);
    void setShowSagittal(int checkBoxState);

private:
    medVectorFieldToolBoxPrivate *d;

};
