#ifndef MEDSHABSTRACTVIEWINTERACTOR_H
#define MEDSHABSTRACTVIEWINTERACTOR_H

#include <dtkCore/dtkAbstractViewInteractor.h>

#include "medCoreExport.h"


/**
 * @class medShAbstractViewInteractor
 * @author Jaime Garcia
 * @brief Abstract base class for Spherical Harmonics visualization view interactor. Somehow works like te med4DAbstractViewInteractor
 *
 */
class MEDCORE_EXPORT medShAbstractViewInteractor: public dtkAbstractViewInteractor
{
    Q_OBJECT
public:
  medShAbstractViewInteractor(){};
  virtual ~medShAbstractViewInteractor(){};

  /** Virtual class to be implemented at the interactor to get the image size it is used to set med gui slider appropiate size*/
  virtual void imageSize(int* imageSize) = 0;

protected:

 signals:

public slots:
private:

};

#endif // MEDSHABSTRACTVIEWINTERACTOR_H
