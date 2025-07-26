/**
 * @file image_loader.h
 * @brief PlayStation 1 image/texture loading and handling library
 *
 * This module provides functions for loading and managing TIM format images
 * for use as textures in PlayStation 1 development.
 *
 * Features:
 * - Loading TIM files into VRAM
 * - Texture page and CLUT management
 * - Support for different color depths (4-bit, 8-bit, 16-bit)
 * - Render-to-texture capability
 * - Multiple texture handling
 *
 * @author marconvcm
 * @date Current
 */
#ifndef IMAGE_LOADER_H
#define IMAGE_LOADER_H

#include <stdint.h>
#include <stdbool.h>
#include <psxgpu.h>
#include <psxgte.h>

/**
 * @brief Structure representing a TIM image loaded into VRAM
 */
typedef struct
{
   uint16_t tpage;    /* Texture page ID for this image */
   uint16_t clut;     /* CLUT ID (if applicable) */
   RECT texture_area; /* VRAM area where the texture is stored */
   RECT clut_area;    /* VRAM area where the CLUT is stored (if applicable) */
   uint8_t mode;      /* Texture mode (bit depth, etc.) */
   uint16_t width;    /* Width in pixels */
   uint16_t height;   /* Height in pixels */
} PSX_IMAGE;

/**
 * @brief Structure representing a render-to-texture target
 */
typedef struct
{
   uint16_t tpage;  /* Texture page for the rendered output */
   RECT area;       /* VRAM area to render into */
   uint16_t width;  /* Width of render target */
   uint16_t height; /* Height of render target */
} PSX_RENDER_TARGET;

/**
 * @brief Initialize the image loader system
 */
void image_init(void);

/**
 * @brief Load a TIM image from a memory location
 *
 * @param tim_data Pointer to TIM data in memory
 * @param image Pointer to PSX_IMAGE structure to fill
 * @return true if the image was loaded successfully
 * @return false if there was an error
 */
bool image_load(const void *tim_data, PSX_IMAGE *image);

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
bool image_create_render_target(int x, int y, int width, int height, int depth, PSX_RENDER_TARGET *target);

/**
 * @brief Begin rendering to a render target
 *
 * @param target The render target to render to
 * @param ordering_table Pointer to the ordering table to use
 * @param clear_color Clear color to use (RGB triplet)
 * @return pointer to next primitive location for ordering
 */
char *image_render_target_begin(PSX_RENDER_TARGET *target, long *ordering_table, CVECTOR *clear_color);

/**
 * @brief End rendering to a render target and return to the previous drawing environment
 *
 * @param target The render target that was being rendered to
 * @param ordering_table Pointer to the ordering table to use
 * @param current_pri Current primitive pointer
 * @return pointer to next primitive location for ordering
 */
char *image_render_target_end(PSX_RENDER_TARGET *target, long *ordering_table, char *current_pri);

/**
 * @brief Free resources associated with an image
 *
 * @param image The image to free
 */
void image_free(PSX_IMAGE *image);

#endif /* IMAGE_LOADER_H */
