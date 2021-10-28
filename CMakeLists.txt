cmake_minimum_required(VERSION 3.7)
project(spectacle-analyzer)

find_package(PkgConfig)
find_package(Threads REQUIRED)
pkg_check_modules(fftw3f "fftw3f" REQUIRED IMPORTED_TARGET)
add_subdirectory("dpf")

option(USE_IMPATIENT_FFT_PLANNING "Use impatient FFT planning. Accelerates the build-run cycle when enabled." OFF)
option(SKIP_FFT_PRECACHING "Skip FFT precaching. Accelerates the build-run cycle when enabled." OFF)

dpf_add_plugin(spectacle-analyzer
  TARGETS lv2 vst2 jack
  UI_TYPE opengl
  FILES_DSP
    "sources/plugin/PluginSpectralAnalyzer.cpp"
    "sources/plugin/Parameters.cpp"
    "sources/dsp/FFTPlanner.cpp"
    "sources/dsp/SpectralAnalyzer.cpp"
    "sources/dsp/STFT.cpp"
    "sources/dsp/MultirateSTFT.cpp"
    "thirdparty/spin_mutex/src/SpinMutex.cpp"
    "thirdparty/rt_semaphore/src/RTSemaphore.cpp"
  FILES_UI
    "sources/plugin/UISpectralAnalyzer.cpp"
    "sources/plugin/Parameters.cpp"
    "sources/plugin/Config.cpp"
    "sources/plugin/ColorPalette.cpp"
    "sources/plugin/FontDefs.cpp"
    "sources/ui/components/SpectrumView.cpp"
    "sources/ui/components/MainToolBar.cpp"
    "sources/ui/components/FloatingWindow.cpp"
    "sources/ui/components/SpinBoxChooser.cpp"
    "sources/ui/components/Slider.cpp"
    "sources/ui/components/TextLabel.cpp"
    "sources/ui/components/SelectionRectangle.cpp"
    "sources/ui/components/ResizeHandle.cpp"
    "sources/ui/FontEngine.cpp"
    "sources/util/format_string.cpp"
    "thirdparty/spline/spline/spline.cpp"
    "thirdparty/simpleini/ConvertUTF.cpp")

function(add_gz_resources TARGET)
  foreach(_stem IN LISTS ARGN)
    set(_in "${CMAKE_SOURCE_DIR}/resources/${_stem}.gz")
    set(_out "${CMAKE_BINARY_DIR}/res/${_stem}.h")
    get_filename_component(_dir "${_out}" DIRECTORY)
    add_custom_command(
      OUTPUT "${_out}"
      COMMAND "${CMAKE_COMMAND}" "-E" "make_directory" "${_dir}"
      COMMAND "${CMAKE_SOURCE_DIR}/scripts/gzbin2c.sh" "${_in}" > "${_out}"
      DEPENDS "${_in}")
    target_sources("${TARGET}" PRIVATE "${_out}")
  endforeach()
endfunction()

add_gz_resources(spectacle-analyzer
  "fonts/liberation/LiberationSans-Regular.ttf"
  "fonts/fontawesome/Font-Awesome-5-Free-Solid-900.otf")

target_compile_definitions(spectacle-analyzer
  PUBLIC
    "_GNU_SOURCE=1")

target_include_directories(spectacle-analyzer
  PUBLIC
    "sources"
    "plugins/spectacle/meta"
    "${CMAKE_BINARY_DIR}/res"
    "thirdparty/blink"
    "thirdparty/spline"
    "thirdparty/stb"
    "thirdparty/simpleini"
    "thirdparty/hiir-1.33"
    "thirdparty/spin_mutex/src"
    "thirdparty/rt_semaphore/src")

target_link_libraries(spectacle-analyzer
  PUBLIC
    PkgConfig::fftw3f
    Threads::Threads)
