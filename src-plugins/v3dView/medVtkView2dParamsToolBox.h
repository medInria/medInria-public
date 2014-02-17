/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medGuiExport.h>
#include <medToolBox.h>
#include <medParameter.h>

class medVtkView;
class medVtkView2dParamsToolBoxPrivate;

class MEDGUI_EXPORT medVtkView2dParamsToolBox : public medToolBox
{
    Q_OBJECT
public:
    explicit medVtkView2dParamsToolBox(QWidget *parent = 0);

public:
    void update(dtkAbstractView * view);
    static bool registered();
    QList<medAbstractParameter*> getParameters(medVtkView* view);

private slots:
    void setWindowing( bool checked );
    void setZooming( bool checked );
    void setSlicing( bool checked ) ;
    void setMeasuring( bool checked );
    void setAxisVisibility( bool visible ) ;
    void setScalarBarVisibility( bool visible );
    void setRulerVisibility( bool visible ) ;
    void setAnnotationsVisibility( bool visible );

private:
    medVtkView2dParamsToolBoxPrivate *d;
};


