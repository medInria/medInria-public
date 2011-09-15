/*
 * medQtDataImage.h
 *
 *  Created on: 4 juil. 2011 10:29:14
 *      Author: John Stark
 */

#ifndef MEDQTDATAIMAGE_H_
#define MEDQTDATAIMAGE_H_

#include "medTestCommonExport.h"

#include <medCore/medAbstractDataImage.h>

class medQtDataImagePrivate;

/** A concrete implementation of a dtkAbstractDataImage for use in testing. */
class MEDTESTCOMMON_EXPORT medQtDataImage : public medAbstractDataImage
{
    Q_OBJECT;
public:
    medQtDataImage();
    virtual ~medQtDataImage();
    virtual QString description(void) const;
    virtual QString identifier(void) const;

    static QString s_description();
    static QString s_identifier();
    static bool registered();

    // Implement dtkAbstractData
    virtual void setData(void* data);
    virtual void setData(void* data, int channel);
    virtual void *output(void);
    virtual void *data(void);
    virtual void *data(int channel);

    virtual       QImage & thumbnail(void)  const;
    virtual QList<QImage>& thumbnails(void) const;

    // Implement dtkAbstractDataImage
    void *image(void);

    virtual int   Dimension(void) const;
    virtual const PixId& PixelType(void) const;

    virtual int xDimension(void);
    virtual int yDimension(void);
    virtual int zDimension(void);
    virtual int tDimension(void);

    virtual int minRangeValue(void);
    virtual int maxRangeValue(void);

    virtual int scalarValueCount(int value);
    virtual int scalarValueMinCount(void);
    virtual int scalarValueMaxCount(void);
public:
protected:
    void generateThumbnails() const;
private:

    medQtDataImagePrivate * d;
};

#endif /* MEDQTDATAIMAGE_H_ */

