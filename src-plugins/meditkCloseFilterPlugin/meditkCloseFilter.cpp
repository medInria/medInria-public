#include "meditkCloseFilter.h"
#include <iostream>

// ///////////////////////////////////////////////////////////////////
//
// ///////////////////////////////////////////////////////////////////

meditkCloseFilter::meditkCloseFilter(void)
{

}

meditkCloseFilter::~meditkCloseFilter(void)
{

}

void meditkCloseFilter::setRadius(double radius)
{
    process.setParameter(radius,0);
}

void meditkCloseFilter::setImage(medAbstractImageData* image)
{
    process.setInput(image);
}

medAbstractImageData* meditkCloseFilter::filteredImage(void) const
{
    return res;
}

void meditkCloseFilter::run(void)
{
    process.update();

    res=dynamic_cast<medAbstractImageData*>(process.output());
    std::cerr<<res;
}
