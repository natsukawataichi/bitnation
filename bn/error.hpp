#pragma once

#include <bn/header.hpp>

namespace bn {

/**
 * Output error message into stderr and exit from program
 *
 * @param[in] *fmt format
 * @param[in] ...  arguments
 */
void
error(const char *fmt, ...);

};
