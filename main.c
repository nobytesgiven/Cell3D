/*
 * Cell3D - Dynamic 3D cellular automata engine with lua script support
 * Copyright (C) 2021
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see https://www.gnu.org/licenses.
 */

#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#include <math.h>
#include <pthread.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef M_PI
#define M_PI 3.1415926535
#endif

#ifndef M_PI_2
#define M_PI_2 1.5707963267
#endif

#define TIME_DILATION 1.0f
#define CAMERA_ROTATION_SPEED 0.3f
#define CUBE_WIRES_ENABLED 1
#define GRID_WIRES_ENABLED 0
#define SHOW_FPS 0

lua_State *L;

bool thread_working = false;
pthread_t luathread;

void *pcall_thread(void *args) {
  lua_settop(L, 0);
  lua_getglobal(L, "update");
  if (lua_pcall(L, 0, 1, 0) != LUA_OK) {
    printf("%s\n", lua_tostring(L, -1));
    lua_close(L);
    CloseWindow();
  }
  thread_working = false;
  return NULL;
}

int main(int argc, char **argv) {
  //-----------------------------------------------------------------
  // Raylib Window Initialization
  //-----------------------------------------------------------------
  char title[255];

  if (argc < 2) {
    puts("ERROR: No .lua input file");
    exit(1);
  }

  strcpy(title, "CA_3D - ");
  strcat(title, argv[1]);

  const int screenWidth = 800;
  const int screenHeight = 450;

  SetConfigFlags(FLAG_WINDOW_RESIZABLE | FLAG_MSAA_4X_HINT);

  InitWindow(screenWidth, screenHeight, title);

  SetTargetFPS(60);

  bool paused = false;

  Model cube = LoadModelFromMesh(GenMeshCube(1.0f, 1.0f, 1.0f));

  //-----------------------------------------------------------------
  // Lua Initialization
  //-----------------------------------------------------------------
  L = luaL_newstate();
  luaL_openlibs(L);

  if (luaL_loadfile(L, argv[1]) == LUA_ERRFILE) {
    puts("ERROR: Invalid .lua input file");
    exit(1);
  }
  lua_pcall(L, 0, LUA_MULTRET, 0);

  lua_getglobal(L, "setup");
  lua_pcall(L, 0, 3, 0);

  double cellUpdateRate = lua_tonumber(L, -1);
  lua_pop(L, 1);

  int mapSize = (int)lua_tonumber(L, -2);

  struct cell { // Cell information
    int state;
    unsigned char r;
    unsigned char g;
    unsigned char b;
  };

  struct cell *map = malloc(mapSize * mapSize * mapSize * sizeof(struct cell));

  // Get initial 3D cell map
  for (int x = 0; x < mapSize; x++) {
    for (int y = 0; y < mapSize; y++) {
      for (int z = 0; z < mapSize; z++) {
        lua_geti(L, -1, x);
        lua_geti(L, -1, y);
        lua_geti(L, -1, z);

        lua_getfield(L, -1, "state");
        map[mapSize * mapSize * x + mapSize * y + z].state =
            (int)lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, -1, "r");
        map[mapSize * mapSize * x + mapSize * y + z].r =
            (int)lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, -1, "g");
        map[mapSize * mapSize * x + mapSize * y + z].g =
            (int)lua_tonumber(L, -1);
        lua_pop(L, 1);

        lua_getfield(L, -1, "b");
        map[mapSize * mapSize * x + mapSize * y + z].b =
            (int)lua_tonumber(L, -1);

        lua_pop(L, 4);
      }
    }
  }

  lua_settop(L, 0); // Clear lua stack and keep next computed table
  lua_getglobal(L, "update");
  if (lua_pcall(L, 0, 1, 0) != LUA_OK) {
    printf("%s\n", lua_tostring(L, -1));
    lua_close(L);
    CloseWindow();
  }

  //-----------------------------------------------------------------
  // Raylib Camera
  //-----------------------------------------------------------------
  Camera camera = {0};

  float theta = M_PI_2;
  float phi = M_PI_2;
  float accel = 0.15f;
  float terminalV = CAMERA_ROTATION_SPEED;
  float velocity = terminalV;
  float zoom = 1.0f;

  camera.position =
      (Vector3){(float)mapSize / 2 + 3 * sin(phi) * cos(theta) * mapSize / 2,
                (float)mapSize / 2 + 3 * sin(phi) * sin(theta) * mapSize / 2,
                (float)mapSize / 2 + 3 * cos(phi) * mapSize / 2};

  camera.target =
      (Vector3){// Point camera to center of cube
                (float)mapSize / 2, (float)mapSize / 2, (float)mapSize / 2};
  camera.up = (Vector3){0.0f, 0.0f, 1.0f};
  camera.fovy = 75.0f;
  camera.projection = CAMERA_PERSPECTIVE;

  Vector2 deltaMouse = {0.0f, 0.0f};             // Delta mouse between frames
  Vector2 mousePos = {GetMouseX(), GetMouseY()}; // Last mouse position

  float dt;          // Delta time between frames
  float totalDt = 0; // Used to keep a constant update rate on cell CA

  //-----------------------------------------------------------------
  // Update
  //-----------------------------------------------------------------
  while (!WindowShouldClose()) {
    dt = GetFrameTime() / TIME_DILATION;
    if (!paused)
      totalDt += dt;

    if (IsKeyPressed(KEY_SPACE))
      paused = paused ? false : true;

    // Rotate and move camera around grid
    velocity += accel * dt;
    velocity = velocity > terminalV ? terminalV : velocity;

    if (!paused)
      theta += velocity * dt;

    if (IsKeyDown(KEY_UP))
      phi -= 0.1 * mapSize * dt;
    if (IsKeyDown(KEY_DOWN))
      phi += 0.1 * mapSize * dt;
    if (IsKeyDown(KEY_RIGHT))
      theta += 1 * dt;
    if (IsKeyDown(KEY_LEFT))
      theta -= 1 * dt;

    zoom -= 1.5 * GetMouseWheelMove() * dt;
    zoom = 0.9f > zoom ? 0.9f : zoom;

    if (IsMouseButtonDown(0)) { // Rotate camera with mouse
      int mx = GetMouseX();
      int my = GetMouseY();
      deltaMouse.x = mousePos.x - mx;
      deltaMouse.y = mousePos.y - my;
      mousePos.x = mx;
      mousePos.y = my;
      velocity = 0;
    } else {
      mousePos.x = GetMouseX();
      mousePos.y = GetMouseY();
    }

    theta += 0.002 * zoom * deltaMouse.x;
    phi += 0.002 * zoom * deltaMouse.y;
    deltaMouse.x = 0.0f;
    deltaMouse.y = 0.0f;

    if (phi < 0.0001) { // Clamp phi between 0 and pi
      phi = 0.0001;
    } else if (phi > M_PI - 0.0001) {
      phi = M_PI - 0.0001;
    }

    if (theta > 2 * M_PI) { // Limit theta to avoid overflow
      theta -= 2 * M_PI;
    } else if (theta < -2 * M_PI) {
      theta += 2 * M_PI;
    }

    camera.position = (Vector3){
        (float)mapSize / 2 + 3 * zoom * sin(phi) * cos(theta) * mapSize / 2,
        (float)mapSize / 2 + 3 * zoom * sin(phi) * sin(theta) * mapSize / 2,
        (float)mapSize / 2 + 3 * zoom * cos(phi) * mapSize / 2};

    // Update cell map when thread is done
    if (lua_istable(L, -1) && thread_working == false &&
        totalDt >= cellUpdateRate) {
      totalDt -= cellUpdateRate;
      pthread_join(luathread, NULL);
      for (int x = 0; x < mapSize; x++) {
        for (int y = 0; y < mapSize; y++) {
          for (int z = 0; z < mapSize; z++) {
            lua_geti(L, -1, x);
            lua_geti(L, -1, y);
            lua_geti(L, -1, z);

            lua_getfield(L, -1, "state");
            map[mapSize * mapSize * x + mapSize * y + z].state =
                (int)lua_tonumber(L, -1);
            lua_pop(L, 1);

            lua_getfield(L, -1, "r");
            map[mapSize * mapSize * x + mapSize * y + z].r =
                (int)lua_tonumber(L, -1);
            lua_pop(L, 1);

            lua_getfield(L, -1, "g");
            map[mapSize * mapSize * x + mapSize * y + z].g =
                (int)lua_tonumber(L, -1);
            lua_pop(L, 1);

            lua_getfield(L, -1, "b");
            map[mapSize * mapSize * x + mapSize * y + z].b =
                (int)lua_tonumber(L, -1);

            lua_pop(L, 4);
          }
        }
      }
      lua_settop(L, 0);

      // Call update() from lua
      thread_working = true;
      pthread_create(&luathread, NULL, pcall_thread, NULL);
    }

    //-------------------------------------------------------------
    // Draw
    //-------------------------------------------------------------
    BeginDrawing();

    ClearBackground(BLACK);
    BeginMode3D(camera);
    if (GRID_WIRES_ENABLED) {
      DrawCubeWires((Vector3){mapSize / 2, mapSize / 2, mapSize / 2}, mapSize,
                    mapSize, mapSize, GREEN);
    }

    // Draw 3d grid with cells
    for (int x = 0; x < mapSize; x++) {
      for (int y = 0; y < mapSize; y++) {
        for (int z = 0; z < mapSize; z++) {
          if (map[mapSize * mapSize * x + mapSize * y + z].state != 0) {
            DrawModel(cube, (Vector3){x, y, z}, 1.0f,
                      (Color){map[mapSize * mapSize * x + mapSize * y + z].r,
                              map[mapSize * mapSize * x + mapSize * y + z].g,
                              map[mapSize * mapSize * x + mapSize * y + z].b,
                              255});
            if (CUBE_WIRES_ENABLED) {
              DrawCubeWiresV((Vector3){x, y, z}, (Vector3){1, 1, 1}, BLACK);
            }
          }
        }
      }
    }

    EndMode3D();
    if (SHOW_FPS) {
      DrawFPS(10, 10);
    }

    EndDrawing();
  }

  if (thread_working == true) {
    pthread_join(luathread, NULL);
  }

  lua_close(L);

  UnloadModel(cube);
  free(map);
  CloseWindow();

  return 0;
}
