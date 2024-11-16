#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#include <time.h>
#define SCREEN_HEIGT 600
#define SCREEN_WIDTH 800
#define EPS          0.001
#define FOOD_SIZE    10
#define FPS          60
#define SNAKE_SIZE   20
#define COUNT        5

void Food_Spawn(){
    //SetRandomSeed(FPS-10);
    int cx = GetRandomValue((FOOD_SIZE/2)+1 , SCREEN_WIDTH-(FOOD_SIZE/2)+1 );
    int cy= GetScreenHeight()/2;
    Vector2 center= {
        .x=cx,
        .y=cy
    };
    DrawCircleV( center,  FOOD_SIZE , RED);
}

typedef struct Snake{
    Vector2 position;
}Snake;

Snake** alloc_snake(size_t count){
    Snake** snakes=(Snake**)malloc(sizeof(Snake*)*COUNT);
    if (snakes==NULL) {
        printf("ERROR : could not allocate memory because of %d\n",errno);
        exit(1);
    };
    for(size_t i=0;i<count;i++){
        snakes[i]=(Snake*)malloc(sizeof(Snake));
        if(snakes[i]==NULL){
            printf("ERROR : could not allocate memory because of %d\n",errno);
            exit(1);
        }
        snakes[i]->position=(Vector2){.x=0,.y=0};
    }

    return snakes;
}
void free_snake(Snake** snakes,size_t count){
    for(size_t i=0;i<count;i++){
        free(snakes[i]);
        snakes[i]=NULL;
    }
    free(snakes);
    snakes=NULL;
}
void print_snakes(Snake** snakes){
    printf("----------------------------------\n");
    for(int i=0;i<COUNT;i++){
        printf("sanke(%d).vector{x=%f,y=%f}\n",i, snakes[i]->position.x, snakes[i]->position.y);
    }
    printf("----------------------------------\n");
}

void Draw_Snake(Vector2 head){
    Vector2 snake_size= {
        .x=SNAKE_SIZE,
        .y=SNAKE_SIZE
    };
    DrawRectangleV(head, snake_size, RED);

}

//find a better name for this function
int Snakes_collision_with_left_border(Snake* head){

    if ( (head->position.x - SNAKE_SIZE/2.0)< EPS) {
         head->position.x=GetScreenWidth()- SNAKE_SIZE/2.0;
        return 1;//collision with left border
}
    return 0;
}
int Snakes_collision_with_right_border(Snake* head){
    if (head->position.x > (GetScreenWidth()- SNAKE_SIZE/2.0)) {
        head->position.x=SNAKE_SIZE/2.0;
        return 1; //collision with right border;
    }else{
        return 0 ;
    }
}
int Snakes_collision_with_up_border(Snake* head){
    if (head->position.y < SNAKE_SIZE/2.0) {
        head->position.y=GetScreenHeight()-SNAKE_SIZE/2.0;
        return 1; //collision with right border;
    }else{
        return 0 ;
    }
}
int Snakes_collision_with_down_border(Snake* head){
    if (head->position.y > (GetScreenHeight()- SNAKE_SIZE/2.0)) {
        head->position.y=SNAKE_SIZE/2.0;
        return 1; //collision with right border;
    }else{
        return 0 ;
    }
}



void Move_Snakes(Snake** snakes,float speed){
    if(IsKeyPressed(KEY_LEFT)){
        Vector2 temp = snakes[0]->position;
        if(!Snakes_collision_with_left_border(snakes[0])){
        snakes[0]->position.x=(snakes[0]->position.x - SNAKE_SIZE)*speed;
        }
        for(int i=1;i<COUNT;i++){
            Vector2 temp2 = snakes[i]->position;
            snakes[i]->position=temp;
            temp= temp2;
        }
    }if(IsKeyPressed(KEY_RIGHT)){
        Vector2 temp = snakes[0]->position;
        if(!Snakes_collision_with_right_border(snakes[0])){
        snakes[0]->position.x=(snakes[0]->position.x + SNAKE_SIZE)*speed;
        }
        for(int i=1;i<COUNT;i++){
            Vector2 temp2 = snakes[i]->position;
            snakes[i]->position=temp;
            temp= temp2;
        }
    }if(IsKeyPressed(KEY_DOWN)){
        Vector2 temp = snakes[0]->position;
        if (!Snakes_collision_with_down_border(snakes[0])){
        snakes[0]->position.y=(snakes[0]->position.y + SNAKE_SIZE)*speed;
    }
        for(int i=1;i<COUNT;i++){
            Vector2 temp2 = snakes[i]->position;
            snakes[i]->position=temp;
            temp= temp2;
        }
    }if(IsKeyPressed(KEY_UP)){
        Vector2 temp = snakes[0]->position;
        if (!Snakes_collision_with_up_border(snakes[0])){
        snakes[0]->position.y=(snakes[0]->position.y - SNAKE_SIZE)*speed;
        }
        for(int i=1;i<COUNT;i++){
            Vector2 temp2 = snakes[i]->position;
            snakes[i]->position=temp;
            temp= temp2;
        }
    }
}


int main(void){

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(SCREEN_WIDTH,SCREEN_HEIGT, "SNAKE GAME");
    SetTargetFPS(FPS);
    Snake **snakes= alloc_snake(COUNT);
    Vector2 init=(Vector2){.x=GetScreenWidth()/2.0,.y=GetScreenHeight()/2.0};
    snakes[0]->position=init;
    for(int i=1;i<COUNT;i++){
        snakes[i]->position.x= snakes[i-1]->position.x + 2*SNAKE_SIZE;
        snakes[i]->position.y= snakes[i-1]->position.y ;
    }
    float deltaTime =1.0f;

    while(!WindowShouldClose()){
        BeginDrawing();
        for(int i=0;i<COUNT;i++){
            Draw_Snake(snakes[i]->position);
        }
        print_snakes(snakes);
        //Move_Snake(snakes[0],deltaTime );
        Move_Snakes(snakes, deltaTime);
        ClearBackground(BLACK);
        //Food_Spawn();
        EndDrawing();
    }

    free_snake(snakes, COUNT);

    CloseWindow();


    return 0;
}

