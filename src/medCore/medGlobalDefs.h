#pragma once

#include <QString>

namespace med {

QString smartBaseName(const QString & fileName);

struct GPUInfo {
    QString renderer;
    QString version;
    QString vendor;
};

GPUInfo gpuModel();

}
