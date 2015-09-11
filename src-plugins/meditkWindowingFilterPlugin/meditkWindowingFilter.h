#pragma once

#include <dtkCore>
#include <medCore.h>

#include "legacy/itkFiltersWindowingProcess.h"

#include <medAbstractWindowingFilter.h>


class meditkWindowingFilter : public medAbstractWindowingFilter
{
    Q_OBJECT

public:
     meditkWindowingFilter(void);
    ~meditkWindowingFilter(void);



public:

    virtual void setMinimumIntensityValue(double value);
    virtual void setMaximumIntensityValue(double value);
    virtual void setMinimumOutputIntensityValue(double value);
    virtual void setMaximumOutputIntensityValue(double value);

    virtual void setImage(medAbstractImageData* image);

public: 
    virtual medAbstractImageData* filteredImage(void) const;

public:
    void run();

private:
    itkFiltersWindowingProcess process;
    medAbstractImageData* res;

};


inline medAbstractWindowingFilter *meditkWindowingFilterCreator(void)
{
    return new meditkWindowingFilter();
}
