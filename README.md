
# CoralPlus (with installer's for linux_x64 and win32)


### CoralPlus service application (ssl mode)

start server : ./CoralPlusSrv mode=work bind=6543 serial=/dev/ttyUSB0:9600 (for win32 serial=COM3:9600)


#### CoralPlus client application (ssl mode)

start client : ./CoralPlusCli serverip=127.0.0.1 cfgfile=coral.npl
