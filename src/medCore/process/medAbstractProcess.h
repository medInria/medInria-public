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
#include <medProcessDataInput.h>

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
//    typedef medProcessOutput<medAbstractData> medOutputDataPort;
//    typedef medProcessInput<medAbstractData> medInputDataPort;

//    typedef medProcessOutput<medAbstractImageData> medOutputImageDataPort;
//    typedef medProcessInput<medAbstractImageData> medInputImageDataPort;

public:
    medAbstractProcess( medAbstractProcess * parent = NULL );
    virtual ~medAbstractProcess();

    QList<medProcessPort*> inputs() const;
    QList<medProcessPort*> outputs() const;

    medProcessPort* input(QString name) const;

    void retrieveInputs(const medAbstractProcess *);

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

    virtual medToolBox* toolbox();
    virtual QWidget* parameterWidget();
    virtual medTriggerParameter* runParameter() const;
    virtual medViewContainerSplitter* viewContainerSplitter();

    virtual bool isInteractive() const = 0;

public slots:
    int start();

signals:
    void showError(QString message, unsigned int timeout = 5000);

protected:
    void appendInput(medProcessPort*);
    void appendOutput(medProcessPort*);

    template <class T>
    void appendDataInput(medProcessDataInput<T> *);

    void appendDataOutput(medProcessPort*);

protected slots:
    virtual void handleInput();
    virtual void handleOutputs();

private:
    virtual int update () = 0;

    virtual void updateContainer(medProcessPort *);
    void appendDataInputToList(medProcessPort* );

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
