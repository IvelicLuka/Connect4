#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"
#define FACTOR 350
#define WIDTH 4*FACTOR
#define HEIGHT 3*FACTOR
#define BOARD_WIDTH WIDTH*0.75
#define BOARD_HEIGHT HEIGHT*0.75
#define COLS 7
#define ROWS 6
#define RADIUS (FACTOR/7)
#define FONT_SIZE (FACTOR/5)
#define THICKNESS (FACTOR/20)
#define BUTTON_WIDTH  (BOARD_WIDTH/2)
#define BUTTON_HEIGHT  (BOARD_HEIGHT/5)
#define PADDING (FACTOR/50)
#define GRAV ((HEIGHT)/(float)500000)

int polje[ROWS][COLS] = { 0 };

int checkWin(int* wi, int* wj, int* ei, int* ej);

int main(void) {
	int wi=0, wj=0, ei=0, ej=0, gi=0, gj=0, win=0, just=0;
	int turn = 1;
	
	double fallx, fally, fallv, hidei, hidej;
	int falling = 0;

	Color TransparentGreen = GREEN;
	TransparentGreen.a = 128;

	InitWindow(WIDTH, HEIGHT, "Connect4");

	Rectangle board = CLITERAL(Rectangle){(WIDTH - BOARD_WIDTH)/2, (HEIGHT - BOARD_HEIGHT)/2, BOARD_WIDTH, BOARD_HEIGHT};
	Rectangle button = CLITERAL(Rectangle){WIDTH/2 - BUTTON_WIDTH/2, HEIGHT/2 - BUTTON_HEIGHT/2, BUTTON_WIDTH, BUTTON_HEIGHT};

	while (!WindowShouldClose()) {
		just = 0;

		Vector2 mouse = GetMousePosition();
		int highlighted =CheckCollisionPointRec(mouse, button); 
		int mi = (mouse.y - board.y)/(BOARD_HEIGHT)*ROWS;
		int mj = (mouse.x - board.x)/(BOARD_WIDTH)*COLS;
		if (mi < 0) mi = 0; if (mi >= ROWS) mi = ROWS-1;
		if (mj < 0) mj = 0; if (mj >= COLS) mj = COLS-1;
		gj = mj;
		gi = mi;
		
		if (!polje[mi][mj]) {
			while (0 <= gi && gi < ROWS-1 && !polje[gi+1][mj]) {
				gi++;
			}
		}

		if (!win && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
			if (!polje[gi][gj]) {
				polje[gi][gj] = turn;

				hidei = gi;
				hidej = gj;
				fallx = board.x + (gj+0.5)*BOARD_WIDTH/COLS;
				fally =  board.y + (0+0.5)*BOARD_HEIGHT/ROWS;
				fallv = 0;
				falling = 1;
				
				win = checkWin(&wi, &wj, &ei, &ej);
				turn *= -1;
				just = 1;
			}
		}
			
		if (falling) {
			fallv += GRAV;
			fally += fallv;
			if (fally >= board.y + (hidei)*BOARD_WIDTH/COLS - RADIUS/2) {
				falling = 0;
			}
		}

		if (IsKeyPressed(KEY_R) || win && !just && IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && highlighted) {
			win = 0; turn = 1;
			for (int i = 0; i < ROWS; i++) {
				for (int j = 0; j < COLS; j++) {
					polje[i][j] = 0;
				}			
			}
		}

		BeginDrawing();
			ClearBackground(WHITE);
			DrawRectangleRec(board, SKYBLUE);
			for (int i = 0; i < ROWS; i++) {
				for (int j = 0; j < COLS; j++) {
					DrawCircle(board.x + (j+0.5)*BOARD_WIDTH/COLS, board.y + (i+0.5)*BOARD_HEIGHT/ROWS, RADIUS, WHITE);
				}
			}

			if (falling) DrawCircle(fallx, fally, RADIUS, -turn == 1 ? YELLOW : RED);

			Color c; int drawing;
			for (int i = 0; i < ROWS; i++) {
				for (int j = 0; j < COLS; j++) {
					if (falling && i == hidei && j == hidej) continue;
					drawing = polje[i][j];
					if (!win) {
						if (i == gi && j == gj) {c = (turn == 1 ? YELLOW : RED); c.a = 128; drawing = 1;}
						if (i == mi && j== mj) {c = GREEN; c.a = 192; drawing = 1;}
					}
					if (polje[i][j] == 1 ) c = YELLOW;
					if (polje[i][j] == -1) c = RED;
					if (drawing) DrawCircle(board.x + (j+0.5)*BOARD_WIDTH/COLS, board.y + (i+0.5)*BOARD_HEIGHT/ROWS, RADIUS, c);
				}
			}
			if (win) {
				DrawLineEx(	CLITERAL(Vector2){board.x + (wj+0.5)*BOARD_WIDTH/COLS, board.y + (wi+0.5)*BOARD_HEIGHT/ROWS},
							CLITERAL(Vector2){board.x + (ej+0.5)*BOARD_WIDTH/COLS, board.y + (ei+0.5)*BOARD_HEIGHT/ROWS}, THICKNESS, GREEN);

				char *str = malloc(128);
				sprintf(str, "The Winner Is: %s", win == 1 ? "Player one" : "Player two");
				int txt_w = MeasureText(str, FONT_SIZE);
				Rectangle title = CLITERAL(Rectangle){WIDTH/2 - txt_w/2 - PADDING, 0, txt_w + 2*PADDING, FONT_SIZE + PADDING};
				DrawRectangleRec(title, RAYWHITE);
				DrawRectangleLinesEx(title, 5, BLACK);
				DrawText(str, WIDTH/2 - txt_w/2, 0, FONT_SIZE, win == 1 ? YELLOW : RED);



				DrawRectangleRec(button, highlighted ? LIME : TransparentGreen);
				DrawRectangleLinesEx(button, 5, BLACK);

				char *button_txt = "Restart";
				int button_txt_w = MeasureText(button_txt, FONT_SIZE);
				DrawText(button_txt, button.x + button.width/2 - button_txt_w/2, button.y + button.height/2 - FONT_SIZE/2, FONT_SIZE, BLACK);
			}
		EndDrawing();
	}
	CloseWindow();
}


int checkWin(int* wi, int* wj, int* ei, int* ej) {
	for (int i = 0; i < ROWS; i++) {
		for (int j = 0; j < COLS-3; j++) {
			if (polje[i][j] && polje[i][j] == polje[i][j+1] && polje[i][j] == polje[i][j+2] && polje[i][j] == polje[i][j+3]) {
				*wi = i; *wj = j; *ei = i; *ej = j+3;return polje[i][j];}
		}
	}	
	for (int j = 0; j < COLS; j++) {
		for (int i = 0; i < ROWS-3; i++) {
			if (polje[i][j] && polje[i][j] == polje[i+1][j] && polje[i][j] == polje[i+2][j] && polje[i][j] == polje[i+3][j]) {
				*wi = i; *wj = j; *ei = i+3; *ej = j;return polje[i][j];}
		}
	}
	for (int j = 0; j < COLS-3; j++) {
		for (int i = 0; i < ROWS-3; i++) {
			if (polje[i][j] && polje[i][j] == polje[i+1][j+1] && polje[i][j] == polje[i+2][j+2] && polje[i][j] == polje[i+3][j+3]) {
				*wi = i; *wj = j; *ei = i+3; *ej = j+3;return polje[i][j];}
		}
	}
	for (int j = 3; j < COLS; j++) {
		for (int i = 0; i < ROWS-3; i++) {
			if (polje[i][j] && polje[i][j] == polje[i+1][j-1] && polje[i][j] == polje[i+2][j-2] && polje[i][j] == polje[i+3][j-3]) {
				*wi = i; *wj = j; *ei = i+3; *ej = j-3;return polje[i][j];}
		}
	}
	return 0;
}
