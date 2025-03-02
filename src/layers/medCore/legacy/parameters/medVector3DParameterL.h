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

class medVector3DParameterLPrivate;
class QVector3D;

class MEDCORE_EXPORT medVector3DParameterL : public medAbstractVector3DParameterL
{
public:
    medVector3DParameterL(QString name = "Unknown Vector 3D parameter", QObject* parent = nullptr);
    virtual ~medVector3DParameterL();

    virtual QWidget* getWidget();

protected:
    virtual void updateInternWigets();

private:
    medVector3DParameterLPrivate *d;

};
