#include "../pomidoras.h"
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

const int PADDING_X = 20;
const int PADDING_Y = 24;

static Clay_String start_pause_button_text = {
    .length = 5,
    .chars = "Start",
};

void HandleOnThemeChangeClick(Clay_ElementId element_id,
                              Clay_PointerData pointerData,
                              intptr_t user_data) {

  if (pointerData.state == CLAY_POINTER_DATA_RELEASED_THIS_FRAME) {
    if (app_config.theme == THEME_LIGHT)
      app_config.theme = THEME_DARK;
    else
      app_config.theme = THEME_LIGHT;
  }
}

void HandleOnSettingIconClick(Clay_ElementId element_id,
                              Clay_PointerData pointerData,
                              intptr_t user_data) {

  if (pointerData.state == CLAY_POINTER_DATA_RELEASED_THIS_FRAME)
    app_state.current_screen = SETTINGS;
}

void HandleTabBarItemClick(Clay_ElementId element_id,
                           Clay_PointerData pointerData, intptr_t user_data) {
  TabBarItemData *tabbar_item_data = (TabBarItemData *)user_data;
  assert(tabbar_item_data->index <= num_tabs);
  if (pointerData.state == CLAY_POINTER_DATA_RELEASED_THIS_FRAME) {
    app_state.previous_selected_tab_index = app_state.selected_tab_index;
    app_state.selected_tab_index = tabbar_item_data->index;
    app_state.timer_started = false;
    switch (app_state.selected_tab_index) {
    case 0:
      app_state.current_time = app_config.focus_timer;
      break;
    case 1:
      app_state.current_time = app_config.short_break_timer;
      break;
    case 2:
      app_state.current_time = app_config.long_break_timer;
      break;
    }
  }
}

void HandleOnStartPauseClick(Clay_ElementId elementId,
                             Clay_PointerData pointerData, intptr_t userData) {
  if (pointerData.state == CLAY_POINTER_DATA_RELEASED_THIS_FRAME) {
    app_state.timer_started = !app_state.timer_started;
  }
}

void HandleSkipButtonClick(Clay_ElementId elementId,
                           Clay_PointerData pointerData, intptr_t userData) {
  if (pointerData.state == CLAY_POINTER_DATA_RELEASED_THIS_FRAME)
    app_state.current_time = 0;
}

void TabBarItem(TabBarItemData *item) {
  const Theme *theme = get_current_theme();
  const int default_font_size = 16;
  const int padding = 6;
  Clay_Color background_color = {0};

  CLAY({
      .id = CLAY_SID(item->label),
      .layout =
          {
              .padding = CLAY_PADDING_ALL(padding),
          },
      .backgroundColor = item->index == app_state.selected_tab_index
                             ? COLOR_PRIMARY_LIGHT
                         : Clay_Hovered() ? COLOR_PRIMARY_HOVER
                                          : COLOR_TRANSPARENT,

      // .backgroundColor = theme->primary,
      .cornerRadius = CLAY_CORNER_RADIUS(4),
  }) {
    Clay_OnHover(HandleTabBarItemClick, (intptr_t)item);
    CLAY_TEXT(item->label,
              CLAY_TEXT_CONFIG({
                                   .textColor = theme->text_primary,
                                   .fontSize = default_font_size,
                                   .letterSpacing = 2,
                               }, ));
  };
}

void TabBarComponent() {
  CLAY({
      .id = CLAY_ID("TabBar"),
      .layout =
          {
              .sizing =
                  {
                      .width = CLAY_SIZING_GROW(0),
                  },
              .childAlignment = {.x = CLAY_ALIGN_X_CENTER},
              .childGap = 16,
          },
  }) {
    for (uint32_t i = 0; i < num_tabs; ++i) {
      TabBarItem(&tab_bar_items[i]);
    }
  };
}

void renderPomoTimer() {
  const Theme *theme = get_current_theme();
  static char time[32];
  uint32_t minutes = (uint32_t)app_state.current_time / 60;
  uint32_t seconds = (uint32_t)app_state.current_time % 60;
  snprintf(time, sizeof(time), "%02d:%02d", minutes, seconds);
  Clay_String timeText = {
      .length = (int32_t)strlen(time),
      .chars = time,
  };

  assert(timeText.chars != NULL && timeText.length > 0);

  const char *s = app_state.timer_started == true ? "Pause" : "Start";
  start_pause_button_text.chars = s;
  start_pause_button_text.length = 5;

  CLAY({
      .layout =
          {
              .sizing =
                  {
                      .width = CLAY_SIZING_GROW(0),
                      .height = CLAY_SIZING_GROW(0),
                  },
              .padding = {PADDING_X, PADDING_X, PADDING_Y, PADDING_Y},
              .childGap = 16,
              .layoutDirection = CLAY_TOP_TO_BOTTOM,
          },
      .backgroundColor = app_state.current_color,
  }) {
    CLAY({
        .layout =
            {
                .sizing =
                    {
                        .width = CLAY_SIZING_GROW(0),
                    },
                .padding = {PADDING_X, PADDING_X, PADDING_Y, PADDING_Y},
                .childGap = 16,
                .layoutDirection = CLAY_TOP_TO_BOTTOM,
            },
        .backgroundColor = (Clay_Color){0, 0, 0, 60},
        .cornerRadius = 6,
    }) {

      CLAY({
          .layout =
              {
                  .sizing =
                      {
                          .width = CLAY_SIZING_GROW(0),
                      },
                  .childAlignment =
                      {
                          .x = CLAY_ALIGN_X_CENTER,
                          .y = CLAY_ALIGN_Y_CENTER,
                      },
              },
      }) {
        CLAY({
            .layout =
                {
                    .sizing =
                        {
                            .width = 22,
                            .height = 22,
                        },
                },
            .image =
                {
                    .imageData = app_config.theme == THEME_LIGHT ? &sunTexture
                                                                 : &moonTexture,
                },
        }) {
          Clay_OnHover(HandleOnThemeChangeClick, 0);
        };

        CLAY({
            .id = CLAY_ID("numberOfSessionContainer"),
            .layout =
                {
                    .sizing =
                        {
                            .width = CLAY_SIZING_GROW(0),
                        },
                    .childGap = 16,
                    .childAlignment =
                        {
                            .x = CLAY_ALIGN_X_CENTER,
                            .y = CLAY_ALIGN_Y_CENTER,
                        },
                },
        }) {
          for (int i = 0; i < app_config.number_of_pomos; i++) {
            Clay_Color backgroundColor = {.r = 0, .g = 0, .b = 0, .a = 100};
            if (i - 1 < app_state.pomos_completed - 1)
              backgroundColor = COLOR_TRANSPARENT;

            CLAY({
                .layout =
                    {
                        .sizing =
                            {
                                .width = 24,
                                .height = 24,
                            },
                    },
                .backgroundColor = backgroundColor,
                .image =
                    {
                        .imageData = &rocketTexture,
                    },
                // .floating =
                //     {
                //         .attachTo = app_state.pomos_completed == i
                //                         ? CLAY_ATTACH_TO_PARENT
                //                         : CLAY_ATTACH_TO_NONE,
                //         .offset = {.x = 0, .y = 0},
                //     },
            });
          }
        };

        CLAY({
            .layout =
                {
                    .sizing =
                        {
                            .width = 22,
                            .height = 22,
                        },
                },
            .image =
                {
                    .imageData = &settingTexture,
                },
        }) {
          Clay_OnHover(HandleOnSettingIconClick, 0);
        };
      };

      CLAY({
          .layout =
              {
                  .sizing =
                      {
                          .width = CLAY_SIZING_GROW(0),
                          .height = CLAY_SIZING_GROW(0),
                      },
                  .childGap = 8,
                  .childAlignment = {.x = CLAY_ALIGN_X_CENTER},
                  .layoutDirection = CLAY_TOP_TO_BOTTOM,
              },
      }) {
        CLAY_TEXT(timeText, CLAY_TEXT_CONFIG({
                                .textColor = theme->text_primary,
                                .fontSize = 112,
                                .letterSpacing = 6,
                            }));

        CLAY({
            .layout =
                {
                    .sizing =
                        {
                            .width = CLAY_SIZING_GROW(0),
                        },
                    .childGap = 8,
                    .childAlignment =
                        {
                            .x = CLAY_ALIGN_X_CENTER,
                            .y = CLAY_ALIGN_Y_CENTER,
                        },
                    .layoutDirection = CLAY_LEFT_TO_RIGHT,
                },
        }) {
          CLAY({
              .id = CLAY_ID("startButton"),
              .layout =
                  {
                      .padding = {16, 16, 8, 8},
                  },
              .backgroundColor = theme->primary,
              .cornerRadius = 4,
          }) {
            Clay_OnHover(HandleOnStartPauseClick, 0);
            CLAY_TEXT(start_pause_button_text,
                      CLAY_TEXT_CONFIG({
                                           .textColor = theme->text_primary,
                                           .fontSize = 24,
                                           .letterSpacing = 2,
                                       }, ));
          };

          if (app_state.timer_started) {
            CLAY({
                .id = CLAY_ID("skipButton"),
                .layout =
                    {
                        .sizing =
                            {
                                .width = CLAY_SIZING_FIXED(28),
                                .height = CLAY_SIZING_FIXED(28),
                            },
                    },
                .image =
                    {
                        .imageData = &skipTexture,
                    },
                .floating =
                    {
                        .offset = {.x = 16, .y = -(28.0 / 2)},
                        .parentId = CLAY_ID("startButton").id,
                        .attachPoints =
                            {
                                .parent = CLAY_ATTACH_POINT_RIGHT_CENTER,
                            },
                        .attachTo = CLAY_ATTACH_TO_ELEMENT_WITH_ID,
                    },
            }) {
              Clay_OnHover(HandleSkipButtonClick, 0);
            };
          }
        };
      };

      TabBarComponent();
    };
  };
}
