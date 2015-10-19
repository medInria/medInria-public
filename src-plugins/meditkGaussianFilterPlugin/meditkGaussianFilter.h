#pragma once

#include <dtkCore>
#include <medCore.h>

#include "itkFiltersGaussianProcess.h"

#include <medAbstractGaussianFilter.h>


class meditkGaussianFilter : public medAbstractGaussianFilter
{
    Q_OBJECT

public:
     meditkGaussianFilter(void);
    ~meditkGaussianFilter(void);



public:
    virtual void setSigma(double sigma);
    virtual void setImage(medAbstractImageData* image);

public: 
    virtual medAbstractImageData* filteredImage(void) const;

public:
    void run();

private:
    itkFiltersGaussianProcess process;
    medAbstractImageData* res;

};


inline medAbstractGaussianFilter *meditkGaussianFilterCreator(void)
{
    return new meditkGaussianFilter();
}
