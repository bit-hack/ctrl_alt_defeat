#include <cstdio>
#include <thread>

#include <GLFW/glfw3.h>

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl2.h"

#include "../rv32i_model/model.h"

#include "encoding.h"


enum {
  MEM_BASE  = 0x1000,
  MEM_SIZE  = 0x1000,
  MEM_STACK = 0x1fff,
};


bool will_continue = false;
bool will_follow = true;

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
  ImGui::Begin("REGISTERS", nullptr,
    ImGuiWindowFlags_NoResize |
    ImGuiWindowFlags_NoMove |
    ImGuiWindowFlags_NoCollapse);

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
  ImGui::SetNextWindowSize(ImVec2{ 16 * 36, 16 * 18 });

  ImGui::Begin("MEMORY", nullptr,
    ImGuiWindowFlags_HorizontalScrollbar |
    ImGuiWindowFlags_NoResize |
    ImGuiWindowFlags_NoMove |
    ImGuiWindowFlags_NoCollapse);

  ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 0));

  uint8_t temp[16];
  ImGuiListClipper clipper{ MEM_SIZE / 16, ImGui::GetTextLineHeight() };

  const uint32_t pc = model.get_pc();

  while (clipper.Step()) {
    for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i) {
      const uint32_t addr = MEM_BASE + i * 16;
      // read in a line
      model.memory().read(temp, addr, 16);
      // print address
      ImGui::Text("%08x: ", addr);
      // print hex view
      for (int i = 0; i < 16; ++i) {
        const uint8_t val = temp[i];
        ImGui::SameLine();
        if ((addr + i) >= pc && (addr + i) < pc + 4) {
          ImGui::TextColored(ImVec4{ 0.8f, 0.7f, 0.2f, 1.f }, "%02x", val);
        }
        else {
          if (val) {
            ImGui::Text("%02x", val);
          }
          else {
            ImGui::TextDisabled("%02x", val);
          }
        }
      }
      // split between hex and ascii viewer
      ImGui::SameLine();
      ImGui::Dummy(ImVec2{8.f, 0.f});
      // print ascii view
      for (int i = 0; i < 16; ++i) {
        const uint8_t val = temp[i];
        ImGui::SameLine();
        if (val) {
          ImGui::Text("%c", (val >= 0x20 && val < 0x7f) ? val : '.');
        }
        else {
          ImGui::TextDisabled(" ");
        }
      }
    }
  }

  ImGui::PopStyleVar(1);
  ImGui::End();
}

static char sys_tmp[128];

void step_model(rv32i_model_t &model) {
  if (!model.ecall_pending) {
    model.step();
    // if we have just hit an ecall
    if (model.ecall_pending) {
      will_follow = true;
    }
  }
}

void gui_ecall(rv32i_model_t &model) {
  if (!model.ecall_pending) {
    return;
  }

  will_continue = false;

  uint32_t a0 = model.get_reg(rv_reg_a0);
  uint32_t a7 = model.get_reg(rv_reg_a7);
  ImGui::SetNextWindowFocus();
  ImGui::SetNextWindowPos(ImVec2{ 512 - 16 * 13, (768 / 2) - 40.f });
  ImGui::SetNextWindowSize(ImVec2{ 16 * 26, 16 * 5 });
  ImGui::Begin("System Call", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove);
  if (a7 == 0) {
    ImGui::InputText("Password", sys_tmp, sizeof(sys_tmp));
    if (ImGui::Button("ok")) {
      model.ecall_pending = false;
      model.memory().write(a0, (const uint8_t *)sys_tmp, (uint32_t)strlen(sys_tmp));
      sys_tmp[0] = '\0';
    }
  }
  if (a7 == 1) {
    model.memory().read_str((uint8_t*)sys_tmp, a0, sizeof(sys_tmp));
    ImGui::Text("Message: %s", sys_tmp);
    if (ImGui::Button("ok")) {
      model.ecall_pending = false;
      sys_tmp[0] = '\0';
    }
  }
  if (a7 == 2) {
    ImGui::Text("Success: Device unlocked");
    if (ImGui::Button("ok")) {
      model.ecall_pending = false;
      sys_tmp[0] = '\0';
    }
  }
  ImGui::End();
}

void gui_cpu(rv32i_model_t &model) {

  ImGui::SetNextWindowPos(ImVec2{ 16 * 48, 16 });
  ImGui::SetNextWindowSize(ImVec2{ 16 * 15, 16 * 4 });

  ImGui::Begin("CPU", nullptr,
    ImGuiWindowFlags_NoResize |
    ImGuiWindowFlags_NoMove |
    ImGuiWindowFlags_NoCollapse);
  if (ImGui::Button("Step")) {
    will_follow = true;
    step_model(model);
  }
  ImGui::SameLine();
  if (ImGui::Button("Run")) {
    will_continue = true;
  }
  ImGui::SameLine();
  if (ImGui::Button("Halt")) {
    will_continue = false;
    will_follow = true;
  }
  ImGui::SameLine();
  if (ImGui::Button("Reset")) {
    model.reset();
    will_follow = true;
  }
  ImGui::End();
}

void gui_disassemble(rv32i_model_t &model) {
  ImGui::SetNextWindowPos(ImVec2{ 16 * 11, 16 * 20 });
  ImGui::SetNextWindowSize(ImVec2{ 16 * 36, 16 * 20 });
  ImGui::Begin("DISASSEMBLY", nullptr,
    ImGuiWindowFlags_HorizontalScrollbar |
    ImGuiWindowFlags_NoResize |
    ImGuiWindowFlags_NoMove |
    ImGuiWindowFlags_NoCollapse);

  const uint32_t pc = model.get_pc();

  if (will_follow) {
    ImGui::SetScrollY(((pc - MEM_BASE) / 4) * ImGui::GetTextLineHeight());
    will_follow = false;
  }

  char temp[128];
  ImGuiListClipper clipper{ MEM_SIZE / 4, ImGui::GetTextLineHeight() };

  while (clipper.Step()) {
    for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i) {
      const uint32_t addr = MEM_BASE + i * 4;
      const uint32_t raw = model.memory().read_w(addr);

      const char *name = model.program().find_symbol(addr);

      if (name) {
        ImGui::Separator();
      }

      rv_inst_t dec;
      if (rv_decode(raw, dec)) {
        temp[0] = '\0';
        rv_print(addr, dec, temp, sizeof(temp));
        if (addr == pc) {
          ImGui::TextColored(ImVec4{ 0.8f, 0.7f, 0.2f, 1.f }, "%08x:  %08x  %s", addr, raw, temp);
        }
        else {
          ImGui::Text("%08x:  %08x  %s", addr, raw, temp);
        }
        // print the destination
        uint32_t target = 0;
        switch (dec.type) {
        case rv_inst_jal:
        case rv_inst_auipc:
        case rv_inst_beq:
        case rv_inst_bne:
        case rv_inst_blt:
        case rv_inst_bge:
        case rv_inst_bltu:
        case rv_inst_bgeu:
          target = addr + dec.imm;
          ImGui::SameLine(350);
          if (const char *t = model.program().find_symbol(target)) {
            ImGui::TextDisabled("<%s>", t);
          }
          else {
            ImGui::TextDisabled("<%08xh>", target);
          }
        }
      }
      else {
        ImGui::TextDisabled("%08x:  %08x  %s", addr, raw, "unknown");
      }

      if (name) {
        ImGui::SameLine(450);
        ImGui::TextDisabled("; %s", name);
      }
    }
  }

  ImGui::End();
}

void gui_stack(rv32i_model_t &model) {
  ImGui::SetNextWindowPos(ImVec2{ 16 * 48, 16 * 6 });
  ImGui::SetNextWindowSize(ImVec2{ 16 * 15, 16 * 34 });
  ImGui::Begin("STACK", nullptr,
    ImGuiWindowFlags_HorizontalScrollbar |
    ImGuiWindowFlags_NoResize |
    ImGuiWindowFlags_NoMove |
    ImGuiWindowFlags_NoCollapse);

  const uint32_t pc = model.get_reg(rv_reg_sp);

  const int items = MEM_STACK - (model.get_reg(rv_reg_sp) & ~0xfu );

  ImGuiListClipper clipper{ items, ImGui::GetTextLineHeight() };

  while (clipper.Step()) {
    for (int i = clipper.DisplayStart; i < clipper.DisplayEnd; ++i) {

      const uint32_t addr = (MEM_STACK & ~3u) - i * 4;
      const uint32_t raw = model.memory().read_w(addr);

      if (addr >= pc) {
        ImGui::Text("%08x:  %08x", addr, raw);
      }
      else {
        ImGui::TextDisabled("%08x:  %08x", addr, raw);
      }
      // split between hex and ascii viewer
      ImGui::SameLine();
      ImGui::Dummy(ImVec2{ 8.f, 0.f });
      // print ascii view
      for (int i = 0; i < 4; ++i) {
        ImGui::SameLine();
        const uint8_t val = ((const uint8_t*)&raw)[i];
        if (val) {
          ImGui::Text("%c", (val >= 0x20 && val < 0x7f) ? val : '.');
        }
        else {
          ImGui::TextDisabled(" ");
        }
      }
      if (addr == pc) {
        ImGui::Separator();
      }
    }
  }

  ImGui::End();
}

int main(int, char**) {
  rv32i_model_t model;
  model.reset();

#if 1
  if (!model.load_elf("C:/repos/ctrl_alt_defeat/levels/overflow/out.elf")) {
#else
  if (!model.load_hex("C:/repos/ctrl_alt_defeat/rom.hex", MEM_BASE)) {
#endif
    return 1;
  }

  // Setup window
  glfwSetErrorCallback(glfw_error_callback);
  if (!glfwInit())
      return 1;
  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  GLFWmonitor* monitor = 0 ? glfwGetPrimaryMonitor() : nullptr;
  GLFWwindow* window = glfwCreateWindow(1024, 768, "Ctrl-Alt-Defeat", monitor, nullptr);
  if (window == nullptr)
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

    if (will_continue) {
      step_model(model);
    }

    gui_cpu(model);
    gui_registers(model);
    gui_memory(model);
    gui_stack(model);
    gui_ecall(model);
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

    // dont wipe out the CPU
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
