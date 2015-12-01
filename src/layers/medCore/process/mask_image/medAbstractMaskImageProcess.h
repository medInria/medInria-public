/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medAbstractProcess.h>

#include <dtkCore>

#include <medCoreExport.h>

class medAbstractImageData;
class medIntParameter;

class medAbstractMaskImageProcessPrivate;
class MEDCORE_EXPORT medAbstractMaskImageProcess : public medAbstractProcess
{
    Q_OBJECT

public:
    medAbstractMaskImageProcess(QObject *parent = NULL);
    virtual ~medAbstractMaskImageProcess();

    void setInput(medAbstractImageData* data);
    medAbstractImageData* input() const;

    void setMask(medAbstractImageData* data);
    medAbstractImageData* mask() const;

    medAbstractImageData* output() const;

protected:
    void setOutput(medAbstractImageData* data);

private:
    const QScopedPointer<medAbstractMaskImageProcessPrivate> d;
};

DTK_DECLARE_PLUGIN        (medAbstractMaskImageProcess, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_MANAGER(medAbstractMaskImageProcess, MEDCORE_EXPORT)
DTK_DECLARE_PLUGIN_FACTORY(medAbstractMaskImageProcess, MEDCORE_EXPORT)
