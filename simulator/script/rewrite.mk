default: VCPU
include VCPU.mk
CXXFLAGS += -MMD -O3 -std=c++14 -fno-exceptions -fPIE -Wno-unused-result
CXXFLAGS += $(shell llvm-config-17 --cxxflags) -fPIC -DDEVICE
#CXXFLAGS += -DAXI
LDFLAGS += -O3 -rdynamic -shared -fPIC
LIBS += $(shell llvm-config-17 --libs)
LIBS += -lreadline -ldl -pie -lSDL2
LINK := g++