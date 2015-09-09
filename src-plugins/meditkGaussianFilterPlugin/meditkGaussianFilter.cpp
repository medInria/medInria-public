#include "meditkGaussianFilter.h"
#include <iostream>

// ///////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////

meditkGaussianFilter::meditkGaussianFilter(void)
{

}

meditkGaussianFilter::~meditkGaussianFilter(void)
{

}

void meditkGaussianFilter::setSigma(double sigma)
{
    process.setParameter(sigma,0);
}

void meditkGaussianFilter::setData(medAbstractImageData* data)
{
    process.setInput(data);
}

medAbstractImageData* meditkGaussianFilter::filteredImage(void) const
{
    return res;
}

void meditkGaussianFilter::run(void)
{
    process.update();

    res=dynamic_cast<medAbstractImageData*>(process.output());
    std::cerr<<res;
}
