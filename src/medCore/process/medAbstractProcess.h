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
#include <medAbstractJob.h>

class medAbstractProcessPrivate;

class medAbstractParameter;
class medToolBox;
class medTriggerParameter;

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
    bool threaded() const;
    void setThreaded(bool threaded = true);

public slots:
    int start();

public:
    virtual medToolBox* toolbox();
    virtual QWidget* parameterWidget();
    virtual medTriggerParameter* runParameter() const;

public:
    virtual bool isInteractive() = 0;

public slots:
    virtual medAbstractData *output() = 0;

private slots:
    virtual int update () = 0;

private:

signals:
    void showError(QString message, unsigned int timeout = 5000);


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

class medRunnableProcess: public medAbstractJob
{
    Q_OBJECT

private:
    medAbstractProcess* m_process;

public:
    medRunnableProcess(medAbstractProcess* process, QString name);

    virtual void internalRun();
    virtual void cancel();
};
