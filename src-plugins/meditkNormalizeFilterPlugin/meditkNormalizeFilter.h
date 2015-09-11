#pragma once

#include <dtkCore>
#include <medCore.h>

#include "legacy/itkFiltersNormalizeProcess.h"

#include <medAbstractNormalizeFilter.h>


class meditkNormalizeFilter : public medAbstractNormalizeFilter
{
    Q_OBJECT

public:
     meditkNormalizeFilter(void);
    ~meditkNormalizeFilter(void);



public:
    virtual void setImage(medAbstractImageData* image);

public: 
    virtual medAbstractImageData* filteredImage(void) const;

public:
    void run();

private:
    itkFiltersNormalizeProcess process;
    medAbstractImageData* res;

};


inline medAbstractNormalizeFilter *meditkNormalizeFilterCreator(void)
{
    return new meditkNormalizeFilter();
}
