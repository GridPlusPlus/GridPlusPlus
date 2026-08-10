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

enum { PIXELFORMAT_UNCOMPRESSED_R8G8B8A8, MOUSE_BUTTON_LEFT, KEY_RIGHT, KEY_UP, KEY_LEFT, KEY_DOWN };

static const Color WHITE = {255, 255, 255, 255};
static const Color RAYWHITE = {245, 245, 245, 255};
static const Color LIGHTGRAY = {200, 200, 200, 255};
static const Color DARKGRAY = {80, 80, 80, 255};
static const Color BLACK = {0, 0, 0, 255};
static const Color BLUE = {0, 121, 241, 255};
static const Color RED = {230, 41, 55, 255};
static const Color GREEN = {0, 228, 48, 255};
static const Color YELLOW = {253, 249, 0, 255};
static const Color ORANGE = {255, 161, 0, 255};
static const Color PINK = {255, 109, 194, 255};
static const Color SKYBLUE = {102, 191, 255, 255};

static int test_frame = 0;
static bool test_window_open = false;
static int test_init_window_calls = 0;
static int test_texture_loads = 0;
static int test_texture_unloads = 0;
static int test_texture_unloads_after_close = 0;
static int test_draw_rectangle_calls = 0;
static Rectangle test_last_rectangle = {};
static Color test_last_shape_color = {};
static int test_draw_circle_calls = 0;
static Vector2 test_last_circle_center = {};
static float test_last_circle_radius = 0;
static int test_draw_poly_calls = 0;
static Vector2 test_last_poly_center = {};
static int test_last_poly_sides = 0;
static float test_last_poly_radius = 0;
static float test_last_poly_rotation = 0;
static int test_draw_triangle_fan_calls = 0;
static Vector2 test_last_triangle_fan_center = {};
static int test_last_triangle_fan_points = 0;
static unsigned int test_last_texture_id = 0;

inline void InitWindow(int, int, const char*) {
    ++test_init_window_calls;
    test_frame = 0;
    test_window_open = true;
}
inline void CloseWindow() { test_window_open = false; }
inline void SetTargetFPS(int) {}
inline bool WindowShouldClose() { return test_frame++ >= 2; }
inline Texture2D LoadTextureFromImage(Image image) {
    const Texture2D texture = {static_cast<unsigned int>(++test_texture_loads), image.width, image.height,
                               image.mipmaps, image.format};
    return texture;
}
inline void UnloadTexture(Texture2D) {
    ++test_texture_unloads;
    if (!test_window_open) ++test_texture_unloads_after_close;
}
inline void DrawTexturePro(Texture2D texture, Rectangle, Rectangle, Vector2, float, Color) {
    test_last_texture_id = texture.id;
}
inline void DrawRectangle(int x, int y, int width, int height, Color color) {
    ++test_draw_rectangle_calls;
    test_last_rectangle = {static_cast<float>(x), static_cast<float>(y), static_cast<float>(width),
                           static_cast<float>(height)};
    test_last_shape_color = color;
}
inline void DrawCircle(int center_x, int center_y, float radius, Color color) {
    ++test_draw_circle_calls;
    test_last_circle_center = {static_cast<float>(center_x), static_cast<float>(center_y)};
    test_last_circle_radius = radius;
    test_last_shape_color = color;
}
inline void DrawPoly(Vector2 center, int sides, float radius, float rotation, Color color) {
    ++test_draw_poly_calls;
    test_last_poly_center = center;
    test_last_poly_sides = sides;
    test_last_poly_radius = radius;
    test_last_poly_rotation = rotation;
    test_last_shape_color = color;
}
inline void DrawTriangleFan(const Vector2* points, int point_count, Color color) {
    ++test_draw_triangle_fan_calls;
    test_last_triangle_fan_center = points[0];
    test_last_triangle_fan_points = point_count;
    test_last_shape_color = color;
}
inline void DrawRectangleLines(int, int, int, int, Color) {}
inline void DrawLine(int, int, int, int, Color) {}
inline void BeginDrawing() {}
inline void ClearBackground(Color) {}
inline void EndDrawing() {}
inline Vector2 GetMousePosition() { return {0, 0}; }
inline bool IsMouseButtonPressed(int) { return false; }
inline bool IsKeyPressed(int) { return false; }
inline bool IsKeyDown(int) { return false; }
inline int MeasureText(const char*, int) { return 0; }
inline void DrawText(const char*, int, int, int, Color) {}
inline int GetRandomValue(int min, int) { return min; }
inline void SetWindowTitle(const char*) {}
inline const char* TextFormat(const char*, ...) { return ""; }
inline int GetScreenWidth() { return 0; }
inline int GetScreenHeight() { return 0; }
inline Color Fade(Color color, float) { return color; }

#endif
