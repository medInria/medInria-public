#ifndef MEDMESHABSTRACTVIEWINTERACTOR_H
#define MEDMESHABSTRACTVIEWINTERACTOR_H

#include <dtkCore/dtkAbstractViewInteractor.h>

#include "medCoreExport.h"

class MEDCORE_EXPORT medMeshAbstractViewInteractor: public dtkAbstractViewInteractor
{
    Q_OBJECT
public:
  medMeshAbstractViewInteractor(){};
  virtual ~medMeshAbstractViewInteractor(){};
  virtual char* getLUTQuery (int meshLayer)=0;
  
 
  
protected:

 signals:

public slots:
    virtual void onColorPropertySet (const QColor& color, int meshLayer)=0;
   

private:

};

#endif // MED4DABSTRACTVIEWINTERACTOR_H