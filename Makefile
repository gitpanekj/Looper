.PHONY: build_modules, clean_modules, test, install

build_modules:
	cd src/build && make

clean_modules:
	cd src/build && make clean

test:
	looper --file tests/del.c

install:
	pip install --editable .

