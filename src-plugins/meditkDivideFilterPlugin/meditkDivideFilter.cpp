#include "meditkDivideFilter.h"
#include <iostream>

// ///////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////

meditkDivideFilter::meditkDivideFilter(void)
{

}

meditkDivideFilter::~meditkDivideFilter(void)
{

}

void meditkDivideFilter::setValue(double value)
{
    process.setParameter(value,0);
}

void meditkDivideFilter::setImage(medAbstractImageData* image)
{
    process.setInput(image);
}

medAbstractImageData* meditkDivideFilter::filteredImage(void) const
{
    return res;
}

void meditkDivideFilter::run(void)
{
    process.update();

    res=dynamic_cast<medAbstractImageData*>(process.output());
    std::cerr<<res;
}
