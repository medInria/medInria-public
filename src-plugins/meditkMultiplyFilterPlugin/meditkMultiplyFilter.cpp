#include "meditkMultiplyFilter.h"
#include <iostream>

// ///////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////

meditkMultiplyFilter::meditkMultiplyFilter(void)
{

}

meditkMultiplyFilter::~meditkMultiplyFilter(void)
{

}

void meditkMultiplyFilter::setValue(double value)
{
    process.setParameter(value,0);
}

void meditkMultiplyFilter::setImage(medAbstractImageData* image)
{
    process.setInput(image);
}

medAbstractImageData* meditkMultiplyFilter::filteredImage(void) const
{
    return res;
}

void meditkMultiplyFilter::run(void)
{
    process.update();

    res=dynamic_cast<medAbstractImageData*>(process.output());
    std::cerr<<res;
}
