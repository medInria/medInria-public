/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include "medTestExport.h"

#include <dtkCore/dtkSmartPointer.h>

#include "medAbstractView.h"

class medAbstractData;
class medQtViewPrivate;

class MEDTEST_EXPORT medQtView : public medAbstractView
{
    Q_OBJECT

public:
             medQtView();
    virtual ~medQtView();

    virtual QString description() const;

    static QString s_description();
    static bool registered();

    QWidget *widget();

    void setData (medAbstractData *data);
    void setData (medAbstractData *data, int layer);

    void *data();
    void *data(int layer);
    virtual medAbstractViewCoordinates * coordinates() {return NULL;}
protected:
private:
    medQtViewPrivate *d;
};


