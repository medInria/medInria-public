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

class MEDCORE_EXPORT medAbstractRegistrationProcessLegacy : public medAbstractProcess
{
    Q_OBJECT

public:
    medAbstractRegistrationProcessLegacy(medAbstractProcess *parent = NULL);
    virtual ~medAbstractRegistrationProcessLegacy();

public:
    virtual bool setFixedInput(medAbstractData *data) = 0;
    virtual bool setMovingInput(medAbstractData *data) = 0;
    virtual bool write(const QString &file) = 0;
    virtual bool write(const QStringList& files) = 0;

};
