import logging

# create logger
# create a handler 
# create a formatter
# add the formatter to the handler
# add the handler to the formatter
# use the logger to record message

log_name="orris_log"
logger=logging.getLogger(log_name)
logger.setLevel(logging.DEBUG)

handler_path="mylog.log"
fh=logging.FileHandler(handler_path)
fh.setLevel(logging.DEBUG)


fmt="%(asctime)s %(levelname)s %(filename)s %(lineno)d %(process)d %(message)s"
datefmt="%Y %B %d %H %M %S"
formatter=logging.Formatter(fmt,datefmt)

fh.setFormatter(formatter)
logger.addHandler(fh)

logger.debug('debug message')
logger.info('info message')
logger.warn('warn message')
logger.error('error message')
logger.critical('critical message')


