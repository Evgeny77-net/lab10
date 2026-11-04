// ============================================================
//  Простой графический калькулятор на C++17
//  Dear ImGui + GLFW + OpenGL3 (Immediate Mode GUI)
// ============================================================

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include <GLFW/glfw3.h>

#include <cstdio>
#include <cmath>
#include <string>

#ifdef _WIN32
   #include <windows.h>
   #undef main            // отключаем макрос main->WinMain из glfwwin32.h
#endif

// ---------- Состояние приложения ----------
static float g_numA        = 0.0f;   // Число А
static float g_numB        = 0.0f;   // Число Б
static float g_result      = 0.0f;   // Результат вычисления
static bool  g_hasResult   = false;  // Была ли выполнена хотя бы одна операция
static bool  g_divByZero   = false;  // Флаг ошибки "деление на ноль"

// ---------- Выполнение операции ----------
static void Calculate(char op)
{
    switch (op)
    {
        case '+': g_result = g_numA + g_numB; g_divByZero = false; break;
        case '-': g_result = g_numA - g_numB; g_divByZero = false; break;
        case '*': g_result = g_numA * g_numB; g_divByZero = false; break;
        case '/':
            if (g_numB == 0.0f)
            {
                // Проверка на деление на ноль
                g_divByZero = true;
                g_result    = 0.0f;
            }
            else
            {
                g_divByZero = false;
                g_result    = g_numA / g_numB;
            }
            break;
        default: return;
    }
    g_hasResult = true;
}

int main(void)
{
    // ---------- Инициализация GLFW ----------
    if (!glfwInit())
        return 1;

    // Версия OpenGL для backend ImGui
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    // Окно фиксированного размера: без флага GLFW_RESIZABLE пользователь
    // не сможет изменить размер окна.
    GLFWwindow* window = glfwCreateWindow(
        460, 320,                 // ширина, высота
        "Калькулятор",            // заголовок
        nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // VSync

    // ---------- Инициализация Dear ImGui ----------
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Поддержки кириллицы в дефолтном шрифте ImGui нет — подгружаем системный
    // шрифт с кириллицей (Windows). Если файл недоступен, интерфейс продолжит
    // работать на дефолтном шрифте.
    ImFont* bigFont = nullptr;
    const char* fontCandidates[] = {
        "C:\\Windows\\Fonts\\arial.ttf",
        "C:\\Windows\\Fonts\\segoeui.ttf",
        "C:\\Windows\\Fonts\\tahoma.ttf",
    };
    for (const char* path : fontCandidates)
    {
        FILE* f = std::fopen(path, "rb");
        if (f)
        {
            std::fclose(f);
            bigFont = ImGui::GetIO().Fonts->AddFontFromFileTTF(
                path, 20.0f, nullptr,
                ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
            break;
        }
    }
    if (!bigFont)
        ImGui::GetIO().Fonts->AddFontDefault();

    // ---------- Главный цикл ----------
    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        {
            // Fixed-Size окно интерфейса (без ImGuiWindowFlags_ResizeFlags)
            ImGui::Begin("Калькулятор", nullptr,
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoMove  |
                ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_AlwaysAutoResize);

            ImGui::PushItemWidth(190.0f);

            // --- Два числовых поля ввода ---
            ImGui::Text("Число А:");
            ImGui::InputFloat("##A", &g_numA, 0.0f, 0.0f, "%.3f");
            ImGui::Spacing();

            ImGui::Text("Число Б:");
            ImGui::InputFloat("##B", &g_numB, 0.0f, 0.0f, "%.3f");

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            // --- Четыре кнопки в ряд ---
            const float btnW = 85.0f;
            if (ImGui::Button("+", ImVec2(btnW, 40))) Calculate('+');
            ImGui::SameLine();
            if (ImGui::Button("-", ImVec2(btnW, 40))) Calculate('-');
            ImGui::SameLine();
            if (ImGui::Button("*", ImVec2(btnW, 40))) Calculate('*');
            ImGui::SameLine();
            if (ImGui::Button("/", ImVec2(btnW, 40))) Calculate('/');

            ImGui::Spacing();
            ImGui::Separator();
            ImGui::Spacing();

            // --- Вывод результата крупным текстом ---
            if (g_divByZero)
            {
                // Ошибка: красным цветом
                ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.2f, 0.2f, 1.0f));
                ImGui::SetWindowFontScale(2.0f); // крупный текст
                ImGui::Text("Ошибка: деление на ноль!");
                ImGui::SetWindowFontScale(1.0f);
                ImGui::PopStyleColor();
            }
            else if (g_hasResult)
            {
                char buf[64];
                std::snprintf(buf, sizeof(buf), "%g", static_cast<double>(g_result));

                ImGui::SetWindowFontScale(2.5f); // крупный текст
                ImGui::Text("Результат: %s", buf);
                ImGui::SetWindowFontScale(1.0f);
            }
            else
            {
                ImGui::TextDisabled("Введите числа и выберите операцию.");
            }

            ImGui::End();
        }

        // ---------- Рендеринг ----------
        ImGui::Render();

        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(0.12f, 0.12f, 0.14f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window);
    }

    // ---------- Завершение ----------
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
