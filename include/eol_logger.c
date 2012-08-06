#include "eol_logger.h"
#include "eol_config.h"
#include <stdarg.h>

/*local globals*/

eolBool      _eol_logger_initialized = eolFalse;
eolLogLevel  _eol_log_threshold = EOL_LOG_INFO;
eolBool      _eol_logger_stdout_echo = eolFalse;
eolLine      _eol_logger_filename;
FILE       * _eol_logger_file = NULL;

/*local functions*/
void eol_logger_deinit(void);

/*function definitions*/
void eol_logger_info(char *msg,...)
{
	va_list ap;
  va_start(ap,msg);
  eol_logger_message(
    EOL_LOG_INFO,
    msg,
    ap);
  va_end(ap);
}

void eol_logger_warn(char *msg,...)
{
	va_list ap;
  va_start(ap,msg);
  eol_logger_message(
    EOL_LOG_WARN,
    msg,
    ap);
  va_end(ap);
}

void eol_logger_error(char *msg,...)
{
	va_list ap;
  va_start(ap,msg);
  eol_logger_message(
    EOL_LOG_ERROR,
    msg,
    ap);
  va_end(ap);
}

void eol_logger_fatal(char *msg,...)
{
	va_list ap;
  va_start(ap,msg);
  eol_logger_message(
    EOL_LOG_FATAL,
    msg,
    ap);
  va_end(ap);
}

void eol_logger_message(eolLogLevel level,char *msg,...)
{
  va_list ap;
  if (level >= EOL_LOG_MAX)
  {
    fprintf(stderr,"eol_logger: unable to write at log level %i\n",level);
  }
  if (level > _eol_log_threshold)return;
  if (_eol_logger_stdout_echo == eolTrue)
  {
    va_start(ap,msg);
    vfprintf(stdout,msg,ap);
    va_end(ap);
  }
  if (_eol_logger_file == NULL)
  {
    fprintf(stderr,"eol_logger: no log file found! Redirecting logs to stdout\n");
	_eol_logger_file = stdout;
    return;
  }
  va_start(ap,msg);
  vfprintf(_eol_logger_file,msg,ap);
  va_end(ap);
}

void eol_logger_init()
{
  if (strlen(_eol_logger_filename) <= 0)
  {
  	sprintf(_eol_logger_filename,"system/engineoflies.log");
  }
  _eol_logger_file = fopen(_eol_logger_filename, "w");
  if (_eol_logger_file == NULL)
  {
    fprintf(stderr,"unable to open log file.");
    return;
  }
  _eol_logger_initialized = eolTrue;
  atexit(eol_logger_deinit);
}

void eol_logger_deinit(void)
{
  if (_eol_logger_file != NULL)
  {
    fclose(_eol_logger_file);
    _eol_logger_file = NULL;
  }
}

void eol_logger_set_threshold(eolLogLevel level)
{
	if (level < EOL_LOG_MAX)
    _eol_log_threshold = level;
}

void eol_logger_load_config()
{
  eolLine buf;
  eolConfig *conf;
  conf = eol_config_load("system/logger.cfg");
  if (conf != NULL)
  {
    eol_config_get_line_by_tag(buf,conf,"log_file");
    if (strlen(buf) > 0)
    {
      if (eol_line_cmp(_eol_logger_filename,buf) != 0)
      {
        eol_line_cpy(_eol_logger_filename,buf);
        if (_eol_logger_file != NULL)fclose(_eol_logger_file);
        _eol_logger_file = fopen(_eol_logger_filename, "w");
      }
    }
    eol_config_get_line_by_tag(buf,conf,"log_level");
    if (strlen(buf) > 0)
    {
      if (eol_line_cmp(buf,"INFO")==0)
      {
        _eol_log_threshold = EOL_LOG_INFO;
      }
      else if (eol_line_cmp(buf,"ERROR")==0)
      {
        _eol_log_threshold = EOL_LOG_ERROR;
      }
      else if (eol_line_cmp(buf,"WARN")==0)
      {
        _eol_log_threshold = EOL_LOG_WARN;
      }
      else if (eol_line_cmp(buf,"FATAL")==0)
      {
        _eol_log_threshold = EOL_LOG_FATAL;
      }
      else if (eol_line_cmp(buf,"DEBUG")==0)
      {
        _eol_log_threshold = EOL_LOG_DEBUG;
      }
      else if (eol_line_cmp(buf,"NONE")==0)
      {
        _eol_log_threshold = EOL_LOG_NONE;
      }
    }
    eol_config_get_line_by_tag(buf,conf,"echo_to_terminal");
    {
      if (eol_line_cmp(buf,"true")==0)
      {
        _eol_logger_stdout_echo = eolTrue;
      }
      else if (eol_line_cmp(buf,"false")==0)
      {
        _eol_logger_stdout_echo = eolFalse;
      }
    }
  }
}

/*eol@eof*/

