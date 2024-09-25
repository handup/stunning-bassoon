#include <alloca.h>
#include <math.h>
#include <raylib.h>
#include <raymath.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "types.c"

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;
const float gridSize = 32;

Vector2 normalize(Vector2 p) {
  float w = sqrt(p.x * p.x + p.y * p.y);
  p.x /= w;
  p.y /= w;
  return p;
}

typedef struct World {
  Entity entities[MAX_ENTITY_COUNT];
  Item_Slot inventory[MAX_INVENTORY_COUNT];
} World;
World *world = 0;

void addItem(ItemType type) { world->inventory[type].Count++; }
void removeItem(ItemType type) {
  if (world->inventory[type].Count > 0) {
    world->inventory[type].Count--;
  } else {
    printf("Cannot remove item with count 0/n");
  }
}

Entity *entity_create() {
  Entity *entity_found = 0;
  for (int i = 0; i < MAX_ENTITY_COUNT; i++) {
    Entity *existing_entity = &world->entities[i];
    if (!existing_entity->is_valid) {
      entity_found = existing_entity;
      break;
    }
  }
  if (entity_found == 0) {
    printf("No more free entities!");
  }

  entity_found->is_valid = true;
  return entity_found;
}

void entity_destroy(Entity *entity) { memset(entity, 0, sizeof(Entity)); }

void setup_bookcase(Entity *entity) { entity->arch = arch_bookcase; }

void setup_player(Entity *entity) {
  Vector2 playerPos = {((float)SCREEN_WIDTH) / 2, ((float)SCREEN_HEIGHT) / 2};
  entity->pos = playerPos;
  entity->sprite = LoadTextureFromImage(LoadImage("resources/MC.png"));
}

void setup_book(Entity *entity) { entity->arch = arch_book; }

int main() {
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "STEVE");
  world = alloca(sizeof(World));

  Entity *player = entity_create();
  setup_player(player);
  Texture2D bookbase =
      LoadTextureFromImage(LoadImage("resources/Bookcase.png"));

  Texture2D book = LoadTextureFromImage(LoadImage("resources/Book.png"));
  Image checkedIm =
      GenImageChecked(SCREEN_WIDTH * 3, SCREEN_HEIGHT * 3, gridSize, gridSize,
                      (Color){30, 9, 77, 100}, (Color){13, 3, 36, 100});
  Texture2D checked = LoadTextureFromImage(checkedIm);

  UnloadImage(checkedIm);

  for (int i = 0; i < 10; i++) {
    Entity *en = entity_create();
    setup_book(en);
    Vector2 randCoordinates = {
        (float)GetRandomValue(0, SCREEN_WIDTH / gridSize) * gridSize,
        (float)GetRandomValue(0, SCREEN_HEIGHT / gridSize) * gridSize + 16};
    en->pos = randCoordinates;
    en->sprite = book;
  }

  Camera2D camera = {0};
  camera.target = (Vector2){player->pos.x + 20.0f, player->pos.y + 20.0f};
  camera.offset = (Vector2){SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f};
  camera.rotation = 0.0f;
  camera.zoom = 2.0f;

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);
    Vector2 mp = GetMousePosition();
    bool clicked = IsMouseButtonPressed(MOUSE_LEFT_BUTTON);

    float dt = GetFrameTime();

    DrawText(TextFormat("FPS: %d", GetFPS() ), 200, 80, 20, RED);
 
    Vector2 input = {0, 0};

    float speed = 500.0f;
    if (IsKeyDown(KEY_RIGHT))
      input.x += 1;
    if (IsKeyDown(KEY_LEFT))
      input.x -= 1;
    if (IsKeyDown(KEY_UP))
      input.y -= 1;
    if (IsKeyDown(KEY_DOWN))
      input.y += 1;

    if(IsKeyReleased(KEY_B) && world->inventory[itemType_book].Count > 4){
      Entity *en = entity_create();
      setup_bookcase(en);
      Vector2 mouseWorld = GetScreenToWorld2D(mp, camera);
      Vector2 randCoordinates = {
        mouseWorld.x - fmod(mouseWorld.x, gridSize),
        mouseWorld.y - fmod(mouseWorld.y, gridSize) + 16};
      en->pos = randCoordinates;
      en->sprite = bookbase;
      world->inventory[itemType_book].Count -= 4;
    }

    if (input.x != 0 || input.y != 0)
      input = normalize(input);

    player->pos.x = player->pos.x + input.x * speed * dt;
    player->pos.y = player->pos.y + input.y * speed * dt;

    if(player->pos.x < -SCREEN_WIDTH){
      player->pos.x = -SCREEN_WIDTH;
    }
    if(player->pos.x + 32 > 2 * SCREEN_WIDTH){
      player->pos.x = 2 * SCREEN_WIDTH - 32;
    }  
    if(player->pos.y < -SCREEN_HEIGHT){
      player->pos.y = -SCREEN_HEIGHT;
    } 
    if(player->pos.y + 32 > 2 * SCREEN_HEIGHT){
      player->pos.y = 2 * SCREEN_HEIGHT - 32;
    } 
    // Camera target follows player
    camera.target = (Vector2){player->pos.x + 20, player->pos.y + 20};
    BeginMode2D(camera);

    DrawTexture(checked, -SCREEN_WIDTH, -SCREEN_HEIGHT, WHITE);

    for (int i = 0; i < MAX_ENTITY_COUNT; i++) {
      Entity *existing_entity = &world->entities[i];
      if (existing_entity->is_valid) {
        switch (existing_entity->arch) {
        case arch_book:
          bool in_range = Vector2Distance(world->entities[i].pos, player->pos) <
                          gridSize * 2;
          if (clicked && in_range) {
            Rectangle rec =
                (Rectangle){world->entities[i].pos.x, world->entities[i].pos.y,
                            book.width, book.height};
            if (CheckCollisionPointRec(GetScreenToWorld2D(mp, camera), rec)) {
              world->entities[i].is_valid = false;
              addItem(itemType_book);
              break;
            }
          } else if (in_range) {
            DrawTexture(book, world->entities[i].pos.x,
                        world->entities[i].pos.y, PINK);
          } else {
            DrawTexture(book, world->entities[i].pos.x,
                        world->entities[i].pos.y, WHITE);
          }
          break;
        case arch_bookcase:
          DrawTexture(bookbase, world->entities[i].pos.x,
                        world->entities[i].pos.y, WHITE);
          break;
        case arch_player:
          DrawTexture(player->sprite, player->pos.x, player->pos.y, WHITE);
          break;
        default:
          break;
        }
      }
    }
    EndMode2D();
    
    if (IsKeyDown(KEY_I)) {
      DrawTexture(book, 100, 32, WHITE);
      DrawText(TextFormat("Books: %03i", world->inventory[itemType_book].Count),
               200, 40, 20, WHITE);
    }
    EndDrawing();
  }

  CloseWindow();

  return 0;
}
