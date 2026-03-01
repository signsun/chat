all:
	@if [ -d "build" ]; then \
		cd build && make -j2; \
	else \
		mkdir build; \
		cd build && cmake .. && make -j2; \
	fi

doc:
	@doxygen Doxyfile

%:
	@if [ -d "build" ]; then \
		cd build && make $@ -j2; \
	else \
		mkdir build; \
		cd build && cmake .. && make $@ -j2; \
	fi

.PHONY: clean distclean
clean:
	@if [ -d "build" ]; then \
		cd build && make clean;\
	fi

distclean:
	rm build -fr
	rm lib -fr
	rm html -fr