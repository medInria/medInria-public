#include "meditkAddFilter.h"
#include <iostream>

// ///////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////

meditkAddFilter::meditkAddFilter(void)
{

}

meditkAddFilter::~meditkAddFilter(void)
{

}

void meditkAddFilter::setValue(double value)
{
    process.setParameter(value,0);
}

void meditkAddFilter::setImage(medAbstractImageData* image)
{
    process.setInput(image);
}

medAbstractImageData* meditkAddFilter::filteredImage(void) const
{
    return res;
}

void meditkAddFilter::run(void)
{
    process.update();

    res=dynamic_cast<medAbstractImageData*>(process.output());
    std::cerr<<res;
}
