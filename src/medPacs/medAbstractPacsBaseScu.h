#ifndef MEDABSTRACTPACSBASESCU_H
#define MEDABSTRACTPACSBASESCU_H

#include <QtCore/QObject>
#include <QVector>
#include "medPacsExport.h"

class medAbstractPacsNode;

class MEDPACS_EXPORT medAbstractPacsBaseScu : public QObject
{
    Q_OBJECT
    
public:
             medAbstractPacsBaseScu();
    virtual ~medAbstractPacsBaseScu();

   /**
    * Enum defining the query level (search hierarchy)
    */
    enum eQueryLevel{
            PATIENT,
            STUDY,
            SERIES,
            IMAGE};

    virtual void setQueryLevel(eQueryLevel level);

    virtual bool addQueryAttribute(int group, int elem, const char* value);

    virtual void clearAllQueryAttributes();

    virtual QVector<medAbstractPacsNode*> getNodeContainer();

};

#endif
