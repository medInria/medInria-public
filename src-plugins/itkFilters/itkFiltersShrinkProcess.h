/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#ifndef ITKFILTERSSHRINKPROCESS_H
#define ITKFILTERSSHRINKPROCESS_H

#include "itkFiltersProcessBase.h"
#include <dtkCore/dtkAbstractData.h>

#include "itkFiltersPluginExport.h"

class itkFiltersShrinkProcessPrivate;
class dtkAbstractData;

class ITKFILTERSPLUGIN_EXPORT itkFiltersShrinkProcess : public itkFiltersProcessBase
{
    Q_OBJECT
    
public:
    itkFiltersShrinkProcess(itkFiltersShrinkProcess * parent = 0);
    itkFiltersShrinkProcess(const itkFiltersShrinkProcess& other);
    virtual ~itkFiltersShrinkProcess(void);

    static bool registered ( void );
    
public slots:

    void setParameter ( double  data, int channel );
    int update ( void );

private:
    DTK_DECLARE_PRIVATE(itkFiltersShrinkProcess)
};

dtkAbstractProcess * createitkFiltersShrinkProcess(void);

#endif // ITKFILTERSSHRINKPROCESS_H
