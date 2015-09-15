#pragma once

#include <dtkCore>
#include <medCore.h>

#include "itkFiltersMedianProcess.h"

#include <medAbstractMedianFilter.h>


class meditkMedianFilter : public medAbstractMedianFilter
{
    Q_OBJECT

public:
     meditkMedianFilter(void);
    ~meditkMedianFilter(void);



public:
    virtual void setImage(medAbstractImageData* image);

public: 
    virtual medAbstractImageData* filteredImage(void) const;

public:
    void run();

private:
    itkFiltersMedianProcess process;
    medAbstractImageData* res;

};


inline medAbstractMedianFilter *meditkMedianFilterCreator(void)
{
    return new meditkMedianFilter();
}
