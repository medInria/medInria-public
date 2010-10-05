#ifndef MEDABSTRACTPACSKEY_H
#define MEDABSTRACTPACSKEY_H

#include <QtCore/QObject>
#include <QString>
#include "medPacsExport.h"

class MEDPACS_EXPORT medAbstractPacsKey : public QObject
{
    Q_OBJECT

public:
   medAbstractPacsKey(void);
   virtual ~medAbstractPacsKey(void);
    
   int group;
   int elem;
   QString value;

};

#endif