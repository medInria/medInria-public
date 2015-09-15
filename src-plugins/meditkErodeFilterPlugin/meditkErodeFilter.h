#pragma once

#include <dtkCore>
#include <medCore.h>

#include "itkFiltersErodeProcess.h"

#include <medAbstractErodeFilter.h>


class meditkErodeFilter : public medAbstractErodeFilter
{
    Q_OBJECT

public:
     meditkErodeFilter(void);
    ~meditkErodeFilter(void);



public:
    virtual void setRadius(double radius);
    virtual void setImage(medAbstractImageData* image);

public: 
    virtual medAbstractImageData* filteredImage(void) const;

public:
    void run();

private:
    itkFiltersErodeProcess process;
    medAbstractImageData* res;

};


inline medAbstractErodeFilter *meditkErodeFilterCreator(void)
{
    return new meditkErodeFilter();
}
