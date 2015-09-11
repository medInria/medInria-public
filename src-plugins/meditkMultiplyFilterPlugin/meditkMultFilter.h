#pragma once

#include <dtkCore>
#include <medCore.h>

#include "legacy/itkFiltersMultiplyProcess.h"

#include <medAbstractMultFilter.h>


class meditkMultFilter : public medAbstractMultFilter
{
    Q_OBJECT

public:
     meditkMultFilter(void);
    ~meditkMultFilter(void);



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


inline medAbstractMultFilter *meditkMultFilterCreator(void)
{
    return new meditkMultFilter();
}
