// File :  GUser.h
// Author: Luc Legeard
//////////////////////////////////////////////////////////////////////////////
//
// Class GUser
//
// This class mange user methodes
//
/////////////////////////////////////////////////////////////////////////////

// ---------------------------------------------------------------------------
// ***************************************************************************
// *                                                                         *
// *   This program is free software; you can redistribute it and/or modify  *
// *   it under the terms of the GNU General Public License as published by  *
// *   the Free Software Foundation; either version 2 of the License, or     *
// *   (at your option) any later version.                                   *
// *                                                                         *
// ***************************************************************************/


#ifndef __GUser__
#define __GUser__

// ROOT headers
#include <TObject.h>
#include <TRint.h>
#include <TString.h>

// GRU headers
#include "General.h"
#include "GAcq.h"
#include "GDevice.h"
#include "GParaCaliXml.h"
#include "GNetServerRoot.h"

// NPTool header
#include "DetectorManager.h"

// Ganil2Root headers
#include "TDetectorManager.h"

class GUser : public  GAcq
{
 protected:
	int fMyLabel;
	TString fMyParameterName;
  
	GSpectra*         MySpectraList;
	GNetServerRoot*   MySpectraServer;
    
 private:
  G2R::TDetectorManager* fDetectorManager;    

  string fNPToolArgument;

 private:
   DetectorManager   *fMyDetector;

 public:
   GUser(GDevice* _fDevIn= NULL, GDevice* _fDevOut= NULL);  // default constructor of GUser object 
   GUser(GDevice* _fDevIn, string NPToolOption);  // NPTool specific constructor 
   ~GUser();// destructor of GUser object 

   virtual void InitUser();
   virtual void InitUserRun();
   virtual void User();
   virtual void EndUserRun();
   virtual void EndUser();
   virtual void InitTTreeUser(); 

   ClassDef (GUser ,1); // User Treatment of Data
};

#endif

