all: compile checkstyle test benchmark

clean:
	rm -rf *build*

build:
	cmake -S . -B build

compile: build
	cmake --build build

checkstyle:

test: build
	cmake --build build --target test

benchmark: build
	cmake --build build --target benchmarks

perf: build
	cmake --build build --target osm2ttl
	for FILE in $(shell ls -Sr input); do echo $${FILE}; time perf record ./build/apps/osm2ttl "./input/$${FILE}" -o "/tmp/$${FILE}.qlever" --use-ram-for-locations; perf report; done

docker-fr:
	mkdir input || true
	mkdir output || true
	wharfer build -t osm2ttl .
	wharfer run --rm -v `pwd`/input/:/input/ -v `pwd`/output/:/output/ -it osm2ttl /input/freiburg-regbez-latest.osm.pbf -o /output/freiburg-regbez-latest.osm.ttl --use-ram-for-locations

docker-bw:
	mkdir input || true
	mkdir output || true
	wharfer build -t osm2ttl .
	wharfer run --rm -v `pwd`/input/:/input/ -v `pwd`/output/:/output/ -it osm2ttl /input/baden-wuerttemberg-latest.osm.pbf -o /output/baden-wuerttemberg-latest.osm.ttl --use-ram-for-locations

docker-de:
	mkdir input || true
	mkdir output || true
	wharfer build -t osm2ttl .
	wharfer run --rm -v `pwd`/input/:/input/ -v `pwd`/output/:/output/ -it osm2ttl /input/germany-latest.osm.pbf -o /output/germany-latest.osm.ttl --use-ram-for-locations
