PREFIX=/usr/local
DESTDIR=/

CFLAGS+=-g -I $(ROOT)/include -DCOMPILING_AUTOPILOT=1 -DPREFIX="\"$(PREFIX)\"" -fPIC -Wall -Wimplicit -Wimplicit-function-declaration -Wimplicit-int
LIBS+=-levent -ldl -llua -lm
