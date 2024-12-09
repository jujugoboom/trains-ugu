/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

For a C++ project simply rename the file to .cpp and re-run the build script

-- Copyright (c) 2020-2024 Jeffery Myers
--
--This software is provided "as-is", without any express or implied warranty. In no event
--will the authors be held liable for any damages arising from the use of this software.

--Permission is granted to anyone to use this software for any purpose, including commercial
--applications, and to alter it and redistribute it freely, subject to the following restrictions:

--  1. The origin of this software must not be misrepresented; you must not claim that you
--  wrote the original software. If you use this software in a product, an acknowledgment
--  in the product documentation would be appreciated but is not required.
--
--  2. Altered source versions must be plainly marked as such, and must not be misrepresented
--  as being the original software.
--
--  3. This notice may not be removed or altered from any source distribution.

I've changed the original file - jujugogoom 2024-12-01

*/

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "raymath.h"

#include "resource_dir.h" // utility header for SearchAndSetResourceDir

#define BLANK 0
#define RAIL 1
#define BUILDING 2
#define STATION 3

#define GRID_SIZE 10
#define WORLD_SIZE 1000

const int screenWidth = 680;
const int screenHeight = 420;

int world[WORLD_SIZE][WORLD_SIZE];

void InitWorld()
{
	for (int i = 0; i < WORLD_SIZE; i++)
	{
		for (int j = 0; j < WORLD_SIZE; j++)
		{
			world[i][j] = 0;
		}
	}
}

void ToggleLocation(int x, int y)
{
	int curr = world[x][y];
	printf("Toggling %d\n", curr);
	world[x][y] = (curr + 1) % 4;
}

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
	// Initialization
	//---------------------------------------------------------------------------------------
	InitWindow(screenWidth, screenHeight, "Tester");
	InitWorld();

	Camera2D camera = {0};
	camera.zoom = 1.0f;
	camera.rotation = 0.f;

	// layout_name: controls initialization
	//----------------------------------------------------------------------------------

	//----------------------------------------------------------------------------------

	SetTargetFPS(60);
	//--------------------------------------------------------------------------------------

	// Main game loop
	while (!WindowShouldClose()) // Detect window close button or ESC key
	{
		// Update
		//----------------------------------------------------------------------------------
		// TODO: Implement required update logic
		//----------------------------------------------------------------------------------

		if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
		{
			Vector2 delta = GetMouseDelta();
			delta = Vector2Scale(delta, -1.0f / camera.zoom);
			camera.target = Vector2Add(camera.target, delta);
		}

		if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
		{
			Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), camera);
			Vector2 clicked = {mousePos.x / GRID_SIZE, mousePos.y / GRID_SIZE};
			ToggleLocation(clicked.x, clicked.y);
		}

		// Draw
		//----------------------------------------------------------------------------------
		BeginDrawing();

		ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

		BeginMode2D(camera);

		for (int i = 0; i < WORLD_SIZE; i++)
		{
			DrawLineV((Vector2){(float)GRID_SIZE * i, 0}, (Vector2){(float)GRID_SIZE * i, WORLD_SIZE * GRID_SIZE}, LIGHTGRAY);
			for (int j = 0; j < WORLD_SIZE; j++)
			{
				if (i == 0)
				{
					DrawLineV((Vector2){0, (float)GRID_SIZE * j}, (Vector2){WORLD_SIZE * GRID_SIZE, (float)GRID_SIZE * j}, LIGHTGRAY);
				}
				if (world[i][j] != 0)
				{
					int currPos = world[i][j];
					if (currPos == RAIL)
					{
						DrawText("R", i * GRID_SIZE, j * GRID_SIZE, 12, BLACK);
					}
					if (currPos == BUILDING)
					{
						DrawText("B", i * GRID_SIZE, j * GRID_SIZE, 12, BLACK);
					}
					if (currPos == STATION)
					{
						DrawText("S", i * GRID_SIZE, j * GRID_SIZE, 12, BLACK);
					}
				}
			}
		}

		EndMode2D();

		// raygui: controls drawing
		//----------------------------------------------------------------------------------

		//----------------------------------------------------------------------------------

		EndDrawing();
		//----------------------------------------------------------------------------------
	}

	// De-Initialization
	//--------------------------------------------------------------------------------------
	CloseWindow(); // Close window and OpenGL context
	//--------------------------------------------------------------------------------------

	return 0;
}
