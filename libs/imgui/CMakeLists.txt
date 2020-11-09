add_library(imgui

  imconfig.h
  imgui.cpp
  imgui.h
  imgui_draw.cpp
  imgui_internal.h
  imgui_widgets.cpp
  imstb_rectpack.h
  imstb_textedit.h
  imstb_truetype.h
  imgui_demo.cpp

  backends/imgui_impl_glfw.cpp
  backends/imgui_impl_glfw.h

  backends/imgui_impl_opengl2.cpp
  backends/imgui_impl_opengl2.h
)

target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
