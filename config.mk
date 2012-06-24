CFLAGS+=-g -I $(ROOT)/include -DCOMPILING_AUTOPILOT=1 -fPIC -Wall -Wimplicit -Wimplicit-function-declaration -Wimplicit-int
LIBS+=-levent -ldl -llua -lm
