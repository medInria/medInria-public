/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <QtCore/QObject>
#include <QString>
#include "medPacsExport.h"

class MEDPACS_EXPORT medAbstractPacsKey : public QObject
{
    Q_OBJECT

public:
   medAbstractPacsKey();
   virtual ~medAbstractPacsKey();
    
   int group;
   int elem;
   QString value;

};

