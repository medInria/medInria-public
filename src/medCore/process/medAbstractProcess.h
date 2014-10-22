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
#include <medAbstractData.h>

#include <medCoreExport.h>

class medAbstractProcessPrivate;

class medAbstractParameter;
class medToolBox;


struct medProcessIOPort
{
    QString name;
};

template <typename T>
struct medProcessInput : public medProcessIOPort
{
    bool isOptional;
    T* input;
};

template <typename T>
struct medProcessOutput : public medProcessIOPort
{
    T* output;
};



/**
 * Extending dtkAbstractProcess class to hold more specific information
 */
class MEDCORE_EXPORT medAbstractProcess : public dtkAbstractProcess
{
    Q_OBJECT

public:
    medAbstractProcess( medAbstractProcess * parent = NULL );
    virtual ~medAbstractProcess();

public:
    QList<medProcessIOPort*> inputs();
    QList<medProcessIOPort*> outputs();

protected:
    void appendInput(medProcessIOPort*);
    void appendOutput(medProcessIOPort*);

public:
    virtual QList<medAbstractParameter*> parameters() = 0;
    medAbstractParameter* parameter(QString parameterName);

public:
    virtual medToolBox* toolbox();

public:
    virtual bool isInteractive() = 0;

public slots:
    virtual medAbstractData *output() = 0;
    virtual int update () = 0;

public:
//    virtual voir prepareViewArea(medViewArea *);

private:
    using dtkAbstractProcess::onCanceled;
    using dtkAbstractProcess::read;
    using dtkAbstractProcess::write;
    using dtkAbstractProcess::setParameter;
    using dtkAbstractProcess::setInput;
    using dtkAbstractProcess::setData;
    //TODO rename our output methode
    //using dtkAbstractProcess::output;
    using dtkAbstractProcess::data;
    using dtkAbstractProcess::channelCount;

private:
    medAbstractProcessPrivate* d;
};
