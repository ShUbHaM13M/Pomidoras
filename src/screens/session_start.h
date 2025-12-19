#include "../pomidoras.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

void HandleOnDecrementClick(Clay_ElementId elementId,
                            Clay_PointerData pointerData, intptr_t userData) {
  if (pointerData.state == CLAY_POINTER_DATA_RELEASED_THIS_FRAME &&
      app_config.number_of_pomos > 1)
    app_config.number_of_pomos -= 1;
}

void HandleStartSessionClick(Clay_ElementId elementId,
                             Clay_PointerData pointerData, intptr_t userData) {
  if (pointerData.state == CLAY_POINTER_DATA_RELEASED_THIS_FRAME)
    app_state.current_screen = POMODORO_TIMER;
}

void HandleOnIncrementClick(Clay_ElementId elementId,
                            Clay_PointerData pointerData, intptr_t userData) {
  if (pointerData.state == CLAY_POINTER_DATA_RELEASED_THIS_FRAME)
    app_config.number_of_pomos += 1;
}

static inline void IconButton(
    const char *label,
    void (*onButtonClick)(Clay_ElementId elementId,
                          Clay_PointerData pointerInfo, intptr_t userData)) {

  const Theme *theme = get_current_theme();
  Clay_String _label = {
      .length = (int32_t)(strlen(label)),
      .chars = label,
  };
  CLAY({
      .layout =
          {
              .sizing =
                  {
                      .height = 48,
                  },
              .padding = {20, 20, 10, 10},
              .childAlignment =
                  {
                      .x = CLAY_ALIGN_X_CENTER,
                      .y = CLAY_ALIGN_Y_CENTER,
                  },
          },
      .cornerRadius = {24, 24, 24, 24},
      .border =
          {
              .color = theme->border,
              .width = {2, 2, 2, 2},
          },
  }) {
    Clay_OnHover(onButtonClick, 0);
    CLAY_TEXT(_label, CLAY_TEXT_CONFIG({
                          .textColor = theme->text_primary,
                          .fontSize = 24,
                      }));
  };
}

void renderNumberOfPomos() {
  const Theme *theme = get_current_theme();
  static char chars[2];
  sprintf(chars, "%d", app_config.number_of_pomos);
  Clay_String number_of_pomos = {
      .length = (int32_t)strlen(chars),
      .chars = chars,
  };
  CLAY({
           .layout =
               {
                   .padding = {42, 42, 16, 16},
               },
           .backgroundColor = theme->background,
           .cornerRadius = {38, 38, 38, 38},
           .border =
               {
                   .color = theme->border,
                   .width = {2, 2, 2, 2},
               },
       }, ) {
    CLAY_TEXT(number_of_pomos, CLAY_TEXT_CONFIG({
                                   .textColor = theme->text_primary,
                                   .fontSize = 48,
                               }));
  };
}

void StartSessionButtion(void (*HandleStartSessionClick)(
    Clay_ElementId elementId, Clay_PointerData pointerInfo,
    intptr_t userData)) {
  const Theme *theme = get_current_theme();
  CLAY({
      .layout =
          {
              .sizing =
                  {
                      .height = 48,
                  },
              .padding = {16, 16, 8, 8},
              .childAlignment =
                  {
                      .x = CLAY_ALIGN_X_CENTER,
                      .y = CLAY_ALIGN_Y_CENTER,
                  },
          },
      .backgroundColor = theme->primary,
      .cornerRadius = CLAY_CORNER_RADIUS(4),
  }) {
    const Theme *theme = get_current_theme();
    Clay_OnHover(HandleStartSessionClick, 0);
    CLAY_TEXT(CLAY_STRING("Start Session"),
              CLAY_TEXT_CONFIG({
                                   .textColor = theme->text_primary,
                                   .fontSize = 22,
                                   .letterSpacing = 2,
                                   .textAlignment = CLAY_TEXT_ALIGN_CENTER,
                               }, ));
  };
}

void renderPomoSelector() {
  const Theme *theme = get_current_theme();
  CLAY({
      .layout =
          {
              .sizing =
                  {
                      .width = CLAY_SIZING_GROW(0),
                      .height = CLAY_SIZING_GROW(0),
                  },
              .padding = CLAY_PADDING_ALL(16),
              .childGap = 8,
              .childAlignment =
                  {
                      .x = CLAY_ALIGN_X_CENTER,
                      .y = CLAY_ALIGN_Y_CENTER,
                  },
              .layoutDirection = CLAY_TOP_TO_BOTTOM,
          },
      .backgroundColor = theme->background,
  }) {
    CLAY_TEXT(CLAY_STRING("How many Pomodoros?"),
              CLAY_TEXT_CONFIG({
                  .textColor = theme->text_primary,
                  .fontSize = 32,
                  .letterSpacing = 2,
                  .textAlignment = CLAY_TEXT_ALIGN_CENTER,
              }));
    CLAY_TEXT(CLAY_STRING("Choose the number of sessions for your work."),
              CLAY_TEXT_CONFIG({
                  .textColor = theme->text_secondary,
                  .fontSize = 24,
                  .letterSpacing = 2,
                  .textAlignment = CLAY_TEXT_ALIGN_CENTER,
              }));
    CLAY({.layout = {
              .sizing = {.width = CLAY_SIZING_GROW(0)},
              .padding = CLAY_PADDING_ALL(16),
              .childGap = 10,
              .childAlignment =
                  {
                      .x = CLAY_ALIGN_X_CENTER,
                      .y = CLAY_ALIGN_Y_CENTER,
                  },
          }}) {

      // TODO: Update these with icon instead of font
      IconButton("-", HandleOnDecrementClick);
      renderNumberOfPomos();
      IconButton("+", HandleOnIncrementClick);
    };
    StartSessionButtion(HandleStartSessionClick);
  };
}
