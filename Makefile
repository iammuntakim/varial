BIN_DIR = bin
LIB_DIR = lib
CGO_SRC = crt0/crt0.ssk
CGO_TMP = ./cgo

.PHONY: all clean setup_cgo

all: setup_cgo $(BIN_DIR)/varial $(LIB_DIR)/libvarial.so
	@rm -f $(CGO_TMP)

setup_cgo:
	@cp $(CGO_SRC) $(CGO_TMP)
	@chmod +x $(CGO_TMP)

$(BIN_DIR)/varial:
	@mkdir -p $(BIN_DIR)
	@$(CGO_TMP) src/varial.c -Iinclude -o $(BIN_DIR)/varial

$(LIB_DIR)/libvarial.so:
	@mkdir -p $(LIB_DIR)
	@$(CGO_TMP) -shared src/varial.c -Iinclude -o $(LIB_DIR)/libvarial.so

clean:
	@rm -rf $(BIN_DIR) $(LIB_DIR)
	@rm -f $(CGO_TMP)
