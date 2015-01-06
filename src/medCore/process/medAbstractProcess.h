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

#include <medProcessPort.h>
#include <medProcessInput.h>
#include <medProcessOutput.h>

#include <medAbstractData.h>
#include <medAbstractImageData.h>

#include <medCoreExport.h>
#include <medAbstractJob.h>

class medAbstractProcessPrivate;

class medAbstractParameter;
class medToolBox;
class medTriggerParameter;
class medViewContainerSplitter;


/**
 * Extending dtkAbstractProcess class to hold more specific information
 */
class MEDCORE_EXPORT medAbstractProcess : public dtkAbstractProcess
{
    Q_OBJECT

public:
    typedef medProcessOutput<medAbstractData> medOutputDataPort;
    typedef medProcessInput<medAbstractData> medInputDataPort;

    typedef medProcessOutput<medAbstractImageData> medOutputImageDataPort;
    typedef medProcessInput<medAbstractImageData> medInputImageDataPort;

public:
    medAbstractProcess( medAbstractProcess * parent = NULL );
    virtual ~medAbstractProcess();

public:
    QList<medProcessPort*> inputs() const;
    QList<medProcessPort*> outputs() const;

    medProcessPort* input(QString name) const;

    void retrieveInputs(const medAbstractProcess *);

protected:
    void appendInput(medProcessPort*);
    void appendOutput(medProcessPort*);

public:
    virtual QList<medAbstractParameter*> parameters() = 0;
    medAbstractParameter* parameter(QString parameterName);

    template <class T>
    void setInput(T* data, unsigned int port);

    template <class T>
    T* input(unsigned int port);

    template <class T>
    void setOutput(T* data, unsigned int port);

    template <class T>
    T* output(unsigned int port);

protected slots:
    virtual void handleInput();
    virtual void handleOutputs();

public slots:
    int start();

public:
    virtual medToolBox* toolbox();
    virtual QWidget* parameterWidget();
    virtual medTriggerParameter* runParameter() const;
    virtual medViewContainerSplitter* viewContainerSplitter();

public:
    virtual bool isInteractive() const = 0;

private:
    virtual int update () = 0;

private:
    virtual void updateContainer(medInputDataPort *);

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

#include <medAbstractProcess.txx>
