ARG PHP_VERSION=8.3
ARG PHP_TYPE=alpine
ARG BASE_IMAGE=php:${PHP_VERSION}-${PHP_TYPE}
ARG LIBMUSTACHE_VERSION

# image0
FROM ${BASE_IMAGE}
ARG LIBMUSTACHE_VERSION
WORKDIR /build

RUN test -n "${LIBMUSTACHE_VERSION}"

RUN apk update && \
    apk --no-cache add alpine-sdk automake autoconf libtool

# libmustache
WORKDIR /build/libmustache
RUN git init && \
    git remote add origin https://github.com/jbboehr/libmustache.git && \
    git fetch --depth=1 origin "${LIBMUSTACHE_VERSION}" && \
    git checkout --detach FETCH_HEAD
RUN autoreconf -fiv
RUN ./configure \
        --prefix /usr/local/ \
        --enable-static \
        --disable-shared \
        --without-json \
        --without-mustache-spec \
        --without-yaml \
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
RUN apk --no-cache add libstdc++
COPY --from=0 /usr/local/lib/php/extensions /usr/local/lib/php/extensions
RUN docker-php-ext-enable mustache
ENTRYPOINT ["docker-php-entrypoint"]
