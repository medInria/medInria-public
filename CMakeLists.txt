################################################################################
#
# medInria
#
# Copyright (c) INRIA 2013. All rights reserved.
# See LICENSE.txt for details.
# 
#  This software is distributed WITHOUT ANY WARRANTY; without even
#  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
#  PURPOSE.
#
################################################################################

cmake_minimum_required(VERSION 3.19)

if(POLICY CMP0020)
  cmake_policy(SET CMP0020 NEW)
endif()

if(NOT DEFINED ${MEDINRIA_SUPERBUILD_VERSION})
    set(MEDINRIA_SUPERBUILD_VERSION 4.0.0)
endif()

SET(CMAKE_CXX_STANDARD 17)

project(MEDINRIA_SUPERBUILD VERSION ${MEDINRIA_SUPERBUILD_VERSION})

## #############################################################################
## External settings
## #############################################################################

# Read the JSON file
file(READ ${CMAKE_CURRENT_SOURCE_DIR}/settings.json MY_JSON_STRING)

# Loop through each element of the JSON array
foreach(IDX RANGE 0)
    string(JSON settingsQrcDirectory GET ${MY_JSON_STRING} ${IDX} settingsQrcDirectory)
    set(SETTINGS_QRC_DIRECTORY ${settingsQrcDirectory} CACHE STRING "settings" FORCE)

    string(JSON applicationName GET ${MY_JSON_STRING} ${IDX} applicationName)
    set(APPLICATION_NAME ${applicationName} CACHE STRING "settings" FORCE)

    string(JSON organizationName GET ${MY_JSON_STRING} ${IDX} organizationName)
    set(ORGANIZATION_NAME ${organizationName} CACHE STRING "settings" FORCE)

    string(JSON organizationDomain GET ${MY_JSON_STRING} ${IDX} organizationDomain)
    set(ORGANIZATION_DOMAIN ${organizationDomain} CACHE STRING "settings" FORCE)

    string(JSON projectContact GET ${MY_JSON_STRING} ${IDX} projectContact)
    set(PROJECT_CONTACT ${projectContact} CACHE STRING "settings" FORCE)

    string(JSON packageVendor GET ${MY_JSON_STRING} ${IDX} packageVendor)
    set(PACKAGE_VENDOR ${packageVendor} CACHE STRING "settings" FORCE)

    string(JSON documentationURL GET ${MY_JSON_STRING} ${IDX} documentationURL)
    set(DOCUMENTATION_URL ${documentationURL} CACHE STRING "settings" FORCE)

    string(JSON projectDescription GET ${MY_JSON_STRING} ${IDX} projectDescription)
    set(PROJECT_DESCRIPTION ${projectDescription} CACHE STRING "settings" FORCE)

    string(JSON largeLogoDarkTheme GET ${MY_JSON_STRING} ${IDX} largeLogoDarkTheme)
    set(LARGE_LOGO_DARK_THEME ${largeLogoDarkTheme} CACHE STRING "settings" FORCE)

    string(JSON largeLogoLightTheme GET ${MY_JSON_STRING} ${IDX} largeLogoLightTheme)
    set(LARGE_LOGO_LIGHT_THEME ${largeLogoLightTheme} CACHE STRING "settings" FORCE)

    string(JSON windowIcon GET ${MY_JSON_STRING} ${IDX} windowIcon)
    set(WINDOW_ICON ${windowIcon} CACHE STRING "settings" FORCE)

    string(JSON windowIconPath GET ${MY_JSON_STRING} ${IDX} windowIconPath)
    set(WINDOW_ICON_PATH ${windowIconPath} CACHE STRING "settings" FORCE)

    string(JSON macOSIconPath GET ${MY_JSON_STRING} ${IDX} macOSIconPath)
    set(MACOS_ICON_PATH ${macOSIconPath} CACHE STRING "settings" FORCE)

    string(JSON aboutFile GET ${MY_JSON_STRING} ${IDX} aboutFile)
    set(ABOUT_FILE ${aboutFile} CACHE STRING "settings" FORCE)

    string(JSON releaseNotes GET ${MY_JSON_STRING} ${IDX} releaseNotes)
    set(RELEASE_NOTES ${releaseNotes} CACHE STRING "settings" FORCE)

    string(JSON additionalAuthorsList GET ${MY_JSON_STRING} ${IDX} additionalAuthorsList)
    set(ADDITIONAL_AUTHORS_LIST ${additionalAuthorsList} CACHE STRING "settings" FORCE)

    string(JSON descriptionHomepage GET ${MY_JSON_STRING} ${IDX} descriptionHomepage)
    set(DESCRIPTION_HOMEPAGE ${descriptionHomepage} CACHE STRING "settings" FORCE)

    string(JSON descriptionPackaging GET ${MY_JSON_STRING} ${IDX} descriptionPackaging)
    set(DESCRIPTION_PACKAGING ${descriptionPackaging} CACHE STRING "settings" FORCE)

    string(JSON licenseFile GET ${MY_JSON_STRING} ${IDX} licenseFile)
    set(LICENSE_FILE ${licenseFile} CACHE STRING "settings" FORCE)

    string(JSON licenseExternalFile GET ${MY_JSON_STRING} ${IDX} licenseExternalFile)
    set(LICENSE_EXTERNAL_FILE ${licenseExternalFile} CACHE STRING "settings" FORCE)

    string(JSON licenceFilePath GET ${MY_JSON_STRING} ${IDX} licenceFilePath)
    set(LICENSE_FILE_PATH ${licenceFilePath} CACHE STRING "settings" FORCE)

    string(JSON readmeFile GET ${MY_JSON_STRING} ${IDX} readmeFile)
    set(README_FILE ${readmeFile} CACHE STRING "settings" FORCE)

    string(JSON welcomeFile GET ${MY_JSON_STRING} ${IDX} welcomeFile)
    set(WELCOME_FILE ${welcomeFile} CACHE STRING "settings" FORCE)

    string(JSON expirationTime GET ${MY_JSON_STRING} ${IDX} expirationTime)
    set(EXPIRATION_TIME ${expirationTime} CACHE STRING "settings" FORCE)
endforeach()

## #############################################################################
## Superbuild
## #############################################################################

add_subdirectory(superbuild)