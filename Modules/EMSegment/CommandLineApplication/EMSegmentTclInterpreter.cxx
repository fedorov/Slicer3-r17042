#include "EMSegmentTclInterpreterCLP.h"
#include "EMSegmentAPIHelper.h"


// -============================
// MAIN
// -============================

int main(int argc, char** argv)
{
  if (argc < 2) {
    cout << "Error: No Input Defined "<< endl;
    return EXIT_FAILURE;
  }

  // =======================================================================
  //  Initialize TCL
  // =======================================================================

  // SLICER COMMON INTERFACE
  vtkSlicerCommonInterface *slicerCommon = vtkSlicerCommonInterface::New();

  // interp has to be set to initialize vtkSlicer
  Tcl_Interp *interp = CreateTclInterp(argc,argv,slicerCommon);
  if (!interp)
    {
      return EXIT_FAILURE;
    }

  vtkSlicerApplicationLogic* appLogic = InitializeApplication(interp,slicerCommon,argc,argv);
  if (!appLogic)
    {
      CleanUp(appLogic,slicerCommon);
      return EXIT_FAILURE;
    }

  // =======================================================================
  //  Souce TCL File
  // =======================================================================
  int exitFlag = EXIT_SUCCESS;

  try
    {
      std::string CMD = std::string("source ") + argv[1] ;
      slicerCommon->EvaluateTcl(CMD.c_str());
    }
  catch (...)
    {
      exitFlag = EXIT_FAILURE;
    }

  // =======================================================================
  //  Clean up
  // =======================================================================
  CleanUp(appLogic,slicerCommon);
  return exitFlag;
}
