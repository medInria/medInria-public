#include "meditkOpenFilter.h"
#include <iostream>

// ///////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////

meditkOpenFilter::meditkOpenFilter(void)
{

}

meditkOpenFilter::~meditkOpenFilter(void)
{

}

void meditkOpenFilter::setRadius(double radius)
{
    process.setParameter(radius,0);
}

void meditkOpenFilter::setImage(medAbstractImageData* image)
{
    process.setInput(image);
}

medAbstractImageData* meditkOpenFilter::filteredImage(void) const
{
    return res;
}

void meditkOpenFilter::run(void)
{
    process.update();

    res=dynamic_cast<medAbstractImageData*>(process.output());
    std::cerr<<res;
}
