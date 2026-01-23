#pragma once

#include <OpenXLSX.hpp>
#include <optional>
#include "common.h"
enum class config_col {COMMAND_TYPE_COL = 1, COMMAND_COL, TARGET_COL, TIMEOUT_COL};

void ReadWithOpenXLSX();