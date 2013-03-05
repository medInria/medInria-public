#ifndef medCompositeDataSetImporterSelectorToolBox_H
#define medCompositeDataSetImporterSelectorToolBox_H

#include <medToolBox.h>

class medCompositeDataSetImporterSelectorToolBoxPrivate;

class MEDGUI_EXPORT medCompositeDataSetImporterSelectorToolBox : public medToolBox
{
    Q_OBJECT

public:
     medCompositeDataSetImporterSelectorToolBox(QWidget *parent = 0);
    ~medCompositeDataSetImporterSelectorToolBox();
    /**
     * @brief initialize layout
     */
    virtual void initialize();

signals:
    /**
     * @brief Emits an error message for the medMessageController to display.
     *
     * When a section failed the medSettingsWidget::validate() method, a message with its name is emitted.
     *
     * @param sender Should be the current Object itself.
     * @param text The error message.
     * @param timeout The timeout before the message disapears.
    */
    void showError(QObject *sender, const QString& text,unsigned int timeout=0);

    /**
     * @brief Emits an info message for the medMessageController to display.
     *
     * Typically here, it is emitted when the settings are successfully saved.
     * @param sender Should be the current Object itself.
     * @param text The error message.
     * @param timeout The timeout before the message disapears.
    */
    void showInfo(QObject *sender, const QString& text,unsigned int timeout=0);


public slots:
    /**
     * @brief Performs validation tests on each section and tires to save.
     *
     * If any section fails, an error message is emitted for it, and the other sections are still saved.
     *
     * The widget closes if the save action is successful.
     *
    */
    virtual void onImportClicked();

    /**
     * @brief Load in non-persistent database.
     *
    */
    virtual void onLoadClicked();

    /**
     * @brief reset the display to the last saved values.
     *
    */
    virtual void onResetClicked();
    /**
     *
    */
    void onCurrentTypeChanged(const int type);

    // @brief Succesful import/load

    //void onSucess();

 protected:
    /**
     * Call save on all child widgets and return the status
     */
    virtual bool import();


 private:
    medCompositeDataSetImporterSelectorToolBoxPrivate* d;

};

#endif
