#include <medAbstractRoi.h>

class medAbstractRoiPrivate
{
public:
    unsigned char orientation;
    unsigned int idSlice;
    bool isSelected;
    RoiStatistics stats;
    bool isMaster; //true when the ROI is new or has been modified (for interpolation)
};

medAbstractRoi::medAbstractRoi( dtkAbstractObject *parent )
    : dtkAbstractObject(parent)
    , d(new medAbstractRoiPrivate)
{
    d->isSelected = false;
}

medAbstractRoi::~medAbstractRoi( void )
{
    delete d;
    d = nullptr;
}

unsigned int medAbstractRoi::getIdSlice() const
{
    return d->idSlice;
}

void medAbstractRoi::setIdSlice(unsigned int idSlice)
{
    d->idSlice=idSlice;
}

unsigned char medAbstractRoi::getOrientation()
{
    return d->orientation;
}

void medAbstractRoi::setOrientation(unsigned char orientation)
{
    d->orientation=orientation;
}

bool medAbstractRoi::isSelected()
{
    return d->isSelected;
}

void medAbstractRoi::select()
{
    d->isSelected = true;
    selected();
}

void medAbstractRoi::unselect()
{
    d->isSelected = false;
}

void medAbstractRoi::setRoiStatistics(RoiStatistics s)
{
    d->stats = s;
}

void medAbstractRoi::setMasterRoi(bool value)
{
    d->isMaster = value;
    setRightColor();
}

bool medAbstractRoi::isMasterRoi() const
{
    return d->isMaster;
}
