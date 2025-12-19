#include "../pomidoras.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void HandleOnBackButtonClick(Clay_ElementId elementId,
                             Clay_PointerData pointerData, intptr_t userData) {
  if (pointerData.state == CLAY_POINTER_DATA_RELEASED_THIS_FRAME) {
    app_state.current_screen = POMODORO_TIMER;
  }
}

void HandleOnInputSwitchClick(Clay_ElementId elementId,
                              Clay_PointerData pointerData, intptr_t userData) {
  if (pointerData.state == CLAY_POINTER_DATA_RELEASED_THIS_FRAME) {
    Input *input = (Input *)userData;
    if (input->chars[0] == '1')
      input->chars[0] = '0';
    else
      input->chars[0] = '1';
  }
}

void HandleOnInputClick(Clay_ElementId elementId, Clay_PointerData pointerData,
                        intptr_t userData) {
  if (pointerData.state == CLAY_POINTER_DATA_RELEASED_THIS_FRAME) {
    Input *input = (Input *)userData;
    app_state.focused_input = input;
  }
}

void InputSwitch(const char *label, Input *input) {
  const Theme *theme = get_current_theme();
  Clay_String label_string = {
      .length = (int32_t)strlen(label),
      .chars = label,
  };
  CLAY({
      .layout =
          {
              .sizing =
                  {
                      .width = CLAY_SIZING_GROW(0),
                      .height = {38},
                  },
              .padding = {8, 8, 8, 8},
              .childGap = 16,
              .childAlignment =
                  {
                      .y = CLAY_ALIGN_Y_CENTER,
                  },
          },
  }) {
    Clay_OnHover(HandleOnInputSwitchClick, (intptr_t)input);
    CLAY({
        .layout =
            {
                .sizing =
                    {
                        .width = CLAY_SIZING_GROW(0),
                    },
            },
    }) {
      CLAY_TEXT(label_string, CLAY_TEXT_CONFIG({
                                  .textColor = theme->text_primary,
                                  .fontSize = 20,
                                  .letterSpacing = 2,
                              }));
    };
    const int padding = 4;
    const int inner_radius = 10;
    const int outer_radius = inner_radius + padding;
    const int width = 60;
    bool selected = input->chars[0] == '1';
    CLAY({
        .layout =
            {
                .sizing =
                    {
                        .width = {width},
                        .height = {28},
                    },
                .padding = CLAY_PADDING_ALL(padding),
            },
        .backgroundColor = theme->card,
        .cornerRadius = CLAY_CORNER_RADIUS(outer_radius),
        .border =
            {
                .color = theme->border,
                .width = CLAY_BORDER_ALL(2),
            },
    }) {
      CLAY({
          .layout =
              {
                  .sizing =
                      {
                          .width = CLAY_SIZING_PERCENT(0.5),
                          .height = {28 - (padding * 2)},
                      },
              },
          .backgroundColor = selected ? theme->primary : theme->border,
          .cornerRadius = CLAY_CORNER_RADIUS(inner_radius),
          .floating =
              {
                  .offset =
                      {
                          .x = selected ? (width * 0.5f - padding) : padding,
                          .y = padding,
                      },
                  .attachPoints = {.parent = CLAY_ATTACH_POINT_LEFT_TOP},
                  .attachTo = CLAY_ATTACH_TO_PARENT,
              },
      });
    };
  };
}

void TextInput(const char *label, Input *input) {
  const Theme *theme = get_current_theme();
  Clay_String value = {
      .length = input->length,
      .chars = input->chars,
  };
  Clay_String label_string = {
      .length = (int32_t)strlen(label),
      .chars = label,
  };
  CLAY({
      .layout =
          {
              .sizing =
                  {
                      .width = CLAY_SIZING_GROW(0),
                      .height = {38},
                  },
              .padding = {8, 8, 8, 8},
              .childGap = 16,
              .childAlignment =
                  {
                      .y = CLAY_ALIGN_Y_CENTER,
                  },
          },
  }) {
    Clay_OnHover(HandleOnInputClick, (intptr_t)input);
    CLAY({
        .layout =
            {
                .sizing =
                    {
                        .width = CLAY_SIZING_GROW(0),
                    },
            },
    }) {
      CLAY_TEXT(label_string, CLAY_TEXT_CONFIG({
                                  .textColor = theme->text_primary,
                                  .fontSize = 20,
                                  .letterSpacing = 2,
                              }));
    }
    CLAY({
        .layout =
            {
                .sizing =
                    {
                        .width = CLAY_SIZING_PERCENT(0.4),
                        .height =
                            {
                                .size = {38, 64},
                                .type = CLAY__SIZING_TYPE_FIXED,
                            },
                    },
                .padding = {16, 16, 10, 10},
            },
        .cornerRadius = CLAY_CORNER_RADIUS(12),
        .border =
            {
                .color = app_state.focused_input == input ? theme->primary
                                                          : theme->border,
                .width = CLAY_BORDER_ALL(2),
            },
    }) {
      CLAY_TEXT(value, CLAY_TEXT_CONFIG({
                           .textColor = theme->text_primary,
                           .fontSize = 20,
                           .letterSpacing = 2,
                       }));
    };
  };
}

static Input focus_timer = {
    .chars = "00",
    .length = 2,
    .max_length = 4,
};

static Input short_timer = {
    .chars = "5",
    .length = 1,
    .max_length = 4,
};

static Input long_timer = {
    .chars = "15",
    .length = 2,
    .max_length = 4,
};

static Input long_break_interval = {
    .chars = "4",
    .length = 1,
    .max_length = 4,
};

static Input auto_start_break = {
    .chars = "1",
    .length = 1,
};

void HandleSaveSettingsClick(Clay_ElementId elementId,
                             Clay_PointerData pointerData, intptr_t userData) {
  if (pointerData.state == CLAY_POINTER_DATA_RELEASED_THIS_FRAME) {
    app_config.focus_timer = (atoi(focus_timer.chars) * 60.0);
    app_config.short_break_timer = (atoi(short_timer.chars) * 60.0);
    app_config.long_break_timer = (atoi(long_timer.chars) * 60.0);
    app_config.long_break_interval = atoi(long_break_interval.chars);
    app_config.auto_start = auto_start_break.chars[0] == '1';
    app_state.current_timer = POMODORO;
    app_state.current_time = app_config.focus_timer;
    app_state.current_screen = POMODORO_TIMER;
    save_config();
  }
}

void renderSettingsHeader() {
  const Theme *theme = get_current_theme();
  CLAY({
      .layout =
          {
              .sizing =
                  {
                      .width = CLAY_SIZING_GROW(0),
                  },
              .childGap = 16,
          },
  }) {
    // TODO: Trying to add a tint to the texture to use the same texture in
    // both dark and light mode
    Clay_Color backgroundColor = {.r = 255, .g = 0, .b = 0, .a = 255};
    CLAY({
        .layout =
            {
                .sizing =
                    {
                        .width = {24},
                        .height = {24},
                    },
                .childAlignment = {.y = CLAY_ALIGN_Y_CENTER},
            },
        .image =
            {
                .imageData = app_config.theme == THEME_LIGHT ? &backTexture
                                                             : &backTextureDark,
            },
    }) {
      Clay_OnHover(HandleOnBackButtonClick, 0);
    };

    CLAY({
        .layout =
            {
                .sizing =
                    {
                        .width = CLAY_SIZING_GROW(0),
                    },
            },
    }) {
      CLAY_TEXT(CLAY_STRING("Settings"), CLAY_TEXT_CONFIG({
                                             .textColor = theme->text_primary,
                                             .fontSize = 24,
                                             .letterSpacing = 2,
                                         }));
    }
    CLAY({
        .layout =
            {
                .sizing =
                    {
                        .width = {24},
                        .height = {24},
                    },
            },
        .image =
            {
                .imageData = app_config.theme == THEME_LIGHT ? &saveTexture
                                                             : &saveTextureDark,
            },
    }) {
      Clay_OnHover(HandleSaveSettingsClick, 0);
    };
  };
}

void renderSettings() {
  const Theme *theme = get_current_theme();
  CLAY({
      .layout =
          {
              .sizing =
                  {
                      .width = CLAY_SIZING_GROW(0),
                      .height = CLAY_SIZING_GROW(0),
                  },
              .padding = {16, 16, 24, 24},
              .childGap = 4,
              .layoutDirection = CLAY_TOP_TO_BOTTOM,
          },
      .backgroundColor = theme->background,
  }) {
    renderSettingsHeader();
    CLAY({
        .layout =
            {
                .sizing =
                    {
                        .width = CLAY_SIZING_GROW(0),
                    },
                .padding = {32, 0, 0, 0},
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
            },
        .clip =
            {
                .vertical = true,
                .childOffset = Clay_GetScrollOffset(),
            },
    }) {
      TextInput("Focus Time", &focus_timer);
      TextInput("Short Break Time", &short_timer);
      TextInput("Long Break Time", &long_timer);
      TextInput("Long Break Interval", &long_break_interval);
      InputSwitch("Auto Start Timers", &auto_start_break);
    }
  };
}
