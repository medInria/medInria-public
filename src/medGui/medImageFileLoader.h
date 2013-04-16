/*=========================================================================

 MedInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

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
    ~medImageFileLoader();

    //! Implement QRunnable
    void run();

    signals:
    /** Emitted when reading is successful.
     * @param image : The image that has been read.
     */
    void completed(const QImage& image);

private:
    medImageFileLoaderPrivate *d;
};


