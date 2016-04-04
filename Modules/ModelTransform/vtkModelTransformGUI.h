#ifndef __vtkModelTransformGUI_h
#define __vtkModelTransformGUI_h

#include "vtkModelTransformWin32Header.h"
#include "vtkSlicerModuleGUI.h"

#include "vtkMRMLScene.h"
#include "vtkMRMLModelTransformNode.h"
#include "vtkModelTransformLogic.h"


// Had to add this to force loading of these support libraries
// Only libraries with entries that will be dynamically called from TCL need to be instantiated
#include "vtkTcl.h"
extern "C" int Vtkslicermodeltransformmodulelogic_Init(Tcl_Interp *interp);

class vtkKWFrame;
class vtkSlicerNodeSelectorWidget;
class vtkSlicerModuleCollapsibleFrame;
class vtkKWPushButton;
class vtkKWEntry;
class vtkKWCheckButtonWithLabel;

class vtkIntArray;

class VTK_MODELTRANSFORM_EXPORT vtkModelTransformGUI : public vtkSlicerModuleGUI
{
  public:
  static vtkModelTransformGUI *New();
  vtkTypeMacro(vtkModelTransformGUI,vtkSlicerModuleGUI);
  void PrintSelf(ostream& os, vtkIndent indent);

  vtkGetObjectMacro ( ParameterSelector, vtkSlicerNodeSelectorWidget );
  vtkGetObjectMacro ( ModelSelector, vtkSlicerNodeSelectorWidget );
  vtkGetObjectMacro ( TransformSelector, vtkSlicerNodeSelectorWidget );
  vtkGetObjectMacro ( ModelOutSelector, vtkSlicerNodeSelectorWidget );

  // Description:
  // Set the logic pointer from parent class pointer.
  // Overloads implementation in vtkSlicerModulesGUI
  // to allow loadable modules.
  virtual void SetModuleLogic ( vtkSlicerLogic *logic )
  {
  this->SetLogic(reinterpret_cast<vtkModelTransformLogic*> (logic)); 
  }

   // Description: Get/Set MRML node
  vtkGetObjectMacro (Logic, vtkModelTransformLogic);
  vtkSetObjectMacro (Logic, vtkModelTransformLogic);

  virtual void BuildGUI ( );
  //BTX
  using vtkSlicerComponentGUI::BuildGUI; 
  //ETX
  virtual void TearDownGUI ( );

  vtkIntArray *NewObservableEvents();

  // Description:
  // Add obsereves to GUI widgets
  virtual void AddGUIObservers ( );

  // Description:
  // Remove obsereves to GUI widgets
  virtual void RemoveGUIObservers ( );
  virtual void RemoveMRMLNodeObservers ( );
  
  // Description:
  // Pprocess events generated by GUI widgets
  virtual void ProcessGUIEvents ( vtkObject *caller, unsigned long event,
                                  void *callData );

  // Description:
  // Pprocess events generated by MRML
  virtual void ProcessMRMLEvents ( vtkObject *caller, unsigned long event, 
                                  void *callData);
  // Description:
  // Describe behavior at module startup and exit.
  virtual void Enter ( );
  //BTX
  using vtkSlicerComponentGUI::Enter; 
  //ETX
  virtual void Exit ( );
  virtual void Init ( );

  // Description:
  // Get the categorization of the module.  The category is used for
  // grouping modules together into menus.
  const char *GetCategory() const {return "Registration";}


protected:
  vtkModelTransformGUI();
  ~vtkModelTransformGUI();
  vtkModelTransformGUI(const vtkModelTransformGUI&);
  void operator=(const vtkModelTransformGUI&);

  vtkMRMLModelTransformNode   *ModelTransformNode;
  vtkMRMLModelNode            *ModelNode;
  vtkMRMLTransformNode        *TransformNode;

  vtkModelTransformLogic  *Logic;


  // Description:
  // Updates GUI widgets based on parameters values in MRML node
  void UpdateGUI();

  void DoTransformModel();

  // Description:
  // updates status text in the slicer window.
  void SetSlicerText( const char *txt);

  void CreateParameterNode();

  void UpdateParameterNode();

  vtkSlicerNodeSelectorWidget *ParameterSelector;
  vtkSlicerNodeSelectorWidget *ModelSelector;
  vtkSlicerNodeSelectorWidget *TransformSelector;
  vtkSlicerNodeSelectorWidget *ModelOutSelector;
  vtkKWCheckButtonWithLabel   *TransformNormalsButton;
  vtkSlicerModuleCollapsibleFrame *SpecificationFrame;
 
  int UpdatingGUI;
  int UpdatingMRML;

 
  // Description:
  // This flag gets set when the module is entered,
  // and unset when the module is exited. It is used
  // to put observers on and off the MRML Scene.
  bool Raised;

};

#endif

