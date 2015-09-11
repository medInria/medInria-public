#include "meditkDilateFilter.h"
#include <iostream>

// ///////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////

meditkDilateFilter::meditkDilateFilter(void)
{

}

meditkDilateFilter::~meditkDilateFilter(void)
{

}

void meditkDilateFilter::setRadius(double radius)
{
    process.setParameter(radius,0);
}

void meditkDilateFilter::setImage(medAbstractImageData* image)
{
    process.setInput(image);
}

medAbstractImageData* meditkDilateFilter::filteredImage(void) const
{
    return res;
}

void meditkDilateFilter::run(void)
{
    process.update();

    res=dynamic_cast<medAbstractImageData*>(process.output());
    std::cerr<<res;
}
