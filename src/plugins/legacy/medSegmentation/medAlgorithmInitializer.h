/*=========================================================================

 medInria

 Copyright (c) INRIA 2013 - 2014. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

//! Register all algorithms with factory.
class medAlgorithmInitializer {
public:
    medAlgorithmInitializer();
    virtual ~medAlgorithmInitializer();

    static bool initialize();
    static bool initializeProcesses();
    static bool initializeWidgets();
};



