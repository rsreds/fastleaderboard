FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    libssl-dev \
    wget \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /tmp
RUN wget https://archives.boost.io/release/1.90.0/source/boost_1_90_0.tar.bz2
RUN tar xf boost_1_90_0.tar.bz2
RUN cd boost_1_90_0 && ./bootstrap.sh --with-libraries=json
RUN cd boost_1_90_0 && ./b2 install -j$(nproc)
RUN rm -rf boost_1_90_0*

WORKDIR /app
COPY . .

RUN cmake -B build -DCMAKE_BUILD_TYPE=Release
RUN cmake --build build -j$(nproc)

EXPOSE 8080

CMD ./build/fastleaderboard ${PORT:-8080} ${THREADS:-4}
