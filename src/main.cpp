#include "raylib.h"

int main() {
    const int W = 1280, H = 720;
    InitWindow(W, H, "My Game");

    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        EndDrawing();
    }

    CloseWindow();

    return 0;
}