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

class medVector2DParameterPrivate;
class QVector2D;

class MEDCORE_EXPORT medVector2DParameter : public medAbstractVector2DParameter
{
public:
    medVector2DParameter(QString name = "Unknow Vector 2D parameter", QObject* parent = 0);
    virtual ~medVector2DParameter();

    virtual QWidget* getWidget();

protected:
    virtual void updateInternWigets();

private:
    medVector2DParameterPrivate *d;

};
