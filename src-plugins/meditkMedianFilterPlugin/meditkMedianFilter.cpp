#include "meditkMedianFilter.h"
#include <iostream>

// ///////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////

meditkMedianFilter::meditkMedianFilter(void)
{

}

meditkMedianFilter::~meditkMedianFilter(void)
{

}

void meditkMedianFilter::setImage(medAbstractImageData* image)
{
    process.setInput(image);
}

medAbstractImageData* meditkMedianFilter::filteredImage(void) const
{
    return res;
}

void meditkMedianFilter::run(void)
{
    process.update();

    res=dynamic_cast<medAbstractImageData*>(process.output());
    std::cerr<<res;
}
