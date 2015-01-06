/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medProcessPort.h>

#ifdef DTK_BUILD_COMPOSER
#include <dtkComposer/dtkComposerTransmitter.h>
#include <dtkComposer/dtkComposerTransmitterReceiver.h>
#endif


template <typename T>
class medProcessInput : public medProcessPort
{
public:
    medProcessInput(QString name, bool isOptional, T* input = NULL);
    virtual ~medProcessInput();

public:
    bool isOptional() const;

    T* input() const;
    void setInput(T* input);

    virtual void retrieveData(medProcessPort *otherport);

//#ifdef DTK_BUILD_COMPOSER
#ifdef IN_PROGRESS
    virtual dtkComposerTransmitter* toTransmitter();
    virtual void updateFromTransmitter(dtkComposerTransmitter* transmitter);
#endif

private:
    bool m_isOptional;
    T* m_input;
};

#include <medProcessInput.txx>
