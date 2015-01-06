/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QString>

#ifdef DTK_BUILD_COMPOSER
#include <dtkComposer/dtkComposerTransmitter.h>
#include <dtkComposer/dtkComposerTransmitterReceiver.h>
#endif

class medProcessPort
{
public:
    medProcessPort(QString name) {m_name = name;}
    virtual ~medProcessPort(){}

public:
    QString name() const {return m_name;}

    virtual void retrieveData(medProcessPort *port) {}

    /////////////////////////////////////////////////////
    //
    // TODO: Sould we consider using dtkComposerTransmitter* directly??
    // or find another way to link process input/output and composer port ??
    //
    /////////////////////////////////////////////////////
    //#ifdef DTK_BUILD_COMPOSER
#ifdef IN_PROGRESS
    virtual dtkComposerTransmitter* toTransmitter()
    {
        return NULL;
    }
    virtual void updateFromTransmitter(dtkComposerTransmitter* )
    {
        return;
    }
#endif

private:
    QString m_name;
};
