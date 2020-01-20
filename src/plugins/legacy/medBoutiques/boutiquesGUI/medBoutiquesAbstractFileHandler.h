#ifndef MEDBOUTIQUESABSTRACTFILEHANDLER_H
#define MEDBOUTIQUESABSTRACTFILEHANDLER_H

#include <QWidget>
#include <QMimeData>
#include "medBoutiquesConfiguration.h"

// A pair: type and extension, can be created from a QJsonArray where the first value is the type, and the second is the extension
struct FormatAndExtension {
    QString type;
    QString extension;
    FormatAndExtension() {}
    FormatAndExtension(const QString &type, const QString &extension): type(type), extension(extension) {}
    FormatAndExtension(const QJsonArray &typeAndExtension);
};

// The type, description and the compatible extensions (returned from possible writers of the given data in medInria)
struct FormatObject {
    QString type;
    QString description;
    QStringList extensions;

    FormatObject() {}
    FormatObject(const QString &type, const QString &description, const QStringList &extensions):
    type(type),
    description(description),
    extensions(extensions)
    {}
};

// See description in filehandler.h
class medBoutiquesAbstractFileHandler : public QObject
{
    Q_OBJECT

public:
    medBoutiquesAbstractFileHandler() = default;
    virtual ~medBoutiquesAbstractFileHandler() = default;

    virtual void checkAcceptDragEvent(QDragEnterEvent *event) = 0;
    virtual QString createTemporaryInputFileForMimeData(const QMimeData *mimeData) = 0;
    virtual QString createTemporaryInputFileForCurrentInput() = 0;
    virtual void deleteTemporaryFiles() = 0;
    virtual bool hasKnownExtension(const QString &fileName) = 0;
    virtual QString normalizePath(const QString &path) = 0;
    virtual QStringList normalizePaths(const QStringList &paths) = 0;
};

#endif // MEDBOUTIQUESABSTRACTFILEHANDLER_H
