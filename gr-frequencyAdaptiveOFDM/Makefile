help:
	echo "Must be used as superuser"
	echo ""
	echo "build: Recompile and install all the files"
	echo "clean-build: Removes build dir and then do a make build"

build:
	mkdir -p build;
	cd build; \
	cmake ..; \
	make ; \
	make install; \
	ldconfig; \

clean-build:
	rm -rf build;
	make build