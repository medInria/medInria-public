#ifndef medAbstractDataCollection_h__
#define medAbstractDataCollection_h__

#include "medCoreExport.h"

class medAbstractData;
class medAttachedData;
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


    /**
    * Add new data to the collection, it will be appended
    * The caller can safely delete the pointer afterwards
    * @params medAbstractData * data
    * @return void
    */
    void addData(medAbstractData* data);

    /**
    * You can only attach one instance per collection
    * the caller can safely delete the pointer afterwards
    * @params medAttachedData * attachedData
    * @return void
    */
    void setAttachData(medAttachedData* attachedData);

    /**
    * Get the attached data instance
    * @return medAttachedData* pointer to data
    */
    medAttachedData* attachedData(); 

private:
    medAbstractDataCollectionPrivate*   d;
};

#endif // medAbstractDataCollection_h__
