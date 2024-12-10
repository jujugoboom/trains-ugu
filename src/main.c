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

#define BLANK_SPACE 0
#define RAIL 1
#define BUILDING 2
#define STATION 3

#define TOGGLES "Empty;Rail;Building;Station"

#define GRID_SIZE 32
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
	world[x][y] = (curr + 1) % 4;
}

bool CheckGuiCollision(Vector2 point, const Rectangle bounds[], int count)
{
	for (int i = 0; i < count; i++)
	{
		if (CheckCollisionPointRec(point, bounds[i]))
		{
			return true;
		}
	}
	return false;
}

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(void)
{
	// Initialization
	//---------------------------------------------------------------------------------------
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(screenWidth, screenHeight, "Tester");
	InitWorld();

	// Const init
	//----------------------------------------------------------------------------------
	const Vector2 ZeroVector = Vector2Zero();

	const Vector2 WorldSizeVector = (const Vector2){WORLD_SIZE, WORLD_SIZE};
	const Vector2 TotalSizeVector = (const Vector2){(WORLD_SIZE * GRID_SIZE), (WORLD_SIZE * GRID_SIZE)};
	//----------------------------------------------------------------------------------

	// Camera init
	//----------------------------------------------------------------------------------
	Camera2D camera = {0};
	camera.zoom = 1.0f;
	camera.rotation = 0.f;
	//----------------------------------------------------------------------------------

	// Texture loading
	//----------------------------------------------------------------------------------
	Texture2D trackTex = LoadTexture("resources/track.png");
	Texture2D buildingTex = LoadTexture("resources/building.png");
	Texture2D stationTex = LoadTexture("resources/station.png");
	//----------------------------------------------------------------------------------

	// layout_name: controls initialization
	//----------------------------------------------------------------------------------
	int SelectedMode = 0;
	bool DropdownActive = false;
	bool DebugActive = false;
	//----------------------------------------------------------------------------------

	SetTargetFPS(1000);
	//--------------------------------------------------------------------------------------

	// Main game loop
	while (!WindowShouldClose()) // Detect window close button or ESC key
	{
		// Update
		//----------------------------------------------------------------------------------

		const int currScreenWidth = GetScreenWidth();
		const int currScreenHeight = GetScreenHeight();
		// Update gui poisition
		const Rectangle GuiDropdownBounds = (Rectangle){(currScreenWidth / 2) - 40, 10, 80, 24};
		const Rectangle GuiDebugToggleBounds = (Rectangle){10, currScreenHeight - 30, 20, 20};

		const Rectangle GuiBounds[] = {GuiDropdownBounds, GuiDebugToggleBounds};

		float wheel = GetMouseWheelMove();
		if (wheel != 0)
		{
			// Get the world point that is under the mouse
			// Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);

			// Set the offset to where the mouse is
			// camera.offset = Vector2Clamp(GetMousePosition(), Vector2Zero(), Vector2Subtract(TotalSizeVector, (Vector2){GetScreenWidth(), GetScreenHeight()}));

			// Set the target to match, so that the camera maps the world space point
			// under the cursor to the screen space point under the cursor at any zoom
			// camera.target = Vector2Clamp(mouseWorldPos, Vector2Zero(), Vector2Subtract(TotalSizeVector, (Vector2){GetScreenWidth(), GetScreenHeight()}));

			// Zoom increment
			float scaleFactor = 1.0f + (0.25f * fabsf(wheel));
			if (wheel < 0)
				scaleFactor = 1.0f / scaleFactor;
			camera.zoom = Clamp(camera.zoom * scaleFactor, 0.125f, 64.0f);
			camera.target = Vector2Clamp(camera.target, ZeroVector, Vector2Subtract(TotalSizeVector, (Vector2){currScreenWidth / camera.zoom, currScreenHeight / camera.zoom}));
		}

		if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
		{
			Vector2 delta = GetMouseDelta();
			delta = Vector2Scale(delta, -1.0f / camera.zoom);
			camera.target = Vector2Clamp(Vector2Add(camera.target, delta), ZeroVector, Vector2Subtract(TotalSizeVector, (Vector2){currScreenWidth / camera.zoom, currScreenHeight / camera.zoom}));
		}

		if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !CheckGuiCollision(GetMousePosition(), GuiBounds, 2) && !DropdownActive)
		{
			Vector2 mousePos = GetScreenToWorld2D(GetMousePosition(), camera);
			Vector2 clicked = Vector2Clamp((Vector2){mousePos.x / GRID_SIZE, mousePos.y / GRID_SIZE}, ZeroVector, WorldSizeVector);
			world[(int)clicked.x][(int)clicked.y] = SelectedMode;
		}

		// Draw
		//----------------------------------------------------------------------------------
		BeginDrawing();

		ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

		// Draw 2d
		//----------------------------------------------------------------------------------
		BeginMode2D(camera);
		// Calculate screen bounds to world bounds
		Vector2 start = GetScreenToWorld2D((Vector2){0, 0}, camera);
		Vector2 end = GetScreenToWorld2D((Vector2){currScreenWidth, currScreenHeight}, camera);
		Vector2 worldStart = Vector2Clamp(Vector2Divide(start, (Vector2){GRID_SIZE, GRID_SIZE}), ZeroVector, WorldSizeVector);
		Vector2 worldEnd = Vector2Clamp(Vector2Divide(end, (Vector2){GRID_SIZE, GRID_SIZE}), ZeroVector, WorldSizeVector);

		// Only bother rendering parts of the world on screen
		for (int i = worldStart.x; i < worldEnd.x; i++)
		{
			DrawLineV((Vector2){(float)GRID_SIZE * i, 0}, (Vector2){(float)GRID_SIZE * i, WORLD_SIZE * GRID_SIZE}, LIGHTGRAY);
			for (int j = worldStart.y; j < worldEnd.y; j++)
			{
				if (i == (int)worldStart.x)
				{
					DrawLineV((Vector2){0, (float)GRID_SIZE * j}, (Vector2){WORLD_SIZE * GRID_SIZE, (float)GRID_SIZE * j}, LIGHTGRAY);
				}
				if (world[i][j] != 0)
				{
					int currPos = world[i][j];
					if (currPos == RAIL)
					{
						// DrawText("R", i * GRID_SIZE, j * GRID_SIZE, GRID_SIZE, BLACK);
						DrawTexture(trackTex, i * GRID_SIZE, j * GRID_SIZE, WHITE);
					}
					if (currPos == BUILDING)
					{
						// DrawText("B", i * GRID_SIZE, j * GRID_SIZE, GRID_SIZE, BLACK);
						DrawTexture(buildingTex, i * GRID_SIZE, j * GRID_SIZE, WHITE);
					}
					if (currPos == STATION)
					{
						// DrawText("S", i * GRID_SIZE, j * GRID_SIZE, GRID_SIZE, BLACK);
						DrawTexture(stationTex, i * GRID_SIZE, j * GRID_SIZE, WHITE);
					}
				}
			}
		}

		EndMode2D();
		//----------------------------------------------------------------------------------

		// raygui: controls drawing
		//----------------------------------------------------------------------------------
		if (GuiDropdownBox(GuiDropdownBounds, TOGGLES, &SelectedMode, DropdownActive))
			DropdownActive = !DropdownActive;
		GuiToggle(GuiDebugToggleBounds, "#191#", &DebugActive);
		if (DebugActive)
		{
			const char *fpsText = TextFormat("CURRENT FPS: %i", GetFPS());
			DrawText(fpsText, currScreenWidth - (MeasureText(fpsText, 20) + 20), currScreenHeight - 30, 20, GREEN);
			const char *renderInfo = TextFormat("Rendering from x %d to %d; y %d to %d", (int)worldStart.x, (int)worldEnd.x, (int)worldStart.y, (int)worldEnd.y);
			DrawText(renderInfo, currScreenWidth - (MeasureText(renderInfo, 20) + 20), currScreenHeight - 60, 20, GREEN);
		}
		//----------------------------------------------------------------------------------

		EndDrawing();
		//----------------------------------------------------------------------------------
	}

	// De-Initialization
	//--------------------------------------------------------------------------------------
	UnloadTexture(trackTex);
	UnloadTexture(buildingTex);
	UnloadTexture(stationTex);
	CloseWindow(); // Close window and OpenGL context
	//--------------------------------------------------------------------------------------

	return 0;
}
