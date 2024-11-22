#include <assert.h>
#include <math.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <raylib.h>
#define SCREEN_HEIGT 600
#define SCREEN_WIDTH 800
#define EPS          1e-4
#define FOOD_SIZE    10
#define FPS          60
#define SNAKE_SIZE   20
#define COUNT        5


typedef enum {
    DIR_LEFT=0,
    DIR_RIGHT=1,
    DIR_DOWN=2,
    DIR_UP=3
}DIRECTION;

float Vector2_distance(Vector2 v1, Vector2 v2)
{
    float result = sqrtf((v1.x - v2.x)*(v1.x - v2.x) + (v1.y - v2.y)*(v1.y - v2.y));

    return result;
}

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

//allocating the snake in memmory
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
//free the memmory
void free_snake(Snake** snakes,size_t count){
    for(size_t i=0;i<count;i++){
        free(snakes[i]);
        snakes[i]=NULL;
    }
    free(snakes);
    snakes=NULL;
}
//this function is for logging
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
    DrawRectangleV(head, snake_size, GREEN);

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

int Snake_collision_with_itself_up(Snake** snakes,float dt){
    assert(COUNT>4 && "Snake could not collide with itself because it's too small" );
    Vector2  next_pos= {snakes[0]->position.x,(snakes[0]->position.y) - SNAKE_SIZE*dt};
    for(int k=3;k<COUNT;k++){
        float dist=Vector2_distance(next_pos, snakes[k]->position );
        if (dist < EPS){
            printf("YOU LOST\n");
            printf("collided with up [dist:%f] [EPS:%f]\n", dist,EPS);
            return 1;
        }
    }
    // the snake did not collide with itself
    return 0;
}
int Snake_collision_with_itself_down(Snake** snakes,float dt){
    assert(COUNT>4 && "Snake could not collide with itself because it's too small" );
    Vector2 next_pos= {snakes[0]->position.x,(snakes[0]->position.y) + SNAKE_SIZE*dt};
    for(int k=3;k<COUNT;k++){
        float dist=Vector2_distance(next_pos, snakes[k]->position );
        if (dist < EPS){
            printf("YOU LOST\n");
            printf("collided with down [dist:%f] [EPS:%f]\n", dist,EPS);
            printf("snakes[%d]\n",k);
            return 1;
        }
    }
    // the snake did not collide with itself
    return 0;
}
int Snake_collision_with_itself_left(Snake** snakes,float dt){
    assert(COUNT>4 && "Snake could not collide with itself because it's too small" );
    Vector2 next_pos= {(snakes[0]->position.x) - SNAKE_SIZE*dt , snakes[0]->position.y};
    for(int k=3;k<COUNT;k++){
        float dist=Vector2_distance(next_pos,snakes[k]->position);
        if (dist< EPS ){
            printf("YOU LOST\n");
            printf("collided with left [dist:%f] [EPS:%f]\n", dist,EPS);
            return 1;
        }
    }
    // the snake did not collide with itself
    return 0;
}
int Snake_collision_with_itself_right(Snake** snakes,float dt){
    assert(COUNT>4 && "Snake could not collide with itself because it's too small" );
    Vector2 next_pos= {(snakes[0]->position.x) + SNAKE_SIZE*dt , snakes[0]->position.y};
    for(int k=3;k<COUNT;k++){
        float dist=Vector2_distance(next_pos, snakes[k]->position);
        if (dist<EPS){
            printf("YOU LOST\n");
            printf("collided with right [dist:%f] [EPS:%f]\n", dist,EPS);
            return 1;
        }
    }
    // the snake did not collide with itself
    return 0;
}

//this function is responsable of the movement of the snake
DIRECTION* Move_Snakes(Snake** snakes,DIRECTION* direction,Vector2* next_pos,float dt){
    Vector2 previous_pos;
    if(IsKeyPressed(KEY_LEFT)){
        *direction=DIR_LEFT;

    }

    if(IsKeyPressed(KEY_RIGHT)){
        *direction=DIR_RIGHT;
    }

    if(IsKeyPressed(KEY_DOWN)){
        *direction=DIR_DOWN;
    }

    if(IsKeyPressed(KEY_UP)){
        *direction=DIR_UP;
    }
    switch (*direction) {
                //Snake moving to the left
                case 0:
                    if(((*next_pos).x<SNAKE_SIZE/2.0)) {
                        (*next_pos).x=GetScreenWidth();
                        (*next_pos).y=snakes[0]->position.y;
                    }else{
                        (*next_pos).x=(*next_pos).x -  SNAKE_SIZE/dt;
                        //(*next_pos).y=snakes[0]->position.y;
                    };
                    previous_pos= snakes[0]->position;
                    snakes[0]->position=*next_pos;
                    for(int i=1;i<COUNT;i++){
                        Vector2 tmp= snakes[i]->position;
                        snakes[i]->position.x=previous_pos.x  ;
                        snakes[i]->position.y=previous_pos.y ;
                        previous_pos=tmp;
                    }
                   ;
                    break;
                //Snake moving to the right
                case 1:
                    if( ((*next_pos).x>(GetScreenWidth()-SNAKE_SIZE/2.0) ) ) {
                        (*next_pos).x=SNAKE_SIZE/2.0;
                        (*next_pos).y=snakes[0]->position.y;
                    }else{
                        (*next_pos).x=(*next_pos).x +  SNAKE_SIZE/dt;
                        //(*next_pos).y=snakes[0]->position.y;
                    };

                    previous_pos= snakes[0]->position;
                    snakes[0]->position=(*next_pos);
                    for(int i=1;i<COUNT;i++){
                        Vector2 tmp= snakes[i]->position;
                        snakes[i]->position.x=previous_pos.x  ;
                        snakes[i]->position.y=previous_pos.y ;
                        previous_pos=tmp;
            };
                    break;
                //Snake moving to the down
                case 2:
                    if( ((*next_pos).y>(GetScreenHeight()-SNAKE_SIZE/2.0) ) ) {
                        (*next_pos).x=snakes[0]->position.x;
                        (*next_pos).y=SNAKE_SIZE/2.0;
                    }else{
                        (*next_pos).y=(*next_pos).y +  SNAKE_SIZE/dt;
                        //(*next_pos).y=snakes[0]->position.y;
                    };
                    previous_pos= snakes[0]->position;
                    snakes[0]->position=(*next_pos);
                    for(int i=1;i<COUNT;i++){
                        Vector2 tmp= snakes[i]->position;
                        snakes[i]->position.x=previous_pos.x  ;
                        snakes[i]->position.y=previous_pos.y ;
                        previous_pos=tmp;
            };

                    break;
                //Snake moving to the up
                case 3:
                    if( ((*next_pos).y<SNAKE_SIZE/2.0)  ) {
                        (*next_pos).x=snakes[0]->position.x;
                        (*next_pos).y=GetScreenHeight()-SNAKE_SIZE/2.0;
                    }else{
                        (*next_pos).y=(*next_pos).y -  SNAKE_SIZE/dt;
                        //(*next_pos).y=snakes[0]->position.y;
                    };
                    previous_pos= snakes[0]->position;
                    snakes[0]->position=(*next_pos);
                    for(int i=1;i<COUNT;i++){
                        Vector2 tmp= snakes[i]->position;
                        snakes[i]->position.x=previous_pos.x  ;
                        snakes[i]->position.y=previous_pos.y ;
                        previous_pos=tmp;
            };
                    break;

                default:
                    break;

            };
    return direction;
}


int main(void){

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);// resize the window
    InitWindow(SCREEN_WIDTH,SCREEN_HEIGT, "SNAKE GAME");
    SetTargetFPS(FPS);
    Snake **snakes= alloc_snake(COUNT);
    //initializing the snake object
    snakes[0]->position=(Vector2){.x=GetScreenWidth()/2.0,.y=GetScreenHeight()/2.0};
    for(int i=1;i<COUNT;i++){
        snakes[i]->position.x= snakes[i-1]->position.x + SNAKE_SIZE;
        snakes[i]->position.y= snakes[i-1]->position.y ;
    }
     DIRECTION direction = DIR_UP;
    int snake_is_alive=1;

    float dt =FPS*0.25*0.25*0.25;
    //float dt =1.0;
    Vector2 next_pos=snakes[0]->position;
    while(!WindowShouldClose()){
        BeginDrawing();
        ClearBackground(BLACK);
            if(snake_is_alive){
            for(int i=0;i<COUNT;i++){
                Draw_Snake(snakes[i]->position);
            }
            direction=*Move_Snakes(snakes, &direction, &next_pos, dt);

                //snake_is_alive = Move_Snakes(snakes, dt) ;
                print_snakes(snakes);
            }else{
                DrawText("YOU LOST",GetScreenWidth()/4.0, GetScreenHeight()/2.0,90 , RED);
            };
            //Food_Spawn();
            EndDrawing();
        }

    free_snake(snakes, COUNT);

    CloseWindow();


    return 0;
}

