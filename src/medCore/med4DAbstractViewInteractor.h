/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkAbstractViewInteractor.h>

#include "medCoreExport.h"

class MEDCORE_EXPORT med4DAbstractViewInteractor: public dtkAbstractViewInteractor
{
    Q_OBJECT
public:
  med4DAbstractViewInteractor(){};
  virtual ~med4DAbstractViewInteractor(){};

  virtual void sequencesRange(double* range) = 0;
  virtual double sequencesMinTimeStep() = 0;
  virtual void setCurrentTime (double time) = 0;
  
protected:

 signals:

public slots:
private:

};


