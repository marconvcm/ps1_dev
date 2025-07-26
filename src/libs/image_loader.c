/**
 * @file image_loader.c
 * @brief PlayStation 1 image/texture loading and handling implementation
 *
 * This module provides implementation for loading and managing TIM format images
 * for use as textures in PlayStation 1 development.
 *
 * @author marconvcm
 * @date Current
 */
#include "image_loader.h"
#include <psxgpu.h>
#include <psxgte.h>
#include <string.h>

// Current active draw environment (used to restore after render-to-texture)
static DRAWENV active_draw_env;
static bool image_system_initialized = false;

/**
 * @brief Initialize the image loader system
 */
void image_init(void)
{
   if (!image_system_initialized)
   {
      // Store initial drawing environment for later restoration
      PutDrawEnv(&active_draw_env);
      image_system_initialized = true;
   }
}

/**
 * @brief Load a TIM image from a memory location
 *
 * @param tim_data Pointer to TIM data in memory
 * @param image Pointer to PSX_IMAGE structure to fill
 * @return true if the image was loaded successfully
 * @return false if there was an error
 */
bool image_load(const void *tim_data, PSX_IMAGE *image)
{
   TIM_IMAGE tim;

   // Clear the image structure
   memset(image, 0, sizeof(PSX_IMAGE));

   // Parse TIM header
   if (GetTimInfo((long *)tim_data, &tim) == 0)
   {
      return false; // Failed to parse TIM header
   }

   // Store the image mode
   image->mode = tim.mode;

   // Upload CLUT if present
   if (tim.mode & 0x8)
   {
      LoadImage(tim.crect, tim.caddr);
      image->clut = getClut(tim.crect->x, tim.crect->y);
      memcpy(&image->clut_area, tim.crect, sizeof(RECT));
   }

   // Upload texture
   LoadImage(tim.prect, tim.paddr);
   image->tpage = getTPage(tim.mode & 0x3, 0, tim.prect->x, tim.prect->y);
   memcpy(&image->texture_area, tim.prect, sizeof(RECT));

   // Calculate dimensions based on mode and VRAM allocation
   switch (tim.mode & 0x3)
   {
   case 0: // 4-bit
      image->width = tim.prect->w * 4;
      break;
   case 1: // 8-bit
      image->width = tim.prect->w * 2;
      break;
   case 2: // 16-bit
      image->width = tim.prect->w;
      break;
   default:
      image->width = tim.prect->w;
      break;
   }

   image->height = tim.prect->h;

   return true;
}

/**
 * @brief Create a render target in VRAM at the specified location
 *
 * @param x X position in VRAM
 * @param y Y position in VRAM
 * @param width Width of render target
 * @param height Height of render target
 * @param depth Color depth (0=4bit, 1=8bit, 2=16bit)
 * @param target Pointer to PSX_RENDER_TARGET structure to fill
 * @return true if the render target was created successfully
 * @return false if there was an error
 */
bool image_create_render_target(int x, int y, int width, int height, int depth, PSX_RENDER_TARGET *target)
{
   if (depth < 0 || depth > 2)
   {
      return false; // Invalid depth
   }

   // Initialize render target structure
   memset(target, 0, sizeof(PSX_RENDER_TARGET));

   // Set up the render target area
   setRECT(&target->area, x, y, width, height);
   target->width = width;
   target->height = height;

   // Create a texture page for this render target
   target->tpage = getTPage(depth, 0, x, y);

   return true;
}

/**
 * @brief Begin rendering to a render target
 *
 * @param target The render target to render to
 * @param ordering_table Pointer to the ordering table to use
 * @param clear_color Clear color to use (RGB triplet)
 * @return pointer to next primitive location for ordering
 */
char *image_render_target_begin(PSX_RENDER_TARGET *target, long *ordering_table, CVECTOR *clear_color)
{
   DRAWENV draw;
   FILL *pfill;
   DR_AREA *parea;
   DR_OFFSET *poffs;
   char *next_pri;

   // Save current drawing environment for later restoration
   PutDrawEnv(&active_draw_env);

   // Set up fill primitive to clear the render target
   pfill = (FILL *)ordering_table;
   setFill(pfill);
   setXY0(pfill, target->area.x, target->area.y);
   setWH(pfill, target->width, target->height);
   setRGB0(pfill, clear_color->r, clear_color->g, clear_color->b);
   addPrim(ordering_table, pfill);
   next_pri = (char *)(pfill + 1);

   // Set drawing area to render target
   parea = (DR_AREA *)next_pri;
   setDrawArea(parea, &target->area);
   addPrim(ordering_table, parea);
   next_pri = (char *)(parea + 1);

   // Set drawing offset to render target
   poffs = (DR_OFFSET *)next_pri;
   setDrawOffset(poffs, target->area.x, target->area.y);
   addPrim(ordering_table, poffs);
   next_pri = (char *)(poffs + 1);

   // Return pointer to next primitive
   return next_pri;
}

/**
 * @brief End rendering to a render target and return to the previous drawing environment
 *
 * @param target The render target that was being rendered to
 * @param ordering_table Pointer to the ordering table to use
 * @param current_pri Current primitive pointer
 * @return pointer to next primitive location for ordering
 */
char *image_render_target_end(PSX_RENDER_TARGET *target, long *ordering_table, char *current_pri)
{
   DR_AREA *parea;
   DR_OFFSET *poffs;

   // Restore original drawing area
   parea = (DR_AREA *)current_pri;
   setDrawArea(parea, &active_draw_env.clip);
   addPrim(ordering_table, parea);
   current_pri = (char *)(parea + 1);

   // Restore original drawing offset
   poffs = (DR_OFFSET *)current_pri;
   setDrawOffset(poffs, active_draw_env.clip.x, active_draw_env.clip.y);
   addPrim(ordering_table, poffs);
   current_pri = (char *)(poffs + 1);

   // Return pointer to next primitive
   return current_pri;
}

/**
 * @brief Free resources associated with an image
 *
 * @param image The image to free
 */
void image_free(PSX_IMAGE *image)
{
   // Currently a no-op as PlayStation doesn't have dynamic memory management for VRAM
   // This is here for API completeness and potential future expansion
   (void)image;
}
