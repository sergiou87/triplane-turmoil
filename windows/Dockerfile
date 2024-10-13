# use ubuntu as base image
FROM joshferrara/docker-mingw-w64:latest

LABEL author="Sergio Padrino (@sergiou87)"

# install dependencies
RUN apt-get update \
   && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends unzip

ENV DOCKER_SDL2_VERSION="2.30.8"
ENV DOCKER_SDL_MIXER_VERSION="2.8.0"

RUN wget https://github.com/libsdl-org/SDL/releases/download/release-${DOCKER_SDL2_VERSION}/SDL2-devel-${DOCKER_SDL2_VERSION}-mingw.zip \
  && unzip SDL2-devel-${DOCKER_SDL2_VERSION}-mingw.zip \
  && cd SDL2-${DOCKER_SDL2_VERSION} \
  && make cross

RUN wget https://github.com/libsdl-org/SDL_mixer/releases/download/release-${DOCKER_SDL_MIXER_VERSION}/SDL2_mixer-devel-${DOCKER_SDL_MIXER_VERSION}-mingw.zip \
  && unzip SDL2_mixer-devel-${DOCKER_SDL_MIXER_VERSION}-mingw.zip \
  && cd SDL2_mixer-${DOCKER_SDL_MIXER_VERSION} \
  && make cross

# Download runtime DLLS for SDL2 and SDL_mixer to include them with the zip release
RUN mkdir -p /usr/local/sdl-dlls \
  && wget https://github.com/libsdl-org/SDL/releases/download/release-${DOCKER_SDL2_VERSION}/SDL2-${DOCKER_SDL2_VERSION}-win32-x64.zip \
  && unzip SDL2-${DOCKER_SDL2_VERSION}-win32-x64.zip -d /usr/local/sdl-dlls

RUN wget https://github.com/libsdl-org/SDL_mixer/releases/download/release-${DOCKER_SDL_MIXER_VERSION}/SDL2_mixer-${DOCKER_SDL_MIXER_VERSION}-win32-x64.zip \
  && unzip SDL2_mixer-${DOCKER_SDL_MIXER_VERSION}-win32-x64.zip -d /usr/local/sdl-dlls \
  && ls -Rla /usr/local/sdl-dlls

ENV CC="x86_64-w64-mingw32-gcc --sysroot=/usr/local/x86_64-w64-mingw32"
ENV CXX="x86_64-w64-mingw32-g++ --sysroot=/usr/local/x86_64-w64-mingw32"
ENV PATH="${PATH}:/usr/local/x86_64-w64-mingw32/bin"

# set working directory
WORKDIR /src
