#include <medGlobal.h>
#include <QCryptographicHash>

QString anonymise(const QString name) {
    QByteArray r = QCryptographicHash::hash(name.toUtf8(),QCryptographicHash::Md4).toHex().toUpper();
    r.resize(10);
    return QString(r);
}
