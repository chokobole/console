// Copyright (c) 2020 The Console Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CONSOLE_FLAG_FORWARD_H_
#define CONSOLE_FLAG_FORWARD_H_

#include <stdint.h>

#include <string>

namespace console {

template <typename T>
class Flag;

typedef Flag<bool> BoolFlag;
typedef Flag<uint8_t> Uint8Flag;
typedef Flag<int8_t> Int8Flag;
typedef Flag<uint16_t> Uint16Flag;
typedef Flag<int16_t> Int16Flag;
typedef Flag<uint32_t> Uint32Flag;
typedef Flag<int32_t> Int32Flag;
typedef Flag<uint64_t> Uint64Flag;
typedef Flag<int64_t> Int64Flag;
typedef Flag<float> FloatFlag;
typedef Flag<double> DoubleFlag;
typedef Flag<std::string> StringFlag;

}  // namespace console

#endif  // CONSOLE_FLAG_FORWARD_H_