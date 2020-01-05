// Copyright (c) 2020 The Console Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONSOLE_EXPORT_H_
#define CONSOLE_EXPORT_H_

#if defined(CONSOLE_COMPONENT_BUILD)

#if defined(_WIN32)
#ifdef CONSOLE_COMPILE_LIBRARY
#define CONSOLE_EXPORT __declspec(dllexport)
#else
#define CONSOLE_EXPORT __declspec(dllimport)
#endif  // defined(CONSOLE_COMPILE_LIBRARY)

#else
#ifdef CONSOLE_COMPILE_LIBRARY
#define CONSOLE_EXPORT __attribute__((visibility("default")))
#else
#define CONSOLE_EXPORT
#endif  // defined(CONSOLE_COMPILE_LIBRARY)
#endif  // defined(_WIN32)

#else
#define CONSOLE_EXPORT
#endif  // defined(CONSOLE_COMPONENT_BUILD)

#endif  // CONSOLE_EXPORT_H_