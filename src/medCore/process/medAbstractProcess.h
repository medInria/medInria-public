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
class medViewContainerSplitter;


class medProcessIOPort
{
public:
    medProcessIOPort(QString name) {m_name = name;}
    virtual ~medProcessIOPort(){}

public:
    QString name() const {return m_name;}

    virtual void retrieveData(medProcessIOPort *port) {}

private:
    QString m_name;
};

template <typename T>
class medProcessInput : public medProcessIOPort
{
public:
    medProcessInput(QString name, bool isOptional) : medProcessIOPort(name)
    {m_isOptional = isOptional;}
    virtual ~medProcessInput(){}

public:
    bool isOptional() const {return m_isOptional;}

    T input() const {return m_input;}
    void setInput(T input) {m_input = input;}

    virtual void retrieveData(medProcessIOPort *otherport)
    {
        medProcessInput<T> *otherInputPort = dynamic_cast<medProcessInput<T> *>(otherport);
        if(otherInputPort)
            this->setInput(otherInputPort->input());
    }

private:
    bool m_isOptional;
    T m_input;
};

template <typename T>
class medProcessOutput : public medProcessIOPort
{
public:
    medProcessOutput(QString name) : medProcessIOPort(name)
    {}
    virtual ~medProcessOutput(){}

public:
    T output() const {return m_output;}
    void setOutput(T output) {m_output = output;}

private:
    T m_output;
};


/**
 * Extending dtkAbstractProcess class to hold more specific information
 */
class MEDCORE_EXPORT medAbstractProcess : public dtkAbstractProcess
{
    Q_OBJECT

public:
    typedef medProcessOutput<medAbstractData*> medOutputDataPort;
    typedef medProcessInput<medAbstractData*> medInputDataPort;

public:
    medAbstractProcess( medAbstractProcess * parent = NULL );
    virtual ~medAbstractProcess();

public:
    QList<medProcessIOPort*> inputs() const;
    QList<medProcessIOPort*> outputs() const;

    medProcessIOPort* input(QString name) const;

    void retrieveInputs(const medAbstractProcess *);

protected:
    void appendInput(medProcessIOPort*);
    void appendOutput(medProcessIOPort*);

public:
    virtual QList<medAbstractParameter*> parameters() = 0;
    medAbstractParameter* parameter(QString parameterName);



template <class T>
void setInput(T data, unsigned int port)
{
    if(port >= (unsigned int)this->inputs().size())
        return;

    medProcessInput<T>* inputPort = reinterpret_cast< medProcessInput<T> *>(this->inputs().at(port));
    if(inputPort)
        inputPort->setInput(data);

    medInputDataPort* inputDataPort = reinterpret_cast< medInputDataPort*>(this->inputs().at(port));
    if(inputDataPort)
    {
        updateContainer(inputDataPort);
    }
}

template <class T>
T input(unsigned int port)
{
    if(port >= (unsigned int)this->inputs().size())
        return NULL;

    medProcessInput<T>* inputPort = reinterpret_cast< medProcessInput<T> *>(this->inputs().at(port));
    if(inputPort)
        return inputPort->input();
    else return NULL;
}

template <class T>
void setOutput(T data, unsigned int port)
{
    if(port >= (unsigned int)this->outputs().size())
        return;

    medProcessOutput<T>* outputPort = reinterpret_cast<medProcessOutput<T> *>(this->outputs().at(port));
    if(outputPort)
        outputPort->setOutput(data);
}

template <class T>
T output(unsigned int port)
{
    if(port >= (unsigned int)this->outputs().size())
        return NULL;

    medProcessOutput<T>* outputPort = reinterpret_cast<medProcessOutput<T> *>(this->outputs().at(port));
    if(outputPort)
        return outputPort->output();
    else return NULL;
}

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
    virtual bool isInteractive() = 0;

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
