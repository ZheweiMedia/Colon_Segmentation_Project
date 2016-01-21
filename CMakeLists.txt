CMAKE_MINIMUM_REQUIRED(VERSION 2.8)
 
PROJECT(ITK_CODE)
 
FIND_PACKAGE(ITK REQUIRED)
INCLUDE(${ITK_USE_FILE})
IF (ITKVtkGlue_LOADED)
  FIND_PACKAGE(VTK REQUIRED)
  INCLUDE(${VTK_USE_FILE})
ELSE()
  FIND_PACKAGE(ItkVtkGlue REQUIRED)
  INCLUDE(${ItkVtkGlue_USE_FILE})
  SET(Glue ItkVtkGlue)
ENDIF()

ADD_EXECUTABLE(CheckForModule CheckForModule.cxx)
TARGET_LINK_LIBRARIES(CheckForModule ${ITK_LIBRARIES})

ADD_EXECUTABLE(QuickViewDemo MACOSX_BUNDLE QuickViewDemo.cxx)
TARGET_LINK_LIBRARIES(QuickViewDemo
    ${Glue}  ${VTK_LIBRARIES} ${ITK_LIBRARIES})

ADD_EXECUTABLE(ImageFileReader ImageFileReader.cxx)
TARGET_LINK_LIBRARIES(ImageFileReader 
    ${Glue} ${VTK_LIBRARIES} ${ITK_LIBRARIES})

ADD_EXECUTABLE(fastMarchingExample fastMarchingExample.cxx)
TARGET_LINK_LIBRARIES(fastMarchingExample
    ${Glue} ${VTK_LIBRARIES} ${ITK_LIBRARIES})

ADD_EXECUTABLE(test_fastMarching test_fastMarching.cxx)
TARGET_LINK_LIBRARIES(test_fastMarching
    ${Glue} ${VTK_LIBRARIES} ${ITK_LIBRARIES})

ADD_EXECUTABLE(gradient_test gradient_test.cxx)
TARGET_LINK_LIBRARIES(gradient_test
    ${Glue} ${VTK_LIBRARIES} ${ITK_LIBRARIES})

ADD_EXECUTABLE(MetaImageReader MetaImageReader.cxx)
TARGET_LINK_LIBRARIES(MetaImageReader
    ${VTK_LIBRARIES})

ADD_EXECUTABLE(readNII readNII.cxx)
TARGET_LINK_LIBRARIES(readNII
    ${Glue} ${VTK_LIBRARIES} ${ITK_LIBRARIES})

ADD_EXECUTABLE(fastMarchingNII fastMarchingNII.cxx)
TARGET_LINK_LIBRARIES(fastMarchingNII
    ${Glue} ${VTK_LIBRARIES} ${ITK_LIBRARIES})

ADD_EXECUTABLE(NiiEdgeEnhancement NiiEdgeEnhancement.cxx)
TARGET_LINK_LIBRARIES(NiiEdgeEnhancement
	${ITK_LIBRARIES})

ADD_EXECUTABLE(2dPick 2dPick.cxx)
TARGET_LINK_LIBRARIES(2dPick
	${VTK_LIBRARIES})

ADD_EXECUTABLE(ImageConvert ImageConvert.cxx)
TARGET_LINK_LIBRARIES(ImageConvert
	${ITK_LIBRARIES})
	
	
ADD_EXECUTABLE(GVF GVF.cxx)
TARGET_LINK_LIBRARIES(GVF
    ${ITK_LIBRARIES})
    
ADD_EXECUTABLE(3dPick 3dPick.cxx)
TARGET_LINK_LIBRARIES(3dPick
    ${VTK_LIBRARIES})
    
ADD_EXECUTABLE(3dPick_inVector 3dPick_inVector.cxx)
TARGET_LINK_LIBRARIES(3dPick_inVector
    ${VTK_LIBRARIES})
    
ADD_EXECUTABLE(Pick_FastMarching Pick_FastMarching.cxx)
TARGET_LINK_LIBRARIES(Pick_FastMarching
    ${VTK_LIBRARIES} ${ITK_LIBRARIES})
    
ADD_EXECUTABLE(Modify_Nii Modify_Nii.cxx)
TARGET_LINK_LIBRARIES(Modify_Nii
    ${ITK_LIBRARIES})
    
ADD_EXECUTABLE(PostProcess postProcess.cxx)
TARGET_LINK_LIBRARIES(PostProcess
    ${ITK_LIBRARIES})
    
ADD_EXECUTABLE(dilate dilate.cxx)
TARGET_LINK_LIBRARIES(dilate
    ${ITK_LIBRARIES})
    
ADD_EXECUTABLE(erosion erosion.cxx)
TARGET_LINK_LIBRARIES(erosion
    ${ITK_LIBRARIES})
    
ADD_EXECUTABLE(connectedThreshold connectedThreshold.cxx)
TARGET_LINK_LIBRARIES(connectedThreshold
    ${ITK_LIBRARIES})
