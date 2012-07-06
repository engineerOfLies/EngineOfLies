#include "eol_logger.h"
#include <stdarg.h>

/*local globals*/

eolBool      _eol_logger_initialized = eolFalse;
eolLogLevel  _eol_log_threshold = EOL_LOG_INFO;
eolBool      _eol_logger_stdout_echo = eolTrue;
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
    fprintf(stderr,"eol_logger: message: %s\n",msg);
    return;
  }
  if (level < _eol_log_threshold)return;
  if (_eol_logger_stdout_echo == eolTrue)
  {
    va_start(ap,msg);
    vfprintf(stdout,msg,ap);
    va_end(ap);
  }
  if (_eol_logger_file == NULL)
  {
    fprintf(stderr,"eol_logger: no log file found!\n");
    fprintf(stderr,"eol_logger: message: %s\n",msg);
    return;
  }
  va_start(ap,msg);
  vfprintf(_eol_logger_file,msg,ap);
  va_end(ap);
}

void eol_logger_init()
{
	fprintf(stdout,"eol_logger: initializing\n");
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
  fprintf(stdout,"eol_logger: initialized\n");
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
  /*TODO: load config, if not found set defaults*/
  _eol_log_threshold = EOL_LOG_INFO;
  _eol_logger_stdout_echo = eolTrue;
  sprintf(_eol_logger_filename,"system/engineoflies.log");
}

/*eol@eof*/

