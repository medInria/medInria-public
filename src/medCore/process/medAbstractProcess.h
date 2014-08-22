/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkAbstractProcess.h>
#include <dtkCore/dtkAbstractProcess_p.h>
#include <medAbstractData.h>

#include <medCoreExport.h>

class medAbstractProcessPrivate;

/**
 * Extending dtkAbstractProcess class to hold more specific information
 */
class MEDCORE_EXPORT medAbstractProcess : public dtkAbstractProcess
{
    Q_OBJECT

public:
    medAbstractProcess( medAbstractProcess * parent = NULL );
    medAbstractProcess(const medAbstractProcess& other);
    virtual ~medAbstractProcess();

public slots:
    virtual medAbstractData *output() = 0;
    virtual int update () = 0;
    virtual void setInput ( medAbstractData *data, int channel = 0 ) = 0;
private:
    using dtkAbstractProcess::onCanceled;
    using dtkAbstractProcess::read;
    using dtkAbstractProcess::write;
    using dtkAbstractProcess::setInput;
    using dtkAbstractProcess::setData;
    //TODO rename our output methode
    //using dtkAbstractProcess::output;
    using dtkAbstractProcess::data;
    using dtkAbstractProcess::channelCount;

private:
    DTK_DECLARE_PRIVATE(medAbstractProcess);
    medAbstractProcessPrivate* d;

};

class MEDCORE_EXPORT medAbstractProcessPrivate : public dtkAbstractProcessPrivate
{
public:
    medAbstractProcessPrivate(medAbstractProcess *q = 0) : dtkAbstractProcessPrivate(q) {}
    medAbstractProcessPrivate(const medAbstractProcessPrivate& other) : dtkAbstractProcessPrivate(other) {}

public:
    virtual ~medAbstractProcessPrivate(void) {}
};

DTK_IMPLEMENT_PRIVATE(medAbstractProcess, dtkAbstractProcess);