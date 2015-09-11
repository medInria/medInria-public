#include "meditkSubFilter.h"
#include <iostream>

// ///////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////

meditkSubFilter::meditkSubFilter(void)
{

}

meditkSubFilter::~meditkSubFilter(void)
{

}

void meditkSubFilter::setValue(double value)
{
    process.setParameter(value,0);
}

void meditkSubFilter::setImage(medAbstractImageData* image)
{
    process.setInput(image);
}

medAbstractImageData* meditkSubFilter::filteredImage(void) const
{
    return res;
}

void meditkSubFilter::run(void)
{
    process.update();

    res=dynamic_cast<medAbstractImageData*>(process.output());
    std::cerr<<res;
}
