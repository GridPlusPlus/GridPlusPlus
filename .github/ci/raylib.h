// CI 專用的 raylib 替身標頭。
//
// 只提供 Grid++ 標頭、範例與教學程式碼會用到的型別、常數與函式宣告，讓 CI 不必安裝 raylib
// 也能確認程式可以編譯與連結。它不會開啟視窗，也不是給學生使用的檔案。
#ifndef RAYLIB_H
#define RAYLIB_H

struct Color {
    unsigned char r, g, b, a;
};
struct Vector2 {
    float x, y;
};
struct Rectangle {
    float x, y, width, height;
};
struct Texture2D {
    unsigned int id;
    int width, height, mipmaps, format;
};
struct Image {
    void* data;
    int width, height, mipmaps, format;
};

enum {
    PIXELFORMAT_UNCOMPRESSED_R8G8B8A8 = 7,
    MOUSE_BUTTON_LEFT = 0,
    KEY_SPACE = 32,
    KEY_A = 65,
    KEY_B = 66,
    KEY_C = 67,
    KEY_D = 68,
    KEY_E = 69,
    KEY_F = 70,
    KEY_G = 71,
    KEY_H = 72,
    KEY_I = 73,
    KEY_J = 74,
    KEY_K = 75,
    KEY_L = 76,
    KEY_M = 77,
    KEY_N = 78,
    KEY_O = 79,
    KEY_P = 80,
    KEY_Q = 81,
    KEY_R = 82,
    KEY_S = 83,
    KEY_T = 84,
    KEY_U = 85,
    KEY_V = 86,
    KEY_W = 87,
    KEY_X = 88,
    KEY_Y = 89,
    KEY_Z = 90,
    KEY_RIGHT = 262,
    KEY_LEFT = 263,
    KEY_DOWN = 264,
    KEY_UP = 265,
};

static const Color LIGHTGRAY = {200, 200, 200, 255};
static const Color GRAY = {130, 130, 130, 255};
static const Color DARKGRAY = {80, 80, 80, 255};
static const Color YELLOW = {253, 249, 0, 255};
static const Color ORANGE = {255, 161, 0, 255};
static const Color PINK = {255, 109, 194, 255};
static const Color RED = {230, 41, 55, 255};
static const Color GREEN = {0, 228, 48, 255};
static const Color SKYBLUE = {102, 191, 255, 255};
static const Color BLUE = {0, 121, 241, 255};
static const Color PURPLE = {200, 122, 255, 255};
static const Color BEIGE = {211, 176, 131, 255};
static const Color WHITE = {255, 255, 255, 255};
static const Color BLACK = {0, 0, 0, 255};
static const Color RAYWHITE = {245, 245, 245, 255};

inline void InitWindow(int, int, const char*) {}
inline void CloseWindow() {}
inline bool IsWindowReady() { return true; }
inline bool WindowShouldClose() { return true; }
inline void SetTargetFPS(int) {}
inline double GetTime() { return 0.0; }
inline int GetRandomValue(int min, int) { return min; }

inline bool IsKeyPressed(int) { return false; }
inline bool IsKeyDown(int) { return false; }
inline bool IsMouseButtonPressed(int) { return false; }
inline Vector2 GetMousePosition() { return {0, 0}; }

inline Texture2D LoadTextureFromImage(Image image) { return {1, image.width, image.height, image.mipmaps, image.format}; }
inline void UnloadTexture(Texture2D) {}

inline void BeginDrawing() {}
inline void EndDrawing() {}
inline void ClearBackground(Color) {}
inline Color Fade(Color color, float) { return color; }
inline void DrawLine(int, int, int, int, Color) {}
inline void DrawRectangle(int, int, int, int, Color) {}
inline void DrawRectangleLines(int, int, int, int, Color) {}
inline void DrawCircle(int, int, float, Color) {}
inline void DrawPoly(Vector2, int, float, float, Color) {}
inline void DrawTriangleFan(const Vector2*, int, Color) {}
inline void DrawTexturePro(Texture2D, Rectangle, Rectangle, Vector2, float, Color) {}
inline int MeasureText(const char*, int) { return 0; }
inline void DrawText(const char*, int, int, int, Color) {}

#endif  // RAYLIB_H
