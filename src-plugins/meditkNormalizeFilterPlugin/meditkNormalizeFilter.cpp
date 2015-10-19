#include "meditkNormalizeFilter.h"
#include <iostream>

// ///////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////

meditkNormalizeFilter::meditkNormalizeFilter(void)
{

}

meditkNormalizeFilter::~meditkNormalizeFilter(void)
{

}

void meditkNormalizeFilter::setImage(medAbstractImageData* image)
{
    process.setInput(image);
}

medAbstractImageData* meditkNormalizeFilter::filteredImage(void) const
{
    return res;
}

void meditkNormalizeFilter::run(void)
{
    process.update();

    res=dynamic_cast<medAbstractImageData*>(process.output());
    std::cerr<<res;
}
