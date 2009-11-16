/* medSeriesPreview.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Thu Oct  8 19:47:43 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Tue Nov  3 09:32:59 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 7
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medSeriesPreview.h"

class medSeriesPreviewPrivate
{
public:
};

medSeriesPreview::medSeriesPreview(QWidget *parent) : QFrame(parent), d(new medSeriesPreviewPrivate)
{

}

medSeriesPreview::~medSeriesPreview(void)
{
    delete d;

    d = NULL;
}

QSize medSeriesPreview::sizeHint(void) const
{
    return QSize();
}

void medSeriesPreview::setup(int seriesId)
{
    Q_UNUSED(seriesId);
}
