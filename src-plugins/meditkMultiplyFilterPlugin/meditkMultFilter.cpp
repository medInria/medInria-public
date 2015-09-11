#include "meditkMultFilter.h"
#include <iostream>

// ///////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////

meditkMultFilter::meditkMultFilter(void)
{

}

meditkMultFilter::~meditkMultFilter(void)
{

}

void meditkMultFilter::setValue(double value)
{
    process.setParameter(value,0);
}

void meditkMultFilter::setImage(medAbstractImageData* image)
{
    process.setInput(image);
}

medAbstractImageData* meditkMultFilter::filteredImage(void) const
{
    return res;
}

void meditkMultFilter::run(void)
{
    process.update();

    res=dynamic_cast<medAbstractImageData*>(process.output());
    std::cerr<<res;
}
