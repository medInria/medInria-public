/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkAbstractViewInteractor.h>
#include <medCoreExport.h>

/**
 * @class medSHAbstractViewInteractor
 * @author Jaime Garcia
 * @brief Abstract base class for Spherical Harmonics visualization view interactor. Somehow works like te med4DAbstractViewInteractor
 *
 */
class MEDCORE_EXPORT medSHAbstractViewInteractor: public dtkAbstractViewInteractor
{
    Q_OBJECT
public:
  medSHAbstractViewInteractor(){};
  virtual ~medSHAbstractViewInteractor(){};

  /** Virtual class to be implemented at the interactor to get the image size it is used to set med gui slider appropiate size*/
  virtual void imageSize(int* imageSize) = 0;

protected:

 signals:

public slots:
private:

};
