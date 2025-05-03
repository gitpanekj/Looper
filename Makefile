.PHONY: build_modules, clean_modules, test, install

build_modules:
	cd rebel/build && make

clean_modules:
	cd rebel/build && make clean

install:
	pip install --editable .

