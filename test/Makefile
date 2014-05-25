SRCDIR = ..

CXX = g++
CXXFLAGS = --std=c++11 -g3 -O0 -Wall -I$(SRCDIR)
LDFLAGS = -lboost_unit_test_framework

NODEPS:=clean cleanlib veryclean tags

tests = threading_test
test_objs := $(tests:%=%.o)
DEPS := $(tests:%=%.d)

all: $(tests)

#Don't create dependencies when we're cleaning, for instance
ifeq (0, $(words $(findstring $(MAKECMDGOALS), $(NODEPS))))
-include $(DEPS)
endif

./%.o: ./%.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $< -MMD -MT $(patsubst %.cc,%.o,$<) -MF $(patsubst %.o,%.d,$@)

define ADDTEST
$(1): $(1).o $(LIB)
	$(CXX) $(1).o $(LDFLAGS) -o $(1)
endef

$(foreach test,$(tests),$(eval $(call ADDTEST,$(test))))

%.o: %.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $< 


clean:
	rm -f $(tests) $(test_objs) $(DEPS)

.PHONY: all clean
