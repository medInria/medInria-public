/*
 * medQtDataImage.h
 *
 *  Created on: 4 juil. 2011 10:29:14
 *      Author: John Stark
 */

#pragma once

#include "medTestExport.h"

#include <medCore/medAbstractDataImage.h>

class medQtDataImagePrivate;

/** A concrete implementation of a dtkAbstractDataImage for use in testing. */
class MEDTEST_EXPORT medQtDataImage : public medAbstractDataImage
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

    virtual       QImage & thumbnail(void)  ;
    virtual QList<QImage>& thumbnails(void) ;

    // Implement dtkAbstractDataImage
    void *image(void);

    virtual int   Dimension(void) const;
    virtual const PixId& PixelType(void) const;

    virtual int xDimension(void) const;
    virtual int yDimension(void) const;
    virtual int zDimension(void) const;
    virtual int tDimension(void) const;

    virtual qreal xSpacing(void) const;
    virtual qreal ySpacing(void) const;
    virtual qreal zSpacing(void) const;
    virtual qreal tSpacing(void) const;

    virtual int minRangeValue(void);
    virtual int maxRangeValue(void);

    virtual int scalarValueCount(int value);
    virtual int scalarValueMinCount(void);
    virtual int scalarValueMaxCount(void);
public:
protected:
    void generateThumbnails();
private:

    medQtDataImagePrivate * d;
};



