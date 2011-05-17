#ifndef _itkDataImageBaseImpl_h
#define _itkDataImageBaseImpl_h

#include <QImage>
#include <QList>

/**
 * Pure abstract base class for the template implementation of the itkDataImage classes.
 **/
class itkDataImageBaseImpl {
public:
    virtual ~itkDataImageBaseImpl();

    virtual QImage        &thumbnail(void) = 0;
    virtual QList<QImage> &thumbnails (void) = 0;

public:
    // derived from dtkAbstractData

    virtual void *output(void) = 0;
    virtual void *data(void) = 0;
    virtual void setData(void* data) = 0;
    virtual void update(void) = 0;

public:
    // derived from dtkAbstractDataImage

    virtual int xDimension(void) = 0;
    virtual int yDimension(void) = 0;
    virtual int zDimension(void) = 0;
    virtual int tDimension(void) = 0;

    virtual int minRangeValue(void) = 0;
    virtual int maxRangeValue(void) = 0;

    virtual int scalarValueCount(int value) = 0;
    virtual int scalarValueMinCount(void) = 0;
    virtual int scalarValueMaxCount(void) = 0;
};


#endif //_itkDataImageBaseImpl_h
