/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    BayesianClassifierInitializer.cxx
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#if defined(_MSC_VER)
#pragma warning ( disable : 4786 )
#endif

//
// This is an example of the itk::BayesianClassifierInitializationImageFilter.
// The example's goal is to serve as an initializer for the 
// BayesianClassifier.cxx example also found in this directory. 
// 
// This example takes an input image (to be classified) and generates membership
// images. The membership images determine the degree to which each pixel
// belongs to a class. 
//
// The membership image generated by the filter is an 
// itk::VectorImage, (with pixels organized as follows: For a 2D image,
// its essentially a 3D array on file with DataType[y][x][c] where c is the 
// number of classes and DataType is the template parameter of the filter 
// (defaults to float). For a 3D image, it will be organized as 
// Datatype[z][y][x][c])
// 
// The example also optionally takes in two more arguments, as a convenience to 
// the user. These arguements extract the specified component 'c' from the 
// membership image and rescale, so the user can fire up a typical image
// viewer and see the relative pixel memberships to class 'c'.
//
// Example args:
//   BrainProtonDensitySlice.png Memberships.mhd 4  2 Class2.png
// 
// Here Memberships.mhd will be a 2x2x4 image containing pixel memberships
// Class2.png shows pixel memberships to the third class, (rescaled for display)
// 
// Notes:
//   The default behaviour of the filter is to generate memberships by centering
// gaussian density functions around K-means of the pixel intensities in the 
// image. The filter allows you to specify your own membership functions as well.
// 

#include "itkImage.h"
#include "itkBayesianClassifierInitializationImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"

int main(int argc, char *argv[])
{

  const unsigned int Dimension = 2;
  if( argc < 4 )
    {
    std::cerr << "Usage arguments: InputImage MembershipImage numberOfClasses [componentToExtract ExtractedImage]" << std::endl;
    std::cerr << "  The MembershipImage image written is a VectorImage, ( an image with multiple components ) ";
    std::cerr << "Given that most viewers can't see vector images, we will optionally extract a component and ";
    std::cerr << "write it out as a scalar image as well." << std::endl;
    return EXIT_FAILURE;
    }

  typedef itk::Image< unsigned char, Dimension > ImageType;
  typedef itk::BayesianClassifierInitializationImageFilter< ImageType > 
                                                BayesianInitializerType;
  BayesianInitializerType::Pointer bayesianInitializer 
                                          = BayesianInitializerType::New();

  typedef itk::ImageFileReader< ImageType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( argv[1] );

  try
    {
    reader->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception thrown " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  bayesianInitializer->SetInput( reader->GetOutput() );
  bayesianInitializer->SetNumberOfClasses( atoi( argv[3] ) );

  // TODO add test where we specify membership functions
  
  typedef itk::ImageFileWriter< 
    BayesianInitializerType::OutputImageType >  WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( bayesianInitializer->GetOutput() );
  writer->SetFileName( argv[2] );
  
  try
    {
    bayesianInitializer->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception thrown " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  try
    {
    writer->Update();
    }
  catch( itk::ExceptionObject & excp )
    {
    std::cerr << "Exception thrown " << std::endl;
    std::cerr << excp << std::endl;
    return EXIT_FAILURE;
    }

  if( argv[4] && argv[5] )
    {
    typedef BayesianInitializerType::OutputImageType MembershipImageType;
    typedef itk::Image< MembershipImageType::InternalPixelType, 
                        Dimension > ExtractedComponentImageType;
    ExtractedComponentImageType::Pointer extractedComponentImage = 
                                    ExtractedComponentImageType::New();
    extractedComponentImage->CopyInformation( 
                          bayesianInitializer->GetOutput() );
    extractedComponentImage->SetBufferedRegion( bayesianInitializer->GetOutput()->GetBufferedRegion() );
    extractedComponentImage->SetRequestedRegion( bayesianInitializer->GetOutput()->GetRequestedRegion() );
    extractedComponentImage->Allocate();
    typedef itk::ImageRegionConstIterator< MembershipImageType > ConstIteratorType;
    typedef itk::ImageRegionIterator< ExtractedComponentImageType > IteratorType;
    ConstIteratorType cit( bayesianInitializer->GetOutput(), 
                     bayesianInitializer->GetOutput()->GetBufferedRegion() );
    IteratorType it( extractedComponentImage, 
                     extractedComponentImage->GetLargestPossibleRegion() );
    
    const unsigned int componentToExtract = atoi( argv[4] );
    cit.GoToBegin();
    it.GoToBegin();
    while( !cit.IsAtEnd() )
      {
      it.Set(cit.Get()[componentToExtract]);
      ++it;
      ++cit;
      }

    // Write out the rescaled extracted component
    typedef itk::Image< unsigned char, Dimension > OutputImageType;
    typedef itk::RescaleIntensityImageFilter< 
      ExtractedComponentImageType, OutputImageType > RescalerType;
    RescalerType::Pointer rescaler = RescalerType::New();
    rescaler->SetInput( extractedComponentImage );
    rescaler->SetOutputMinimum( 0 );
    rescaler->SetOutputMaximum( 255 );
    typedef itk::ImageFileWriter<  OutputImageType
                        >  ExtractedComponentWriterType;
    ExtractedComponentWriterType::Pointer 
               rescaledImageWriter = ExtractedComponentWriterType::New();
    rescaledImageWriter->SetInput( rescaler->GetOutput() );
    rescaledImageWriter->SetFileName( argv[5] );
    rescaledImageWriter->Update();
    }

  return EXIT_SUCCESS;
}

