FROM fastleaderboard-boost:1.90

WORKDIR /app
COPY . .

RUN cmake -B build -DCMAKE_BUILD_TYPE=Release
RUN cmake --build build -j$(nproc)

EXPOSE 8080

CMD ["/bin/bash", "-c", "./build/fastleaderboard ${PORT:-8080} ${THREADS:-4}"]
