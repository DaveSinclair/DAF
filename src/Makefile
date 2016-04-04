NOW = $(shell date +"20%y%m%d.%H%M%S")

# work out the OS that we are compiling on

OS     := $(word 1,$(shell uname -a))
ifeq ($(OS),Linux)
     dummy := $(info Compiling on Linux - timestamp is $(NOW))
     OS = LINUX
     CFLAGS = -Wall -g 
     CFLAGS1 = -g
     LIBS = -L /opt/lampp/lib -lpthread -lmysqlclient -lz
     LIBS_AGENT = -lpthread
     INCLUDES = -I/usr/include/mysql
     DEFINES = -DUNAME="\"$(shell uname -a)\"" -DBUILDMACHINE="\"$(HOSTNAME)\""  -D_GNU_SOURCE -D_THREAD_SAFE -DLINUX -DPOLLING_SERVER
endif

ifeq ($(OS),SunOS)
     dummy := $(info Compiling on Solaris - timestamp is $(NOW))
     OS = SOLARIS
     CFLAGS = -Wall -DPORTMAP -DSOLARIS -g
     CFLAGS1 = -g
     LIBS = -lpthread -lsocket -lnsl  -lmysqlclient -lz
     INCLUDES = -I/usr/include/mysql
     DEFINES = -DUNAME="\"$(shell uname -a)\"" -DBUILDMACHINE="\"$(HOSTNAME)\"" -DSOLARIS -DPOLLING_SERVER
endif

# check to see if we are running this compile on a 64 bit machine
#CPU     := $(word 12,$(shell uname -a))
#ifeq ($(CPU),x86_64)
#     dummy := $(info Compile machine has $(CPU) architecture)
#else
#     dummy := $(info Compile machine has $(CPU) architecture)
#endif

# check to see if we are running this compile on a 2.6 kernel machine
#KERNEL  := $(word 3,$(shell uname -a))
#KERNEL  := $(shell echo $(KERNEL) | cut -c-3)
#ifeq ($(KERNEL),2.6)
#     dummy := $(info Compile machine has $(KERNEL) kernel - ok to proceed)
#else
#     dummy := $(error Compile machine has $(KERNEL) kernel - 2.6 Kernel machine needed - cannot proceed)
#endif


DAFOBJ = dafservice.o dafclient.o daf_util.o OSstuff.o daf_service_environment.o daf_shared_memory.o afmysql.o af_mysql.o daf.o daf_protocol_xdr.o email.o licence.o daf_threads.o
DAFAGENTOBJ =  dafservice_agent.o dafclient.o daf_util.o OSstuff.o daf_service_environment.o daf_shared_memory.o daf_agent.o daf_protocol_xdr.o daf_threads.o
DAFGENERATELICENCEOBJ = daf_generate_licence.o licence.o daf_util.o
DAFLICENCEOBJ = daf_licence.o licence.o daf_util.o

BINS = daf daf_agent daf_generate_licence daf_licence 
OBJS = $(DAFOBJ) $(DAFAGENTOBJ) $(DAFGENERATELICENCEOBJ) $(DAFLICENCEOBJ)

GEN = daf_protocol.h 
RPCGEN = rpcgen -M
CC = gcc


all: $(GEN) $(OBJS) $(BINS)

daf: $(DAFOBJ)
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o $@ $(DAFOBJ) ${LIBS}

daf_agent: $(DAFAGENTOBJ)
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o $@ $(DAFAGENTOBJ) ${LIBS_AGENT}

daf_generate_licence: $(DAFGENERATELICENCEOBJ)
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o $@ $(DAFGENERATELICENCEOBJ) ${LIBS}

daf_licence: $(DAFLICENCEOBJ)
	$(CC) $(CFLAGS) $(DEFINES) $(INCLUDES) -o $@ $(DAFLICENCEOBJ) ${LIBS}

daf.o: daf.c 
	gcc $(CFLAGS) $(DEFINES) $(INCLUDES) daf.c -c

daf_agent.o: daf.c 
	gcc $(CFLAGS) $(DEFINES) $(INCLUDES) -DDAF_AGENT -o daf_agent.o daf.c -c

dafclient.o: dafclient.c
	gcc $(CFLAGS) $(DEFINES) $(INCLUDES) dafclient.c -c

dafservice.o: dafservice.c 
	gcc $(CFLAGS) $(DEFINES) $(INCLUDES) dafservice.c -c

dafservice_agent.o: dafservice.c 
	gcc $(CFLAGS) $(DEFINES) $(INCLUDES) -DDAF_AGENT -o dafservice_agent.o dafservice.c -c

daf_service_environment.o: daf_service_environment.c 
	gcc $(CFLAGS) $(DEFINES) $(INCLUDES) daf_service_environment.c -c

daf_shared_memory.o: daf_shared_memory.c 
	gcc $(CFLAGS) $(DEFINES) $(INCLUDES) daf_shared_memory.c -c

daf_threads.o: daf_threads.c 
	gcc $(CFLAGS) $(DEFINES) $(INCLUDES) daf_threads.c -c

daf_util.o: daf_util.c daf_util.h
	gcc $(CFLAGS) $(DEFINES) $(INCLUDES) daf_util.c -c

OSstuff.o: OSstuff.c OSstuff.h
	gcc $(CFLAGS) $(DEFINES) $(INCLUDES) OSstuff.c -c

daf_protocol.h: daf_protocol.x
	$(RPCGEN) -h daf_protocol.x -o daf_protocol.h 

af_mysql.o: af_mysql.c
	gcc $(CFLAGS) $(DEFINES) $(INCLUDES) af_mysql.c -c

afmysql.o: afmysql.c
	gcc $(CFLAGS) $(DEFINES) $(INCLUDES) afmysql.c -c

daf_protocol_xdr.o: daf_protocol_xdr.c 
	gcc $(CFLAGS) $(DEFINES) $(INCLUDES) daf_protocol_xdr.c -c -Wno-unused-variable

daf_licence.o: daf_licence.c
	gcc $(CFLAGS) $(DEFINES) $(INCLUDES) daf_licence.c -c

daf_generate_licence.o: daf_generate_licence.c
	gcc $(CFLAGS) $(DEFINES) $(INCLUDES) daf_generate_licence.c -c

licence.o: licence.c 
	gcc $(CFLAGS) $(DEFINES) $(INCLUDES) licence.c -c

email.o: email.c 
	gcc $(CFLAGS) $(DEFINES) $(INCLUDES) email.c -c


clean:
	rm -f $(GEN) $(OBJS) $(BINS)
