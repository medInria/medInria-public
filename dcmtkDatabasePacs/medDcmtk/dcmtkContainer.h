
#ifndef DCMTKCONTAINER_H
#define DCMTKCONTAINER_H

#include <vector>
#include <algorithm>

/**
* @class dcmtkContainer
* @author Michael Knopke
* @brief Wrapps a std::vector and std::iterator to ease the use of container data
* 
*/
template <class T>
class dcmtkContainer
{
public:

    /**
    * destructor needs to free elements in data container
    */
    ~dcmtkContainer();

    /**
    * getFirst element in vector and rewind iterator to begin()
    * @return   Template instance
    */
    T getFirst();

    /**
    * check if iterator is pointing to element
    * @return   true if iterator points to element
    */
    bool isValid();

    /**
    * add element to vector (will be deleted automatically)
    * @params: T const & d add a new instance to vector
    */
    void add(T const &d);

    /**
    * getNext element in vector or return NULL (check isValid before accessing)
    * @return   T instance pointer
    */
    T getNext();

    /**
    * getAtPos returns the element (pointer) at the position
    * @params: pos Position(index) in vector
    * @return   T instance pointer
    */
    T getAtPos(unsigned int pos);

    /**
    * clear the vector and clean up mem alloc
    */
    void clear();

    /**
    * return number of elements in the vector
    * @return   int size
    */
    int size();

    /**
    * get the index number at the current iterator position
    * @return   int index number
    */
    int index();

protected:

    int             m_index;
    std::vector<T>  data;
    typename std::vector<T>::const_iterator iter;
};


#include "dcmtkContainer.txx"

#endif //DCMTKCONTAINER_H
