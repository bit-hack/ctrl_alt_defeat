// Dear ImGui: standalone example application for GLFW + OpenGL2, using legacy fixed pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

// **DO NOT USE THIS CODE IF YOUR CODE/ENGINE IS USING MODERN OPENGL (SHADERS, VBO, VAO, etc.)**
// **Prefer using the code in the example_glfw_opengl2/ folder**
// See imgui_impl_glfw.cpp for details.

#include <cstdio>
#include <thread>

#include <GLFW/glfw3.h>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl2.h"

#include "../rv32i_model/model.h"

#include "encoding.h"


enum {
  MEM_BASE = 0x1000,
  MEM_SIZE = 0x1000,
};


bool will_continue = false;

static void glfw_error_callback(int error, const char* description) {
  fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

static const char *regname(uint32_t i) {
  static const char *name[32] = {
    "zero",
    "ra",
    "sp",
    "gp",
    "tp",
    "t0", "t1", "t2",
    "fp", "s1",
    "a0", "a1", "a2", "a3", "a4", "a5", "a6", "a7",
    "s2", "s3", "s4", "s5", "s6", "s7", "s8", "s9", "s10", "s11",
    "t3", "t4", "t5", "t6",
  };
  return name[i & 0x1f];
}

void gui_registers(rv32i_model_t &model) {

  ImGui::SetNextWindowPos(ImVec2{ 16, 16 });
  ImGui::SetNextWindowSize(ImVec2{ 16 * 9, 16 * 39 });
  ImGui::Begin("REGISTERS", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

  ImGui::Columns(2);
  ImGui::Separator();
  ImGui::Text("Name");
  ImGui::NextColumn();
  ImGui::Text("Value");
  ImGui::Separator();
  ImGui::NextColumn();

  ImGui::Text("pc");
  ImGui::NextColumn();
  ImGui::Text("%08x", model.get_pc());
  ImGui::NextColumn();
  for (uint32_t i = 0; i < 32; ++i) {
    ImGui::Text("%s", regname(i));
    ImGui::NextColumn();
    const uint32_t value = model.get_reg(i);
    if (value) {
      ImGui::Text("%08x", value);
    }
    else {
      ImGui::TextDisabled("%08x", value);
    }
    ImGui::NextColumn();
  }
  ImGui::Columns(1);
  ImGui::End();
}

void gui_memory(rv32i_model_t &model) {

  ImGui::SetNextWindowPos(ImVec2{ 16 * 11, 16 });
  ImGui::SetNextWindowSize(ImVec2{ 16 * 36, 16 * 20 });

  ImGui::Begin("MEMORY", nullptr, ImGuiWindowFlags_HorizontalScrollbar |
                                  ImGuiWindowFlags_NoResize |
                                  ImGuiWindowFlags_NoMove);

  uint8_t temp[16];
  ImGuiListClipper clipper{ MEM_SIZE / 16, ImGui::GetTextLineHeight() };

  const uint32_t pc = model.get_pc();

  while (clipper.Step()) {
    for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i) {
      const uint32_t addr = MEM_BASE + i * 16;

      model.memory().read(temp, addr, 16);

      ImGui::Text("%08x: ", addr);

      for (int i = 0; i < 16; ++i) {
        ImGui::SameLine();
        ImGui::Text("%02x", temp[i]);
      }

      for (int i = 0; i < 16; ++i) {

      }
    }
  }
  ImGui::End();
}

void gui_cpu(rv32i_model_t &model) {

  ImGui::SetNextWindowPos(ImVec2{ 16 * 48, 16 });
  ImGui::SetNextWindowSize(ImVec2{ 16 * 15, 16 * 4 });

  ImGui::Begin("CPU", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);
  if (ImGui::Button("Step")) {
    model.step();
  }
  ImGui::SameLine();
  if (ImGui::Button("Run")) {
    will_continue = true;
  }
  ImGui::SameLine();
  if (ImGui::Button("Halt")) {
    will_continue = false;
  }
  ImGui::SameLine();
  if (ImGui::Button("Reset")) {
    model.reset();
  }
  ImGui::End();
}

void gui_disassemble(rv32i_model_t &model) {
  ImGui::SetNextWindowPos(ImVec2{ 16 * 11, 16 * 22 });
  ImGui::SetNextWindowSize(ImVec2{ 16 * 36, 16 * 20 });
  ImGui::Begin("DISASSEMBLY", nullptr, ImGuiWindowFlags_HorizontalScrollbar |
                                       ImGuiWindowFlags_NoResize |
                                       ImGuiWindowFlags_NoMove);

  char temp[1024];
  ImGuiListClipper clipper{ MEM_SIZE / 4, ImGui::GetTextLineHeight() };

  const uint32_t pc = model.get_pc();

  while (clipper.Step()) {
    for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i) {
      const uint32_t addr = MEM_BASE + i * 4;
      const uint32_t raw = model.memory().read_w(addr);

      rv_inst_t dec;
      if (rv_decode(raw, dec)) {
        temp[0] = '\0';
        rv_print(dec, temp, sizeof(temp));
        if (addr == pc) {
          ImGui::TextColored(ImVec4{ 0.8f, 0.7f, 0.2f, 1.f }, "%08x:  %08x  %s", addr, raw, temp);
        }
        else {
          ImGui::Text("%08x:  %08x  %s", addr, raw, temp);
        }
      }
      else {
        ImGui::TextDisabled("%08x:  %08x  %s", addr, raw, "unknown");
      }
    }
  }

  ImGui::End();
}

int main(int, char**) {
  rv32i_model_t model;
  model.reset();

#if 0
  if (!model.load_elf("C:/repos/ctrl_alt_defeat/rv32i_model/tests/compliance/elf/I-ADD-01.elf")) {
#else
  if (!model.load_hex("C:/repos/ctrl_alt_defeat/rom.hex", MEM_BASE)) {
#endif
    return 1;
  }

  // Setup window
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit())
      return 1;
  GLFWwindow* window = glfwCreateWindow(1024, 768, "Ctrl-Alt-Defeat", NULL, NULL);
  if (window == NULL)
      return 1;
  glfwMakeContextCurrent(window);
  glfwSwapInterval(1);

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO(); (void)io;

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();

  // Setup Platform/Renderer backends
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL2_Init();

  // Main loop
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();

    // Start the Dear ImGui frame
    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    gui_cpu(model);
    gui_registers(model);
    gui_memory(model);
    gui_disassemble(model);

    // Rendering
    ImGui::Render();
    int display_w, display_h;
    glfwGetFramebufferSize(window, &display_w, &display_h);
    glViewport(0, 0, display_w, display_h);
    glClearColor(.3f, .3f, .3f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
    glfwMakeContextCurrent(window);
    glfwSwapBuffers(window);

    if (will_continue) {
      model.step();
    }

    std::this_thread::yield();
  }

  // Cleanup
  ImGui_ImplOpenGL2_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}
