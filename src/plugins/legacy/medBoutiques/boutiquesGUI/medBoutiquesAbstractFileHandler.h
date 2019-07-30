#ifndef MEDBOUTIQUESABSTRACTFILEHANDLER_H
#define MEDBOUTIQUESABSTRACTFILEHANDLER_H

#include <QWidget>
#include <QMimeData>

struct FormatAndExtension {
    QString type;
    QString extension;
    FormatAndExtension() {}
    FormatAndExtension(const QString &type, QString &extension): type(type), extension(extension) {}
    FormatAndExtension(const QJsonArray &typeAndExtension);
};

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

class medBoutiquesAbstractFileHandler : public QObject
{
    Q_OBJECT

public:
    medBoutiquesAbstractFileHandler();
    virtual ~medBoutiquesAbstractFileHandler();

    virtual void checkAcceptDragEvent(QDragEnterEvent *event) = 0;
    virtual QString createTemporaryInputFileForMimeData(const QMimeData *mimeData) = 0;
    virtual QString createTemporaryInputFileForCurrentInput() = 0;
};

#endif // MEDBOUTIQUESABSTRACTFILEHANDLER_H
