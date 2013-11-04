/*=========================================================================

 medInria

 Copyright (c) INRIA 2013. All rights reserved.
 See LICENSE.txt for details.
 
  This software is distributed WITHOUT ANY WARRANTY; without even
  the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
  PURPOSE.

=========================================================================*/

#pragma once

namespace mseg {

//! Register all algorithms with factory.
class AlgorithmInitializer {
public:
    AlgorithmInitializer();
    virtual ~AlgorithmInitializer();

    static bool initialize();
    static bool initializeProcesses();
    static bool initializeWidgets();
};

} // namespace mseg


