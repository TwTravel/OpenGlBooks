#pragma once

#define OPTION_DEBUG			(1<<2)
#define OPTION_DISPLAY_HELP		(1<<3)

extern unsigned long GetCurrentOptions();
extern unsigned long GetCurrentSolverIndex();
extern void RegisterUICallbacks();
extern float* GetCameraPos();
extern void UpdateCameraTransform();

extern void shapesPrintf (int row, int col, const char *fmt, ...);
