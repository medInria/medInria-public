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

class medVector3DParameterPrivate;
class QVector3D;

class MEDCORE_EXPORT medVector3DParameter : public medAbstractVector3DParameter
{
public:
    medVector3DParameter(QString name = "Unknow Vector 3D parameter", QObject* parent = 0);
    virtual ~medVector3DParameter();

    virtual QWidget* getWidget();

protected:
    virtual void updateInternWigets();

private:
    medVector3DParameterPrivate *d;

};
