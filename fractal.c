#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <SDL2/SDL.h>

// Constants
const int MAX_ITERATIONS = 1000;

// Struct to represent a complex number
typedef struct {
    double real;
    double imaginary;
} Complex;

// Function to calculate the squared magnitude of a complex number
double magnitudeSquared(Complex c) {
    return c.real * c.real + c.imaginary * c.imaginary;
}

// Function to calculate the Mandelbrot set value for a given complex number
int mandelbrot(Complex c) {
    Complex z = {0, 0};
    int iterations = 0;

    while (iterations < MAX_ITERATIONS && magnitudeSquared(z) <= 4) {
        // Calculate z = z^2 + c
        Complex temp = {
            z.real * z.real - z.imaginary * z.imaginary + c.real,
            2 * z.real * z.imaginary + c.imaginary
        };
        z = temp;
        iterations++;
    }

    return iterations;
}

// Function to map a value from one range to another
double map(double value, double in_min, double in_max, double out_min, double out_max) {
    return (value - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

// Function to set the pixel color based on the Mandelbrot set value
void setColor(SDL_Surface* surface, int x, int y, int value) {
    Uint32* pixels = (Uint32*)surface->pixels;
    Uint32 color = SDL_MapRGB(surface->format, value, value, value);
    pixels[y * surface->w + x] = color;
}

// Function to render the Mandelbrot set image
void renderMandelbrot(SDL_Surface* surface, double zoom, double offsetX, double offsetY) {
    for (int x = 0; x < surface->w; x++) {
        for (int y = 0; y < surface->h; y++) {
            // Map pixel coordinates to the corresponding complex number
            double a = map(x, 0, surface->w, -2.5 / zoom + offsetX, 1 / zoom + offsetX);
            double b = map(y, 0, surface->h, -1 / zoom + offsetY, 1 / zoom + offsetY);
            Complex c = {a, b};

            // Calculate the Mandelbrot set value for the complex number
            int value = mandelbrot(c);

            // Set the pixel color based on the Mandelbrot set value
            setColor(surface, x, y, value);
        }
    }
}

// Function to save the surface as a BMP image
void saveImage(SDL_Surface* surface, const char* filename) {
    SDL_SaveBMP(surface, filename);
    printf("Image saved to %s\n", filename);
}

int main(int argc, char* argv[]) {
    // Check command line arguments
    if (argc != 3) {
        printf("Usage: %s [width] [height]\n", argv[0]);
        return 1;
    }

    // Parse width and height from command line arguments
    int width = atoi(argv[1]);
    int height = atoi(argv[2]);

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL initialization failed: %s\n", SDL_GetError());
        return 1;
    }

    // Create a window
    SDL_Window* window = SDL_CreateWindow("Fractal", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Window creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create a renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Renderer creation failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Create a texture
    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STATIC, width, height);
    if (!texture) {
        printf("Texture creation failed: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Create a surface to store the pixel data
    SDL_Surface* surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
    if (!surface) {
        printf("Surface creation failed: %s\n", SDL_GetError());
        SDL_DestroyTexture(texture);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Set initial zoom and offsets
    double zoom = 1.0;
    double offsetX = -0.5;
    double offsetY = 0.0;

    // Render the initial Mandelbrot set image
    renderMandelbrot(surface, zoom, offsetX, offsetY);

    // Main loop flag
    bool quit = false;

    // Event handler
    SDL_Event e;

    // Main loop
    while (!quit) {
        // Handle events on the queue
        while (SDL_PollEvent(&e) != 0) {
            // Quit event
            if (e.type == SDL_QUIT) {
                quit = true;
            }
            // Keydown event
            else if (e.type == SDL_KEYDOWN) {
                switch (e.key.keysym.sym) {
                    case SDLK_PLUS:
                    case SDLK_KP_PLUS:
                        zoom *= 1.1;
                        break;
                    case SDLK_MINUS:
                    case SDLK_KP_MINUS:
                        zoom /= 1.1;
                        break;
                    case SDLK_r:
                        zoom = 1.0;
                        offsetX = -0.5;
                        offsetY = 0.0;
                        break;
                    case SDLK_s:
                        saveImage(surface, "fractal.bmp");
                        break;
                    case SDLK_q:
                        quit = true;
                        break;
                }
                renderMandelbrot(surface, zoom, offsetX, offsetY);
            }
            // Mouse button down event
            else if (e.type == SDL_MOUSEBUTTONDOWN) {
                int mouseX, mouseY;
                SDL_GetMouseState(&mouseX, &mouseY);
                double clickX = map(mouseX, 0, width, -2.5 / zoom + offsetX, 1 / zoom + offsetX);
                double clickY = map(mouseY, 0, height, -1 / zoom + offsetY, 1 / zoom + offsetY);
                switch (e.button.button) {
                    case SDL_BUTTON_LEFT:
                        zoom *= 1.1;
                        offsetX = clickX - (clickX - offsetX) * 1.1;
                        offsetY = clickY - (clickY - offsetY) * 1.1;
                        break;
                    case SDL_BUTTON_RIGHT:
                        zoom /= 1.1;
                        offsetX = clickX - (clickX - offsetX) / 1.1;
                        offsetY = clickY - (clickY - offsetY) / 1.1;
                        break;
                    case SDL_BUTTON_MIDDLE:
                        zoom = 1.0;
                        offsetX = -0.5;
                        offsetY = 0.0;
                        break;
                }
                renderMandelbrot(surface, zoom, offsetX, offsetY);
            }
            // Keyup event
            else if (e.type == SDL_KEYUP) {
                if (e.key.keysym.sym == SDLK_c) {
                    // Cycle through color schemes
                    // Implement the color scheme cycling logic here
                }
            }
        }

        // Update the texture with the new pixel data
        SDL_UpdateTexture(texture, NULL, surface->pixels, surface->pitch);

        // Clear the renderer
        SDL_RenderClear(renderer);

        // Copy the texture to the renderer
        SDL_RenderCopy(renderer, texture, NULL, NULL);

        // Render the scene
        SDL_RenderPresent(renderer);
    }

    // Clean up resources
    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
