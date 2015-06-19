/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

#include <medCoreExport.h>
#include <dtkCoreSupport/dtkAbstractFactory.h>
#include <QtCore>

class medSettingsWidget;
class medSettingsWidgetFactoryPrivate;
struct medSettingDetails;

/**
 * @brief This factory creates Widgets that are pages in the medSettingsEditor widget.
*/
class MEDCORE_EXPORT medSettingsWidgetFactory : public dtkAbstractFactory
{
  Q_OBJECT

public:

  //! This function pointer designates functions allocating memory
  typedef medSettingsWidget*(*medSettingsWidgetCreator)(QWidget *);

  //! Type designating the internal has table containing the creator functions.
  typedef QHash<QString, medSettingDetails*> medSettingsWidgetCreatorHash;

public:

  static medSettingsWidgetFactory * instance();
  /**
   * @brief Registers a new widget type, and its creator function.
   *
   * @param type name of the widget type.
   * @param name Human readable name(Potentially localised).
   * @param description short description, mainly used for tooltips
   * @return bool true if type was not registered already, false if it exists.
  */
  template <typename settingType>
  bool registerSettingsWidget()
  {
      //we must keep the templated part in the .h file for library users
      medSettingsWidgetCreator creator = create<settingType>;
      return registerSettingsWidget(settingType::staticIdentifier(),
                                    settingType::staticName(),
                                    settingType::staticDescription(),
                                    creator);
  }

  QList<QString> settingsWidgets();

  medSettingDetails* settingDetailsFromId (const QString& id ) const;

public slots:

  medSettingsWidget * createSettingsWidget(QString type, QWidget * parent);

protected:

  medSettingsWidgetFactory();
  ~medSettingsWidgetFactory();

private:
  static medSettingsWidgetFactory * s_instance; /**< TODO */
  /**
   * @brief Templated method returning a pointer to an allocated settingsWidget.
   * @see template<class settingType> registerToolBox
   * @warning keep it static if you don't want to freeze your brain (solution in http://www.parashift.com/c++-faq-lite/pointers-to-members.html#faq-33.5 for those interested)
   */
  template < typename T >
  static medSettingsWidget* create ( QWidget* parent ) {
  return ( new T(parent) );
  }

  bool registerSettingsWidget (const QString& type,
                               QString name,
                               QString description,
                               medSettingsWidgetCreator);

private:
  medSettingsWidgetFactoryPrivate * d; /**< TODO */
};

/**
 * @brief Stores the details for a particular settingsWidget,
 * and a function to allocate memory.
 *
 */
struct MEDCORE_EXPORT medSettingDetails{
    QString name; /** Readable name*/
    QString description; /** (tooltip) short description */
    medSettingsWidgetFactory::medSettingsWidgetCreator creator; /** function pointer allocating memory for the widget*/
    medSettingDetails(QString name,QString description,
                     medSettingsWidgetFactory::medSettingsWidgetCreator creator):
        name(name),description(description),
        creator(creator){}
};


