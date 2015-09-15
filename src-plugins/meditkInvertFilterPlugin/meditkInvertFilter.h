#pragma once

#include <dtkCore>
#include <medCore.h>

#include "itkFiltersInvertProcess.h"

#include <medAbstractInvertFilter.h>


class meditkInvertFilter : public medAbstractInvertFilter
{
    Q_OBJECT

public:
     meditkInvertFilter(void);
    ~meditkInvertFilter(void);



public:
    virtual void setImage(medAbstractImageData* image);

public: 
    virtual medAbstractImageData* filteredImage(void) const;

public:
    void run();

private:
    itkFiltersInvertProcess process;
    medAbstractImageData* res;

};


inline medAbstractInvertFilter *meditkInvertFilterCreator(void)
{
    return new meditkInvertFilter();
}
