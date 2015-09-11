#include "meditkShrinkFilter.h"
#include <iostream>

// ///////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////

meditkShrinkFilter::meditkShrinkFilter(void)
{

}

meditkShrinkFilter::~meditkShrinkFilter(void)
{

}

void meditkShrinkFilter::setXShrink(double xShrink)
{
    process.setParameter(xShrink,0);
}

void meditkShrinkFilter::setYShrink(double yShrink)
{
    process.setParameter(yShrink,1);
}

void meditkShrinkFilter::setZShrink(double zShrink)
{
    process.setParameter(zShrink,2);
}

void meditkShrinkFilter::setImage(medAbstractImageData* image)
{
    process.setInput(image);
}

medAbstractImageData* meditkShrinkFilter::filteredImage(void) const
{
    return res;
}

void meditkShrinkFilter::run(void)
{
    process.update();

    res=dynamic_cast<medAbstractImageData*>(process.output());
    std::cerr<<res;
}
