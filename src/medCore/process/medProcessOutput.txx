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
    m_output = output;
}

template <typename T>
medProcessOutput<T>::~medProcessOutput()
{

}

template <typename T>
T* medProcessOutput<T>::output() const
{
    return m_output;
}


template <typename T>
void medProcessOutput<T>::setOutput(T* output)
{
    m_output = output;
}
