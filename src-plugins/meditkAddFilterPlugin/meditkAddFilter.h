#pragma once

#include <dtkCore>
#include <medCore.h>

#include "legacy/itkFiltersAddProcess.h"

#include <medAbstractAddFilter.h>


class meditkAddFilter : public medAbstractAddFilter
{
    Q_OBJECT

public:
     meditkAddFilter(void);
    ~meditkAddFilter(void);



public:
    virtual void setValue(double value);
    virtual void setImage(medAbstractImageData* image);

public: 
    virtual medAbstractImageData* filteredImage(void) const;

public:
    void run();

private:
    itkFiltersAddProcess process;
    medAbstractImageData* res;

};


inline medAbstractAddFilter *meditkAddFilterCreator(void)
{
    return new meditkAddFilter();
}
