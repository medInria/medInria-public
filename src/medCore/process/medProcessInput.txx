/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <medProcessInput.h>

template <typename T>
medProcessInput<T>::medProcessInput(QString name, bool isOptional, T* input) : medProcessPort(name)
{
    m_isOptional = isOptional;
    m_input = input;
    m_variant.setValue(input);
}

template <typename T>
medProcessInput<T>::~medProcessInput()
{

}

template <typename T>
bool medProcessInput<T>::isOptional() const
{
    return m_isOptional;
}

template <typename T>
T* medProcessInput<T>::input() const
{
    //return m_input;
    return m_variant.value<T*>();
}

template <typename T>
QObject* medProcessInput<T>::content() const
{
    //return m_input;
    return m_variant.value<T*>();
}

template <typename T>
void medProcessInput<T>::setInput(T* input)
{
    m_input = input;
    m_variant.setValue(input);
}

template <typename T>
bool medProcessInput<T>::setContent(QObject* obj)
{
    T* data = dynamic_cast<T*>(obj);
    if(data)
    {
        this->setInput(data);
        return true;
    }
    return false;
}

template <typename T>
void medProcessInput<T>::retrieveContentFromPort(medProcessPort *otherport)
{
    medProcessInput<T> *otherInputPort = dynamic_cast<medProcessInput<T> *>(otherport);
    if(otherInputPort)
        this->setInput(otherInputPort->input());
}

//#ifdef DTK_BUILD_COMPOSER
#ifdef IN_PROGRESS
template <typename T>
medProcessInput<T>::dtkComposerTransmitter* toTransmitter()
{
    dtkComposerTransmitterReceiver<T> *transmitter = new dtkComposerTransmitterReceiver<T>();
    return transmitter;
}

template <typename T>
void medProcessInput<T>::updateFromTransmitter(dtkComposerTransmitter* transmitter)
{
    dtkComposerTransmitterReceiver<T> *receiver = dynamic_cast<dtkComposerTransmitterReceiver<T> *>(transmitter);
    if(receiver)
    {
        this->setInput(receiver->data());
    }
    return;
}
#endif

