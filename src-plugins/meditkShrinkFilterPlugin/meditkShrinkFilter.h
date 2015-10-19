#pragma once

#include <dtkCore>
#include <medCore.h>

#include "itkFiltersShrinkProcess.h"

#include <medAbstractShrinkFilter.h>


class meditkShrinkFilter : public medAbstractShrinkFilter
{
    Q_OBJECT

public:
     meditkShrinkFilter(void);
    ~meditkShrinkFilter(void);

public:
     void setXShrink(double xShrink);
     void setYShrink(double yShrink);
     void setZShrink(double zShrink);

     virtual void setImage(medAbstractImageData* image);

public: 
    virtual medAbstractImageData* filteredImage(void) const;

public:
    void run();

private:
    itkFiltersShrinkProcess process;
    medAbstractImageData* res;

};


inline medAbstractShrinkFilter *meditkShrinkFilterCreator(void)
{
    return new meditkShrinkFilter();
}
