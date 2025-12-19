#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#define CLAY_IMPLEMENTATION
#include "../include/clay.h"
#include "../include/clay_renderer_raylib.c"
#include "./resources/back_icon.h"
#include "./resources/left_arrow_icon.h"
#include "./resources/moon_icon.h"
#include "./resources/notification.h"
#include "./resources/rocket_icon.h"
#include "./resources/save_icon.h"
#include "./resources/setting_icon.h"
#include "./resources/skip_icon.h"
#include "./resources/sun_icon.h"
#include "./screens/session.h"
#include "./screens/session_complete.h"
#include "./screens/session_start.h"
#include "./screens/settings.h"
#include "platform_notification.h"
#include "pomidoras.h"

#define RAYLIB_VECTOR2_TO_CLAY_VECTOR2(vector)                                 \
  (Clay_Vector2) { .x = vector.x, .y = vector.y }

void HandleClayErrors(Clay_ErrorData errorData) {
  printf("%s\n", errorData.errorText.chars);
}

const int screenWidth = 460;
const int screenHeight = 340;

// const int PADDING_X = 20;
// const int PADDING_Y = 24;

AppConfig app_config = {
    .debug_enabled = false,
    .number_of_pomos = 3,
    .focus_timer = DEFAULT_FOCUS_TIME * 60.0,
    .short_break_timer = DEFAULT_SHORT_BREAK * 60.0,
    .long_break_timer = DEFAULT_LONG_BREAK * 60.0,
    .long_break_interval = DEFAULT_LONG_BREAK_INTERVAL,
    .auto_start = true,
    .theme = THEME_DARK,
};

AppState app_state = {
    .current_screen = SESSION_START,
    .previous_selected_tab_index = 0,
    .current_timer = POMODORO,
    .pomos_completed = 0,
};

static float elapsed_time = 0.0;
static bool is_animating = false;

Clay_RenderCommandArray CreateLayout(void) {
  Clay_BeginLayout();

  switch (app_state.current_screen) {
  case SESSION_START:
    renderPomoSelector();
    break;
  case POMODORO_TIMER:
    renderPomoTimer();
    break;
  case SESSION_COMPLETE:
    renderSessionComplete();
    break;
  case SETTINGS:
    renderSettings();
    break;
  }

  return Clay_EndLayout();
}

float easeInOut(float t) { return t * t * (3.0f - 2.0f * t); }

void reset_app() {
  app_state = (AppState){0};
  app_state.current_screen = SESSION_START;
  app_state.current_color = get_current_theme()->focus;
  app_state.current_timer = POMODORO;
  app_state.current_time = app_config.focus_timer;
}

void HandleInputKeyPress(Input *input) {
  int key = GetKeyPressed();
  if (key == KEY_TAB) {
    // TODO: Implement tab functionality by input = input->next
    return;
  }
  if (key == KEY_BACKSPACE && input->length > 0) {
    input->length--;
    input->chars[input->length] = '\0';
    if (input->chars[0] == '\0') {
      input->length++;
      input->chars[0] = '0';
      input->chars[input->length] = '\0';
    }
  }

  int ch = GetCharPressed();
  while (ch > 0) {
    if (input->chars[0] == '0' && input->length > 0) {
      for (int i = 0; i < input->length; i++) {
        input->chars[i] = input->chars[i + 1];
      }
      input->length--;
    }
    if (input->length < 255 && input->length < input->max_length) {
      if (ch >= '0' && ch <= '9') {
        input->chars[input->length] = (char)ch;
        input->length++;
        input->chars[input->length] = '\0';
      }
    }
    ch = GetCharPressed();
  }
}

void initialize_resources(void) {
  Wave notification_wave = LoadWaveFromMemory(".wav", notification_sound_wav,
                                              notification_sound_wav_len);
  notification_sound = LoadSoundFromWave(notification_wave);
  UnloadWave(notification_wave);

  Image skip_image = LoadImageFromMemory(".png", skip_icon, skip_icon_len);
  skipTexture = LoadTextureFromImage(skip_image);
  UnloadImage(skip_image);

  Image rocket_image =
      LoadImageFromMemory(".png", rocket_icon_data, rocket_icon_data_len);
  rocketTexture = LoadTextureFromImage(rocket_image);
  UnloadImage(rocket_image);

  Image sun_image = LoadImageFromMemory(".png", sun_icon, sun_icon_len);
  sunTexture = LoadTextureFromImage(sun_image);
  Image moon_image = LoadImageFromMemory(".png", moon_icon, moon_icon_len);
  moonTexture = LoadTextureFromImage(moon_image);
  UnloadImage(sun_image);
  UnloadImage(moon_image);

  Image setting_image =
      LoadImageFromMemory(".png", setting_icon, setting_icon_len);
  settingTexture = LoadTextureFromImage(setting_image);
  UnloadImage(setting_image);

  Image back_image =
      LoadImageFromMemory(".png", left_arrow_icon, left_arrow_icon_len);
  backTexture = LoadTextureFromImage(back_image);
  ImageColorInvert(&back_image);
  backTextureDark = LoadTextureFromImage(back_image);
  UnloadImage(back_image);

  Image save_image = LoadImageFromMemory(".png", save_icon, save_icon_len);
  saveTexture = LoadTextureFromImage(save_image);
  ImageColorInvert(&save_image);
  saveTextureDark = LoadTextureFromImage(save_image);

  UnloadImage(save_image);
}

void initialize_inputs() {
  sprintf(focus_timer.chars, "%f", app_config.focus_timer / 60.0);
  sprintf(short_timer.chars, "%f", app_config.short_break_timer / 60.0);
  sprintf(long_timer.chars, "%f", app_config.long_break_timer / 60.0);
  sprintf(long_break_interval.chars, "%d", app_config.long_break_interval);
}

int main(void) {
  load_config();
  app_state.current_color = get_current_theme()->focus,
  app_state.current_time = app_config.focus_timer;
  initialize_inputs();

  Clay_Raylib_Initialize(screenWidth, screenHeight, "Pomidoras",
                         FLAG_WINDOW_RESIZABLE | FLAG_WINDOW_HIGHDPI |
                             FLAG_MSAA_4X_HINT | FLAG_VSYNC_HINT);

  InitAudioDevice();
  if (!IsAudioDeviceReady()) {
      printf("Audio device failed to initialize!\n");
  }
  initialize_resources();

  uint64_t clayRequiredMemory = Clay_MinMemorySize();
  Clay_Arena clayMemory = Clay_CreateArenaWithCapacityAndMemory(
      clayRequiredMemory, malloc(clayRequiredMemory));
  Clay_Context *clayContext = Clay_Initialize(clayMemory,
                                              (Clay_Dimensions){
                                                  (float)GetScreenWidth(),
                                                  (float)GetScreenHeight(),
                                              },
                                              (Clay_ErrorHandler){
                                                  HandleClayErrors,
                                                  0,
                                              });

  Font fonts[1];
  fonts[0] = GetFontDefault();
  Clay_SetMeasureTextFunction(Raylib_MeasureText, fonts);

  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    tab_bar_items[0].color = get_current_theme()->focus;
    tab_bar_items[1].color = get_current_theme()->short_break;
    tab_bar_items[2].color = get_current_theme()->long_break;

    float delta_time = GetFrameTime();
    if (app_state.timer_started && app_state.current_screen == POMODORO_TIMER) {
      if (app_state.current_time > 0)
        app_state.current_time -= delta_time;
      if (app_state.current_time <= 0) {
        PlaySound(notification_sound);
        app_state.timer_started = false;
        switch (app_state.current_timer) {
        case POMODORO:
          app_state.pomos_completed++;
          if (app_state.pomos_completed == app_config.number_of_pomos) {
            reset_app();
            platform_raise_notification("Pomidoras",
                                        "Congratulations! Session completed");
            continue;
          }
          if (app_state.pomos_completed == app_config.long_break_interval) {
            app_state.current_timer = LONG_BREAK;
            platform_raise_notification("Pomidoras", "Time for a break!");
            app_state.current_time = app_config.long_break_timer;
            app_state.selected_tab_index += 2;
            break;
          }
          app_state.current_timer = SHORT_BREAK;
          platform_raise_notification("Pomidoras", "Time for a short break!");
          app_state.selected_tab_index += 1;
          app_state.current_time = app_config.short_break_timer;
          break;
        case SHORT_BREAK:
        case LONG_BREAK:
          app_state.current_timer = POMODORO;
          app_state.current_time = app_config.focus_timer;
          app_state.selected_tab_index = 0;
          platform_raise_notification("Pomidoras", "Time to focus!");
          break;
        }
        if (app_config.auto_start) {
          app_state.timer_started = true;
        }
      }
    }
    if (IsKeyPressed(KEY_D)) {
      // app_config.debug_enabled = !app_config.debug_enabled;
      // Clay_SetDebugModeEnabled(app_config.debug_enabled);
      PlaySound(notification_sound);
    }

    Clay_SetLayoutDimensions((Clay_Dimensions){
        (float)GetScreenWidth(),
        (float)GetScreenHeight(),
    });
    Clay_RenderCommandArray renderCommands = CreateLayout();

    Clay_Vector2 mousePosition =
        RAYLIB_VECTOR2_TO_CLAY_VECTOR2(GetMousePosition());
    Clay_SetPointerState(mousePosition, IsMouseButtonDown(0));

    Clay_UpdateScrollContainers(
        true, RAYLIB_VECTOR2_TO_CLAY_VECTOR2(GetMouseWheelMoveV()), delta_time);

    // if (app_state.current_screen == POMODORO_TIMER) {
    //   Clay_ElementData active_session_element =
    //       Clay_GetElementData(Clay_GetElementId((Clay_String){
    //           .chars = "activeSession",
    //           .length = strlen("activeSession"),
    //       }));
    //       int x = active_session_element.boundingBox.x + random;
    //       int y = active_session_element.boundingBox.y + random;
    //       active_session_element.boundingBox.x = x;
    //       active_session_element.boundingBox.y = y;
    // }

    if (!is_animating &&
        app_state.selected_tab_index != app_state.previous_selected_tab_index) {
      elapsed_time = 0.0f;
      is_animating = true;
    }

    if (is_animating) {
      float dt = GetFrameTime();
      elapsed_time += dt;
      float duration = 0.5f;
      float factor = elapsed_time / duration;

      if (factor >= 1.0f) {
        factor = 1.0f;
        is_animating = false;
        app_state.previous_selected_tab_index = app_state.selected_tab_index;
      }

      float eased = easeInOut(factor);

      Clay_Color prev =
          tab_bar_items[app_state.previous_selected_tab_index].color;
      Clay_Color next = tab_bar_items[app_state.selected_tab_index].color;

      app_state.current_color =
          (Clay_Color){(unsigned char)(prev.r + (next.r - prev.r) * eased),
                       (unsigned char)(prev.g + (next.g - prev.g) * eased),
                       (unsigned char)(prev.b + (next.b - prev.b) * eased),
                       (unsigned char)(prev.a + (next.a - prev.a) * eased)};
    } else {
      app_state.current_color =
          tab_bar_items[app_state.selected_tab_index].color;
    }

    if (app_state.focused_input != NULL) {
      Input *focused_input = app_state.focused_input;
      HandleInputKeyPress(focused_input);
    }

    BeginDrawing();
    ClearBackground(WHITE);
    Clay_Raylib_Render(renderCommands, fonts);
    EndDrawing();
  }

  UnloadSound(notification_sound);
  UnloadTexture(skipTexture);
  UnloadTexture(rocketTexture);
  UnloadTexture(sunTexture);
  UnloadTexture(moonTexture);
  UnloadTexture(settingTexture);
  UnloadTexture(backTexture);
  CloseAudioDevice();
  Clay_Raylib_Close();
  return 0;
}
