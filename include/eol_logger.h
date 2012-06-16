#ifndef __EOL_LOGGER__
#define __EOL_LOGGER__
/*
    Copyright 2012 Engineer of Lies
    This file is part of the Engine of Lies game engine library

    The Engine of Lies (EOL) is free software: you can redistribute it and/or modify
    it under the terms of the GNU Lesser General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    EOL is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Lesser General Public License for more details.

    You should have received a copy of the GNU Lesser General Public License
    along with the EOL game engine.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "eol_types.h"

typedef enum _eol_log_levels {
	EOL_LOG_NONE  = 0,  /**<*/
	EOL_LOG_FATAL = 1,  /**<only log FATAL errors*/
	EOL_LOG_ERROR = 2,  /**<only log ERROR level and worse*/
	EOL_LOG_WARN  = 3,  /**<only log Warning level and worse*/
	EOL_LOG_INFO  = 4,  /**<log all*/
	EOL_LOG_MAX
}eolLogLevel;

/**
 * @brief initializes logging system
 */
void eol_logger_init();

/**
 * @brief returns the initialization status of the logger
 *
 * @return eolTrue if eol_logger is initialized
 */
eolBool eol_logger_is_initialized();

/**
 * @brief loads up configuration from file or if no file available it will set
 * defaults.
 */
void eol_logger_load_config();

/**
 * @brief sets how verbose logging will be.
 */
void eol_logger_set_threshold(eolLogLevel level);

/**
 * @brief log a message at the specified warning levels.  It will ignore warning
 * levels below the threshold.
 *
 * @param level the warning level to log this at.
 * @param msg the warning message to log.
 * @param .... this function uses the printf convention
 */
void eol_logger_message(eolLogLevel level,char *msg,...);

/**
 * @brief log a message at the info warning level.  It will ignore warning
 * levels below the threshold.
 *
 * @param msg the warning message to log.
 * @param .... this function uses the printf convention
 */
void eol_logger_info(char *msg,...);

/**
 * @brief log a message at the "warning" warning level.  It will ignore warning
 * levels below the threshold.
 *
 * @param msg the warning message to log.
 * @param .... this function uses the printf convention
 */
void eol_logger_warn(char *msg,...);

/**
 * @brief log a message at the error warning level.  It will ignore warning
 * levels below the threshold.
 *
 * @param msg the warning message to log.
 * @param .... this function uses the printf convention
 */
void eol_logger_error(char *msg,...);

/**
 * @brief log a message at the fatal warning level.  It will ignore warning
 * levels below the threshold.
 *
 * @param msg the warning message to log.
 * @param .... this function uses the printf convention
 */
void eol_logger_fatal(char *msg,...);

#endif

