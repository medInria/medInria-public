#include "meditkInvertFilter.h"
#include <iostream>

// ///////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////

meditkInvertFilter::meditkInvertFilter(void)
{

}

meditkInvertFilter::~meditkInvertFilter(void)
{

}

void meditkInvertFilter::setImage(medAbstractImageData* image)
{
    process.setInput(image);
}

medAbstractImageData* meditkInvertFilter::filteredImage(void) const
{
    return res;
}

void meditkInvertFilter::run(void)
{
    process.update();

    res=dynamic_cast<medAbstractImageData*>(process.output());
    std::cerr<<res;
}
