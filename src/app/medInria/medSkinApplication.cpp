#include "medSkinApplication.h"

#include <QMap>

#include <QDir>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QPixmap>

#include <QDebug>


inline bool fileIsPresent(QString path, QString file)
{
    bool bRes = QFile::exists(path + file);

    if (!bRes)
    {
        qDebug() << "File " << file << " is missing in the custom directory";
    }

    return bRes;
}

inline void insertDataLang(QString keyName, QMap<QString, QString> & langTextMap, QString &lang, QString &text);
inline void readJsonLangArray(QString keyName, QJsonValueRef &briefEntry, QString &lang, QString &text);
inline void extractJsonLangArrayData(QString keyName, QJsonValueRef &briefEntry, QMap<QString, QString> & langTextMap);




class medLoadCustomPrivate
{
public:
    bool validCustom;
    QString rootPath;

    QString name;
    QString version;
    QString defaultTheme;
    QMap<QString, QString> aboutMap; //lang, text
    QMap<QString, QString> briefMap; //lang, text
    QMap<QString, QPair<bool, QString> > additionalThemes; //Theme name, <dark, fileTheme>
    QMap<QString, QString> additionalShopURIs; //Shop name, uri
    
    QPixmap logoDark;
    QPixmap logoLight;
    QPixmap iconDark;
    QPixmap iconLight;

    QString additionalAuthors;
    QString licence;
    QString licencesDependencies;
};

medLoadCustom::medLoadCustom(QObject *parent)
{
    d = new medLoadCustomPrivate();
}

medLoadCustom::~medLoadCustom()
{
    delete d;
}

void medLoadCustom::reset()
{
    d->validCustom = false;
    d->rootPath.clear();

    d->name.clear();
    d->version.clear();
    d->defaultTheme.clear();
    d->aboutMap.clear(); //lang, text
    d->briefMap.clear(); //lang, text
    d->additionalThemes.clear(); //Theme name, <dark, fileTheme>
    d->additionalShopURIs.clear(); //Shop name, uri

    d->logoDark  = QPixmap();
    d->logoLight = QPixmap();
    d->iconDark  = QPixmap();
    d->iconLight = QPixmap();

    d->additionalAuthors.clear();
    d->licence.clear();
    d->licencesDependencies.clear();
}

bool medLoadCustom::loadCustom(QString path)
{
    bool bRes = true;
    
    QDir dir(path);
    if (dir.exists())
    {
        bRes = bRes && fileIsPresent(path, "custom.json");

        bRes = bRes && fileIsPresent(path, "logo_dark.png");
        bRes = bRes && fileIsPresent(path, "logo_light.png");
        bRes = bRes && fileIsPresent(path, "icon_dark.png");
        bRes = bRes && fileIsPresent(path, "icon_light.png");
        bRes = bRes && fileIsPresent(path, "authors.html");
        bRes = bRes && fileIsPresent(path, "release_notes.txt");
        bRes = bRes && fileIsPresent(path, "licenses_dep.txt");
        bRes = bRes && fileIsPresent(path, "license.txt");

        bRes = bRes && loadImage(d->logoDark, "logo_dark.png", 256, 512, false);
        bRes = bRes && loadImage(d->logoLight, "logo_light.png", 256, 512, false);
        bRes = bRes && loadImage(d->iconDark, "icon_dark.png", 256, 256);
        bRes = bRes && loadImage(d->iconLight, "icon_light.png", 256, 256);

        bRes = bRes && loadTextBasedFile("release_notes.txt", d->additionalAuthors);
        bRes = bRes && loadTextBasedFile("license.txt", d->licence);
        bRes = bRes && loadTextBasedFile("licenses_dep.txt" , d->licencesDependencies);


        if (bRes)
        {
            d->rootPath = path;
            bRes = bRes && parseCustomJson();
        }
        else
        {
            reset();
        }
    }
    else
    {
        bRes = false;
    }

    if (!bRes)
    {
        qDebug() << "[ERROR] Custom directory is not valid";
    }

    d->validCustom = bRes;

    return bRes;
}




bool medLoadCustom::isValidSkin()
{
    d->validCustom;
    return true;
}

QString medLoadCustom::appName()
{
    return d->name;
}

QString medLoadCustom::appVersion()
{
    return d->version;
}

bool medLoadCustom::appAbout(QString &about, QString lang)
{
    if (lang.isEmpty())
    {
        about = *d->aboutMap.begin();
    }
    else
    {
        about = d->aboutMap.value(lang);
    }

    return  d->aboutMap.contains(lang);
}

bool medLoadCustom::appBrief(QString &brief, QString lang)
{
    if (lang.isEmpty())
    {
        brief = *d->briefMap.begin();
    }
    else
    {
        brief = d->briefMap.value(lang);
    }

    return  d->briefMap.contains(lang);
}

QMap<QString, QString> medLoadCustom::shopURIs()
{
    return d->additionalShopURIs;
}

QMap<QString, QPair<bool, QString>> medLoadCustom::themes()
{
    return d->additionalThemes;
}

QString medLoadCustom::defaultTheme()
{
    return d->defaultTheme;
}

QPixmap medLoadCustom::logoDark()
{
    return d->logoDark;
}

QPixmap medLoadCustom::logoLight()
{
    return d->logoLight;
}

QPixmap medLoadCustom::iconDark()
{
    return d->iconDark;
}

QPixmap medLoadCustom::iconLight()
{
    return d->iconLight;
}

QString medLoadCustom::additionalAuthors()
{
    return d->additionalAuthors;
}

QString medLoadCustom::licence()
{
    return d->licence;
}

QString medLoadCustom::licencesDependencies()
{
    return d->licencesDependencies;
}



#include <QTextStream>


bool medLoadCustom::loadTextBasedFile(const QString &fileName, QString &contents)
{
    bool bRes = true;

    QFile file(d->rootPath + fileName);
    if (file.open(QFile::ReadOnly))
    {
        contents = file.readAll();
        file.close();
    }
    else
    {
        qDebug() << "[ERROR] Custom file " << fileName << " is not valid";
        bRes = false;
    }

    return bRes;
}

bool medLoadCustom::loadImage(QPixmap &image, QString const &fileName, int height, int width, bool widthStrict)
{
    bool bRes = true;

    if (image.load(d->rootPath + fileName))
    {
        if (!(image.height() == height && (image.width() == width || ((image.width() <= width) && (!widthStrict)))))
        {
            QString widthErrorMsg;
            widthStrict ? widthErrorMsg = QString("and  width is ") + QString::number(width) : widthErrorMsg = "and  width is less or equal " + QString::number(width);
            qDebug() << "[ERROR] Custom invalid image size for : " << fileName << "  expected height is " << height << " pixels " << widthErrorMsg;
            bRes = false;
        }
    }
    else
    {
        qDebug() << "[ERROR] Custom invalid image format for : " << fileName;
        bRes = false;
    }

    return bRes;
}

bool medLoadCustom::parseCustomJson()
{
    bool bRes = true;
    
    QFile customFile("custom.json");
    if (customFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {        
        QJsonDocument customJson(QJsonDocument::fromJson(QString(customFile.readAll()).toUtf8()));
        if (customJson.isObject())
        {
            auto customMainObject = customJson.object();

            bRes = bRes && readMandatoryEntry(customMainObject, "appName", d->name);
            bRes = bRes && readLangArray(customMainObject, "appBrief", "brief", d->briefMap);
            bRes = bRes && readLangArray(customMainObject, "appAbout", "about", d->aboutMap);
            bRes = bRes && readOptionalEntry(customMainObject, "appVersion", d->version);
            bRes = bRes && readOptionalEntry(customMainObject, "defaultTheme", d->defaultTheme);
            bRes = bRes && readOptionalAdditionalShops(customMainObject);
            bRes = bRes && readOptionalAdditionalThems(customMainObject);
        }
        else
        {
            qDebug() << "[ERROR] Custom invalid custom.json does not contain a JSon object at root";
        }
    }
    else
    {
        qDebug() << "[ERROR] Custom invalid costum.json ";
    }

    return bRes;
}










bool medLoadCustom::readMandatoryEntry(QJsonObject &customMainObject, QString entryName, QString &valueRes)
{
    bool bRes = customMainObject.contains(entryName);

    if (bRes)
    {
        if (customMainObject[entryName].isString())
        {
            valueRes = customMainObject[entryName].toString();
        }
        else
        {
            qDebug() << "[ERROR] Custom invalid \"" << entryName << "\" is malformed";
            bRes = false;
        }
    }
    else
    {
        qDebug() << "[ERROR] Custom invalid \"" << entryName << "\" is not present";
    }

    return bRes;
}

bool medLoadCustom::readOptionalEntry(QJsonObject &customMainObject, QString entryName, QString &valueRes)
{
    bool bRes = true;

    if (customMainObject.contains(entryName))
    {
        if (customMainObject[entryName].isString())
        {
            valueRes = customMainObject[entryName].toString();
        }
        else
        {
            qDebug() << "[ERROR] Custom invalid \"" << entryName << "\" is malformed";
            bRes = false;
        }
    }
    else
    {
        qDebug() << "[INFO] Custom " << entryName << " is not present";
    }

    return bRes;
}

bool medLoadCustom::readLangArray(QJsonObject &customMainObject, QString arrayName, QString arrayEntryName, QMap<QString, QString> & langTextMap)
{
    bool bRes = true;

    if (customMainObject.contains(arrayName) && customMainObject[arrayName].isArray())
    {
        auto array = customMainObject[arrayName].toArray();
        for (auto entry : array)
        {
            if (entry.isObject())
            {
                extractJsonLangArrayData(arrayEntryName, entry, langTextMap);
            }
        }
        if (langTextMap.isEmpty())
        {
            qDebug() << "[ERROR] Custom invalid \"" << arrayName << "\" array doesn't have correct entry";
            bRes = false;
        }
    }
    else
    {
        qDebug() << "[ERROR] Custom invalid \"" << arrayName << "\" is not present or not an array";
        bRes = false;
    }

    return bRes;
}

bool medLoadCustom::readOptionalAdditionalShops(QJsonObject &customMainObject)
{
    bool bRes = true;

    const QString shopsURIsArrayName("additionalShopURIs");

    if (customMainObject.contains(shopsURIsArrayName))
    {
        if (customMainObject[shopsURIsArrayName].isArray())
        {
            for (auto uri : customMainObject[shopsURIsArrayName].toArray())
            {
                if (uri.isObject() && uri.toObject().keys().size() > 0)
                {
                    QString shopName = uri.toObject().keys()[0];
                    auto shopUriAsJson = uri.toObject()[shopName];
                    if (shopUriAsJson.isString())
                    {
                        d->additionalShopURIs[shopName] = shopUriAsJson.toString();
                    }
                    else
                    {
                        qDebug() << "[WARN] Custom invalid Shop URI entry";
                    }
                }
                else
                {
                    qDebug() << "[WARN] Custom invalid Shop URI entry";
                }
            }
        }
        else
        {
            qDebug() << "[ERROR] Custom invalid additional Shop URI is malformed";
            bRes = false;
        }
    }
    else
    {
        qDebug() << "[INFO] Custom additional Shop URI is not present";
    }

    return bRes;
}

bool medLoadCustom::readOptionalAdditionalThems(QJsonObject &customMainObject)
{
    bool bRes = true;

    const QString themeArrayName("additionalThemes");

    if (customMainObject.contains(themeArrayName))
    {
        if (customMainObject[themeArrayName].isArray())
        {
            auto array = customMainObject[themeArrayName].toArray();
            for (auto entry : array)
            {
                if (entry.isObject())
                {
                    auto theme = entry.toObject();
                    if (theme.contains("themeName") && theme.contains("file") && theme.contains("themeType") &&
                        theme["themeName"].isString() && theme["file"].isString() && theme["themeType"].isString() &&
                        QFile::exists(d->rootPath + theme["file"].toString()) &&
                        (theme["themeType"].toString() == "light" || theme["themeType"].toString() == "dark"))
                    {
                        d->additionalThemes[theme["themeName"].toString()] = { theme["themeType"].toString() == "dark", d->rootPath + theme["file"].toString() };
                    }
                    else
                    {
                        qDebug() << "[WARN] Custom invalid theme entry";
                    }
                }
                else
                {
                    qDebug() << "[WARN] Custom invalid theme entry";
                }
            }
        }
        else
        {
            qDebug() << "[ERROR] Custom invalid additional themes is malformed";
            bRes = false;
        }
    }
    else
    {
        qDebug() << "[INFO] Custom additional themes is not present";
    }

    return bRes;
}




void extractJsonLangArrayData(QString keyName, QJsonValueRef &briefEntry, QMap<QString, QString> & langTextMap)
{
    QString lang;
    QString text;
    readJsonLangArray(keyName, briefEntry,  lang, text);
    insertDataLang   (keyName, langTextMap, lang, text);
}

void readJsonLangArray(QString keyName, QJsonValueRef &briefEntry, QString &lang, QString &text)
{
    auto entryAsObject = briefEntry.toObject();
    if (entryAsObject.contains("lang") && entryAsObject["lang"].isString() &&
        entryAsObject.contains(keyName) && entryAsObject[keyName].isString())
    {
        lang = entryAsObject["lang"].toString().toLower();
        text = entryAsObject[keyName].toString();

    }
    else
    {
        auto keys = entryAsObject.keys();
        qDebug() << "[WARN] Custom invalid " << keyName << " entry detected with keys " << keys << "\r\n with values \r\n";
        for (auto key : keys)
        {
            qDebug() << entryAsObject[key];
        }
    }
}

void insertDataLang(QString keyName, QMap<QString, QString> & langTextMap, QString &lang, QString &text)
{
    if (!lang.isEmpty() && lang.size() < 3)
    {
        if (!langTextMap.contains(lang))
        {
            langTextMap[lang] = text;
        }
        else
        {
            qDebug() << "[WARN] Custom for " << keyName << " lang " << lang << " is already registered";
        }
    }
    else
    {
        qDebug() << "[WARN] Custom for " << keyName << " lang " << lang << " is too long, do not exceed 3 Latin characters";
    }
}

#include <QCoreApplication>
#include <QApplication>
#include <QIcon>

#define MEDINRIA_ABOUT "medInria (" MEDINRIA_VERSION ") is a medical imaging platform developed at Inria.<br/><center>Inria, Copyright 2013</center>"
#define MEDINRIA_BRIEF "<b>medInria</b> is a cross-platform medical image "\
                       "processing and visualisation software, "\
                       "and it is <b>free</b>. Through an intuitive user "\
                       "interface, <b>medInria</b> offers from standard "\
                       "to cutting-edge processing functionalities for "\
                       "your medical images such as 2D/3D/4D image "\
                       "visualisation, image registration, or diffusion "\
                       "MR processing and tractography."
class medSkinApplicationPrivate
{
public:
    //QMap<QString, QPair<bool, QString> >themes; //Theme name, <dark, fileTheme>
    medLoadCustom custom;


    QString name;
    QString version;
    QString about;
    QString brief;
    
    QString additionalShopURIs;

    QPixmap logo;
    QPixmap icon;

    QString additionalAuthors;
    QString licence;
    QString licencesDependencies;


    QString currentTheme;
    QString lang;
    bool dark;
};

medSkinApplication::medSkinApplication(QObject *parent)
{
    d->name    = "medInria";
    d->version = MEDINRIA_VERSION;
    d->about   = QString::fromUtf8( MEDINRIA_ABOUT );
    d->brief   = QString::fromUtf8( MEDINRIA_BRIEF );
    
    d->additionalShopURIs;
    
    d->logo = QPixmap(":pixmaps/medInria-logo-homepage.png");
    d->icon = QPixmap(":/medInria.ico");
    
    d->additionalAuthors;
    d->licence;
    d->licencesDependencies;
    
    
    d->currentTheme;
    d->lang;



    //setApplicationName("medInria");
    //setApplicationVersion(MEDINRIA_VERSION);
    //setOrganizationName("inria");
    //setOrganizationDomain("fr");
    //setWindowIcon(QIcon(":/medInria.ico"));
    //
    //
    //QApplication::setStyle(QStyleFactory::create("fusion"));
    //medStyleSheetParser parser(dtkReadFile(":/medInria.qss"));
    //this->setStyleSheet(parser.result());


    d->currentTheme = "medInria_dark";
    d->lang = "en";
    d->dark = true;
}

medSkinApplication::~medSkinApplication()
{
}

QString medSkinApplication::appName()
{
    QString appNameRes = QCoreApplication::instance()->applicationName();
    
    if (d->custom.isValidSkin())
    {
        appNameRes = d->custom.appName();
    }

    return appNameRes;
}

QString medSkinApplication::appVersion()
{
    QString appVersionRes = QCoreApplication::instance()->applicationVersion();

    if (d->custom.isValidSkin())
    {
        appVersionRes = d->custom.appVersion();
    }

    return appVersionRes;
}

QString medSkinApplication::appAbout()
{
    QString appAboutRes = QString::fromUtf8(
        "%1 (%2) is a medical imaging platform developed at "
        "Inria.<br/>"
        "<center>Inria, Copyright 2013</center>")
        .arg(qApp->applicationName())
        .arg(qApp->applicationVersion());

    if (d->custom.isValidSkin())
    {
        if (!d->custom.appAbout(appAboutRes, d->lang))
        {
            if (!d->custom.appAbout(appAboutRes, "en"))
            {
                d->custom.appAbout(appAboutRes);
            }
        }
    }

    return appAboutRes;
}

QString medSkinApplication::appBrief()
{
    QString appBriefRes = d->brief;

    if (d->custom.isValidSkin())
    {
        if (!d->custom.appBrief(appBriefRes, d->lang))
        {
            if (!d->custom.appBrief(appBriefRes, "en"))
            {
                d->custom.appBrief(appBriefRes);
            }
        }
    }

    return appBriefRes;
}

QPixmap medSkinApplication::logo()
{
    QPixmap logoRes = QCoreApplication::instance()->applicationVersion();

    if (d->custom.isValidSkin())
    {
        logoRes = d->custom.logoDark();
    }

    return QPixmap();
}



void medSkinApplication::useDarkTheme(bool pi_bDark)
{
    d->dark = pi_bDark;
}

void medSkinApplication::selectLang(QString pi_lang)
{
    d->lang = pi_lang;
}
