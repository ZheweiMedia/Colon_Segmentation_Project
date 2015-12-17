#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkFlatStructuringElement.h"
#include "itkBinaryDilateImageFilter.h"

int main(int argc, char *argv[])
{
  if( argc < 4 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " <inputImage> <outputImage> <radius>";
    std::cerr << std::endl;
    return EXIT_FAILURE;
    }
  const char * inputImage = argv[1];
  const char * outputImage = argv[2];
  const unsigned int radiusValue = atoi( argv[3] );

  typedef unsigned char PixelType;
  const unsigned int Dimension = 3;

  typedef itk::Image< PixelType, Dimension >    ImageType;
  typedef itk::ImageFileReader< ImageType >     ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( inputImage );

  typedef itk::FlatStructuringElement< Dimension >
    StructuringElementType;
  StructuringElementType::RadiusType radius;
  radius.Fill( radiusValue );
  StructuringElementType structuringElement =
    StructuringElementType::Ball( radius );

  typedef itk::BinaryDilateImageFilter< ImageType, ImageType,
    StructuringElementType > BinaryDilateImageFilterType;

  BinaryDilateImageFilterType::Pointer dilateFilter =
    BinaryDilateImageFilterType::New();
  dilateFilter->SetInput( reader->GetOutput() );
  dilateFilter->SetKernel( structuringElement );

  typedef itk::ImageFileWriter< ImageType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( dilateFilter->GetOutput() );
  writer->SetFileName( outputImage );

  try
    {
    writer->Update();
    }
  catch( itk::ExceptionObject & error )
    {
    std::cerr << "Error: " << error << std::endl;
    return EXIT_FAILURE;
    }

  return EXIT_SUCCESS;
}
