/*
Input parameters:
1. name of input image
2. name of output image
3. sigma for DoG, 0.01
4. alpha for sigmoid mapping, 1/100
5. beta for sigmoid mapping, 13
6. time threshold for fastMarching, 30
7. name of seed file, seedIndex 
8. sigma for gaussian smoothing, 2
*/



#include "itkCurvatureAnisotropicDiffusionImageFilter.h"
#include "itkGradientMagnitudeRecursiveGaussianImageFilter.h"
#include "itkSigmoidImageFilter.h"
#include "itkFastMarchingImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkSmoothingRecursiveGaussianImageFilter.h"

int main ( int argc, char* argv[])
{
    //define input images
    typedef     float		        InternalPixelType;
    const       unsigned int        Dimension = 3;
    typedef itk::Image<InternalPixelType, Dimension>    InternalImageType;
    
    //define output images
    typedef     float       OutputPixelType;
    typedef itk::Image<OutputPixelType, Dimension>   OutputImageType;

    //set threshold for binary image
    typedef itk::BinaryThresholdImageFilter<InternalImageType, OutputImageType> ThresholdingFilterType;
    
    //read image, then write image
    typedef itk::ImageFileReader<InternalImageType> ReaderType;
    typedef itk::ImageFileWriter<OutputImageType> WriterType;
	typedef itk::CurvatureAnisotropicDiffusionImageFilter<InternalImageType, InternalImageType> SmoothingFilterType;
	typedef itk::GradientMagnitudeRecursiveGaussianImageFilter<InternalImageType,InternalImageType> GradientFilterType;
	typedef itk::SigmoidImageFilter<InternalImageType, InternalImageType> SigmoidFilterType;
    typedef itk::FastMarchingImageFilter<InternalImageType,
                                    InternalImageType>  FastMarchingFilterType;
    typedef itk::SmoothingRecursiveGaussianImageFilter<InternalImageType, InternalImageType> GaussianSmoothingType;

	
	
    ReaderType::Pointer  			reader = ReaderType::New();
    WriterType::Pointer  			writer = WriterType::New();
	SmoothingFilterType::Pointer	smoothing = SmoothingFilterType::New();
	GradientFilterType::Pointer		gradientMagnitude = GradientFilterType::New();
	SigmoidFilterType::Pointer	sigmoid = SigmoidFilterType::New();
    FastMarchingFilterType::Pointer   fastMarching = FastMarchingFilterType::New();
	ThresholdingFilterType::Pointer thresholder = ThresholdingFilterType::New();
	GaussianSmoothingType::Pointer  gaussianFilter = GaussianSmoothingType::New();

    reader->SetFileName(argv[1]);//parameter 1
	reader->UpdateOutputInformation();
	InternalImageType::Pointer image = reader->GetOutput();
    writer->SetFileName(argv[2]);//parameter 2

	//smoothing
	smoothing->SetTimeStep(0.025);
	smoothing->SetNumberOfIterations(5);
	smoothing->SetConductanceParameter(0.15);
	
	
	//Dog
	//The sigma of this Gaussian can be used to control
  	//the range of influence of the image edges
	const double sigma = atof(argv[3]);
	gradientMagnitude->SetSigma(sigma);

	//sigmoid mapping
	sigmoid->SetOutputMinimum(0.0);
	sigmoid->SetOutputMaximum(1.0);
	const double alpha = atof(argv[4]);
	const double beta  = atof(argv[5]);
	sigmoid->SetAlpha(alpha);
	sigmoid->SetBeta(beta);

	smoothing->SetInput(reader->GetOutput());
	//gradientMagnitude->SetInput(reader->GetOutput());
	//sigmoid->SetInput(reader->GetOutput());
    fastMarching->SetInput(reader->GetOutput());
	thresholder->SetInput(fastMarching->GetOutput());
	gaussianFilter->SetInput(thresholder->GetOutput());
    writer->SetInput(smoothing->GetOutput());

	
	//gaussian smoothing sigma
	gaussianFilter->SetSigma(atof(argv[8]));
	typedef FastMarchingFilterType::NodeContainer	NodeContainer;
	typedef FastMarchingFilterType::NodeType		NodeType;
	NodeContainer::Pointer	node = NodeContainer::New();
	
	//read index from a seed_file
	char * seed_file = argv[7];
	std::ifstream ifs (seed_file);
	int id = 0;
	while (ifs.good())
	{
		InternalImageType::IndexType SeedPosition;
		ifs >> SeedPosition[0] >> SeedPosition[1] >> SeedPosition[2];
		if (ifs.good())
			{
				NodeType	seed;
				const double seedValue = 0.0;
	
				seed.SetValue(seedValue);
				seed.SetIndex(SeedPosition);
				node->InsertElement(id,seed);
				id++;
			}
	}
	
	std::cout << id << std::endl;	
	fastMarching->SetTrialPoints(node);
	fastMarching->SetStoppingValue(300);

	

    //time threshold for fast marching<--not time, it's time * speed
    const InternalPixelType timeThreshold = atof (argv[6]);

    //Set the threshold for image showing
    thresholder->SetLowerThreshold(0.0);
    thresholder->SetUpperThreshold(timeThreshold);

    //Mapping the threshold to the output image
    thresholder->SetOutsideValue(0);
    thresholder->SetInsideValue(255);

	






    //test result
    writer->Update();

    return EXIT_SUCCESS;
    



}
