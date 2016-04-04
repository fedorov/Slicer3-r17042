#include "vtkObjectFactory.h"
#include "vtkSlicerCommonInterface.h"

#include <sstream>


#ifdef Slicer3_USE_KWWIDGETS

  // Slicer3
#include "vtkKWTkUtilities.h"
#include "../../Applications/GUI/Slicer3Helper.cxx"

#endif

//----------------------------------------------------------------------------
vtkSlicerCommonInterface* vtkSlicerCommonInterface::New()
{
  // First try to create the object from the vtkObjectFactory
  vtkObject* ret =
    vtkObjectFactory::CreateInstance("vtkSlicerCommonInterface");
  if(ret)
    {
    return (vtkSlicerCommonInterface*)ret;
    }
  // If the factory was unable to create the object, then create it here.
  return new vtkSlicerCommonInterface;
}


//----------------------------------------------------------------------------
vtkSlicerCommonInterface::vtkSlicerCommonInterface() {

}

//----------------------------------------------------------------------------
vtkSlicerCommonInterface::~vtkSlicerCommonInterface() {

}

//----------------------------------------------------------------------------
Tcl_Interp* vtkSlicerCommonInterface::GetTclInterpeter(int argc, char *argv[], ostream *err)
{
#ifdef Slicer3_USE_KWWIDGETS

  // Slicer3
  return vtkSlicerApplication::InitializeTcl(argc, argv, err);

#endif

  return 0;
}

//----------------------------------------------------------------------------

int vtkSlicerCommonInterface::SourceTclFile(const char *tclFile)
{

#ifdef Slicer3_USE_KWWIDGETS

  // Slicer3

  // Load Tcl File defining the setting
  if (!vtkSlicerApplication::GetInstance()->LoadScript(tclFile))
    {
      return 1;
    }

#endif

  return 0;

}

//-----------------------------------------------------------------------------
const char* vtkSlicerCommonInterface::EvaluateTcl(const char* command)
{

#ifdef Slicer3_USE_KWWIDGETS

  // Slicer3
  return vtkSlicerApplication::GetInstance()->Script(command);
  //#else

  // TODO Slicer4

#endif

  return 0;
}

//-----------------------------------------------------------------------------
const char* vtkSlicerCommonInterface::GetTclNameFromPointer(vtkObject *obj)
{

#ifdef Slicer3_USE_KWWIDGETS

  // Slicer3
  return vtkKWTkUtilities::GetTclNameFromPointer(vtkSlicerApplication::GetInstance()->GetMainInterp(),obj);
  //#else

  // TODO Slicer4

#endif

  return 0;
}

//-----------------------------------------------------------------------------
void vtkSlicerCommonInterface::RegisterObjectWithTcl(vtkObject* obj, const char* name)
{

#ifdef Slicer3_USE_KWWIDGETS

  const char* tclName = this->GetTclNameFromPointer(obj);

  std::stringstream cmd;
  cmd << "namespace eval slicer3 set ";
  cmd << std::string(name) + " ";
  cmd << std::string(tclName);

  this->EvaluateTcl(cmd.str().c_str());


#endif

}

//-----------------------------------------------------------------------------
void vtkSlicerCommonInterface::InitializePythonViaTcl(Tcl_Interp* interp, int argc, char **argv)
{

#if defined(Slicer3_USE_KWWIDGETS) && defined(Slicer3_USE_PYTHON)
  // Initialize Python

  #if defined(_WIN32) || defined(WIN32)
  #define PathSep ";"
  #else
  #define PathSep ":"
  #endif

  vtksys_stl::string slicerHome;
  if ( !vtksys::SystemTools::GetEnv("Slicer3_HOME", slicerHome) )
    {
    slicerHome = std::string(this->GetBinDirectory()) + "/..";
    slicerHome = vtksys::SystemTools::CollapseFullPath(slicerHome.c_str());
    }

  // Set up the search path
  std::string pythonEnv = "PYTHONPATH=";

  const char* existingPythonEnv = vtksys::SystemTools::GetEnv("PYTHONPATH");
  if ( existingPythonEnv )
    {
    pythonEnv += std::string ( existingPythonEnv ) + PathSep;
    }

  pythonEnv += slicerHome + "/" + Slicer3_INSTALL_LIB_DIR + "/SlicerBaseGUI/Python" + PathSep;
  pythonEnv += slicerHome + "/" + Slicer3_INSTALL_PLUGINS_BIN_DIR + PathSep;
  vtkKWApplication::PutEnv(const_cast <char *> (pythonEnv.c_str()));

  Py_Initialize();
  PySys_SetArgv(argc, argv);
  PyObject* PythonModule = PyImport_AddModule("__main__");
  if (PythonModule == NULL)
    {
    std::cout << "Warning: Failed to initialize python" << std::endl;
    }
  PyObject* PythonDictionary = PyModule_GetDict(PythonModule);

  // Intercept _tkinter, and use ours...
  init_mytkinter(vtkSlicerApplication::GetInstance()->GetMainInterp());
  init_slicer();
  PyObject* v;

  std::vector<std::string> pythonInitStrings;

  pythonInitStrings.push_back(std::string("import _tkinter;"));
  pythonInitStrings.push_back(std::string("import Tkinter;"));
  pythonInitStrings.push_back(std::string("import sys;"));
  pythonInitStrings.push_back(std::string("from os.path import join as j;"));
  pythonInitStrings.push_back(std::string("tk = Tkinter.Tk();"));
  pythonInitStrings.push_back(std::string("sys.path.append ( j('" + slicerHome + "','" + Slicer3_INSTALL_LIB_DIR + "', 'SlicerBaseGUI', 'Python')" + " );"));
  pythonInitStrings.push_back(std::string("sys.path.append ( j('" + slicerHome + "','" + Slicer3_INSTALL_PLUGINS_BIN_DIR + "') );"));

  /*
  std::string TkinitString = "import Tkinter, sys;"
    "from os.path import join as j;"
    "tk = Tkinter.Tk();"
    "sys.path.append ( j('"
    + slicerHome + "','" + Slicer3_INSTALL_LIB_DIR + "', 'SlicerBaseGUI', 'Python')"
    + " );\n"
    "sys.path.append ( j('"
    + slicerHome + "','" + Slicer3_INSTALL_PLUGINS_BIN_DIR
    + "') );\n";
    */

  std::vector<std::string>::iterator strIt;
  strIt = pythonInitStrings.begin();
  for (; strIt != pythonInitStrings.end(); strIt++)
    {
    v = PyRun_String( (*strIt).c_str(),
                      Py_file_input,
                      PythonDictionary,
                      PythonDictionary );
    if (v == NULL)
      {
      PyObject *exception, *v, *tb;
      PyObject *exception_s, *v_s, *tb_s;

      PyErr_Fetch(&exception, &v, &tb);
      if (exception != NULL)
        {
        PyErr_NormalizeException(&exception, &v, &tb);
        if (exception != NULL)
          {
          exception_s = PyObject_Str(exception);
          v_s = PyObject_Str(v);
          tb_s = PyObject_Str(tb);
          const char *e_string, *v_string, *tb_string;
          cout << "Running: " << (*strIt).c_str() << endl;
          e_string = PyString_AS_STRING(exception_s);
          cout << "Exception: " << e_string << endl;
          v_string = PyString_AS_STRING(v_s);
          cout << "V: " << v_string << endl;
          tb_string = PyString_AS_STRING(PyObject_Str(tb_s));
          cout << "TB: " << tb_string << endl;
          Py_DECREF ( exception_s );
          Py_DECREF ( v_s );
          Py_DECREF ( tb_s );
          Py_DECREF ( exception );
          Py_DECREF ( v );
          if ( tb )
            {
            Py_DECREF ( tb );
            }
          }
        }

      PyErr_Print();
      }
    else
      {
      if (Py_FlushLine())
        {
        PyErr_Clear();
        }
      }
    }

  vtkSlicerApplication::GetInstance()->InitializePython(
    (void*)PythonModule, (void*)PythonDictionary);


#endif

}

//-----------------------------------------------------------------------------
void vtkSlicerCommonInterface::EvaluatePython(const char* command)
{

#if defined(Slicer3_USE_KWWIDGETS) && defined(Slicer3_USE_PYTHON)

  PyObject* PythonModule = PyImport_AddModule("__main__");
  if (PythonModule == NULL)
    {
    std::cout << "Warning: Failed to initialize python" << std::endl;
    }
  PyObject* PythonDictionary = PyModule_GetDict(PythonModule);

  //command = "from Slicer import slicer; print slicer.vtkSlicerVolumesLogic();";

  PyObject* v = PyRun_String( command,
                    Py_file_input,
                    PythonDictionary,PythonDictionary);
  if (v == NULL)
    {
    PyErr_Print();
    }

#endif

}


//-----------------------------------------------------------------------------
const char* vtkSlicerCommonInterface::GetApplicationTclName()
{
#ifdef Slicer3_USE_KWWIDGETS

  return this->GetTclNameFromPointer(vtkSlicerApplication::GetInstance());

#endif

  return 0;
}

//-----------------------------------------------------------------------------
const char* vtkSlicerCommonInterface::GetTemporaryDirectory()
{

#ifdef Slicer3_USE_KWWIDGETS

  // Slicer3
  return vtkSlicerApplication::GetInstance()->GetTemporaryDirectory();

#else

  // Slicer4
  return qSlicerApplication::application()->temporaryPath().toLatin1();

#endif

}

//-----------------------------------------------------------------------------
const char* vtkSlicerCommonInterface::GetBinDirectory()
{

#ifdef Slicer3_USE_KWWIDGETS

  // Slicer3
  return vtkSlicerApplication::GetInstance()->GetBinDir();

//#else

  // Slicer4
  //return qSlicerApplication::application()->temporaryPath().toLatin1();

#endif

  return 0;

}


//-----------------------------------------------------------------------------
const char* vtkSlicerCommonInterface::GetRepositoryRevision()
{

#ifdef Slicer3_USE_KWWIDGETS

  // Slicer3
  return vtkSlicerApplication::GetInstance()->GetSvnRevision();

#else

  // Slicer4
  return qSlicerApplication::application()->repositoryRevision().toLatin1();

#endif

  return 0;

}

//-----------------------------------------------------------------------------
void vtkSlicerCommonInterface::SetApplicationBinDir(const char* bindir)
{
#ifdef Slicer3_USE_KWWIDGETS

  vtkSlicerApplication::GetInstance()->SetBinDir(bindir);

#endif
}

//-----------------------------------------------------------------------------
vtkHTTPHandler* vtkSlicerCommonInterface::GetHTTPHandler(vtkMRMLScene* scene)
{

#ifdef Slicer3_USE_KWWIDGETS

  // Slicer3
  return vtkHTTPHandler::SafeDownCast(scene->FindURIHandlerByName("HTTPHandler"));

#else

  // Slicer4
  // TODO
  return 0;

#endif

  return 0;

}

//-----------------------------------------------------------------------------
void vtkSlicerCommonInterface::PromptBeforeExitOff()
{

#ifdef Slicer3_USE_KWWIDGETS

  vtkSlicerApplication::GetInstance()->PromptBeforeExitOff();

#endif

}

//-----------------------------------------------------------------------------
void vtkSlicerCommonInterface::DestroySlicerApplication()
{

#ifdef Slicer3_USE_KWWIDGETS

  vtkSlicerApplication* app = vtkSlicerApplication::GetInstance();

  if (app)
    {
      app->Exit();
      app->Delete();
      app = NULL;
    }

#endif

}

//-----------------------------------------------------------------------------
void vtkSlicerCommonInterface::AddDataIOToScene(vtkMRMLScene* mrmlScene, vtkSlicerApplicationLogic *appLogic, vtkDataIOManagerLogic *dataIOManagerLogic)
{

#ifdef Slicer3_USE_KWWIDGETS

  // Slicer3
  Slicer3Helper::AddDataIOToScene(mrmlScene,vtkSlicerApplication::GetInstance(),appLogic,dataIOManagerLogic);

#endif

}

//-----------------------------------------------------------------------------
void vtkSlicerCommonInterface::RemoveDataIOFromScene(vtkMRMLScene* mrmlScene, vtkDataIOManagerLogic *dataIOManagerLogic)
{

#ifdef Slicer3_USE_KWWIDGETS

  // Slicer3
  Slicer3Helper::RemoveDataIOFromScene(mrmlScene,dataIOManagerLogic);

#endif

}
