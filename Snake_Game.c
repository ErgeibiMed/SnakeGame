#include <assert.h>
#include <errno.h>
#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#define SCREEN_HEIGT 600
#define SCREEN_WIDTH 800
#define FOOD_SIZE 10
#define SNAKE_SIZE 20
#define COUNT 4
#define MAX_COUNT 100

// Direction of movement
typedef enum {
  DIR_LEFT = 0,
  DIR_RIGHT = 1,
  DIR_DOWN = 2,
  DIR_UP = 3
} direction_t;

typedef struct Snake {
  Vector2 position;
} snake_t;
// allocating the snake in memory MAX_COUNT RECTANCLES
snake_t **alloc_snake() {
  snake_t **snakes = (snake_t **)malloc(sizeof(snake_t *) * MAX_COUNT);
  if (snakes == NULL) {
    printf("ERROR : could not allocate memory because of %d\n", errno);
    exit(1);
  };
  for (size_t i = 0; i < MAX_COUNT; i++) {
    snakes[i] = (snake_t *)malloc(sizeof(snake_t));
    if (snakes[i] == NULL) {
      printf("ERROR : could not allocate memory because of %d\n", errno);
      exit(1);
    }
    snakes[i]->position = (Vector2){.x = 0, .y = 0};
  }
  return snakes;
}
// free the memory
void free_snake(snake_t **snakes) {
  for (size_t i = 0; i < MAX_COUNT; i++) {
    free(snakes[i]);
    snakes[i] = NULL;
  }
  free(snakes);
  snakes = NULL;
}
// this function draw the snake
void Draw_Snake(snake_t **snakes, size_t count) {
  for (size_t i = 0; i < count; i++) {
    DrawRectangleV(snakes[i]->position,
                   (Vector2){.x = SNAKE_SIZE, .y = SNAKE_SIZE}, GREEN);
  }
}
// this function is responsable of the movement of the snake
direction_t *Move_Snakes(snake_t **snakes, direction_t *direction,
                         Vector2 *next_pos, size_t count) {
  Vector2 previous_pos;
  if (IsKeyPressed(KEY_LEFT) && *direction != DIR_RIGHT) {
    *direction = DIR_LEFT;
  }
  if (IsKeyPressed(KEY_RIGHT) && *direction != DIR_LEFT) {
    *direction = DIR_RIGHT;
  }
  if (IsKeyPressed(KEY_DOWN) && *direction != DIR_UP) {
    *direction = DIR_DOWN;
  }
  if (IsKeyPressed(KEY_UP) && *direction != DIR_DOWN) {
    *direction = DIR_UP;
  }
  switch (*direction) {
  // Snake moving to the left
  case 0:
    if (((*next_pos).x < SNAKE_SIZE / 2.0)) {
      (*next_pos).x = GetScreenWidth();
      (*next_pos).y = snakes[0]->position.y;
    } else {
      (*next_pos).x = (*next_pos).x - SNAKE_SIZE;
      //(*next_pos).y=snakes[0]->position.y;
    };
    previous_pos = snakes[0]->position;
    snakes[0]->position.x = (*next_pos).x;
    snakes[0]->position.y = (*next_pos).y;
    for (size_t i = 1; i < count; i++) {
      Vector2 tmp = snakes[i]->position;
      snakes[i]->position.x = previous_pos.x;
      snakes[i]->position.y = previous_pos.y;
      previous_pos = tmp;
    };
    break;
  // Snake moving to the right
  case 1:
    if (((*next_pos).x > (GetScreenWidth() - SNAKE_SIZE / 2.0))) {
      (*next_pos).x = SNAKE_SIZE / 2.0;
      (*next_pos).y = snakes[0]->position.y;
    } else {
      (*next_pos).x = (*next_pos).x + SNAKE_SIZE;
      //(*next_pos).y=snakes[0]->position.y;
    };
    previous_pos = snakes[0]->position;
    snakes[0]->position = (*next_pos);
    for (size_t i = 1; i < count; i++) {
      Vector2 tmp = snakes[i]->position;
      snakes[i]->position.x = previous_pos.x;
      snakes[i]->position.y = previous_pos.y;
      previous_pos = tmp;
    };
    break;
  // Snake moving to the down
  case 2:
    if (((*next_pos).y > (GetScreenHeight() - SNAKE_SIZE / 2.0))) {
      (*next_pos).x = snakes[0]->position.x;
      (*next_pos).y = SNAKE_SIZE / 2.0;
    } else {
      (*next_pos).y = (*next_pos).y + SNAKE_SIZE;
      //(*next_pos).y=snakes[0]->position.y;
    };
    previous_pos = snakes[0]->position;
    snakes[0]->position = (*next_pos);
    for (size_t i = 1; i < count; i++) {
      Vector2 tmp = snakes[i]->position;
      snakes[i]->position.x = previous_pos.x;
      snakes[i]->position.y = previous_pos.y;
      previous_pos = tmp;
    };
    break;
  // Snake moving to the up
  case 3:
    if (((*next_pos).y < SNAKE_SIZE / 2.0)) {
      (*next_pos).x = snakes[0]->position.x;
      (*next_pos).y = GetScreenHeight() - SNAKE_SIZE / 2.0;
    } else {
      (*next_pos).y = (*next_pos).y - SNAKE_SIZE;
      //(*next_pos).y=snakes[0]->position.y;
    };
    previous_pos = snakes[0]->position;
    snakes[0]->position = (*next_pos);
    for (size_t i = 1; i < count; i++) {
      Vector2 tmp = snakes[i]->position;
      snakes[i]->position.x = previous_pos.x;
      snakes[i]->position.y = previous_pos.y;
      previous_pos = tmp;
    };
    break;
  default:
    break;
  };
  return direction;
}
// initializing the snake object
void initialize_snake(snake_t **snakes) {
  snakes[0]->position =
      (Vector2){.x = GetScreenWidth() / 2.0, .y = GetScreenHeight() / 2.0};
  for (size_t i = 1; i < COUNT; i++) {
    snakes[i]->position.x = snakes[i - 1]->position.x + SNAKE_SIZE;
    snakes[i]->position.y = snakes[i - 1]->position.y;
  }
}

int main(void) {
  SetConfigFlags(FLAG_WINDOW_RESIZABLE); // resize the window
  InitWindow(SCREEN_WIDTH, SCREEN_HEIGT, "SNAKE GAME");
  SetTargetFPS(15); // setting FPS=15
  size_t count = COUNT;
  snake_t **snakes = alloc_snake();
  initialize_snake(snakes);
  direction_t direction = DIR_LEFT;
  bool snake_is_alive = true;
  bool food_is_eaten = 0;
  Vector2 food_pos = {
      .x = GetRandomValue(SNAKE_SIZE, GetScreenWidth() - SNAKE_SIZE),
      .y = GetRandomValue(SNAKE_SIZE, GetScreenHeight() - SNAKE_SIZE)};
  Vector2 next_pos = snakes[0]->position;
  while (!WindowShouldClose()) {
    assert(count < MAX_COUNT &&
           "the snake head count is bigger then what was allocated initially");
    if (snake_is_alive) {
      direction = *Move_Snakes(snakes, &direction, &next_pos, count);
      food_is_eaten = CheckCollisionPointCircle(
          (Vector2){.x = (snakes[0]->position.x) / 2.5,
                    .y = (snakes[0]->position.y) / 2.5},
          (Vector2){.x = food_pos.x / 2.5, .y = food_pos.y / 2.5}, FOOD_SIZE);
      for (size_t i = 4; i < count; i++) {
        Rectangle rec = {.x = snakes[i]->position.x - SNAKE_SIZE,
                         .y = snakes[i]->position.y - SNAKE_SIZE,
                         .width = SNAKE_SIZE,
                         .height = SNAKE_SIZE};
        bool snake_collided_with_itself =
            CheckCollisionPointRec(snakes[0]->position, rec);
        if (snake_collided_with_itself) {
          snake_is_alive = false;
          break;
        }
      }
    }
    if (food_is_eaten) {
      // the food position is recalculated after the food food_is_eaten
      food_pos = (Vector2){
          .x = GetRandomValue(SNAKE_SIZE, GetScreenWidth() - SNAKE_SIZE),
          .y = GetRandomValue(SNAKE_SIZE, GetScreenHeight() - SNAKE_SIZE)};
      food_is_eaten = false; // reseting
      count =
          count + 1; // increase the size of the snake after the food_is_eaten
      snakes[count - 1]->position.x = snakes[count - 2]->position.x;
      snakes[count - 1]->position.y = snakes[count - 2]->position.y;
    };
    BeginDrawing();
    ClearBackground(BLACK);
    if (!snake_is_alive) {
      // snake collided with itself
      //  draw last position of the snake
      Draw_Snake(snakes, count);
      // Display that the GAME is OVER
      DrawText("GAME OVER", GetScreenWidth() / 8.0, 0, 90, RED);
      // Display tha pressing ENTER the game will restart
      DrawText("PRESS ENTER to START OVER", GetScreenWidth() / 8.0, 180, 40,
               BLUE);
      if (IsKeyPressed(KEY_ENTER)) {
        // by pressing enter th game start over
        free_snake(snakes); // deallocating the memory
        count = COUNT;      // setting the count to 4
        snakes =
            alloc_snake(); // allocting the memory again because it was freed
        initialize_snake(snakes);
        snake_is_alive = true;
        food_is_eaten = false;
      }
    } else {
      Draw_Snake(snakes, count);
      DrawCircleV(food_pos, FOOD_SIZE, RED); // DRAW the food
    }
    EndDrawing();
  }
  free_snake(snakes);
  CloseWindow();
  return 0;
}
