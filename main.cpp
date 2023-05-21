#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const int FPS = 60;
const int CELL_SIZE = 1;
const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 450;

bool previousCells[SCREEN_HEIGHT][SCREEN_HEIGHT];
bool cells[SCREEN_HEIGHT][SCREEN_WIDTH];

int getCellNeighborCount(int cellX, int cellY)
{
    int neigborCount = 0;
    for (int y = cellY - 1; y < cellY + 2; ++y)
    {
        for (int x = cellX - 1; x < cellX + 2; ++x)
        {
            if (x == cellX && y == cellY)
                continue;
            if (x < 0 || y < 0)
                continue;
            if (x >= SCREEN_WIDTH || y >= SCREEN_HEIGHT)
                continue;

            if (previousCells[y][x])
            {
                ++neigborCount;
            }
        }
    }
    return neigborCount;
}

void copyCurrentState()
{
    for (int j = 0; j < SCREEN_HEIGHT; ++j)
    {
        for (int i = 0; i < SCREEN_WIDTH; ++i)
        {
            previousCells[j][i] = cells[j][i];
        }
    }
}

void initializeCells()
{
    for (int y = 0; y < SCREEN_HEIGHT; ++y)
    {
        for (int x = 0; x < SCREEN_WIDTH; ++x)
        {
            cells[y][x] = rand() % 2 == 1;
        }
    }
    copyCurrentState();
}

// https://github.com/raylib-extras/examples-c/tree/main/mouse_zoom
void handleCameraMovement(Camera2D &camera)
{
    // Translate based on right click
    if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
    {
        Vector2 delta = GetMouseDelta();
        delta.x = delta.x * (-1.0f / camera.zoom);
        delta.y = delta.y * (-1.0f / camera.zoom);

        auto newTarget = Vector2{camera.target.x + delta.x, camera.target.y + delta.y};

        camera.target = newTarget;
    }

    // Zoom based on wheel
    float wheel = GetMouseWheelMove();
    if (wheel != 0)
    {
        // Get the world point that is under the mouse
        Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);

        // Set the offset to where the mouse is
        camera.offset = GetMousePosition();

        // Set the target to match, so that the camera maps the world space point under the cursor to the screen space point under the cursor at any zoom
        camera.target = mouseWorldPos;

        // Zoom
        camera.zoom += wheel * 0.125f;
        if (camera.zoom < 0.125f)
            camera.zoom = 0.125f;
    }
}

int main(void)
{
    srand(time(NULL));

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Game of life");

    Camera2D camera = {0};
    camera.zoom = 1;

    initializeCells();

    SetTargetFPS(FPS);

    while (!WindowShouldClose())
    {
        handleCameraMovement(camera);

        BeginDrawing();

        ClearBackground(BLACK);

        BeginMode2D(camera);

        // Draw cells
        for (int j = 0; j < SCREEN_HEIGHT; ++j)
        {
            for (int i = 0; i < SCREEN_WIDTH; ++i)
            {
                if (!cells[j][i])
                    continue;
                DrawRectangle(i * CELL_SIZE, j * CELL_SIZE, CELL_SIZE, CELL_SIZE, WHITE);
            }
        }

        EndDrawing();

        // Prepare next iteration
        for (int j = 0; j < SCREEN_HEIGHT; ++j)
        {
            for (int i = 0; i < SCREEN_WIDTH; ++i)
            {
                auto isCellAlive = cells[j][i];

                int neigborCount = getCellNeighborCount(i, j);

                if (isCellAlive)
                {
                    // Any live cell with two or three live neighbors survives.
                    cells[j][i] = neigborCount == 2 || neigborCount == 3;
                }
                else
                {
                    // Any dead cell with three live neighbors becomes a live cell.
                    cells[j][i] = neigborCount == 3;
                }
            }
        }

        copyCurrentState();
    }

    CloseWindow();

    return 0;
}