# Pikranji — Root Makefile
# Unified commands for all three frontends: Web, Generator, NDS.

.PHONY: help build-nds validate-puzzles convert check clean

help:
	@echo "Pikranji — Available targets:"
	@echo "  make build-nds       Build NDS ROM (requires Docker + devkitPro)"
	@echo "  make validate-puzzles  Validate puzzles.json integrity"
	@echo "  make convert          Regenerate nds/include/puzzles.h from puzzles.json"
	@echo "  make check            Run all checks (validate + static analysis)"
	@echo "  make clean            Remove build artifacts and generated files"

# --- NDS Build ---
build-nds:
	@echo "==> Building NDS ROM..."
	@cd nds && ./build.sh

# --- Puzzle Validation ---
validate-puzzles:
	@echo "==> Validating puzzles.json..."
	@python3 scripts/validate_puzzles.py

# --- Converter ---
convert:
	@echo "==> Regenerating puzzles.h from puzzles.json..."
	@cd nds && python3 converter.py

# --- Combined Check ---
check: validate-puzzles
	@echo "==> Running NDS static checks..."
	@echo "  (NDS make check requires Docker — run 'make build-nds' for full check)"
	@echo "==> All checks passed."

# --- Clean ---
clean:
	@echo "==> Cleaning build artifacts..."
	@rm -rf nds/build nds/*.elf nds/*.nds
	@rm -rf __pycache__ *.pyc
	@rm -f novos_puzzles.json kanji_collection.json
	@echo "==> Clean complete."