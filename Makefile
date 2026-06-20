BUILD_DIR ?= build
TARGETS ?= "seq;seq_checks;mt_pth"

GENERATOR ?= "Unix Makefiles"

.PHONY: all build clean

all: build

build:
	cmake -B $(BUILD_DIR) -G $(GENERATOR) -DTARGETS=$(TARGETS)
	+cmake --build $(BUILD_DIR)

clean:
	$(RM) -r $(BUILD_DIR)
