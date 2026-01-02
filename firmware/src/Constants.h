/**
 * Constants.h
 *
 * Defines the physical dimensions and LED configuration for the 16x16x16 LED cube.
 * These constants are used throughout the firmware for coordinate mapping and LED addressing.
 */

#pragma once

// The cube is 16 LEDs on each edge (16x16x16 = 4096 total LEDs)
constexpr int CUBE_SIZE = 16;

// Each OctoWS2811 strip drives one Z-slice of the cube (16x16 = 256 LEDs per strip)
constexpr int LEDS_PER_STRIP = CUBE_SIZE * CUBE_SIZE;  // 256

// 16 strips total, one for each Z-layer from front to back
constexpr int STRIP_COUNT = CUBE_SIZE;                  // 16

// Total LED count in the entire cube
constexpr int LED_COUNT = LEDS_PER_STRIP * STRIP_COUNT; // 4096