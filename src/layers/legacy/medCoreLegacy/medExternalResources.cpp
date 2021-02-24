#include "medExternalResources.h"

#if defined(Q_OS_MACOS)
#include <CoreFoundation/CFBundle.h>
#endif

#include <QApplication>
#include <QDir>

namespace med
{

namespace
{

#if defined(Q_OS_MACOS)

// Frameworks are accessed as bundles.
CFBundleRef getBundleOrFramework(QString libraryName)
{
    CFBundleRef bundle;

    if (libraryName.isEmpty())
    {
        bundle = CFBundleGetMainBundle();
    }
    else
    {
        QString frameworkId = QString("%1.%2").arg(MEDINRIA_IDENTIFIER).arg(libraryName);
        CFStringRef frameworkIdRef = CFStringCreateWithCString(nullptr, qUtf8Printable(frameworkId), kCFStringEncodingUTF8);
        bundle = CFBundleGetBundleWithIdentifier(frameworkIdRef);
        CFRelease(frameworkIdRef);
    }

    return bundle;
}

// On macOS the main resources are located in the bundle's resource folder and
// the library-specific resources are located in the library's framework
// resource folder. If 'libraryName' is empty then the bundle is searched.
QString getResourcePathForMacPackage(QString filename, QString libraryName)
{
    QString result;
    CFBundleRef bundle = getBundleOrFramework(libraryName);

    if (bundle)
    {
        CFStringRef resourceName = CFStringCreateWithCString(nullptr, qUtf8Printable(filename), kCFStringEncodingUTF8);
        CFURLRef resourceURL = CFBundleCopyResourceURL(bundle, resourceName, nullptr, nullptr);
        CFRelease(bundle);
        CFRelease(resourceName);

        if (resourceURL)
        {
            CFStringRef resourcePath = CFURLCopyFileSystemPath(resourceURL, kCFURLPOSIXPathStyle);
            CFRelease(resourceURL);
            CFIndex utf16length = CFStringGetLength(resourcePath);
            CFIndex maxUtf8length = CFStringGetMaximumSizeForEncoding(utf16length, kCFStringEncodingUTF8);
            std::string pathString(maxUtf8length, '\0');

            if (CFStringGetCString(resourcePath, pathString.data(), maxUtf8length, kCFStringEncodingUTF8))
            {
                result = pathString.c_str();
            }

            CFRelease(resourcePath);
        }
    }

    return result;
}

#else

// Search for the resource in the '../resources/[libraryName/]' directory
// relative to the application directory.
QString getResourcePathFromApplicationDirectory(QString filename, QString libraryName)
{
    QString result;
    QString applicationDirectory = qApp->applicationDirPath();

    if (!applicationDirectory.isEmpty())
    {
        QDir resourcesDirectory = applicationDirectory;

        if (resourcesDirectory.cdUp()
            && resourcesDirectory.cd("resources")
            && (libraryName.isEmpty() || resourcesDirectory.cd(libraryName)))
        {
            QString resourceFilePath = resourcesDirectory.filePath(filename);

            if (QFileInfo::exists(resourceFilePath))
            {
                result = resourceFilePath;
            }
        }
    }

    return result;
}

#endif

} // namespace

QString getExternalResourcePath(QString filename, QString libraryName)
{
    QString result;

#if defined(Q_OS_MACOS)
    result = getResourcePathForMacPackage(filename, libraryName);
#else
    result = getResourcePathFromApplicationDirectory(filename, libraryName);
#endif

    return result;
}

} // namespace med
