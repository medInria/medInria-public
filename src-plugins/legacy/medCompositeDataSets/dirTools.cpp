/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#include <dirTools.h>

bool RemoveDirectory(const QDir& dir) {
    bool has_err = false;
    if (dir.exists()) {
        QFileInfoList entries = dir.entryInfoList(QDir::NoDotAndDotDot|QDir::Dirs|QDir::Files);
        int count = entries.size();
        //for (int idx = 0;((idx<count)&&(RBCFS_OK==has_err));++idx) 
        for (int idx = 0;idx<count;++idx) {
            QFileInfo entryInfo = entries[idx];
            QString path = entryInfo.absoluteFilePath();
            if (entryInfo.isDir())
                has_err = RemoveDirectory(QDir(path));
            else {
                QFile file(path);
                if (!file.remove())
                    has_err = true;
            }
        }
        if (!dir.rmdir(dir.absolutePath()))
            has_err = true;
    }
    return has_err;
}

//  Create the in-zip dirname from the name of the zip file.
//  This means that for ./EmptyStudy/dwi1.cds, this function will provide the name dwi1.

QString zip_dirname(const QString& name) {
    const unsigned ind1 = name.lastIndexOf('/')+1;
    const unsigned ind2 = name.lastIndexOf('.');
    return name.mid(ind1,ind2-ind1);
}
