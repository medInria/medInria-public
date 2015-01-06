/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medAbstractProcess.h>


template <class T>
void medAbstractProcess::setInput(T* data, unsigned int port)
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
T* medAbstractProcess::input(unsigned int port)
{
    if(port >= (unsigned int)this->inputs().size())
        return NULL;

    medProcessInput<T>* inputPort = reinterpret_cast< medProcessInput<T> *>(this->inputs().at(port));
    if(inputPort)
        return inputPort->input();
    else return NULL;
}

template <class T>
void medAbstractProcess::setOutput(T* data, unsigned int port)
{
    if(port >= (unsigned int)this->outputs().size())
        return;

    medProcessOutput<T>* outputPort = reinterpret_cast<medProcessOutput<T> *>(this->outputs().at(port));
    if(outputPort)
        outputPort->setOutput(data);
}

template <class T>
T* medAbstractProcess::output(unsigned int port)
{
    if(port >= (unsigned int)this->outputs().size())
        return NULL;

    medProcessOutput<T>* outputPort = reinterpret_cast<medProcessOutput<T> *>(this->outputs().at(port));
    if(outputPort)
        return outputPort->output();
    else return NULL;
}
