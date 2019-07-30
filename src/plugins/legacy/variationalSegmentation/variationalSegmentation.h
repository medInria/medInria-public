/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2019. All rights reserved.
 See LICENSE.txt for details.

  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/
#ifndef VARIATIONALSEGMENTATION_H
#define VARIATIONALSEGMENTATION_H

#include <dtkCoreSupport/dtkAbstractProcess.h>

#include <variationalSegmentationPluginExport.h>

class VARIATIONALSEGMENTATIONPLUGIN_EXPORT variationalSegmentation : public dtkAbstractProcess
{
    Q_OBJECT
    
public:
    variationalSegmentation();

    virtual QString description() const;
    
    static bool registered();
};

dtkAbstractProcess *createVariationalSegmentation();

#endif
