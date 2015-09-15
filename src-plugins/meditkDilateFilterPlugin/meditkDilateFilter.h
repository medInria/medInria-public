#pragma once

#include <dtkCore>
#include <medCore.h>

#include "itkFiltersDilateProcess.h"

#include <medAbstractDilateFilter.h>


class meditkDilateFilter : public medAbstractDilateFilter
{
    Q_OBJECT

public:
     meditkDilateFilter(void);
    ~meditkDilateFilter(void);



public:
    virtual void setRadius(double radius);
    virtual void setImage(medAbstractImageData* image);

public: 
    virtual medAbstractImageData* filteredImage(void) const;

public:
    void run();

private:
    itkFiltersDilateProcess process;
    medAbstractImageData* res;

};


inline medAbstractDilateFilter *meditkDilateFilterCreator(void)
{
    return new meditkDilateFilter();
}
