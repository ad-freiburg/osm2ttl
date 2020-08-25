FROM ubuntu:20.04

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y git clang clang-tidy g++ libboost-dev libexpat1-dev cmake libbz2-dev zlib1g-dev
COPY . /app/
RUN cd /app/ && make
ENTRYPOINT ["/app/build/apps/osm2ttl"]
