/* medImageFileLoader.h ---
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Tue Dec 15 09:43:31 2009 (+0100)
 * Version: $Id$
 * Last-Updated: Tue Dec 15 09:43:31 2009 (+0100)
 *           By: Julien Wintz
 *     Update #: 1
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#pragma once

#include <QtCore>
#include <QtGui>

class medImageFileLoaderPrivate;

//! Reads an image from the file, emitting completed when done.
class medImageFileLoader: public QObject, public QRunnable {
    Q_OBJECT

public:
    //! Constructor : Supply the image path as \a path.
    medImageFileLoader(const QString& path);
    ~medImageFileLoader(void);

    //! Implement QRunnable
    void run(void);

    signals:
    /** Emitted when reading is successful.
     * @param image : The image that has been read.
     */
    void completed(const QImage& image);

private:
    medImageFileLoaderPrivate *d;
};


