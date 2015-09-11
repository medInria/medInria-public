#pragma once

#include <dtkCore>
#include <medCore.h>

#include "legacy/itkFiltersSubstractProcess.h"

#include <medAbstractSubFilter.h>


class meditkSubFilter : public medAbstractSubFilter
{
    Q_OBJECT

public:
     meditkSubFilter(void);
    ~meditkSubFilter(void);



public:
    virtual void setValue(double value);
    virtual void setData(medAbstractImageData* data);

public: 
    virtual medAbstractImageData* filteredImage(void) const;

public:
    void run();

private:
    itkFiltersSubstractProcess process;
    medAbstractImageData* res;

};


inline medAbstractSubFilter *meditkSubFilterCreator(void)
{
    return new meditkSubFilter();
}
