#include <medGlobalDefs.h>

#include <QFileInfo>
#include <QtOpenGL/QtOpenGL>

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

GPUInfo gpuModel()
{
    // just fill it once, we are not going to change GPU on the fly
    static GPUInfo gpu;
    if (gpu.renderer.isEmpty()) {
        // glGetString requires a valid OpenGL context, the easiest way is to
        // create a bogus QGLWidget and force a render.
        QGLWidget glw;
        glw.updateGL();
        gpu.renderer = QString::fromLocal8Bit(reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
        gpu.version = QString::fromLocal8Bit(reinterpret_cast<const char*>(glGetString(GL_VERSION)));
        gpu.vendor = QString::fromLocal8Bit(reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
    }
    return gpu;
}

}
