/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medCoreExport.h>

#include <medAbstractProcess.h>

class medAbstractData;

class MEDCORE_EXPORT medAbstractEstimateTransformationProcess : public medAbstractProcess
{
    Q_OBJECT

public:
    medAbstractEstimateTransformationProcess(medAbstractProcess *parent = NULL);
    virtual ~medAbstractEstimateTransformationProcess();

public:
    virtual bool setFixedInput(medAbstractData *data) = 0;
    virtual bool setMovingInput(medAbstractData *data) = 0;
    virtual bool writeTransform(const QString &file) = 0;

public:
    virtual bool isInteractive() const;

public:
    virtual QList<medAbstractParameter*> parameters();

};
