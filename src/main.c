#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <psxgpu.h>
#include "assets/assets.h"
#include "libs/game_pad.h"
#include "libs/numeric.h"
#include "libs/math.h"
#include "libs/image_loader.h"

// Length of the ordering table, i.e. the range Z coordinates can have, 0-15 in
// this case. Larger values will allow for more granularity with depth (useful
// when drawing a complex 3D scene) at the expense of RAM usage and performance.
#define OT_LENGTH 16

// Size of the buffer GPU commands and primitives are written to. If the program
// crashes due to too many primitives being drawn, increase this value.
#define BUFFER_LENGTH 8192

/* Framebuffer/display list class */

typedef struct
{
   DISPENV disp_env;
   DRAWENV draw_env;

   uint32_t ot[OT_LENGTH];
   uint8_t buffer[BUFFER_LENGTH];
} RenderBuffer;

typedef struct
{
   RenderBuffer buffers[2];
   uint8_t *next_packet;
   int active_buffer;
} RenderContext;

void setup_context(RenderContext *ctx, int w, int h, int r, int g, int b)
{
   // Place the two framebuffers vertically in VRAM.
   SetDefDrawEnv(&(ctx->buffers[0].draw_env), 0, 0, w, h);
   SetDefDispEnv(&(ctx->buffers[0].disp_env), 0, 0, w, h);
   SetDefDrawEnv(&(ctx->buffers[1].draw_env), 0, h, w, h);
   SetDefDispEnv(&(ctx->buffers[1].disp_env), 0, h, w, h);

   // Set the default background color and enable auto-clearing.
   setRGB0(&(ctx->buffers[0].draw_env), r, g, b);
   setRGB0(&(ctx->buffers[1].draw_env), r, g, b);
   ctx->buffers[0].draw_env.isbg = 1;
   ctx->buffers[1].draw_env.isbg = 1;

   // Initialize the first buffer and clear its OT so that it can be used for
   // drawing.
   ctx->active_buffer = 0;
   ctx->next_packet = ctx->buffers[0].buffer;
   ClearOTagR(ctx->buffers[0].ot, OT_LENGTH);

   // Turn on the video output.
   SetDispMask(1);
}

void flip_buffers(RenderContext *ctx)
{
   // Wait for the GPU to finish drawing, then wait for vblank in order to
   // prevent screen tearing.
   DrawSync(0);
   VSync(0);

   RenderBuffer *draw_buffer = &(ctx->buffers[ctx->active_buffer]);
   RenderBuffer *disp_buffer = &(ctx->buffers[ctx->active_buffer ^ 1]);

   // Display the framebuffer the GPU has just finished drawing and start
   // rendering the display list that was filled up in the main loop.
   PutDispEnv(&(disp_buffer->disp_env));
   DrawOTagEnv(&(draw_buffer->ot[OT_LENGTH - 1]), &(draw_buffer->draw_env));

   // Switch over to the next buffer, clear it and reset the packet allocation
   // pointer.
   ctx->active_buffer ^= 1;
   ctx->next_packet = disp_buffer->buffer;
   ClearOTagR(disp_buffer->ot, OT_LENGTH);
}

void *new_primitive(RenderContext *ctx, int z, size_t size)
{
   // Place the primitive after all previously allocated primitives, then
   // insert it into the OT and bump the allocation pointer.
   RenderBuffer *buffer = &(ctx->buffers[ctx->active_buffer]);
   uint8_t *prim = ctx->next_packet;

   addPrim(&(buffer->ot[z]), prim);
   ctx->next_packet += size;

   // Make sure we haven't yet run out of space for future primitives.
   assert(ctx->next_packet <= &(buffer->buffer[BUFFER_LENGTH]));

   return (void *)prim;
}

// A simple helper for drawing text using PSn00bSDK's debug font API. Note that
// FntSort() requires the debug font texture to be uploaded to VRAM beforehand
// by calling FntLoad().
void draw_text(RenderContext *ctx, int x, int y, int z, const char *text)
{
   RenderBuffer *buffer = &(ctx->buffers[ctx->active_buffer]);

   ctx->next_packet = (uint8_t *)
       FntSort(&(buffer->ot[z]), ctx->next_packet, x, y, text);

   assert(ctx->next_packet <= &(buffer->buffer[BUFFER_LENGTH]));
}

/* Pong Game Structures and Constants */

#define SCREEN_XRES 320
#define SCREEN_YRES 240

#define PADDLE_WIDTH 8
#define PADDLE_HEIGHT 60
#define BALL_SIZE 8
#define PADDLE_SPEED 4
#define INITIAL_BALL_SPEED 2
#define PADDLE_MARGIN 10

typedef struct
{
   int x, y;
   int vel_x, vel_y;
} Ball;

typedef struct
{
   int y;
   int score;
} Paddle;

typedef enum
{
   GAME_MENU,
   GAME_PLAYING,
   GAME_PAUSED,
   GAME_OVER
} GameState;

void reset_ball(Ball *ball)
{
   ball->x = SCREEN_XRES / 2 - BALL_SIZE / 2;
   ball->y = SCREEN_YRES / 2 - BALL_SIZE / 2;
   // Random direction: either left or right
   ball->vel_x = (ball->x % 2 == 0) ? INITIAL_BALL_SPEED : -INITIAL_BALL_SPEED;
   ball->vel_y = (ball->y % 3 == 0) ? 1 : -1;
}

void update_ball(Ball *ball, Paddle *left_paddle, Paddle *right_paddle)
{
   // Move ball
   ball->x += ball->vel_x;
   ball->y += ball->vel_y;

   // Bounce off top and bottom walls
   if (ball->y <= 0 || ball->y >= SCREEN_YRES - BALL_SIZE)
   {
      ball->vel_y = -ball->vel_y;
      ball->y = (ball->y <= 0) ? 0 : SCREEN_YRES - BALL_SIZE;
   }

   // Check collision with left paddle
   if (ball->x <= PADDLE_WIDTH + PADDLE_MARGIN &&
       ball->y + BALL_SIZE >= left_paddle->y &&
       ball->y <= left_paddle->y + PADDLE_HEIGHT)
   {
      ball->vel_x = -ball->vel_x;
      ball->x = PADDLE_WIDTH + PADDLE_MARGIN;
      // Add some vertical velocity based on where ball hits paddle
      int hit_pos = (ball->y + BALL_SIZE / 2) - (left_paddle->y + PADDLE_HEIGHT / 2);
      ball->vel_y += hit_pos / 15;
   }

   // Check collision with right paddle
   if (ball->x + BALL_SIZE >= SCREEN_XRES - PADDLE_WIDTH - PADDLE_MARGIN &&
       ball->y + BALL_SIZE >= right_paddle->y &&
       ball->y <= right_paddle->y + PADDLE_HEIGHT)
   {
      ball->vel_x = -ball->vel_x;
      ball->x = SCREEN_XRES - PADDLE_WIDTH - BALL_SIZE - PADDLE_MARGIN;
      // Add some vertical velocity based on where ball hits paddle
      int hit_pos = (ball->y + BALL_SIZE / 2) - (right_paddle->y + PADDLE_HEIGHT / 2);
      ball->vel_y += hit_pos / 15;
   }

   // Limit ball velocity
   if (ball->vel_y > 3)
      ball->vel_y = 3;
   if (ball->vel_y < -3)
      ball->vel_y = -3;
}

int check_scoring(Ball *ball, Paddle *left_paddle, Paddle *right_paddle)
{
   // Check if ball went off screen
   if (ball->x < 0)
   {
      right_paddle->score++;
      return 1; // Right player scored
   }
   if (ball->x > SCREEN_XRES)
   {
      left_paddle->score++;
      return 2; // Left player scored
   }
   return 0; // No score
}

void draw_paddle(RenderContext *ctx, int x, int y)
{
   TILE *tile = (TILE *)new_primitive(ctx, 1, sizeof(TILE));
   setTile(tile);
   setXY0(tile, x, y);
   setWH(tile, PADDLE_WIDTH, PADDLE_HEIGHT);
   setRGB0(tile, 255, 255, 255);
}

void draw_ball(RenderContext *ctx, Ball *ball)
{
   TILE *tile = (TILE *)new_primitive(ctx, 1, sizeof(TILE));
   setTile(tile);
   setXY0(tile, ball->x, ball->y);
   setWH(tile, BALL_SIZE, BALL_SIZE);
   setRGB0(tile, 255, 255, 0);
}

void draw_center_line(RenderContext *ctx)
{
   for (int y = 0; y < SCREEN_YRES; y += 16)
   {
      TILE *tile = (TILE *)new_primitive(ctx, 0, sizeof(TILE));
      setTile(tile);
      setXY0(tile, SCREEN_XRES / 2 - 1, y);
      setWH(tile, 2, 8);
      setRGB0(tile, 128, 128, 128);
   }
}

int main(int argc, const char **argv)
{
   // Initialize the GPU and load the default font texture provided by
   // PSn00bSDK at (960, 0) in VRAM.
   ResetGraph(0);
   FntLoad(960, 0);

   // Set up our rendering context.
   RenderContext ctx;
   image_init();
   setup_context(&ctx, SCREEN_XRES, SCREEN_YRES, 0, 0, 60); // Dark blue background

   // Initialize game pads for both players
   GamePad pad1 = init_game_pad(0); // Player 1 (left paddle)
   GamePad pad2 = init_game_pad(1); // Player 2 (right paddle)

   // Game state
   GameState state = GAME_MENU;
   Ball ball;
   Paddle left_paddle = {SCREEN_YRES / 2 - PADDLE_HEIGHT / 2, 0};
   Paddle right_paddle = {SCREEN_YRES / 2 - PADDLE_HEIGHT / 2, 0};

   reset_ball(&ball);

   for (;;)
   {
      // Sync pad states
      sync_pad(&pad1);
      sync_pad(&pad2);

      char text_buffer[256];

      switch (state)
      {
      case GAME_MENU:
         draw_text(&ctx, SCREEN_XRES / 2 - 32, SCREEN_YRES / 2 - 40, 0, "PONG");
         draw_text(&ctx, SCREEN_XRES / 2 - 80, SCREEN_YRES / 2 - 16, 0, "PRESS X TO START");
         draw_text(&ctx, SCREEN_XRES / 2 - 120, SCREEN_YRES / 2 + 8, 0, "PLAYER 1: LEFT PADDLE (PAD 1)");
         draw_text(&ctx, SCREEN_XRES / 2 - 120, SCREEN_YRES / 2 + 24, 0, "PLAYER 2: RIGHT PADDLE (PAD 2)");
         draw_text(&ctx, SCREEN_XRES / 2 - 80, SCREEN_YRES / 2 + 48, 0, "USE D-PAD UP/DOWN");

         if (is_button_just_released(&pad1, PAD_BUTTON_CIRCLE))
         {
            state = GAME_PLAYING;
            left_paddle.score = 0;
            right_paddle.score = 0;
            reset_ball(&ball);
         }

         break;

      case GAME_PLAYING:

         if (pad1.connected)
         {
            if (pad1.dpad.up && left_paddle.y > 0)
            {
               left_paddle.y -= PADDLE_SPEED;
            }
            if (pad1.dpad.down && left_paddle.y < SCREEN_YRES - PADDLE_HEIGHT)
            {
               left_paddle.y += PADDLE_SPEED;
            }

            // Handle analog stick for player 1
            if (is_analog_available(&pad1))
            {
               float analog_y = get_analog_y_normalized(&pad1, true);
               left_paddle.y += (int)(analog_y * PADDLE_SPEED);
            }
         }
         else
         {
            // AI for left paddle if no controller
            int ball_center = ball.y + BALL_SIZE / 2;
            int paddle_center = left_paddle.y + PADDLE_HEIGHT / 2;
            if (ball_center < paddle_center - 10)
            {
               left_paddle.y -= PADDLE_SPEED - 2;
            }
            else if (ball_center > paddle_center + 10)
            {
               left_paddle.y += PADDLE_SPEED - 2;
            }
         }

         // Player 2 (right paddle) controls
         if (pad2.connected)
         {
            if (pad2.dpad.up && right_paddle.y > 0)
            {
               right_paddle.y -= PADDLE_SPEED;
            }
            if (pad2.dpad.down && right_paddle.y < SCREEN_YRES - PADDLE_HEIGHT)
            {
               right_paddle.y += PADDLE_SPEED;
            }

            // Handle analog stick for player 2
            if (is_analog_available(&pad2))
            {
               float analog_y = get_analog_y_normalized(&pad2, true);
               right_paddle.y += (int)(analog_y * PADDLE_SPEED);
            }
         }
         else
         {
            // AI for right paddle if no controller
            int ball_center = ball.y + BALL_SIZE / 2;
            int paddle_center = right_paddle.y + PADDLE_HEIGHT / 2;
            if (ball_center < paddle_center - 10)
            {
               right_paddle.y -= PADDLE_SPEED - 1;
            }
            else if (ball_center > paddle_center + 10)
            {
               right_paddle.y += PADDLE_SPEED - 1;
            }
         }

         // Keep paddles in bounds
         if (left_paddle.y < 0)
            left_paddle.y = 0;
         if (left_paddle.y > SCREEN_YRES - PADDLE_HEIGHT)
            left_paddle.y = SCREEN_YRES - PADDLE_HEIGHT;
         if (right_paddle.y < 0)
            right_paddle.y = 0;
         if (right_paddle.y > SCREEN_YRES - PADDLE_HEIGHT)
            right_paddle.y = SCREEN_YRES - PADDLE_HEIGHT;

         // Update ball
         update_ball(&ball, &left_paddle, &right_paddle);

         // Check for scoring
         int scored = check_scoring(&ball, &left_paddle, &right_paddle);
         if (scored)
         {
            reset_ball(&ball);
            if (left_paddle.score >= 5 || right_paddle.score >= 5)
            {
               state = GAME_OVER;
            }
         }

         // Pause functionality
         if ((pad1.connected && pad1.face.triangle) || (pad2.connected && pad2.face.triangle))
         {
            state = GAME_PAUSED;
         }

         // Draw game elements
         draw_center_line(&ctx);
         draw_paddle(&ctx, PADDLE_MARGIN, left_paddle.y);
         draw_paddle(&ctx, SCREEN_XRES - PADDLE_WIDTH - PADDLE_MARGIN, right_paddle.y);
         draw_ball(&ctx, &ball);

         // Draw scores
         sprintf(text_buffer, "%d", left_paddle.score);
         draw_text(&ctx, SCREEN_XRES / 2 - 40, 20, 0, text_buffer);
         sprintf(text_buffer, "%d", right_paddle.score);
         draw_text(&ctx, SCREEN_XRES / 2 + 32, 20, 0, text_buffer);

         // Draw controller status
         draw_text(&ctx, 8, SCREEN_YRES - 16, 0, pad1.connected ? "P1: OK" : "P1: AI");
         draw_text(&ctx, SCREEN_XRES - 48, SCREEN_YRES - 16, 0, pad2.connected ? "P2: OK" : "P2: AI");
         break;

      case GAME_PAUSED:
         draw_text(&ctx, SCREEN_XRES / 2 - 24, SCREEN_YRES / 2, 0, "PAUSED");
         draw_text(&ctx, SCREEN_XRES / 2 - 64, SCREEN_YRES / 2 + 16, 0, "TRIANGLE TO RESUME");

         if ((pad1.connected && pad1.face.triangle) || (pad2.connected && pad2.face.triangle))
         {
            state = GAME_PLAYING;
         }
         break;

      case GAME_OVER:
         draw_text(&ctx, SCREEN_XRES / 2 - 32, SCREEN_YRES / 2 - 16, 0, "GAME OVER");
         if (left_paddle.score >= 5)
         {
            draw_text(&ctx, SCREEN_XRES / 2 - 48, SCREEN_YRES / 2, 0, "PLAYER 1 WINS!");
         }
         else
         {
            draw_text(&ctx, SCREEN_XRES / 2 - 48, SCREEN_YRES / 2, 0, "PLAYER 2 WINS!");
         }
         draw_text(&ctx, SCREEN_XRES / 2 - 72, SCREEN_YRES / 2 + 24, 0, "PRESS X TO PLAY AGAIN");

         if ((pad1.connected && pad1.face.x) || (pad2.connected && pad2.face.x))
         {
            state = GAME_MENU;
         }
         break;
      }

      flip_buffers(&ctx);
   }

   // Cleanup (though this won't be reached in this example)
   cleanup_game_pad(&pad1);
   cleanup_game_pad(&pad2);
   return 0;
}