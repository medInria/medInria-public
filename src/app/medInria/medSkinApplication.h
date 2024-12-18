#include <QObject>
#include <QString>
#include <QStringList>

#include <QPixmap>

class medLoadCustomPrivate;
class medLoadCustom : public QObject
{
    Q_OBJECT

public:
    medLoadCustom(QObject *parent = nullptr);
    ~medLoadCustom();

    bool loadCustom(QString path);
    bool isValidSkin();

    QString appName();
    QString appVersion();
    bool appAbout(QString &about, QString lang = "");
    bool appBrief(QString &brief, QString lang = "");

    QMap<QString, QString> shopURIs();
    QMap<QString, QPair<bool, QString> > themes();
    QString defaultTheme();

    QPixmap logoDark();
    QPixmap logoLight();
    QPixmap iconDark();
    QPixmap iconLight();

    QString additionalAuthors();
    QString licence();
    QString licencesDependencies();

private:
    bool loadTextBasedFile(const QString &fileName, QString &contents);
    bool loadImage(QPixmap &image, QString const &filePath, int height, int width, bool widthStrict = true);
    bool parseCustomJson();

    bool readMandatoryEntry(QJsonObject &customMainObject, QString entryName, QString &valueRes);
    bool readOptionalEntry(QJsonObject &customMainObject, QString entryName, QString &valueRes);
    bool readLangArray(QJsonObject &customMainObject, QString arrayName, QString arrayEntryName, QMap<QString, QString> & langTextMap);
    bool readOptionalAdditionalShops(QJsonObject &customMainObject);
    bool readOptionalAdditionalThems(QJsonObject &customMainObject);

    void reset();

private:
    medLoadCustomPrivate *d;
};

class medSkinApplicationPrivate;
class medSkinApplication : public QObject
{
    Q_OBJECT
public:
    medSkinApplication(QObject *parent = nullptr);
    ~medSkinApplication();

    QString appName();
    QString appVersion();
    QString appAbout();
    QString appBrief();

    QPixmap logo();    
    QPixmap icon();    

    QString authors();

public slots:
    void useDarkTheme(bool pi_bDark = true);
    void selectLang(QString pi_lang = "en");

private:
    medSkinApplicationPrivate *d;
};

class medThemes : public QObject
{
    Q_OBJECT
public:
    medThemes(QObject *parent = nullptr);
    //~medThemes();

    //bool isDark();

};
