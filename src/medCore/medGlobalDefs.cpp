#include <medGlobalDefs.h>

#include <QRegExp>

namespace med {

QString smartBaseName(const QString & fileName) {
    // remove extension after the first dot, or if it ends in a compression extension,
    // the one before. So "test.nii.gz" -> "test" and "test.mha" -> "test"

    QRegExp r1("^(.+)(?:\\.[^.]+(?:\\.(?:gz|Z|bz2)))$", Qt::CaseInsensitive, QRegExp::RegExp2);
    QRegExp r2("^(.+)(?:\\.[^.]+)$", Qt::CaseInsensitive, QRegExp::RegExp2);
    r1.setMinimal(true);
    r2.setMinimal(true);

    return r1.exactMatch(fileName) ? r1.cap(1) : (r2.exactMatch(fileName) ? r2.cap(1) : fileName);
}

}
