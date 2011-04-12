#ifndef medAbstractDataCollection_h__
#define medAbstractDataCollection_h__

#include "medCoreExport.h"

class medAbstractData;
class medAbstractDataCollectionPrivate;

/**
 * Collection type of medAbstractData
 * using qVector internally
 */
class MEDCORE_EXPORT medAbstractDataCollection
{
public:
    medAbstractDataCollection();
    ~medAbstractDataCollection();

    /**
     * Returns the number of medAbstractData items currently collected
     */
    int count();

    /**
     * Get the first item in the container or null
     * rewinds the iterator
     */
    medAbstractData* first();

    /**
     * Get the next item or null 
     * increments the iterator
     */
    medAbstractData* next();

    /**
     * Return item at index or null
     */
    medAbstractData* at(int index);


private:
    medAbstractDataCollectionPrivate*   d;
};

#endif // medAbstractDataCollection_h__
