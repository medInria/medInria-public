#include "meditkSubstractFilter.h"
#include <iostream>

// ///////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////

meditkSubstractFilter::meditkSubstractFilter(void)
{

}

meditkSubstractFilter::~meditkSubstractFilter(void)
{

}

void meditkSubstractFilter::setValue(double value)
{
    process.setParameter(value,0);
}

void meditkSubstractFilter::setImage(medAbstractImageData* image)
{
    process.setInput(image);
}

medAbstractImageData* meditkSubstractFilter::filteredImage(void) const
{
    return res;
}

void meditkSubstractFilter::run(void)
{
    process.update();

    res=dynamic_cast<medAbstractImageData*>(process.output());
    std::cerr<<res;
}
