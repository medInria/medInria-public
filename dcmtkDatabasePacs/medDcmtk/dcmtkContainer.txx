#ifndef DCMTKCONTAINER_TXX
#define DCMTKCONTAINER_TXX

#include "dcmtkContainer.h"

//---------------------------------------------------------------------------------------------

template <typename T>
T dcmtkContainer<T>::getFirst()
{
    m_index = 0;
    iter = data.begin();
    if ( data.empty()) return NULL;
    else return *iter;
}

//---------------------------------------------------------------------------------------------

template <typename T>
bool dcmtkContainer<T>::isValid()
{
    if ((iter) != data.end()) 
        return true;
    else 
        return false;
}

//---------------------------------------------------------------------------------------------

template <class T>
dcmtkContainer<T>::~dcmtkContainer()
{
    this->clear();
}

//---------------------------------------------------------------------------------------------

template <typename T>
void dcmtkContainer<T>::add( T const &d )
{
    data.push_back(d);
}

//---------------------------------------------------------------------------------------------

template <typename T>
T dcmtkContainer<T>::getNext()
{
    iter++;
    if ( iter != data.end() )
    {
        m_index++;
        return (*iter);
    }
    else
        return NULL;
}

//---------------------------------------------------------------------------------------------

template <typename T>
T dcmtkContainer<T>::getAtPos( unsigned int pos )
{
    if (pos < data.size())
        return data.at(pos);
    else
        return NULL;
}

//---------------------------------------------------------------------------------------------

template <typename T>
void dcmtkContainer<T>::clear()
{
    for (iter = data.begin(); iter != data.end(); iter++)
    {
        delete (*iter);
    }
    data.clear();
}

//---------------------------------------------------------------------------------------------

template <typename T>
int dcmtkContainer<T>::size()
{
    return data.size();
}

//---------------------------------------------------------------------------------------------

template <typename T>
int dcmtkContainer<T>::index()
{
    return m_index;
}

//---------------------------------------------------------------------------------------------

#endif DCMTKCONTAINER_TXX