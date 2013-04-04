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

