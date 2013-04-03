#ifndef MED4DABSTRACTVIEWINTERACTOR_H
#define MED4DABSTRACTVIEWINTERACTOR_H

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

#endif // MED4DABSTRACTVIEWINTERACTOR_H
