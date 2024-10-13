# use ubuntu as base image
FROM joshferrara/docker-mingw-w64:latest

LABEL author="Sergio Padrino (@sergiou87)"

# install dependencies
RUN apt-get update \
   && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends unzip
#   && pip3 install --break-system-packages mingw_ldd

RUN wget https://github.com/libsdl-org/SDL/releases/download/release-2.30.8/SDL2-devel-2.30.8-mingw.zip \
  && unzip SDL2-devel-2.30.8-mingw.zip \
  && cd SDL2-2.30.8 \
  && make cross

RUN wget https://github.com/libsdl-org/SDL_mixer/releases/download/release-2.8.0/SDL2_mixer-devel-2.8.0-mingw.zip \
  && unzip SDL2_mixer-devel-2.8.0-mingw.zip \
  && cd SDL2_mixer-2.8.0 \
  && make cross

ENV CC="x86_64-w64-mingw32-gcc --sysroot=/usr/local/x86_64-w64-mingw32"
ENV CXX="x86_64-w64-mingw32-g++ --sysroot=/usr/local/x86_64-w64-mingw32"
ENV PATH="${PATH}:/usr/local/x86_64-w64-mingw32/bin"

# set working directory
WORKDIR /src
