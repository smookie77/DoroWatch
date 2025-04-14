/*
 * Copyright 2023 NXP
 * SPDX-License-Identifier: MIT
 * The auto-generated can only be used on NXP devices
 */

#ifndef GUI_GUIDER_H
#define GUI_GUIDER_H
#ifdef __cplusplus
extern "C" {
#endif

#include "lvgl.h"
#include "guider_fonts.h"


typedef struct
{
	lv_obj_t *blueCounter;
	bool blueCounter_del;
	lv_obj_t *blueCounter_analog_clock_1;
}lv_ui;

void ui_init_style(lv_style_t * style);
void init_scr_del_flag(lv_ui *ui);
void setup_ui(lv_ui *ui);
extern lv_ui guider_ui;
void setup_scr_blueCounter(lv_ui *ui);
void clock_count(int *hour, int *min, int *sec);

#ifdef __cplusplus
}
#endif
#endif