cmake_minimum_required(VERSION 3.26)
project(warp-sabre LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(ImageMagick REQUIRED COMPONENTS MagickWand)
include_directories(${ImageMagick_INCLUDE_DIRS})
add_compile_options(${ImageMagick_COMPILE_OPTIONS})
add_compile_options(-DUSE_MT_GENTILES)
add_compile_options(-Wall)

find_package(Boost REQUIRED COMPONENTS system thread)
find_package(LibXml2 REQUIRED)

include_directories(${LIBXML2_INCLUDE_DIR})

add_library(morph STATIC
    src/libmorph/my_malloc.c
    src/libmorph/warp.c
    src/libmorph/mesh.c
    src/libmorph/spline.c
    src/libmorph/tga.c
    src/libmorph/RgbaImage.c
    src/libmorph/spl-array.c
    src/libmorph/warp2.c
    src/libmorph/relax.c
    src/libmorph/resample.c
)

target_compile_options(morph PRIVATE -DREAL=double)

add_library(newmat STATIC
    src/newmat-10/newmat1.cpp
    src/newmat-10/newmat2.cpp
    src/newmat-10/newmat3.cpp
    src/newmat-10/newmat4.cpp
    src/newmat-10/newmat5.cpp
    src/newmat-10/newmat6.cpp
    src/newmat-10/newmat7.cpp
    src/newmat-10/newmat8.cpp
    src/newmat-10/newmatex.cpp
    src/newmat-10/bandmat.cpp
    src/newmat-10/submat.cpp
    src/newmat-10/myexcept.cpp
    src/newmat-10/cholesky.cpp
    src/newmat-10/evalue.cpp
    src/newmat-10/fft.cpp
    src/newmat-10/hholder.cpp
    src/newmat-10/jacobi.cpp
    src/newmat-10/newfft.cpp
    src/newmat-10/sort.cpp
    src/newmat-10/svd.cpp
    src/newmat-10/newmatrm.cpp
    src/newmat-10/newmat9.cpp
)

add_executable(warp-sabre
    src/ImageWarpByFunc.cpp
    src/ImgMagick.cpp
    src/ReadDelimitedFile.cpp
    src/ProgramOptions.cpp
    src/StringUtils.cpp
    src/Tile.cpp
    src/TransformPoly.cpp
    src/WarpOs.cpp
    src/WriteKml.cpp
    src/ganzc/LatLong-OSGBconversion.cpp
    src/gbos1936/Gbos1936.cpp
)

add_executable(gentiles
    src/CopyPixels.cpp
    src/GenTiles.cpp
    src/GetBounds.cpp
    src/ImageWarpByFunc.cpp
    src/ImgMagick.cpp
    src/ProgramOptions.cpp
    src/ReadDelimitedFile.cpp
    src/ReadKmlFile.cpp
    src/SourceKml.cpp
    src/StringUtils.cpp
    src/Tile.cpp
    src/gbos1936/Gbos1936.cpp
)

add_executable(cleartiles
    src/ClearTiles.cpp
    src/CopyPixels.cpp
    src/GetBounds.cpp
    src/ImageWarpByFunc.cpp
    src/ImgMagick.cpp
    src/ProgramOptions.cpp
    src/ReadDelimitedFile.cpp
    src/ReadKmlFile.cpp
    src/SourceKml.cpp
    src/StringUtils.cpp
    src/Tile.cpp
    src/gbos1936/Gbos1936.cpp
)

add_executable(clearbounds
    src/ClearBounds.cpp
    src/CopyPixels.cpp
    src/GetBounds.cpp
    src/ImageWarpByFunc.cpp
    src/ImgMagick.cpp
    src/ProgramOptions.cpp
    src/ReadDelimitedFile.cpp
    src/ReadKmlFile.cpp
    src/SourceKml.cpp
    src/StringUtils.cpp
    src/Tile.cpp
    src/gbos1936/Gbos1936.cpp
)

add_executable(testcas
    src/TestCas.cpp
    src/gbos1936/Gbos1936.cpp
)

target_include_directories(warp-sabre PUBLIC
    ${PROJECT_SOURCE_DIR}/libmorph
    ${PROJECT_SOURCE_DIR}/newmat-10
)

target_link_libraries(warp-sabre
    PRIVATE ImageMagick::MagickWand
    PRIVATE Boost::system
    PRIVATE morph
    PRIVATE newmat
)

target_include_directories(gentiles PUBLIC
    ${PROJECT_SOURCE_DIR}/libmorph
)

target_link_libraries(gentiles
    PRIVATE ImageMagick::MagickWand
    PRIVATE xml2
    PRIVATE Boost::thread
    PRIVATE Boost::system
    PRIVATE morph
)

target_include_directories(cleartiles PUBLIC
    ${PROJECT_SOURCE_DIR}/libmorph
)

target_link_libraries(cleartiles
    PRIVATE ImageMagick::MagickWand
    PRIVATE xml2
    PRIVATE Boost::thread
    PRIVATE Boost::system
    PRIVATE morph
)

target_include_directories(clearbounds PUBLIC
    ${PROJECT_SOURCE_DIR}/libmorph
)

target_link_libraries(clearbounds
    PRIVATE ImageMagick::MagickWand
    PRIVATE xml2
    PRIVATE Boost::thread
    PRIVATE Boost::system
    PRIVATE morph
)