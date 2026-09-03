
# Fedora 44's GCC 16 currently breaks libmustache's Cista type-version witness.
ARG BASE_IMAGE=fedora:43
ARG LIBMUSTACHE_VERSION

# image0
FROM ${BASE_IMAGE}
ARG LIBMUSTACHE_VERSION
WORKDIR /build

RUN test -n "${LIBMUSTACHE_VERSION}"

RUN dnf install \
    autoconf \
    automake \
    diffutils \
    gcc \
    gcc-c++ \
    git \
    libtool \
    m4 \
    make \
    php-devel \
    pkgconf-m4 \
    pkgconf-pkg-config \
    -y

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
RUN dnf install php-cli -y
# this probably won't work on other arches
COPY --from=0 /usr/lib64/php/modules/mustache.so /usr/lib64/php/modules/mustache.so
COPY --from=0 /usr/lib64/php/build/run-tests.php /usr/local/lib/php/build/run-tests.php
RUN printf '%s\n' 'extension=mustache.so' > /etc/php.d/90-mustache.ini
