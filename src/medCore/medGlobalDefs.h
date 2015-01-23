#pragma once

#include <QString>
#include <QSize>

namespace med {

QString smartBaseName(const QString & fileName);

struct GPUInfo {
    QString renderer;
    QString version;
    QString vendor;
};

GPUInfo gpuModel();

static const QSize defaultThumbnailSize = QSize(320,320);

}
