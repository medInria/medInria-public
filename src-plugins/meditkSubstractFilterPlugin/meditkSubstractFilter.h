#pragma once

#include <dtkCore>
#include <medCore.h>

#include "itkFiltersSubtractProcess.h"

#include <medAbstractSubstractFilter.h>


class meditkSubstractFilter : public medAbstractSubstractFilter
{
    Q_OBJECT

public:
     meditkSubstractFilter(void);
    ~meditkSubstractFilter(void);



public:
    virtual void setValue(double value);
    virtual void setImage(medAbstractImageData* image);

public: 
    virtual medAbstractImageData* filteredImage(void) const;

public:
    void run();

private:
    itkFiltersSubtractProcess process;
    medAbstractImageData* res;

};


inline medAbstractSubstractFilter *meditkSubstractFilterCreator(void)
{
    return new meditkSubstractFilter();
}
