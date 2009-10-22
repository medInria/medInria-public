/* medGuiExport.h --- 
 * 
 * Author: Julien Wintz
 * Copyright (C) 2008 - Julien Wintz, Inria.
 * Created: Wed Oct 21 15:16:21 2009 (+0200)
 * Version: $Id$
 * Last-Updated: Wed Oct 21 15:16:36 2009 (+0200)
 *           By: Julien Wintz
 *     Update #: 1
 */

/* Commentary: 
 * 
 */

/* Change log:
 * 
 */

#ifdef WIN32
    #ifdef medGui_EXPORTS
        #define MEDGUI_EXPORT __declspec(dllexport) 
    #else
        #define MEDGUI_EXPORT __declspec(dllimport) 
    #endif
#else
    #define MEDGUI_EXPORT	
#endif
