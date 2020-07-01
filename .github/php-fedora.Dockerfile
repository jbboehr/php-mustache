
ARG BASE_IMAGE=fedora:latest
ARG LIBMUSTACHE_VERSION=master

# image0
FROM ${BASE_IMAGE}
ARG LIBMUSTACHE_VERSION
WORKDIR /build

RUN dnf groupinstall 'Development Tools' -y
RUN dnf install \
    gcc \
    automake \
    autoconf \
    libtool \
    libyaml-devel \
    json-c-devel \
    php-devel \
    -y

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
RUN dnf install php-cli -y
# this probably won't work on other arches
COPY --from=0 /usr/lib64/php/modules/mustache.so /usr/lib64/php/modules/mustache.so
# please forgive me
COPY --from=0 /usr/lib64/php/build/run-tests.php /usr/local/lib/php/build/run-tests.php
RUN echo extension=mustache.so | sudo tee /etc/php.d/90-mustache.ini
