# ---------- Stage 1: Build ----------
# Тежък образ с компилатор и CMake - нужен само за да компилираме кода
FROM ubuntu:24.04 AS builder

RUN apt-get update && \
    apt-get install -y --no-install-recommends cmake g++ make && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY CMakeLists.txt ./
COPY include/ ./include/
COPY src/ ./src/

# Билдваме само production кода - без тестовете (BUILD_TESTS остава OFF)
RUN cmake -S . -B build -DCMAKE_BUILD_TYPE=Release && \
    cmake --build build --config Release

# ---------- Stage 2: Runtime ----------
# Минимален образ - съдържа само това, което е нужно да СЕ ПУСНЕ програмата
FROM ubuntu:24.04 AS runtime

# Непривилегирован потребител - никога не пускаме процеси като root в контейнер
RUN useradd --no-create-home --shell /usr/sbin/nologin appuser

WORKDIR /app

# Копираме само готовия изпълним файл от builder етапа - нищо друго
COPY --from=builder /app/build/password_generator /app/password_generator

USER appuser

ENTRYPOINT ["/app/password_generator"]
# По подразбиране: парола с дължина 12 (може да се override-не при docker run)
CMD ["-l", "12"]
