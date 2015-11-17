
/*

store the seed index in a vector

in the pick part:
\ to next;
; to previous;
c to cancel the last one seed;


parameters:

VTK 
1. file you want to open, *.mha

ITK
Input parameters:
2. name of input image, *.nii
3. name of output image, *.nii
4. sigma for DoG, 0.2
5. alpha for sigmoid mapping, 1/6
6. beta for sigmoid mapping, 20
7. time threshold for fastMarching, 20
8. sigma for gaussian smoothing, 1

*/




#include <cstdlib>
#include <iostream>

#include <vtkObjectFactory.h>
#include "vtkSmartPointer.h"
#include "vtkObject.h"
#include "vtkCommand.h"
#include <vtkNIFTIImageReader.h>
#include "vtkImageData.h"
#include "vtkImageResliceMapper.h"
#include "vtkImageSlice.h"
#include "vtkCamera.h"
#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkInteractorStyleImage.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkCellPicker.h"
#include "vector"
#include "vtkImageProperty.h"

#include "itkCurvatureAnisotropicDiffusionImageFilter.h"
#include "itkGradientMagnitudeRecursiveGaussianImageFilter.h"
#include "itkSigmoidImageFilter.h"
#include "itkFastMarchingImageFilter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkSmoothingRecursiveGaussianImageFilter.h"


//VTK part, pick the seeds

// 2DVector for seedIndex

std::vector< std::vector<int> > seedIndex;

typedef vtkSmartPointer< vtkCellPicker > PickerType;

//
//  Picking
//
class PrintPickCallback : public vtkCommand
{
public:

  static PrintPickCallback *New();
  vtkTypeMacro( PrintPickCallback, vtkCommand );

  virtual void Execute(vtkObject *, unsigned long event, void *)
    {
    int idx[3];
    std::vector<int> seed;
    picker->GetPointIJK( idx );
    std::cout << idx[0] << " " << idx[1] << " " << idx[2] << std::endl;
    for(int i = 0; i <3; i++)
        seed.push_back(idx[i]);
    seedIndex.push_back(seed);
    
    /*
    //test
    std::vector< std::vector<int> > ::iterator it;
    
    for (it = seedIndex.begin(); it != seedIndex.end(); it++)
        {
            std::vector<int> allseed;
            allseed = *it;
            std::cout << allseed[0] << " " << allseed[1] << " " << allseed[2] << std::endl;
        }
       */
     }
     

  void SetPicker( PickerType p ) { picker = p; }

private:
  PickerType picker;
};
vtkStandardNewMacro( PrintPickCallback );


//
// Advancing interactor
//
class AdvancingInteractorStyle : public vtkInteractorStyleImage
{
public:
  static AdvancingInteractorStyle * New();
  vtkTypeMacro( AdvancingInteractorStyle, vtkInteractorStyleImage );

  void AdvanceSlice( double step )
    {
    vtkRenderWindowInteractor *rwi = this->Interactor;
    double fp[3];
    double normal[3];
    vtkRenderer * renderer = this->GetCurrentRenderer();
    vtkCamera * camera = renderer->GetActiveCamera();
    camera->GetFocalPoint( fp );
    camera->GetViewPlaneNormal( normal );
    fp[0] += normal[0] * step;
    fp[1] += normal[1] * step;
    fp[2] += normal[2] * step;
    camera->SetFocalPoint( fp );
    rwi->Render();
    }

  virtual void OnChar()
    {
    // Get the keypress
    vtkRenderWindowInteractor *rwi = this->Interactor;
    char key = rwi->GetKeyCode();
    
    if ( key == 'c' )
    {
       //if made a mistake, then click 'c' to cancle last seed
       seedIndex.pop_back(); 
       /*
       //test
        std::vector< std::vector<int> > ::iterator it;
    
        for (it = seedIndex.begin(); it != seedIndex.end(); it++)
            {
                std::vector<int> allseed;
                allseed = *it;
                std::cout << allseed[0] << " " << allseed[1] << " " << allseed[2] << std::endl;
            }
        */
     }
    
    if ( key == '\'' )
      {
      //std::cout << "up" << std::endl;
      AdvanceSlice( 1.0 );
      }
    if ( key == ';' )
      {
      //std::cout << "down" << std::endl;
      AdvanceSlice( -1.0 );
      }
    // Forward events
    vtkInteractorStyleImage::OnChar();
    }
};
vtkStandardNewMacro( AdvancingInteractorStyle );

int main( int argc, char ** argv )
{
  char * in_file = argv[1];

  //typedef vtkSmartPointer< vtkMetaImageReader > VReaderType;
  typedef vtkSmartPointer< vtkNIFTIImageReader > VReaderType;
  VReaderType Vreader = VReaderType::New();
  Vreader->SetFileName( in_file );
  Vreader->Update();

  typedef vtkSmartPointer< vtkImageResliceMapper > MapperType;
  MapperType mapper = MapperType::New();
  mapper->SetInputConnection( Vreader->GetOutputPort() );
  mapper->SliceAtFocalPointOn();
  mapper->SliceFacesCameraOn();
  
  typedef vtkSmartPointer< vtkImageProperty > PropertyType;
  PropertyType property = PropertyType::New();
  property->SetColorWindow(700);
  property->SetColorLevel(1000);
  property->SetAmbient(0.0);
  property->SetDiffuse(1.0);
  property->SetOpacity(1.0);
  property->SetInterpolationTypeToLinear(); 

  typedef vtkSmartPointer< vtkImageSlice > ActorType;  
  ActorType actor = ActorType::New();
  actor->SetMapper( mapper );
  actor->SetProperty(property);

  typedef vtkSmartPointer< vtkRenderer > RenderereType;
  RenderereType renderer = RenderereType::New();
  renderer->SetBackground( 0.2, 0.2, 0.2 );  
  renderer->AddActor(  actor );

  typedef vtkSmartPointer< vtkRenderWindow > WindowType;
  WindowType window = WindowType::New();
  window->AddRenderer( renderer );

  typedef vtkSmartPointer< AdvancingInteractorStyle > StyleType;
  StyleType style = StyleType::New();
  style->SetInteractionModeToImage3D();
  style->SetCurrentRenderer( renderer );

  typedef vtkSmartPointer< PrintPickCallback > PrintType;
  PrintType print_pick = PrintType::New();

  PickerType picker = PickerType::New();
  picker->AddObserver( vtkCommand::EndPickEvent, print_pick );

  print_pick->SetPicker( picker ); 

  typedef vtkSmartPointer< vtkRenderWindowInteractor > InteractorType;
  InteractorType interactor = InteractorType::New();
  interactor->SetInteractorStyle( style );
  interactor->SetPicker( picker );
  
  
  interactor->SetRenderWindow( window );
  interactor->Initialize();

  window->SetInteractor(interactor);
  window->Render();
  
  interactor->Start();
  
  
  //ITK part
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

    reader->SetFileName(argv[2]);//parameter 1
	reader->UpdateOutputInformation();
	InternalImageType::Pointer image = reader->GetOutput();
    writer->SetFileName(argv[3]);//parameter 2

	//smoothing
	smoothing->SetTimeStep(0.05);
	smoothing->SetNumberOfIterations(5);
	smoothing->SetConductanceParameter(0.5);
	
	
	//Dog
	//The sigma of this Gaussian can be used to control
  	//the range of influence of the image edges
	const double sigma = atof(argv[4]);
	gradientMagnitude->SetSigma(sigma);

	//sigmoid mapping
	sigmoid->SetOutputMinimum(0.0);
	sigmoid->SetOutputMaximum(1.0);
	const double alpha = atof(argv[5]);
	const double beta  = atof(argv[6]);
	sigmoid->SetAlpha(alpha);
	sigmoid->SetBeta(beta);

	//smoothing->SetInput(reader->GetOutput());
	//gradientMagnitude->SetInput(reader->GetOutput());
	//sigmoid->SetInput(reader->GetOutput());
    fastMarching->SetInput(reader->GetOutput());
	thresholder->SetInput(fastMarching->GetOutput());
	gaussianFilter->SetInput(thresholder->GetOutput());
    writer->SetInput(gaussianFilter->GetOutput());

	
	//gaussian smoothing sigma
	gaussianFilter->SetSigma(atof(argv[8]));
	typedef FastMarchingFilterType::NodeContainer	NodeContainer;
	typedef FastMarchingFilterType::NodeType		NodeType;
	NodeContainer::Pointer	node = NodeContainer::New();
	
	//read index from a seed_file
	//char * seed_file = argv[7];
	//std::ifstream ifs (seed_file);
	int id = 0;
	
	std::vector< std::vector<int> > ::iterator it;
	InternalImageType::IndexType SeedPosition;
    
    for (it = seedIndex.begin(); it != seedIndex.end(); it++)
        {
            std::vector<int> allseed;
            allseed = *it;
            SeedPosition[0] = allseed[0];
            SeedPosition[1] = allseed[1];
            SeedPosition[2] = allseed[2];
            
            NodeType    seed;
            const double SeedValue = 0.0;
            seed.SetValue(SeedValue);
            seed.SetIndex(SeedPosition);
            node->InsertElement(id, seed);
            id++;
            }
	
	
	std::cout << id << std::endl;	
	fastMarching->SetTrialPoints(node);
	fastMarching->SetStoppingValue(300);

	

    //time threshold for fast marching<--not time, it's time * speed
    const InternalPixelType timeThreshold = atof (argv[7]);

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
