/*=========================================================================

  Copyright 2005 Brigham and Women's Hospital (BWH) All Rights Reserved.

  See Doc/copyright/copyright.txt
  or http://www.slicer.org/copyright/copyright.txt for details.

  Program:   Module Description Parser
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/branches/Slicer-3-6/Libs/LoadableModule/LoadableModuleWin32Header.h $
  Date:      $Date: 2010-01-22 14:42:07 -0500 (Fri, 22 Jan 2010) $
  Version:   $Revision: 11741 $

==========================================================================*/
///  LoadableModuleWin32Header - manage Windows system differences
/// 
/// The LoadableModuleWin32Header captures some system differences between Unix
/// and Windows operating systems. 

#ifndef __LoadableModuleWin32Header_h
#define __LoadableModuleWin32Header_h

#include <LoadableModuleConfigure.h>

#if defined(WIN32) && !defined(LoadableModule_STATIC)
#if defined(LoadableModule_EXPORTS)
#define LoadableModule_EXPORT __declspec( dllexport ) 
#else
#define LoadableModule_EXPORT __declspec( dllimport ) 
#endif
#else
#define LoadableModule_EXPORT 
#endif

#if defined(_MSC_VER)
#  pragma warning (disable: 4251) /* missing DLL-interface */
#endif

#endif
