help:
	echo "Must be used as superuser"
	echo ""
	echo "reinstall: Recompile and install all the files without making a new make file"
	echo "build: Recompile and install all the files"
	echo "clean-build: Removes build dir and then do a make build"

reinstall:
	cd build; \
	make install; \
	ldconfig; \

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