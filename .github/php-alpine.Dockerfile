ARG PHP_VERSION=7.4
ARG PHP_TYPE=alpine
ARG BASE_IMAGE=php:${PHP_VERSION}-${PHP_TYPE}
ARG LIBMUSTACHE_VERSION=master

# image0
FROM ${BASE_IMAGE}
ARG LIBMUSTACHE_VERSION
WORKDIR /build

RUN apk update && \
    apk --no-cache add alpine-sdk automake autoconf libtool json-c-dev yaml-dev

# libmustache
RUN git clone https://github.com/jbboehr/libmustache.git
WORKDIR /build/libmustache
RUN git checkout $LIBMUSTACHE_VERSION && git submodule update --init
RUN autoreconf -fiv
RUN ./configure \
        --prefix /usr/local/ \
        --enable-static \
        --disable-shared \
        CXXFLAGS="-O3 -fPIC -DPIC -flto" \
        RANLIB=gcc-ranlib \
        AR=gcc-ar \
        NM=gcc-nm \
        LD=gcc
RUN make
RUN make install

# php-mustache
WORKDIR /build/php-mustache
ADD . .
RUN phpize
RUN ./configure CXXFLAGS="-O3 -fPIC -DPIC"
RUN make
RUN make install

# image1
FROM ${BASE_IMAGE}
RUN apk --no-cache add json-c yaml libstdc++
COPY --from=0 /usr/local/lib/php/extensions /usr/local/lib/php/extensions
RUN docker-php-ext-enable mustache
ENTRYPOINT ["docker-php-entrypoint"]
