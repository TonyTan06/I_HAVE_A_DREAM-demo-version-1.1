#include "scene.h"
#include "raylib.h"

int main() {
    // 窗口宽度与当前 1200px 平台一致，高度继续保持 450px。
    InitWindow(1200, 450, "I HAVE A DREAM");
    SetTargetFPS(60);

    {
        // Scene 必须在 raylib 窗口存在期间加载并释放显存纹理。
        Scene scene;
        if (!scene.loadAssets()) {
            TraceLog(LOG_WARNING, "Player sprite assets/移动.png could not be loaded");
        }

        while (!WindowShouldClose()) {
            // GetFrameTime() 是上一帧实际经过的秒数，保证不同帧率下移动速度一致。
            scene.update(GetFrameTime());

            // raylib 的每帧绘制必须位于 BeginDrawing/EndDrawing 之间。
            BeginDrawing();
            scene.draw();
            EndDrawing();
        }
    }

    // Scene 已先析构并释放纹理，此时再安全关闭 raylib 窗口。
    CloseWindow();
    return 0;
}
