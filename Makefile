asan:
	LSAN_OPTIONS=suppressions=asan.suppr ./build/Debug/hello &> asan.out

debug:
	./build/Debug/hello

stats:
	cloc src
