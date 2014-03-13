/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkAbstractProcess.h>
#include <medAbstractData.h>

#include <medCoreExport.h>

class medAbstractProcessPrivate;



/**
 * Extending dtkAbstractProcess class to hold more specific information
 */
class MEDCORE_EXPORT medAbstractProcess : public dtkAbstractProcess
{
    Q_OBJECT

public:
    medAbstractProcess( medAbstractProcess * parent = NULL );
    virtual ~medAbstractProcess();

public slots:
    virtual medAbstractData *output(void);


    //TODO: to complete hiding dtkAbstractProcess API
private slots:
    virtual dtkAbstractData *output(int channel){return 0;}
    virtual dtkAbstractData *output(int channel, int frame){return 0;}


private:
    medAbstractProcessPrivate* d;

};


