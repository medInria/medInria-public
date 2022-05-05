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
QString getResourcesDirectoryForMacPackage(QString libraryName)
{
    QString result;
    CFBundleRef bundle = getBundleOrFramework(libraryName);

    if (bundle)
    {
        CFURLRef relativeResourcesDirectoryURL = CFBundleCopyResourcesDirectoryURL(bundle);
        CFURLRef resourcesDirectoryURL = CFURLCopyAbsoluteURL(relativeResourcesDirectoryURL);
        CFRelease(relativeResourcesDirectoryURL);

        if (resourcesDirectoryURL)
        {
            CFStringRef resourcesDirectory = CFURLCopyFileSystemPath(resourcesDirectoryURL, kCFURLPOSIXPathStyle);
            CFRelease(resourcesDirectoryURL);
            CFIndex utf16length = CFStringGetLength(resourcesDirectory);
            CFIndex maxUtf8length = CFStringGetMaximumSizeForEncoding(utf16length, kCFStringEncodingUTF8);
            std::string pathString(maxUtf8length, '\0');

            if (CFStringGetCString(resourcesDirectory, pathString.data(), maxUtf8length, kCFStringEncodingUTF8))
            {
                result = pathString.c_str();
            }

            CFRelease(resourcesDirectory);
        }
    }

    return result;
}

// (see getResourcesDirectoryForMacPackage for implementation notes)
QString getResourcePathForMacPackage(QString filename, QString subdirectory, QString libraryName)
{
    QString result;
    CFBundleRef bundle = getBundleOrFramework(libraryName);

    if (bundle)
    {
        CFStringRef resourceName = CFStringCreateWithCString(nullptr, qUtf8Printable(filename), kCFStringEncodingUTF8);
        CFStringRef resourceSubDir = nullptr;

        if (!subdirectory.isEmpty())
        {
            resourceSubDir = CFStringCreateWithCString(nullptr, qUtf8Printable(subdirectory), kCFStringEncodingUTF8);
        }

        CFURLRef resourceURL = CFBundleCopyResourceURL(bundle, resourceName, nullptr, resourceSubDir);
        CFRelease(resourceName);

        if (resourceSubDir)
        {
            CFRelease(resourceSubDir);
        }

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

#endif

// Return the '../resources/[libraryName/]' directory relative to
// the application directory.
QString getResourcesDirectoryFromApplicationDirectory(QString libraryName)
{
    QString result;
    QString applicationDirectory = qApp->applicationDirPath();

    if (!applicationDirectory.isEmpty())
    {
        QDir resourcesDirectory = applicationDirectory;

        if (resourcesDirectory.cd(QString("../resources/%1").arg(libraryName)))
        {
            result = resourcesDirectory.path();
        }
    }

    return result;
}

// Search for the resource in the '../resources/[libraryName/][subdirectory]'
// directory relative to the application directory.
QString getResourcePathFromApplicationDirectory(QString filename, QString subdirectory, QString libraryName)
{
    QString result;
    QString resourcesDirectory = getResourcesDirectoryFromApplicationDirectory(libraryName);

    if (!resourcesDirectory.isEmpty())
    {
        QDir resourcesSubdirectory = resourcesDirectory;

        if (subdirectory.isEmpty() || resourcesSubdirectory.cd(subdirectory))
        {
            QString resourceFilePath = resourcesSubdirectory.filePath(filename);

            if (QFileInfo::exists(resourceFilePath))
            {
                result = resourceFilePath;
            }
        }
    }

    return result;
}

} // namespace

QString getExternalResourcesDirectory(QString subdirectory, QString libraryName)
{
    QString result;

#if defined(Q_OS_MACOS)
    QString path = getResourcesDirectoryForMacPackage(libraryName);
#else
    QString path = getResourcesDirectoryFromApplicationDirectory(libraryName);
#endif

    if (!path.isEmpty())
    {
        QDir resourcesDirectory = path;

        if (subdirectory.isEmpty() || resourcesDirectory.cd(subdirectory))
        {
            result = resourcesDirectory.absolutePath();
        }
    }

    return result;
}

QString getExternalResourcePath(QString filepath, QString libraryName)
{
    QString result;
    QFileInfo fileInfo = filepath;
    QString filename = fileInfo.fileName();
    QString subdirectory = fileInfo.dir().path();

    if (subdirectory == ".")
    {
        subdirectory.clear();
    }

#if defined(Q_OS_MACOS)
    result = getResourcePathForMacPackage(filename, subdirectory, libraryName);
#else
    result = getResourcePathFromApplicationDirectory(filename, subdirectory, libraryName);
#endif

    return result;
}

} // namespace med
