#pragma once

#include <dtkCore>
#include <medCore.h>

#include "itkFiltersMultiplyProcess.h"

#include <medAbstractMultiplyFilter.h>


class meditkMultiplyFilter : public medAbstractMultiplyFilter
{
    Q_OBJECT

public:
     meditkMultiplyFilter(void);
    ~meditkMultiplyFilter(void);



public:
    virtual void setValue(double value);
    virtual void setImage(medAbstractImageData* image);

public: 
    virtual medAbstractImageData* filteredImage(void) const;

public:
    void run();

private:
    itkFiltersMultiplyProcess process;
    medAbstractImageData* res;

};


inline medAbstractMultiplyFilter *meditkMultiplyFilterCreator(void)
{
    return new meditkMultiplyFilter();
}
