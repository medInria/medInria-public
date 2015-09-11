#pragma once

#include <dtkCore>
#include <medCore.h>

#include "legacy/itkFiltersCloseProcess.h"

#include <medAbstractCloseFilter.h>


class meditkCloseFilter : public medAbstractCloseFilter
{
    Q_OBJECT

public:
     meditkCloseFilter(void);
    ~meditkCloseFilter(void);



public:
    virtual void setRadius(double radius);
    virtual void setImage(medAbstractImageData* image);

public: 
    virtual medAbstractImageData* filteredImage(void) const;

public:
    void run();

private:
    itkFiltersCloseProcess process;
    medAbstractImageData* res;

};


inline medAbstractCloseFilter *meditkCloseFilterCreator(void)
{
    return new meditkCloseFilter();
}
