FROM gcc:13

WORKDIR /app

RUN apt-get update && apt-get install -y --no-install-recommends \
    cmake \
    libssl-dev \
    && rm -rf /var/lib/apt/lists/*

COPY . .

# Build the manifest_foundation library (risk math, regime filter, entry validation)
RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release \
    && cmake --build build --config Release -j"$(nproc)"

EXPOSE 9000

# TODO: expose C++ engine functions via a small Python/FastAPI sidecar or gRPC
CMD ["echo", "C++ engine built. Expose risk/regime functions via bridge service."]
