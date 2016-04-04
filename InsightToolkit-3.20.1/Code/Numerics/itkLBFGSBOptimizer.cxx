/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkLBFGSBOptimizer.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkLBFGSBOptimizer_txx
#define __itkLBFGSBOptimizer_txx

#include "itkLBFGSBOptimizer.h"
#include "vnl/algo/vnl_lbfgsb.h"
#include <vnl/vnl_math.h>

extern "C" {
  extern double v3p_netlib_dpmeps_();
}

namespace itk
{


/** \class LBFGSBOptimizerHelper
 * \brief Wrapper helper around vnl_lbfgsb.
 *
 * This class is used to translate iteration events, etc, from
 * vnl_lbfgsb into iteration events in ITK.
 */
class ITK_EXPORT LBFGSBOptimizerHelper :
    public vnl_lbfgsb
{
public:
  typedef LBFGSBOptimizerHelper               Self;
  typedef vnl_lbfgsb                          Superclass;

  /** Create with a reference to the ITK object */
  LBFGSBOptimizerHelper( vnl_cost_function& f,
                         LBFGSBOptimizer* itkObj );

  /** Handle new iteration event */
  virtual bool report_iter();

private:
  LBFGSBOptimizer*     m_ItkObj;
};

  


/**
 * Constructor
 */
LBFGSBOptimizer
::LBFGSBOptimizer()
{
  m_OptimizerInitialized = false;
  m_VnlOptimizer         = 0;
  m_Trace                              = false;

  m_LowerBound       = InternalBoundValueType(0);
  m_UpperBound       = InternalBoundValueType(0); 
  m_BoundSelection   = InternalBoundSelectionType(0);

  m_CostFunctionConvergenceFactor   = 1e+7;
  m_ProjectedGradientTolerance      = 1e-5;
  m_MaximumNumberOfIterations       = 500;
  m_MaximumNumberOfEvaluations      = 500;
  m_MaximumNumberOfCorrections      = 5;
  m_CurrentIteration                = 0;
  m_InfinityNormOfProjectedGradient = 0.0;
  m_StopConditionDescription.str("");
 
}


/**
 * Destructor
 */
LBFGSBOptimizer
::~LBFGSBOptimizer()
{
  delete m_VnlOptimizer;
}

/**
 * PrintSelf
 */
void
LBFGSBOptimizer
::PrintSelf( std::ostream& os, Indent indent) const
{  
  Superclass::PrintSelf(os, indent);
  os << indent << "Trace: ";
  if ( m_Trace )
    {
    os << "On";
    }
  else
    { os << "Off";
    }
  os << std::endl;

  os << indent << "LowerBound: " << m_LowerBound << std::endl;
  os << indent << "UpperBound: " << m_UpperBound << std::endl;
  os << indent << "BoundSelection: " << m_BoundSelection << std::endl;

  os << indent << "CostFunctionConvergenceFactor: " << 
    m_CostFunctionConvergenceFactor << std::endl;

  os << indent << "ProjectedGradientTolerance: " <<
    m_ProjectedGradientTolerance << std::endl;

  os << indent << "MaximumNumberOfIterations: " <<
    m_MaximumNumberOfIterations << std::endl;

  os << indent << "MaximumNumberOfEvaluations: " <<
    m_MaximumNumberOfEvaluations << std::endl;

  os << indent << "MaximumNumberOfCorrections: " << 
    m_MaximumNumberOfCorrections << std::endl;

  os << indent << "CurrentIteration: " << 
    m_CurrentIteration << std::endl;

  os << indent << "Value: " <<
    this->GetValue() << std::endl;

  os << indent << "InfinityNormOfProjectedGradient: " <<
    m_InfinityNormOfProjectedGradient << std::endl;

  if( this->m_VnlOptimizer )
    {
    os << indent << "Vnl LBFGSB Failure Code: " << 
      this->m_VnlOptimizer->get_failure_code() << std::endl;
    }
}

/**
 * Set the optimizer trace flag
 */
void
LBFGSBOptimizer
::SetTrace( bool flag )
{
  if ( flag == m_Trace )
    {
    return;
    }

  m_Trace = flag;
  if ( m_OptimizerInitialized )
    {
    m_VnlOptimizer->set_trace( m_Trace );
    }

  this->Modified();
}

/**
 * Set lower bound
 */
void
LBFGSBOptimizer
::SetLowerBound(
const BoundValueType& value )
{
  m_LowerBound = value;
  if( m_OptimizerInitialized )
    {
    m_VnlOptimizer->set_lower_bound( m_LowerBound );
    }

  this->Modified();
}

/**
 * Get lower bound
 */
const
LBFGSBOptimizer 
::BoundValueType &
LBFGSBOptimizer
::GetLowerBound()
{
  return m_LowerBound;
} 

/** 
 * Return Current Value
 */
LBFGSBOptimizer::MeasureType
LBFGSBOptimizer
::GetValue() const
{
  return this->GetCachedValue();
}

/**
 * Set upper bound
 */
void
LBFGSBOptimizer
::SetUpperBound(
const BoundValueType& value )
{
  m_UpperBound = value;
  if( m_OptimizerInitialized )
    {
    m_VnlOptimizer->set_upper_bound( m_UpperBound );
    }

  this->Modified();
}

/**
 * Get upper bound
 */
const
LBFGSBOptimizer 
::BoundValueType &
LBFGSBOptimizer
::GetUpperBound()
{
  return m_UpperBound;
} 


/**
 * Set bound selection array
 */
void
LBFGSBOptimizer
::SetBoundSelection(
const BoundSelectionType& value )
{
  m_BoundSelection = value;
  if( m_OptimizerInitialized )
    {
    m_VnlOptimizer->set_bound_selection( m_BoundSelection );
    }
  this->Modified();
}

/**
 * Get bound selection array
 */
const
LBFGSBOptimizer 
::BoundSelectionType &
LBFGSBOptimizer
::GetBoundSelection()
{
  return m_BoundSelection;
} 


/** Set/Get the CostFunctionConvergenceFactor. Algorithm terminates
 * when the reduction in cost function is less than factor * epsmcj
 * where epsmch is the machine precision.
 * Typical values for factor: 1e+12 for low accuracy; 
 * 1e+7 for moderate accuracy and 1e+1 for extremely high accuracy.
 */
void
LBFGSBOptimizer
::SetCostFunctionConvergenceFactor( double value )
{
  if( value < 1.0 )
    {
    itkExceptionMacro("Value " << value 
      << " is too small for SetCostFunctionConvergenceFactor()"
      << "a typical range would be from 1.0 to 1e+12");
    }
  m_CostFunctionConvergenceFactor = value;
  if( m_OptimizerInitialized )
    {
    m_VnlOptimizer->set_cost_function_convergence_factor(
      m_CostFunctionConvergenceFactor );
    }
  this->Modified();
}


/** Set/Get the ProjectedGradientTolerance. Algorithm terminates
 * when the project gradient is below the tolerance. Default value
 * is 1e-5.
 */
void
LBFGSBOptimizer
::SetProjectedGradientTolerance( double value )
{
  m_ProjectedGradientTolerance = value;
  if( m_OptimizerInitialized )
    {
    m_VnlOptimizer->set_projected_gradient_tolerance(
      m_ProjectedGradientTolerance );
    }
  this->Modified();
}


/** Set/Get the MaximumNumberOfIterations. Default is 500 */
void
LBFGSBOptimizer
::SetMaximumNumberOfIterations( unsigned int value )
{
  m_MaximumNumberOfIterations = value;
  this->Modified();
}


/** Set/Get the MaximumNumberOfEvaluations. Default is 500 */
void
LBFGSBOptimizer
::SetMaximumNumberOfEvaluations( unsigned int value )
{
  m_MaximumNumberOfEvaluations = value;
  if( m_OptimizerInitialized )
    {
    m_VnlOptimizer->set_max_function_evals( m_MaximumNumberOfEvaluations );
    }
  this->Modified();
}


/** Set/Get the MaximumNumberOfCorrections. Default is 5 */
void
LBFGSBOptimizer
::SetMaximumNumberOfCorrections( unsigned int value )
{
  m_MaximumNumberOfCorrections = value;
  if( m_OptimizerInitialized )
    {
    m_VnlOptimizer->set_max_variable_metric_corrections(
      m_MaximumNumberOfCorrections );
    }
  this->Modified();
}


/**
 * Connect a Cost Function
 */
void
LBFGSBOptimizer::
SetCostFunction( SingleValuedCostFunction * costFunction )
{
  m_CostFunction = costFunction;

  const unsigned int numberOfParameters = 
    costFunction->GetNumberOfParameters();

  CostFunctionAdaptorType * adaptor = 
    new CostFunctionAdaptorType( numberOfParameters );
       
  adaptor->SetCostFunction( costFunction );

  if( m_OptimizerInitialized )
    { 
    delete m_VnlOptimizer;
    }
    
  this->SetCostFunctionAdaptor( adaptor );

  m_VnlOptimizer = new InternalOptimizerType( *adaptor, this );

  // set the optimizer parameters
  m_VnlOptimizer->set_lower_bound( m_LowerBound );
  m_VnlOptimizer->set_upper_bound( m_UpperBound );
  m_VnlOptimizer->set_bound_selection( m_BoundSelection );
  m_VnlOptimizer->set_cost_function_convergence_factor(
    m_CostFunctionConvergenceFactor );
  m_VnlOptimizer->set_projected_gradient_tolerance( 
    m_ProjectedGradientTolerance );
  m_VnlOptimizer->set_max_function_evals( m_MaximumNumberOfEvaluations );
  m_VnlOptimizer->set_max_variable_metric_corrections(
    m_MaximumNumberOfCorrections );

  m_OptimizerInitialized = true;

  this->Modified();
}


/**
 * Start the optimization
 */
void
LBFGSBOptimizer
::StartOptimization( void )
{
  
  // Check if all the bounds parameters are the same size as the initial parameters.
  unsigned int numberOfParameters = m_CostFunction->GetNumberOfParameters();

  if ( this->GetInitialPosition().Size() < numberOfParameters )
    {
    itkExceptionMacro( << "InitialPosition array does not have sufficient number of elements" );
    }

  if ( m_LowerBound.size() < numberOfParameters )
    {
    itkExceptionMacro( << "LowerBound array does not have sufficient number of elements" );
    }

  if ( m_UpperBound.size() < numberOfParameters )
    {
    itkExceptionMacro( << "UppperBound array does not have sufficient number of elements" );
    }

  if ( m_BoundSelection.size() < numberOfParameters )
    {
    itkExceptionMacro( << "BoundSelection array does not have sufficient number of elements" );
    }

  if( this->GetMaximize() )
    {
    this->GetNonConstCostFunctionAdaptor()->NegateCostFunctionOn();
    }

  this->SetCurrentPosition( this->GetInitialPosition() );

  ParametersType parameters( this->GetInitialPosition() );

  // Clear the description
  m_StopConditionDescription.str("");

  // vnl optimizers return the solution by reference 
  // in the variable provided as initial position
  m_VnlOptimizer->minimize( parameters );

  if ( parameters.size() != this->GetInitialPosition().Size() )
    {
    // set current position to initial position and throw an exception
    this->SetCurrentPosition( this->GetInitialPosition() );
    itkExceptionMacro( << "Error occured in optimization" );
    }

  this->SetCurrentPosition( parameters );

  this->InvokeEvent( EndEvent() );

}


/*-------------------------------------------------------------------------
 * helper class
 *-------------------------------------------------------------------------
 */

/** Create with a reference to the ITK object */
LBFGSBOptimizerHelper
::LBFGSBOptimizerHelper( vnl_cost_function& f,
                         LBFGSBOptimizer* itkObj )
  : vnl_lbfgsb( f ),
    m_ItkObj( itkObj )
{
}


/** Handle new iteration event */
bool
LBFGSBOptimizerHelper
::report_iter()
{
  Superclass::report_iter();

  m_ItkObj->m_InfinityNormOfProjectedGradient =
    this->get_inf_norm_projected_gradient();

  m_ItkObj->InvokeEvent( IterationEvent() );

  m_ItkObj->m_CurrentIteration = this->num_iterations_;

  // Return true to terminate the optimization loop.
  if( this->num_iterations_ > m_ItkObj->m_MaximumNumberOfIterations )
    {
    m_ItkObj->m_StopConditionDescription <<
      "Too many iterations. Iterations = " <<
      m_ItkObj->m_MaximumNumberOfIterations;
    return true;
    }
  else
    {
    return false;
    }
}

const std::string
LBFGSBOptimizer::
GetStopConditionDescription() const
{
  if (m_VnlOptimizer)
    {
    m_StopConditionDescription.str("");
    m_StopConditionDescription << this->GetNameOfClass() << ": ";
    switch (m_VnlOptimizer->get_failure_code())
      {
      case vnl_nonlinear_minimizer::ERROR_FAILURE:
        m_StopConditionDescription << "Failure";
        break;
      case vnl_nonlinear_minimizer::ERROR_DODGY_INPUT:
        m_StopConditionDescription << "Dodgy input";
        break;
      case vnl_nonlinear_minimizer::CONVERGED_FTOL:
        m_StopConditionDescription << "Function tolerance reached after "
                                   << m_CurrentIteration
                                   << " iterations. "
                                   << "The relative reduction of the cost function <= "
                                   << m_CostFunctionConvergenceFactor * v3p_netlib_dpmeps_()
                                   << " = CostFunctionConvergenceFactor ("
                                   << m_CostFunctionConvergenceFactor
                                   << ") * machine precision ("
                                   << v3p_netlib_dpmeps_()
                                   << ").";
        break;
      case vnl_nonlinear_minimizer::CONVERGED_XTOL:
        m_StopConditionDescription << "Solution tolerance reached";
        break;
      case vnl_nonlinear_minimizer::CONVERGED_XFTOL:
        m_StopConditionDescription << "Solution and Function tolerance both reached";
        break;
      case vnl_nonlinear_minimizer::CONVERGED_GTOL:
        m_StopConditionDescription << "Gradient tolerance reached. "
                                   << "Projected gradient tolerance is "
                                   << m_ProjectedGradientTolerance;
        break;
      case vnl_nonlinear_minimizer::FAILED_TOO_MANY_ITERATIONS:
        m_StopConditionDescription << "Too many iterations. Iterations = "
                                   << m_MaximumNumberOfEvaluations;
        break;
      case vnl_nonlinear_minimizer::FAILED_FTOL_TOO_SMALL:
        m_StopConditionDescription << "Function tolerance too small";
        break;
      case vnl_nonlinear_minimizer::FAILED_XTOL_TOO_SMALL:
        m_StopConditionDescription << "Solution tolerance too small";
        break;
      case vnl_nonlinear_minimizer::FAILED_GTOL_TOO_SMALL:
        m_StopConditionDescription << "Gradient tolerance too small";
        break;
      case vnl_nonlinear_minimizer::FAILED_USER_REQUEST:
        break;
      }
    return m_StopConditionDescription.str();
    }
  else
    {
    return std::string("");
    }
}

} // end namespace itk

#endif
