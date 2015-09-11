#pragma once

#include <dtkCore>
#include <medCore.h>

#include "legacy/itkFiltersDivideideProcess.h"

#include <medAbstractDivideFilter.h>


class meditkDivideFilter : public medAbstractDivideFilter
{
    Q_OBJECT

public:
     meditkDivideFilter(void);
    ~meditkDivideFilter(void);



public:
    virtual void setValue(double value);
    virtual void setImage(medAbstractImageData* image);

public: 
    virtual medAbstractImageData* filteredImage(void) const;

public:
    void run();

private:
    itkFiltersDivideideProcess process;
    medAbstractImageData* res;

};


inline medAbstractDivideFilter *meditkDivideFilterCreator(void)
{
    return new meditkDivideFilter();
}
