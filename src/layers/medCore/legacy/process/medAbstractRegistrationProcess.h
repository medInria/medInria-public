#pragma once
/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2020. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractProcessLegacy.h>

class medAbstractData;

class MEDCORE_EXPORT medAbstractRegistrationProcess : public medAbstractProcessLegacy
{
    Q_OBJECT

public:
    medAbstractRegistrationProcess(medAbstractProcessLegacy *parent = nullptr);
    virtual ~medAbstractRegistrationProcess();

public:
    virtual bool setFixedInput(medAbstractData *data) = 0;
    virtual bool setMovingInput(medAbstractData *data) = 0;
    virtual bool write(const QString &file) = 0;
    virtual bool write(const QStringList& files) = 0;
};
