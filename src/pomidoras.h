#ifndef POMIDORAS_H
#define POMIDORAS_H

#include "../include/clay.h"
#include "../raylib/src/raylib.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// #define COLOR_WHITE (Clay_Color){255, 255, 255, 255}
// #define COLOR_CARD_BACKGROUND (Clay_Color){255, 255, 255, 25}
#define COLOR_TRANSPARENT (Clay_Color){0, 0, 0, 0}
// #define COLOR_PRIMARY (Clay_Color){200, 100, 100, 255}
#define COLOR_PRIMARY_LIGHT (Clay_Color){0, 0, 0, 38}
#define COLOR_PRIMARY_HOVER (Clay_Color){0, 0, 0, 15}
// #define COLOR_POMODORO (Clay_Color){200, 100, 100, 255}
// #define COLOR_SHORT_BREAK (Clay_Color){56, 133, 138, 225}
// #define COLOR_LONG_BREAK (Clay_Color){57, 112, 151, 255}
// #define COLOR_BLACK (Clay_Color){0, 0, 0, 255}

const char *FOCUS_TIMER = "FOCUS_TIMER";
const char *SHORT_BREAK_TIMER = "SHORT_BREAK_TIMER";
const char *LONG_BREAK_TIMER = "LONG_BREAK_TIMER";
const char *LONG_BREAK_INTERVAL = "LONG_BREAK_INTERVAL";
const char *AUTO_START = "AUTO_START";
const char *NUMBER_OF_SESSIONS = "NUMBER_OF_SESSIONS";
const char *THEME = "THEME";

const int DEFAULT_FOCUS_TIME = 25;
const int DEFAULT_SHORT_BREAK = 05;
const int DEFAULT_LONG_BREAK = 15;
const int DEFAULT_LONG_BREAK_INTERVAL = 3;

const char *CONFIG_FILE = "pomo_config";

typedef enum { THEME_LIGHT, THEME_DARK } AppTheme;

typedef struct {
  Clay_Color background;
  Clay_Color text_primary;
  Clay_Color text_secondary;
  Clay_Color card;

  Clay_Color focus;
  Clay_Color short_break;
  Clay_Color long_break;

  Clay_Color focus_hover;
  Clay_Color short_break_hover;
  Clay_Color long_break_hover;

  Clay_Color primary;
  Clay_Color primary_hover;

  Clay_Color border;
} Theme;

typedef enum AppScreen {
  SESSION_START = 1,
  POMODORO_TIMER,
  SESSION_COMPLETE,
  SETTINGS,
} AppScreen;

typedef enum AppTimer {
  POMODORO,
  SHORT_BREAK,
  LONG_BREAK,
} AppTimer;

typedef struct Input {
  char chars[256];
  int32_t length;
  uint32_t max_length;
} Input;

typedef struct AppConfig {
  uint32_t number_of_pomos;
  uint32_t focus_timer;
  uint32_t short_break_timer;
  uint32_t long_break_timer;
  uint32_t long_break_interval;
  bool debug_enabled;
  bool auto_start;
  AppTheme theme;
  // TODO: include font
} AppConfig;

typedef struct AppState {
  float current_time;
  int pomos_completed;
  uint32_t previous_selected_tab_index;
  uint32_t selected_tab_index;
  AppScreen current_screen;
  AppTimer current_timer;
  bool timer_started;
  Clay_Color current_color;
  Input *focused_input;
} AppState;

typedef struct TabBarItemData {
  int index;
  Clay_String label;
  Clay_Color color;
} TabBarItemData;

static TabBarItemData tab_bar_items[] = {
    {
        .index = 0,
        .label =
            {
                .length = (int32_t)strlen("Focus"),
                .chars = "Focus",
            },
    },
    {
        .index = 1,
        .label =
            {
                .length = (int32_t)strlen("Short Break"),
                .chars = "Short Break",
            },
    },
    {
        .index = 2,
        .label =
            {
                .length = (int32_t)strlen("Long Break"),
                .chars = "Long Break",
            },
    },
};

static uint32_t num_tabs = sizeof(tab_bar_items) / sizeof(tab_bar_items[0]);

static Sound notification_sound;
static Texture2D skipTexture;
static Texture2D rocketTexture;
static Texture2D sunTexture;
static Texture2D moonTexture;
static Texture2D settingTexture;
static Texture2D backTexture;
static Texture2D backTextureDark;
static Texture2D saveTexture;
static Texture2D saveTextureDark;

extern AppState app_state;
extern AppConfig app_config;

const Theme light_theme = {
    .background = {250, 250, 250, 255},
    .text_primary = {46, 46, 46, 255},
    .text_secondary = {110, 110, 110, 255},
    .card = {255, 255, 255, 255},

    .focus = {255, 179, 186, 255},
    .short_break = {186, 255, 255, 255},
    .long_break = {192, 252, 198, 255},

    .focus_hover = {255, 154, 165, 255},
    .short_break_hover = {159, 212, 255, 255},
    .long_break_hover = {162, 247, 181, 255},

    .primary = {200, 182, 255, 255},
    .primary_hover = {179, 156, 255, 255},

    .border = {231, 229, 228, 225},
};

const Theme dark_theme = {
    .background = {28, 28, 28, 255},
    .text_primary = {237, 237, 237, 255},
    .text_secondary = {179, 179, 179, 255},
    .card = {42, 42, 42, 255},

    .focus = {255, 122, 133, 255},
    .short_break = {110, 193, 255, 255},
    .long_break = {112, 232, 144, 255},

    .focus_hover = {255, 102, 115, 255},
    .short_break_hover = {85, 181, 255, 255},
    .long_break_hover = {85, 224, 124, 255},

    .primary = {164, 135, 255, 255},
    .primary_hover = {179, 156, 255, 255},

    .border = {231, 229, 228, 225},
};

const Theme *get_current_theme(void) {
  return app_config.theme == THEME_LIGHT ? &light_theme : &dark_theme;
}

void parse_config(const char *buf) {
  char *var = strtok(buf, "=");
  char *val = strtok(NULL, "=");
  for (int i = 0; i < strlen(val); ++i) {
    if (val[i] == '\n')
      val[i] = '\0';
  }

  if (strcmp(var, FOCUS_TIMER) == 0) {
    app_config.focus_timer = (strtol(val, NULL, 10) * 60.0);
  } else if (strcmp(var, SHORT_BREAK_TIMER) == 0) {
    app_config.short_break_timer = (strtol(val, NULL, 10) * 60.0);
  } else if (strcmp(var, LONG_BREAK_TIMER) == 0) {
    app_config.long_break_timer = (strtol(val, NULL, 10) * 60.0);
  } else if (strcmp(var, LONG_BREAK_INTERVAL) == 0) {
    app_config.long_break_interval = strtol(val, NULL, 10);
  } else if (strcmp(var, AUTO_START) == 0) {
    app_config.auto_start = strtol(val, NULL, 10) == 1;
  } else if (strcmp(var, NUMBER_OF_SESSIONS) == 0) {
    app_config.number_of_pomos = strtol(val, NULL, 10);
  } else if (strcmp(var, THEME) == 0) {
    printf("%s | %s | %i\n", val, "THEME_LIGHT", strcmp(val, "THEME_LIGHT"));
    app_config.theme =
        strcmp(val, "THEME_LIGHT") == 0 ? THEME_LIGHT : THEME_DARK;
  }
}

void load_config() {
  FILE *config_file = fopen(CONFIG_FILE, "r");
  if (config_file) {
    char buf[512];
    while (fgets(buf, sizeof(buf), config_file) != NULL) {
      parse_config(buf);
    }
  } else {
    printf("INFO: Creating Default Config\n");
    config_file = fopen(CONFIG_FILE, "w");
    if (NULL == config_file) {
      fprintf(stderr, "ERROR: Unable to create config file\n");
      exit(1);
    }
  }
  fclose(config_file);
}

void save_config() {
  FILE *config_file = fopen(CONFIG_FILE, "w");
  if (config_file == NULL) {
    fprintf(stderr, "ERROR: Failed to config file");
  } else {
    fprintf(config_file, "%s=%f\n", FOCUS_TIMER, app_config.focus_timer / 60.0);
    fprintf(config_file, "%s=%f\n", SHORT_BREAK_TIMER,
            app_config.short_break_timer / 60.0);
    fprintf(config_file, "%s=%f\n", LONG_BREAK_TIMER,
            app_config.long_break_timer / 60.0);
    fprintf(config_file, "%s=%i\n", LONG_BREAK_INTERVAL,
            app_config.long_break_interval);
    fprintf(config_file, "%s=%i\n", AUTO_START, app_config.auto_start);
    fprintf(config_file, "%s=%i\n", NUMBER_OF_SESSIONS,
            app_config.number_of_pomos);
    fprintf(config_file, "%s=%s\n", THEME,
            app_config.theme == THEME_LIGHT ? "THEME_LIGHT" : "THEME_DARK");
  }
  fclose(config_file);
}

// void update_config(const char *var, const char *val) {
//   FILE *config_file = fopen(CONFIG_FILE, "w");
//   if (config_file == NULL) {
//     fprintf(stderr, "ERROR: Failed to config file");
//   } else {
//     char buf[512];
//     while (fgets(buf, sizeof(buf), config_file) != NULL) {
//       char *_var = strtok(buf, "=");
//       if (strcmp(var, _var) == 0) {
//       }
//       char *_val = strtok(NULL, "=");
//       // for (int i = 0; i < sizeof(val); ++i) {
//       //   val[i] = '\0';
//       // }
//     }
//   }
// }

#endif // POMIDORAS_H
