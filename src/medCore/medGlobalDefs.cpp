#include <medGlobalDefs.h>

#include <QFileInfo>

namespace med {

QString smartBaseName(const QString & fileName) {
    // remove extension after the first dot, or if it ends in a compression extension,
    // the one before. So "test.nii.gz" -> "test" and "test.mha" -> "test"
    QFileInfo info(fileName);
    if( info.suffix().toLower() == "gz"
     || info.suffix().toLower() == "z"
     || info.suffix().toLower() == "bz2")
    {
        QFileInfo infoWithoutCompression(info.completeBaseName());
        return infoWithoutCompression.completeBaseName();
    }
    else return info.completeBaseName();
}

}
