#include "itkImage.h"
#include "itkBinaryErodeImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkBinaryBallStructuringElement.h"
 
 
int main(int argc, char *argv[])
{
  if( argc < 4 )
    {
    std::cerr << "Usage: " << std::endl;
    std::cerr << argv[0] << " <inputImage> <outputImage> <radius>";
    std::cerr << std::endl;
    return EXIT_FAILURE;
    }
 
  unsigned int radius = atoi(argv[3]);
  const char * outputImage = argv[2];

  const unsigned int Dimension = 3;
  typedef itk::Image<unsigned char, Dimension>    ImageType;
  typedef itk::ImageFileReader<ImageType> ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(argv[1]);
 
  typedef itk::BinaryBallStructuringElement<
    ImageType::PixelType, Dimension>                  StructuringElementType;
  StructuringElementType structuringElement;
  structuringElement.SetRadius(radius);
  structuringElement.CreateStructuringElement();
 
  typedef itk::BinaryErodeImageFilter <ImageType, ImageType, StructuringElementType>
    BinaryErodeImageFilterType;
 
  BinaryErodeImageFilterType::Pointer erodeFilter
    = BinaryErodeImageFilterType::New();
  erodeFilter->SetInput(reader->GetOutput());
  erodeFilter->SetKernel(structuringElement);
 
  typedef itk::ImageFileWriter< ImageType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetInput( erodeFilter->GetOutput() );
  writer->SetFileName( outputImage );
  
  writer->Update();
  return EXIT_SUCCESS;
}
