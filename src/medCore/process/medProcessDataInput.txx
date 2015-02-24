///*=========================================================================

// medInria

// Copyright (c) INRIA 2013 - 2014. All rights reserved.
// See LICENSE.txt for details.

//  This software is distributed WITHOUT ANY WARRANTY; without even
//  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE.

//=========================================================================*/

//#include <medProcessDataInput.h>

//template <typename T>
//medProcessDataInput<T>::medProcessDataInput(QString name, bool isOptional, T* input) : medProcessInput<T>(name, isOptional, input)
//{
//    this->m_variant.setValue(input);
//}

//template <typename T>
//medProcessDataInput<T>::~medProcessDataInput()
//{

//}

//template <typename T>
//medAbstractData* medProcessDataInput<T>::input() const
//{
//    return this->m_variant.template value<T*>();
//}

//template <typename T>
//void medProcessDataInput<T>::setInput(medAbstractData* input)
//{
//    this->m_variant.setValue(input);
//}


////template <typename T>
////void medProcessDataInput<T>::retrieveContentFromPort(medProcessPort *otherport)
////{
////    medProcessDataInput<T> *otherInputPort = dynamic_cast<medProcessDataInput<T> *>(otherport);
////    if(otherInputPort)
////        this->setInput(otherInputPort->input());
////}

////#ifdef DTK_BUILD_COMPOSER
//#ifdef IN_PROGRESS
//template <typename T>
//medProcessDataInput<T>::dtkComposerTransmitter* toTransmitter()
//{
//    dtkComposerTransmitterReceiver<T> *transmitter = new dtkComposerTransmitterReceiver<T>();
//    return transmitter;
//}

//template <typename T>
//void medProcessDataInput<T>::updateFromTransmitter(dtkComposerTransmitter* transmitter)
//{
//    dtkComposerTransmitterReceiver<T> *receiver = dynamic_cast<dtkComposerTransmitterReceiver<T> *>(transmitter);
//    if(receiver)
//    {
//        this->setInput(receiver->data());
//    }
//    return;
//}
//#endif

