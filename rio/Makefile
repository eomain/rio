
CC = cc
LIBDIR = ../lib
LIBS = $(LIBDIR)/libcx/libcx/libcx.a
INCLUDE = -I $(LIBDIR)/libcx/include
CFLAGS = -I ../include $(INCLUDE) -std=c11 -O2 -Werror

OBJS = $(patsubst %.c, %.o, $(wildcard *.c))
HEADERS = $(wildcard ../include/libcx/*.h)

LANG = en

OUT = rio

default: $(OUT)

$(OUT): $(OBJS) $(HEADERS) libs
	$(CC) $(OBJS) $(LIBS) -o $(OUT)

%.o: %.c $(HEADERS)
	-@rio strings.rio -o ../include/rio/strings.h -l $(LANG)
	$(CC) $(CFLAGS) -c $(?)
	
libs:
	@make -C $(LIBDIR)/libcx/libcx -s

clean:
	@make -C $(LIBDIR)/libcx/libcx -s clean
	@rm -f $(OBJS) $(OUT)
