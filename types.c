#include <raylib.h>

typedef enum Archetype {
  arch_nil = 0,
  arch_player = 1,
  arch_book = 2,
  arch_bookcase = 3,
} Archetype;

typedef struct Entity {
  Vector2 pos;
  bool is_valid;
  Archetype arch;
  Texture2D sprite;
} Entity;

typedef enum ItemType {
  itemType_nil = 0,
  itemType_book = 1,
} ItemType;

typedef struct Item {
  ItemType type;
  Texture2D sprite;
} Item;

typedef struct Item_Slot {
  Item item;
  int Count;
} Item_Slot;

#define MAX_INVENTORY_COUNT 128
#define MAX_ENTITY_COUNT 1024

