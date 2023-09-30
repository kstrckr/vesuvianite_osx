file(GLOB HEADER_LIST CONFIGURE_DEPENDS "${Vesuvianite_SRC_DIR}/include/vesuvianite/*.hpp")

# add_library(rawLoaderLib RawLoader.cxx ${HEADER_LIST})
# add_library(houghLines HoughLines.cxx ${HEADER_LIST})
# add_library(convexHull ConvexHull.cxx ${HEADER_LIST})
# add_library(IsolateSubject IsolateSubject.cxx ${HEADER_LIST})
# add_library(GetScaledIsolationRect GetScaledIsolationRect.cxx ${HEADER_LIST})
# add_library(SubjectPlacement INTERFACE ${HEADER_LIST})
add_library(SourceImage SourceImage.cxx ${HEADER_LIST})
add_library(XmpWriter XmpWriter.cxx ${HEADER_LIST})

find_package(OpenCV)
if(OpenCV_FOUND)
  message("Opencv dirs == ${OpenCV_INCLUDE_DIRS}")
  include_directories( ${OpenCV_INCLUDE_DIRS} )
  target_link_libraries( SourceImage ${OpenCV_LIBS} )
endif()

include_directories(${PROJECT_SOURCE_DIR}/XMP-Toolkit-SDK-in-vesuvianite)
include_directories(${PROJECT_SOURCE_DIR}/XMP-Toolkit-SDK-in-vesuvianite/public/include)

target_link_libraries(
  XmpWriter
  ${CMAKE_DL_LIBS}
  # ${PROJECT_SOURCE_DIR}/XMP-Toolkit-SDK-in-vesuvianite/public/libraries/macintosh/universal/Debug/libXMPCoreStatic.a
  ${PROJECT_SOURCE_DIR}/XMP-Toolkit-SDK-in-vesuvianite/public/libraries/macintosh/intel_64_libcpp/Debug/libXMPCoreStatic.a
  # ${PROJECT_SOURCE_DIR}/XMP-Toolkit-SDK-in-vesuvianite/public/libraries/macintosh/universal/Debug/libXMPFilesStatic.a
  ${PROJECT_SOURCE_DIR}/XMP-Toolkit-SDK-in-vesuvianite/public/libraries/macintosh/intel_64_libcpp/Debug/libXMPFilesStatic.a
)

include_directories(${PROJECT_SOURCE_DIR}/libraw_static/libraw)

target_link_libraries(
  SourceImage
  ${CMAKE_DL_LIBS}
  ${PROJECT_SOURCE_DIR}/libraw_static/lib/libraw_r.a
  ${PROJECT_SOURCE_DIR}/libraw_static/lib/libraw.a
)


target_include_directories(XmpWriter PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_features(XmpWriter PUBLIC cxx_std_20)

# target_include_directories(rawLoaderLib PUBLIC ${PROJECT_SOURCE_DIR}/include)
# target_compile_features(rawLoaderLib PUBLIC cxx_std_20)

# target_include_directories(houghLines PUBLIC ${PROJECT_SOURCE_DIR}/include)
# target_compile_features(houghLines PUBLIC cxx_std_20)

# target_include_directories(convexHull PUBLIC ${PROJECT_SOURCE_DIR}/include)
# target_compile_features(convexHull PUBLIC cxx_std_20)

# target_include_directories(IsolateSubject PUBLIC ${PROJECT_SOURCE_DIR}/include)
# target_compile_features(IsolateSubject PUBLIC cxx_std_20)

# target_include_directories(GetScaledIsolationRect PUBLIC ${PROJECT_SOURCE_DIR}/include)
# target_compile_features(GetScaledIsolationRect PUBLIC cxx_std_20)

# target_include_directories(SubjectPlacement INTERFACE ${PROJECT_SOURCE_DIR}/include)
# target_compile_features(SubjectPlacement INTERFACE cxx_std_20)

target_include_directories(SourceImage PUBLIC ${PROJECT_SOURCE_DIR}/include)
target_compile_features(SourceImage PUBLIC cxx_std_20)