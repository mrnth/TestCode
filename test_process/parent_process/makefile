CC=/usr/bin/c++
APP=parent_process.elf

CXX_FLAGS = -std=c++14 -fopenmp -Wall -O3 -fpermissive

CXX_INCLUDES = -I/usr/include -I/usr/local/include

CXX_LIB_DIR = -L/usr/local/lib -L/lib/x86_64-linux-gnu -L/usr/lib/x86_64-linux-gnu

LIBS := -lpthread

SUBDIRS := $(wildcard */) $(wildcard */*/) $(wildcard */*/*/) $(wildcard */*/*/*/) $(wildcard */*/*/*/*/)

DEPS += \
$(wildcard *.h) \
$(wildcard $(addsuffix *.h,$(SUBDIRS))) \
$(wildcard *.hpp) \
$(wildcard $(addsuffix *.hpp,$(SUBDIRS))) 

SRCS+= \
$(wildcard *.cpp) \
$(wildcard $(addsuffix *.cpp,$(SUBDIRS)))

OBJS:= $(SRCS:.cpp=.o)

%.o: %.cpp $(DEPS)
	@echo 'Building file: $<'
	$(CC) $(CXX_DEFINES) $(CXX_FLAGS) $(CXX_INCLUDES) $(CXX_LIB_DIR) -c -fmessage-length=0 -MT$@ -fPIC -fopenmp -MMD -MP -MF$(@:%.o=%.d) -o $@ $<
	# $(CC) $(CXX_DEFINES) $(CXX_FLAGS) $(CXX_INCLUDES) $(CXX_LIB_DIR) -c -fmessage-length=0 -MT$@ -fPIC -fopenmp -shared -MMD -MP -MF$(@:%.o=%.d) -o $@ $<
	@echo 'Finished building: $<'
	@echo ' '

# All Target
all: ${APP}

# Tool invocations
${APP}: $(OBJS)  
	$(CC) $(CXX_LIB_DIR) -std=c++14 -fopenmp -o "${APP}" $(OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

.PHONY: all clean dependents
.SECONDARY:

clean:
	find . -name '*.o' -delete
	find . -name '*.d' -delete
	rm -f *.elf
