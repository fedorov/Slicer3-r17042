/*=auto=========================================================================

Portions (c) Copyright 2006 Brigham and Women's Hospital (BWH) All Rights Reserved.

See Doc/copyright/copyright.txt
or http://www.slicer.org/copyright/copyright.txt for details.

Program:   3D Slicer
Module:    $RCSfile: vtkMRMLdGEMRICProceduralColorNode.cxx,v $
Date:      $Date: 2006/03/03 22:26:39 $
Version:   $Revision: 1.0 $

=========================================================================auto=*/
#include <string>
#include <iostream>
#include <sstream>

#include "vtkObjectFactory.h"
#include "vtkCallbackCommand.h"

#include "vtkMRMLdGEMRICProceduralColorNode.h"
#include "vtkMRMLScene.h"

#include "vtkColorTransferFunction.h"

//------------------------------------------------------------------------------
vtkMRMLdGEMRICProceduralColorNode* vtkMRMLdGEMRICProceduralColorNode::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLdGEMRICProceduralColorNode");
  if(ret)
    {
    return (vtkMRMLdGEMRICProceduralColorNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLdGEMRICProceduralColorNode;
}

//-----------------------------------------------------------------------------

vtkMRMLNode* vtkMRMLdGEMRICProceduralColorNode::CreateNodeInstance()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret = vtkObjectFactory::CreateInstance("vtkMRMLdGEMRICProceduralColorNode");
  if(ret)
    {
    return (vtkMRMLdGEMRICProceduralColorNode*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkMRMLdGEMRICProceduralColorNode;
}


//----------------------------------------------------------------------------
vtkMRMLdGEMRICProceduralColorNode::vtkMRMLdGEMRICProceduralColorNode()
{

  // all this is done in the superclass...
  //this->Name = NULL;
  //this->SetName("");
  //this->FileName = NULL;

  //this->ColorTransferFunction = NULL;
  //this->ColorTransferFunction = vtkColorTransferFunction::New();
}

//----------------------------------------------------------------------------
vtkMRMLdGEMRICProceduralColorNode::~vtkMRMLdGEMRICProceduralColorNode()
{
  if (this->ColorTransferFunction)
    {
    this->ColorTransferFunction->Delete();
    this->ColorTransferFunction = NULL;
    }
}

//----------------------------------------------------------------------------
void vtkMRMLdGEMRICProceduralColorNode::WriteXML(ostream& of, int nIndent)
{
  // Write all attributes not equal to their defaults
  
  Superclass::WriteXML(of, nIndent);

}

//----------------------------------------------------------------------------
void vtkMRMLdGEMRICProceduralColorNode::ReadXMLAttributes(const char** atts)
{

  Superclass::ReadXMLAttributes(atts);
  
}


//----------------------------------------------------------------------------
// Copy the node's attributes to this object.
// Does NOT copy: ID, FilePrefix, Name, ID
void vtkMRMLdGEMRICProceduralColorNode::Copy(vtkMRMLNode *anode)
{
  Superclass::Copy(anode);
  //vtkMRMLdGEMRICProceduralColorNode *node = (vtkMRMLdGEMRICProceduralColorNode *) anode;
}

//----------------------------------------------------------------------------
void vtkMRMLdGEMRICProceduralColorNode::PrintSelf(ostream& os, vtkIndent indent)
{
  
  Superclass::PrintSelf(os,indent);
  if (this->ColorTransferFunction != NULL)
    {
    os << indent << "ColorTransferFunction:" << endl;
    this->ColorTransferFunction->PrintSelf(os, indent.GetNextIndent());
    }
}

//-----------------------------------------------------------

void vtkMRMLdGEMRICProceduralColorNode::UpdateScene(vtkMRMLScene *scene)
{
  Superclass::UpdateScene(scene);
}

//---------------------------------------------------------------------------
void vtkMRMLdGEMRICProceduralColorNode::ProcessMRMLEvents ( vtkObject *caller,
                                           unsigned long event, 
                                           void *callData )
{
  Superclass::ProcessMRMLEvents(caller, event, callData);
  return;
}

//---------------------------------------------------------------------------
void vtkMRMLdGEMRICProceduralColorNode::SetTypeTo15T()
{
  this->SetType(this->dGEMRIC15T);
}

//---------------------------------------------------------------------------
void vtkMRMLdGEMRICProceduralColorNode::SetTypeTo3T()
{
  this->SetType(this->dGEMRIC3T);
}


//---------------------------------------------------------------------------
const char *vtkMRMLdGEMRICProceduralColorNode::GetTypeAsIDString()
{
   if (this->Type == this->dGEMRIC15T)
    {
    return "vtkMRMLdGEMRICProceduralColorNode1.5T";
    }
  else if (this->Type == this->dGEMRIC3T)
    {
    return "vtkMRMLdGEMRICProceduralColorNode3T";
    }
   return "(unknown)";
}

//---------------------------------------------------------------------------
const char *vtkMRMLdGEMRICProceduralColorNode::GetTypeAsString()
{
  if (this->Type == this->dGEMRIC15T)
    {
    return "dGEMRIC-1.5T";
    }
  else if (this->Type == this->dGEMRIC3T)
    {
    return "dGEMRIC-3T";
    }
  return "(unknown)";
}

//---------------------------------------------------------------------------
void vtkMRMLdGEMRICProceduralColorNode::SetType(int type)
{

  this->Type = type;

  vtkDebugMacro(<< this->GetClassName() << " (" << this << "): setting Type to " << type << " = " << this->GetTypeAsString());

  // is it created yet?
  if (this->ColorTransferFunction == NULL)
    {
    this->ColorTransferFunction = vtkColorTransferFunction::New();
    }
  
  // clear it out
  this->ColorTransferFunction->RemoveAllPoints();
  this->ColorTransferFunction->SetColorSpaceToRGB();
  
  // Set up the custom colours here for this type
  if (this->Type == this->dGEMRIC15T)
    {
    this->SetDescription("Useful for displaying 1.5 tesla delayed gadolinium-enhanced MRI of cartilage");
    if (this->ColorTransferFunction->AddRGBPoint(0, 0.0, 0.0, 0.0) == -1)
      {
      vtkErrorMacro("SetType 1.5T: error adding point 0");
      }
    if (this->ColorTransferFunction->AddRGBPoint(150, 0.36, 0.04, 0.06) == -1)
      {
      vtkErrorMacro("SetType 1.5T: error adding point 150");
      }
    if (this->ColorTransferFunction->AddRGBPoint(277, 0.84, 0.4, 0.12) == -1)
      {
      vtkErrorMacro("SetType 1.5T: error adding point 277");
      }
    if (this->ColorTransferFunction->AddRGBPoint(404, 0.68, 0.87, 0.13) == -1)
      {
      vtkErrorMacro("SetType 1.5T: error adding point 404");
      }
    if (this->ColorTransferFunction->AddRGBPoint(531, 0.23, 0.9, 0.37) == -1)
      {
      vtkErrorMacro("SetType 1.5T: error adding point 531");
      }
    if (this->ColorTransferFunction->AddRGBPoint(658, 0.14, 0.74, 0.93) == -1)
      {
      vtkErrorMacro("SetType 1.5T: error adding point 658");
      }
    if (this->ColorTransferFunction->AddRGBPoint(785, 0.11, 0.17, 0.94) == -1)
      {
      vtkErrorMacro("SetType 1.5T: error adding point 785");
      }
    }
  else if (this->Type == this->dGEMRIC3T)
    {
    this->SetDescription("Useful for displaying 3 Tesla delayed gadolinium-enhanced MRI of cartilage");
    if (this->ColorTransferFunction->AddRGBPoint(0, 0.0, 0.0, 0.0) == -1)
      {
      vtkErrorMacro("SetType 1.5T: error adding point 0");
      }
    if (this->ColorTransferFunction->AddRGBPoint(200, 0.36, 0.04, 0.06) == -1)
      {
      vtkErrorMacro("SetType 1.5T: error adding point 200");
      }
    if (this->ColorTransferFunction->AddRGBPoint(415, 0.84, 0.4, 0.12) == -1)
      {
      vtkErrorMacro("SetType 1.5T: error adding point 415");
      }
    if (this->ColorTransferFunction->AddRGBPoint(630, 0.68, 0.87, 0.13) == -1)
      {
      vtkErrorMacro("SetType 1.5T: error adding point 630");
      }
    if (this->ColorTransferFunction->AddRGBPoint(845, 0.23, 0.9, 0.37) == -1)
      {
      vtkErrorMacro("SetType 1.5T: error adding point 845");
      }
    if (this->ColorTransferFunction->AddRGBPoint(1066.2, 0.14, 0.74, 0.93) == -1)
      {
      vtkErrorMacro("SetType 1.5T: error adding point 1066.2");
      }
    if (this->ColorTransferFunction->AddRGBPoint(1297.3, 0.11, 0.17, 0.94) == -1)
      {
      vtkErrorMacro("SetType 1.5T: error adding point 1297.3");
      }
    }

  // build it
  
  // invoke a modified event
  this->Modified();
  
  // invoke a type  modified event
  this->InvokeEvent(vtkMRMLProceduralColorNode::TypeModifiedEvent);
}
