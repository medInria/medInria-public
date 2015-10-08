/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <dtkCore/dtkAbstractDataReader.h>

#include <%1PluginExport.h>

class %1Private;
    
class %2PLUGIN_EXPORT %1 : public dtkAbstractDataReader
{
    Q_OBJECT
    
public:
    %1();
    virtual ~%1();
    
    virtual QString identifier()  const;
    virtual QString description() const;
    virtual QStringList handled() const;
    
    static bool registered();
    
private:
    %1Private *d;
};

dtkAbstractDataReader *create();


