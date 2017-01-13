#include "../../include/yrws.h"
#include "../../include/kernel.h"
#include "../../include/sh.h"

struct Point {
	int x, y;
};

struct Ball {
	int x, y, vx, vy;
};

char isAlphabet(char code);
char isKeyPushed(char code);
void InitGame(struct YURI_WINDOW *window);

void DrawEnRacket(struct YURI_WINDOW *window, int y);
void EraseEnRacket(struct YURI_WINDOW *window, int y);
void DrawMyRacket(struct YURI_WINDOW *window, int y);
void EraseMyRacket(struct YURI_WINDOW *window, int y);
void DrawBall(struct YURI_WINDOW *window);
void EraseBall(struct YURI_WINDOW *window);

#define RACKET_WIDTH 8
#define RACKET_HEIGHT 50
#define BALL 8
#define RACKET_BACK 20

char KeyTable[26];

struct Point *Player;
struct Point *Enemy;
struct Ball *ball;
/*
 *キーボードの情報
 *キーを話した時は、キーコードに0x80を足した値が帰ってくる
 */

int tt_main(){
	
	i32_t i, diff;
	struct QUEUE *irq_queue = (struct QUEUE *)memory_alloc(memman, sizeof(struct QUEUE));
	struct Process *me = task_now();

	struct YURI_WINDOW *window = create_window("tennis", 50, 50, 600, 600);

	struct TIMER *timer = timer_alloc();
	
	queue_init(irq_queue, 128, (i32_t *)memory_alloc(memman, sizeof(i32_t) * 128), me);
	/*
	 *タイムリミットになったら10が割り込んでくる
	 */
	timer_init(timer, irq_queue, 10);

	ch_keybuf(irq_queue);

	timer_settime(timer, 1000 / 30);

	InitGame(window);

	while(1){
		if(!queue_size(irq_queue)){
			//キューが空っぽだから寝る
			task_sleep(me);
			io_sti();
		}else{
			i = queue_pop(irq_queue);
			/*
			 *キーボードからの割り込み
			 */
			if(i >= 256 && i <= 511){
				/*
				 *キーコードに直す
				 */
				i -= 256;
				if(i >= 0x01 && i <= 0x58){
					/*
					 *キーを押し込んだ
					 */
					KeyTable[keys0[i] - 0x41] = 1;
				}else if(isAlphabet(keys0[i - 0x80] - 0x41)){
					/*
					 *キー離した
					 */
					KeyTable[keys0[i - 0x80] - 0x41] = 0;
				}
			}else if(i == 10){

                        /*
				 *あたり判定
				 */

				//プレイヤーのあたり判定
				if(ball->x >= Player->x - (RACKET_WIDTH >> 1)){
					if(ball->y >= (Player->y - (RACKET_HEIGHT >> 1)) && ball->y <= (Player->y + (RACKET_HEIGHT))){
						ball->vx *= -1;
						diff = ball->y - Player->y - (RACKET_HEIGHT >> 1);
						if(diff >= 0 && diff <= 15){
							ball->vy = -ball->vx;
						}else if(diff >= 16 && diff <= 35){
							ball->vy = 0;
						}else if(diff >= 36 && diff <= 50){
							ball->vy = ball->vx;
						}
						ball->vx++;
						goto SkipToTimer;
					}
				}

				//敵のあたり判定
				else if(ball->x <= Enemy->x + (RACKET_WIDTH >> 1)){
					if(ball->y >= (Enemy->y - (RACKET_HEIGHT >> 1)) && ball->y <= (Enemy->y + (RACKET_HEIGHT >> 1))){
						ball->vx *= -1;
						diff = ball->y - Enemy->y - (RACKET_HEIGHT >> 1);
						if(diff >= 0 && diff <= 15){
							ball->vy = -ball->vx;
						}else if(diff >= 16 && diff <= 35){
							ball->vy = 0;
						}else if(diff >= 36 && diff <= 50){
							ball->vy = ball->vx;
						}
						ball->vx++;
						goto SkipToTimer;
					}
				}

				if(isKeyPushed('W')){
					if(Player->y > 5){
						EraseMyRacket(window, Player->y);
						Player->y -= 5;
						DrawMyRacket(window, Player->y);
					}
				}else if(isKeyPushed('S')){
					if(Player->y < window->layer->height - 16 - RACKET_HEIGHT){
						EraseMyRacket(window, Player->y);
						Player->y += 5;
						DrawMyRacket(window, Player->y);
					}
				}

			SkipToTimer:

				if(ball->y <= 5){
					ball->vy *= -1;
				}else if(ball->x >= window->layer->height - 10){
					ball->vy *= -1;
				}

				EraseBall(window);
				ball->x += ball->vx;
				ball->y += ball->vy;
				DrawBall(window);

				/*
				 *タイマ再設定
				 */
				timer_settime(timer, 1000 / 30);
			}
		}
	}
}

void DrawEnRacket(struct YURI_WINDOW *window, int y){
	DrawRect(window, RACKET_BACK, y, RACKET_WIDTH, RACKET_HEIGHT, __RGB256COL__(255, 255, 255));
}

void DrawMyRacket(struct YURI_WINDOW *window, int y){
	DrawRect(window, window->layer->width - RACKET_BACK - RACKET_WIDTH, y, RACKET_WIDTH, RACKET_HEIGHT, __RGB256COL__(255, 255, 255));
}

void EraseEnRacket(struct YURI_WINDOW *window, int y){
	DrawRect(window, RACKET_BACK, y, RACKET_WIDTH, RACKET_HEIGHT, __RGB256COL__(0, 0, 0));
}

void EraseMyRacket(struct YURI_WINDOW *window, int y){
	DrawRect(window, window->layer->width - RACKET_BACK - RACKET_WIDTH, y, RACKET_WIDTH, RACKET_HEIGHT, __RGB256COL__(0, 0, 0));
}

void DrawBall(struct YURI_WINDOW *window){
	DrawRect(window, ball->x - (BALL >> 1), ball->y - (BALL >> 1), BALL, BALL, __RGB256COL__(255, 255, 255));
}

void EraseBall(struct YURI_WINDOW *window){
	DrawRect(window, ball->x - (BALL >> 1), ball->y - (BALL >> 1), BALL, BALL, __RGB256COL__(0, 0, 0));
}

void InitGame(struct YURI_WINDOW *window){
      BackGroundColor(window, __RGB256COL__(0, 0, 0));
	Player = (struct Point *)memory_alloc(memman, sizeof(struct Point));
	Enemy = (struct Point *)memory_alloc(memman, sizeof(struct Point));
	ball = (struct Ball *)memory_alloc(memman, sizeof(struct Ball));

	Player->x = window->layer->width - RACKET_BACK - RACKET_WIDTH;
	Player->y = (window->layer->height >> 1) - (RACKET_HEIGHT >> 1);
	Enemy->x = RACKET_BACK;
	Enemy->y = (window->layer->height >> 1) - (RACKET_HEIGHT >> 1);
	ball->x = window->layer->width >> 1;
	ball->y = window->layer->height >> 1;
	ball->vx = 3;
	ball->vy = 0;

	DrawMyRacket(window, Player->y);
	DrawEnRacket(window, Enemy->y);
	DrawBall(window);
}

char isAlphabet(char code){
	return (code >= 0 && code <= 25) ? 1 : 0;
}

char isKeyPushed(char code){
	return KeyTable[code - 0x41] ? 1 : 0;
}
