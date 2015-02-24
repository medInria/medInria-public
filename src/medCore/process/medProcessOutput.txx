/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include "medProcessOutput.h"


template <typename T>
medProcessOutput<T>::medProcessOutput(QString name, T* output) : medProcessPort(name)
{
    //m_output = output;
    m_variant.setValue(output);
}

template <typename T>
medProcessOutput<T>::~medProcessOutput()
{

}

template <typename T>
T* medProcessOutput<T>::output() const
{
    return m_variant.value<T*>();
}


template <typename T>
void medProcessOutput<T>::setOutput(T* output)
{
    m_variant.setValue(output);
}

template <typename T>
QObject* medProcessOutput<T>::content() const
{
    return m_variant.value<T*>();
}

template <typename T>
bool medProcessOutput<T>::setContent(QObject* obj)
{
    T* data = dynamic_cast<T*>(obj);
    if(data)
    {
        setOutput(data);
        return true;
    }
    else
        return false;
}
