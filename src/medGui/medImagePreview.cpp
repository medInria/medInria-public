/* medImagePreview.cpp --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Oct  7 16:13:54 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Oct  7 16:18:48 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 6
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#include "medImagePreview.h"

class medImagePreviewPrivate
{
public:
};

medImagePreview::medImagePreview(QWidget *parent) : QWidget(parent), d(new medImagePreviewPrivate)
{

}

medImagePreview::~medImagePreview(void)
{
    delete d;

    d = NULL;
}
