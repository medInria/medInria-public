#include "meditkErodeFilter.h"
#include <iostream>

// ///////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////

meditkErodeFilter::meditkErodeFilter(void)
{

}

meditkErodeFilter::~meditkErodeFilter(void)
{

}

void meditkErodeFilter::setRadius(double radius)
{
    process.setParameter(radius,0);
}

void meditkErodeFilter::setImage(medAbstractImageData* image)
{
    process.setInput(image);
}

medAbstractImageData* meditkErodeFilter::filteredImage(void) const
{
    return res;
}

void meditkErodeFilter::run(void)
{
    process.update();

    res=dynamic_cast<medAbstractImageData*>(process.output());
    std::cerr<<res;
}
