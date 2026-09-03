
ARG PHP_VERSION=8.3
ARG BASE_IMAGE=php:$PHP_VERSION
ARG LIBMUSTACHE_VERSION

# image0
FROM ${BASE_IMAGE}
ARG LIBMUSTACHE_VERSION
WORKDIR /build

RUN test -n "${LIBMUSTACHE_VERSION}"

RUN apt-get update && apt-get install -y --no-install-recommends \
        autoconf \
        automake \
        g++ \
        gcc \
        git \
        libtool \
        m4 \
        make \
        pkg-config

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
COPY --from=0 /usr/local/lib/php/extensions /usr/local/lib/php/extensions
RUN docker-php-ext-enable mustache
ENTRYPOINT ["docker-php-entrypoint"]
