// SPDX-License-Identifier: MIT
// Copyright 2023 NXP

#ifndef __CUSTOM_H_
#define __CUSTOM_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "gui_guider.h"

int blueCounter_analog_clock_1_hour_value;
int blueCounter_analog_clock_1_min_value;
int blueCounter_analog_clock_1_sec_value;

void custom_init(lv_ui *ui);

#ifdef __cplusplus
}
#endif
#endif /* EVENT_CB_H_ */
