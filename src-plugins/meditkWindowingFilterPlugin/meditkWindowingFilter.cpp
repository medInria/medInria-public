#include "meditkWindowingFilter.h"
#include <iostream>

// ///////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////

meditkWindowingFilter::meditkWindowingFilter(void)
{

}

meditkWindowingFilter::~meditkWindowingFilter(void)
{

}

void meditkWindowingFilter::setMinimumIntensityValue(double value)
{
    process.setParameter(value,0);
}

void meditkWindowingFilter::setMaximumIntensityValue(double value)
{
    process.setParameter(value,1);
}

void meditkWindowingFilter::setMinimumOutputIntensityValue(double value)
{
    process.setParameter(value,2);
}

void meditkWindowingFilter::setMaximumOutputIntensityValue(double value)
{
    process.setParameter(value,3);
}

void meditkWindowingFilter::setImage(medAbstractImageData* image)
{
    process.setInput(image);
}

medAbstractImageData* meditkWindowingFilter::filteredImage(void) const
{
    return res;
}

void meditkWindowingFilter::run(void)
{
    process.update();

    res=dynamic_cast<medAbstractImageData*>(process.output());
    std::cerr<<res;
}
