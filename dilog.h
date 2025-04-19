/**
 *   __       ___
 *  /\ \  __ /\_ \
 *  \_\ \/\_\\//\ \     ___      __
 *  /'_` \/\ \ \ \ \   / __`\  /'_ `\
 * /\ \L\ \ \ \ \_\ \_/\ \L\ \/\ \L\ \
 * \ \___,_\ \_\/\____\ \____/\ \____ \
 *  \/__,_ /\/_/\/____/\/___/  \/___L\ \
 *                               /\____/
 *                               \_/__/
 *
 * dilog 1.0 - Modern & minimalist C log utility
 */

// Copyright (c) 2025 - Diego Moreno
// This software is licensed under the MIT license (see LICENSE.txt)

#ifndef DILOG_H
#define DILOG_H

#include <stdatomic.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>

#else
#include <sys/time.h>  // gettimeofday()
#endif

// Constants
#define DILOG_MAX_TOPICS      16
#define DILOG_MAX_NAME_LEN    32
#define DILOG_MAX_FORMAT_LEN  128
#define DILOG_MAX_TYPE_COUNT  8
#define DILOG_MAX_TYPE_NAME   16
#define DILOG_MAX_COLOR_LEN   16
#define DILOG_MAX_OUTPUT_PATH 256

#ifdef __cplusplus
extern "C" {
#endif

// ──────────────── Structures ──────────────── //
typedef struct {
  char type_name[DILOG_MAX_TYPE_NAME];
  char color[DILOG_MAX_COLOR_LEN];
} dilog_log_type_t;

typedef struct {
  int year, month, day, hour, minute, second, ms;
  char timezone[8];
} dilog_timestamp_t;

typedef struct {
  char name[DILOG_MAX_NAME_LEN];
  char format[DILOG_MAX_FORMAT_LEN];
  char default_type[DILOG_MAX_TYPE_NAME];

  char file_output_path[DILOG_MAX_OUTPUT_PATH];
  bool terminal_output;
  bool terminal_advanced;

  dilog_log_type_t types[DILOG_MAX_TYPE_COUNT];
  size_t type_count;

  FILE* file;
} dilog_topic_t;

// ──────────────── Global State ──────────────── //
typedef struct {
  dilog_topic_t topics[DILOG_MAX_TOPICS];
  size_t topic_count;
  atomic_size_t current_topic_index;
  char default_topic[DILOG_MAX_NAME_LEN];
} dilog_context_t;

static dilog_context_t g_dilog_ctx = { .topic_count = 0, .current_topic_index = 0 };

// ──────────────── Utils ──────────────── //
static inline dilog_topic_t* dilog_get_topic(const char* name) {
  if (!name)
    return NULL;

  for (size_t i = 0; i < g_dilog_ctx.topic_count; ++i) {
    if (strncmp(g_dilog_ctx.topics[i].name, name, DILOG_MAX_NAME_LEN) == 0) {
      return &g_dilog_ctx.topics[i];
    }
  }
  return NULL;
}

static inline void dilog_get_timestamp_struct(dilog_timestamp_t* ts_out) {
#if defined(_WIN32) || defined(_WIN64)
  SYSTEMTIME st;
  GetLocalTime(&st);

  ts_out->year = st.wYear;
  ts_out->month = st.wMonth;
  ts_out->day = st.wDay;
  ts_out->hour = st.wHour;
  ts_out->minute = st.wMinute;
  ts_out->second = st.wSecond;
  ts_out->ms = st.wMilliseconds;

  strcpy_s(ts_out->timezone, sizeof(ts_out->timezone), "UTC");

#else
  struct timeval tv;
  struct tm tm;
  gettimeofday(&tv, NULL);
  localtime_r(&tv.tv_sec, &tm);

  ts_out->year = tm.tm_year + 1900;
  ts_out->month = tm.tm_mon + 1;
  ts_out->day = tm.tm_mday;
  ts_out->hour = tm.tm_hour;
  ts_out->minute = tm.tm_min;
  ts_out->second = tm.tm_sec;
  ts_out->ms = tv.tv_usec / 1000;

  strftime(ts_out->timezone, sizeof(ts_out->timezone), "%Z", &tm);
#endif
}

static inline void dilog_console_print(const char* topic_name, const char* type, const char* message) {
  if (!type || !message)
    return;

  dilog_topic_t* topic =
      topic_name ? dilog_get_topic(topic_name) : &g_dilog_ctx.topics[atomic_load(&g_dilog_ctx.current_topic_index)];

  if (!topic || !topic->terminal_output)
    return;

  const char* color_code = NULL;
  for (size_t i = 0; i < topic->type_count; ++i) {
    if (strncmp(topic->types[i].type_name, type, DILOG_MAX_TYPE_NAME) == 0) {
      color_code = topic->types[i].color;
      break;
    }
  }

  dilog_timestamp_t ts = { 0 };
  dilog_get_timestamp_struct(&ts);

  const char* fmt = topic->format;
  char output[1024] = { 0 };
  size_t out_index = 0;

  for (size_t i = 0; fmt[i] && out_index < sizeof(output) - 1;) {
    bool matched = false;

#define MATCH_AND_WRITE(KEYWORD, FORMAT, VALUE)                                           \
  if (strncmp(&fmt[i], KEYWORD, sizeof(KEYWORD) - 1) == 0) {                              \
    out_index += snprintf(&output[out_index], sizeof(output) - out_index, FORMAT, VALUE); \
    i += sizeof(KEYWORD) - 1;                                                             \
    matched = true;                                                                       \
  }

    MATCH_AND_WRITE("year", "%04d", ts.year)
    else MATCH_AND_WRITE("month", "%02d", ts.month)
        else MATCH_AND_WRITE("day", "%02d", ts.day)
        else MATCH_AND_WRITE("hour", "%02d", ts.hour)
        else MATCH_AND_WRITE("minute", "%02d", ts.minute)
        else MATCH_AND_WRITE("second", "%02d", ts.second)
        else MATCH_AND_WRITE("ms", "%03d", ts.ms)
        else MATCH_AND_WRITE("timezone", "%s", ts.timezone)
        else MATCH_AND_WRITE("type", "%s", type)
        else MATCH_AND_WRITE("log", "%s", message)
#undef MATCH_AND_WRITE

        if (!matched) {
      output[out_index++] = fmt[i++];
    }
  }

  output[out_index] = '\0';

  if (color_code) {
    int r = (int)strtol(&color_code[1], NULL, 16) >> 16 & 0xFF;
    int g = (int)strtol(&color_code[1], NULL, 16) >> 8 & 0xFF;
    int b = (int)strtol(&color_code[1], NULL, 16) & 0xFF;
    printf("\x1b[38;2;%d;%d;%dm%s\x1b[0m\n", r, g, b, output);
  } else {
    printf("%s\n", output);
  }
}

// ──────────────── API ──────────────── //
static inline void dilog_init(void) {
  if (g_dilog_ctx.topic_count > 0)
    return;

  strncpy(g_dilog_ctx.default_topic, "main", DILOG_MAX_NAME_LEN - 1);

  dilog_topic_t* topic = &g_dilog_ctx.topics[0];
  memset(topic, 0, sizeof(dilog_topic_t));

  strncpy(topic->name, "main", DILOG_MAX_NAME_LEN - 1);
  strncpy(topic->format, "[year-month-day hour:minute:second.ms] <type>: log", DILOG_MAX_FORMAT_LEN - 1);

  topic->terminal_output = true;
  topic->terminal_advanced = false;

  strncpy(topic->types[0].type_name, "error", DILOG_MAX_TYPE_NAME - 1);
  strncpy(topic->types[0].color, "#B1FF31", DILOG_MAX_COLOR_LEN - 1);

  strncpy(topic->types[1].type_name, "info", DILOG_MAX_TYPE_NAME - 1);
  strncpy(topic->types[1].color, "#11FF11", DILOG_MAX_COLOR_LEN - 1);

  strncpy(topic->default_type, "info", DILOG_MAX_TYPE_NAME - 1);

  topic->type_count = 2;

  topic->file_output_path[0] = '\0';
  topic->file = NULL;

  g_dilog_ctx.topic_count = 1;
  atomic_store(&g_dilog_ctx.current_topic_index, 0);
}

static inline void dilog(const char* log) {
  if (!g_dilog_ctx.default_topic || !log)
    return;

  dilog_topic_t* topic = g_dilog_ctx.default_topic ? dilog_get_topic(g_dilog_ctx.default_topic)
                                                   : &g_dilog_ctx.topics[atomic_load(&g_dilog_ctx.current_topic_index)];

  if (!topic || !topic->terminal_output)
    return;

  dilog_console_print(topic->name, topic->default_type, log);
}

#ifdef __cplusplus
}
#endif

#endif  // DILOG_H