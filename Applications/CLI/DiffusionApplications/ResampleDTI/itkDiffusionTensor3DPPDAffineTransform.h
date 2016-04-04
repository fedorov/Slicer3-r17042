/*=========================================================================

  Program:   Diffusion Applications
  Module:    $HeadURL: http://svn.slicer.org/Slicer3/branches/Slicer-3-6/Applications/CLI/DiffusionApplications/ResampleDTI/itkDiffusionTensor3DPPDAffineTransform.h $
  Language:  C++
  Date:      $Date: 2010-04-07 17:38:41 -0400 (Wed, 07 Apr 2010) $
  Version:   $Revision: 12682 $

  Copyright (c) Brigham and Women's Hospital (BWH) All Rights Reserved.

  See License.txt or http://www.slicer.org/copyright/copyright.txt for details.

==========================================================================*/
#ifndef __itkDiffusionTensor3DPPDAffineTransform_h
#define __itkDiffusionTensor3DPPDAffineTransform_h


#include "itkDiffusionTensor3DAffineTransform.h"
#include <vnl/vnl_vector.h>
#include <vnl/vnl_cross.h>


namespace itk
{
/**
 * \class DiffusionTensor3DFSAffineTransform
 * 
 * 
 * This class implements an affine transformation for diffusion tensors. It implements the 
 * Preservation of Principal Direction method presented in the following paper:
 * D.C. Alexander, Member IEEE, C. Pierpaoli, P.J. Basser and J.C Gee: 
 * Spatial Transformations of Diffusion Tensor Magnetic Resonance Images, 
 * IEEE Transactions on Medical Imaging, Vol 20, No. 11, November 2001 
 * 
 */
template< class TData >
class DiffusionTensor3DPPDAffineTransform :
  public DiffusionTensor3DAffineTransform< TData >
{
public:
  typedef TData DataType ;
  typedef DiffusionTensor3DPPDAffineTransform Self ;
  typedef DiffusionTensor3DAffineTransform< DataType > Superclass ;
  typedef typename Superclass::TensorDataType TensorDataType ;
  typedef typename Superclass::MatrixDataType MatrixDataType ;
  typedef typename Superclass::MatrixTransformType MatrixTransformType ;
  typedef typename Superclass::InternalTensorDataType InternalTensorDataType ;
  typedef typename Superclass::InternalMatrixDataType InternalMatrixDataType ;
  typedef typename Superclass::InternalMatrixTransformType InternalMatrixTransformType ;
  typedef SmartPointer< Self > Pointer ;
  typedef SmartPointer< const Self > ConstPointer ;
  typedef typename Superclass::VectorType VectorType ;
  typedef DiffusionTensor3DExtended< double >::EigenValuesArrayType EValuesType ;
  typedef DiffusionTensor3DExtended< double >::EigenVectorsMatrixType EVectorsType ;
  itkNewMacro( Self ) ;
  TensorDataType EvaluateTransformedTensor( TensorDataType &tensor ) ;
  void SetMatrix( MatrixTransformType &matrix ) ;
protected:
  void PreCompute() ;
  InternalMatrixTransformType ComputeMatrixFromAxisAndAngle( VectorType axis , double cosangle ) ;
  InternalMatrixTransformType m_TransformMatrixInverse ;
};

}//end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkDiffusionTensor3DPPDAffineTransform.txx"
#endif

#endif