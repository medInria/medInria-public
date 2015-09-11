#pragma once

#include <dtkCore>
#include <medCore.h>

#include "legacy/itkFiltersOpenProcess.h"

#include <medAbstractOpenFilter.h>


class meditkOpenFilter : public medAbstractOpenFilter
{
    Q_OBJECT

public:
     meditkOpenFilter(void);
    ~meditkOpenFilter(void);



public:
    virtual void setRadius(double radius);
    virtual void setImage(medAbstractImageData* image);

public: 
    virtual medAbstractImageData* filteredImage(void) const;

public:
    void run();

private:
    itkFiltersOpenProcess process;
    medAbstractImageData* res;

};


inline medAbstractOpenFilter *meditkOpenFilterCreator(void)
{
    return new meditkOpenFilter();
}
