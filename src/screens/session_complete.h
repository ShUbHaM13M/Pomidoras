#include "../pomidoras.h"

void HandleOnSessionCompletedClick(Clay_ElementId element_id,
                                   Clay_PointerData pointerData,
                                   intptr_t user_data) {
  if (pointerData.state == CLAY_POINTER_DATA_RELEASED_THIS_FRAME) {
    app_state.current_screen = SESSION_START;
  }
}

void renderSessionComplete() {
    const Theme* theme = get_current_theme();
  CLAY({
      .layout =
          {
              .sizing =
                  {
                      .width = CLAY_SIZING_GROW(0),
                      .height = CLAY_SIZING_GROW(0),
                  },
              .padding = {10, 10, 24, 10},
              .childGap = 16,
              .childAlignment =
                  {
                      .x = CLAY_ALIGN_X_CENTER,
                      .y = CLAY_ALIGN_Y_CENTER,
                  },
              .layoutDirection = CLAY_TOP_TO_BOTTOM,
          },
      .backgroundColor = theme->background,
  }) {
    CLAY_TEXT(CLAY_STRING("Congratulations!"),
              CLAY_TEXT_CONFIG({
                                   .textColor = theme->text_primary,
                                   .fontSize = 32,
                                   .letterSpacing = 3,
                               }, ));
    CLAY_TEXT(CLAY_STRING("Session Completed"), CLAY_TEXT_CONFIG({
                                                    .textColor = theme->text_secondary,
                                                    .fontSize = 24,
                                                    .letterSpacing = 2,
                                                }));
    CLAY({
        .layout =
            {
                .padding = CLAY_PADDING_ALL(16),
            },
        .backgroundColor = theme->primary,
        .cornerRadius = 4,
    }) {
      Clay_OnHover(HandleOnSessionCompletedClick, 0);
      CLAY_TEXT(CLAY_STRING("Start a new session"),
                CLAY_TEXT_CONFIG({
                    .textColor = theme->text_primary,
                    .fontSize = 16,
                    .letterSpacing = 2,
                }));
    }
  }
}
