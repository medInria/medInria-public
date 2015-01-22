/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medProcessInput.h>
#include <medAbstractData.h>

#ifdef DTK_BUILD_COMPOSER
#include <dtkComposer/dtkComposerTransmitter.h>
#include <dtkComposer/dtkComposerTransmitterReceiver.h>
#endif


template <typename T>
class medProcessDataInput : public medProcessInput<T>
{
public:
    medProcessDataInput(QString name, bool isOptional, T* input = NULL);
    virtual ~medProcessDataInput();

public:
    medAbstractData* input() const;
    void setInput(medAbstractData* input);

    //virtual void retrieveContentFromPort(medProcessPort *otherport);

//#ifdef DTK_BUILD_COMPOSER
#ifdef IN_PROGRESS
    virtual dtkComposerTransmitter* toTransmitter();
    virtual void updateFromTransmitter(dtkComposerTransmitter* transmitter);
#endif

private:
    medAbstractData* m_input;
};

#include <medProcessDataInput.txx>
