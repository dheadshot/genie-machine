LINUXLIBS = $(shell pkg-config --libs gtk+-3.0 gdk-3.0)
WINDOWSLIBS = -lgdi32 -lcomdlg32 -lcomctl32 -luuid -loleaut32 -lole32

ifdef $(WIN32)
LIBS += -liup ${WINDOWSLIBS}
DEFS += -DWIN32
else
LIBS += -liup ${LINUXLIBS}
endif

LIBS += -liupimglib -lim -liupim -lcd -liupcd

DEFS += -DUNICODE -DUTF8MODE=YES

CFLAGS += ${DEFS}

CC = gcc
LINKER = g++

OBJECTS = main.o dbaccess.o safeiupconf.o utf8.o 

.c.o:
	${CC} ${CFLAGS} -c $<

ifdef $(WIN32)
genie-machine.exe: ${OBJECTS}
	${LINKER} -o $@  ${OBJECTS} ${LIBS}
else
genie-machine:  ${OBJECTS}
	${LINKER} -o $@  ${OBJECTS} ${LIBS}
endif
