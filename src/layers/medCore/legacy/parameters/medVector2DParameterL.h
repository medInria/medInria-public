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

class medVector2DParameterLPrivate;
class QVector2D;

class MEDCORE_EXPORT medVector2DParameterL : public medAbstractVector2DParameterL
{
public:
    medVector2DParameterL(QString name = "Unknown Vector 2D parameter", QObject* parent = nullptr);
    virtual ~medVector2DParameterL();

    virtual QWidget* getWidget();

protected:
    virtual void updateInternWigets();

private:
    medVector2DParameterLPrivate *d;

};
