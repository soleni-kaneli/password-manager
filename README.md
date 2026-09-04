# Password Generator

DevOps проект: генератор на пароли, написан на C++, с пълен CI/CD пайплайн, контейнеризация и оркестрация чрез Kubernetes.

## Съдържание

- Описание
- Функционалност
- Структура на проекта
- Локално стартиране
- HTTP API
- Тестове
- Docker
- Kubernetes
- CI/CD пайплайн
- Технологии

## Описание

Проектът представлява генератор на случайни пароли с два режима на работа: команден ред (CLI) и HTTP услуга. Освен самата програма, проектът демонстрира пълен DevOps работен процес: контрол на версиите, автоматизирано тестване, статичен анализ на сигурността, сканиране за уязвимости, контейнеризация с multi-stage Docker build и deployment в Kubernetes.

## Функционалност

- Генериране на пароли с настройваема дължина
- Избор кои категории символи да се включат: главни букви, малки букви, цифри, символи
- Изключване на визуално объркващи символи (например 0, O, 1, l, I)
- CLI режим за еднократно генериране от терминал
- HTTP режим (serve) за генериране на пароли през мрежова заявка

## Структура на проекта

```
password-generator/
  include/
    generator.hpp       декларация на класа PasswordGenerator
    httplib.h            трета страна библиотека за HTTP сървър
  src/
    generator.cpp        логика на генератора
    main.cpp              входна точка, CLI и HTTP режим
  tests/
    test_generator.cpp   unit тестове с GoogleTest
  k8s/
    deployment.yaml       Kubernetes Deployment манифест
    service.yaml           Kubernetes Service манифест
  .github/workflows/
    ci.yml                  build и unit тестове
    security.yml            CodeQL и Trivy сканиране на файлова система
    docker-publish.yml      build, сканиране и качване на Docker образ
  CMakeLists.txt
  Dockerfile
  .dockerignore
  .gitignore
```

## Локално стартиране

Изисквания: CMake версия 3.14 или по-нова, компилатор с поддръжка на C++17, GoogleTest (само за тестовете).

Билдване на проекта:

```
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Генериране на парола от команден ред:

```
./build/password_generator -l 20 --no-symbols
```

Налични опции:

```
-l <брой>       дължина на паролата, по подразбиране 12
--no-upper       изключва главните букви
--no-lower       изключва малките букви
--no-digits      изключва цифрите
--no-symbols     изключва символите
-h                показва помощ
```

Стартиране на HTTP услугата:

```
./build/password_generator serve --port 8080
```

## HTTP API

При стартиране в режим serve, приложението излага следните два ендпойнта.

GET /healthz

Връща статус на услугата. Използва се от Kubernetes за liveness и readiness проверки.

```
curl http://localhost:8080/healthz
```

GET /generate

Генерира парола според подадените query параметри. Всички параметри са незадължителни.

Параметри: length, upper, lower, digits, symbols, excludeAmbiguous. Стойностите за upper, lower, digits, symbols се задават с true или false. Стойността за excludeAmbiguous се задава с true.

Примери:

```
curl "http://localhost:8080/generate?length=16"
curl "http://localhost:8080/generate?length=8&upper=false&symbols=false"
curl "http://localhost:8080/generate?length=24&excludeAmbiguous=true"
```

Отговорът е във формат JSON, например:

```
{"password":"kDu0nh0%HVPF"}
```

При невалидни параметри отговорът е с HTTP статус 400 и съдържа описание на грешката.

## Тестове

Проектът използва GoogleTest за unit тестване на логиката на генератора.

```
cmake -S . -B build -DBUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

Тестовете покриват коректна дължина на генерираната парола, спазване на избраните категории символи, изключване на объркващи символи, обработка на невалидни настройки и уникалност на генерираните пароли.

## Docker

Проектът използва multi-stage Dockerfile. Първият етап компилира приложението с помощта на CMake и компилатор, а вторият етап съдържа само готовия изпълним файл, без инструментите за компилация. Крайният образ работи с непривилегирован потребител.

Построяване на образа:

```
docker build -t password-generator .
```

Стартиране на контейнера:

```
docker run --rm -p 8080:8080 password-generator
```

По подразбиране контейнерът стартира HTTP услугата на порт 8080. За еднократно CLI генериране без стартиране на сървър:

```
docker run --rm password-generator -l 20
```

## Kubernetes

В папка k8s се намират манифестите за deployment на приложението в Kubernetes клъстер.

deployment.yaml описва Deployment с две реплики на приложението, ограничения на изразходваните ресурси, и liveness и readiness проверки чрез ендпойнта healthz.

service.yaml описва Service от тип ClusterIP, който насочва трафик от порт 80 към порт 8080 на подовете.

Прилагане на манифестите:

```
kubectl apply -f k8s/deployment.yaml
kubectl apply -f k8s/service.yaml
```

Проверка на състоянието:

```
kubectl get pods
kubectl get service password-generator
```

Локален достъп до услугата чрез port forward:

```
kubectl port-forward service/password-generator 8080:80
```

## CI/CD пайплайн

Проектът съдържа три отделни GitHub Actions workflow файла, разположени в .github/workflows.

ci.yml се изпълнява при push или pull request към main. Компилира проекта и изпълнява unit тестовете с GoogleTest. При провал на тест или грешка в компилацията, пайплайнът отбелязва грешка.

security.yml се изпълнява при push, pull request и веднъж седмично по разписание. Включва две отделни проверки: CodeQL анализ на изходния код за уязвимости, и Trivy сканиране на файловата система за уязвими зависимости. Резултатите се публикуват в раздел Security на репозиторито.

docker-publish.yml се изпълнява при push към main. Построява Docker образа, сканира го с Trivy за уязвимости, и качва образа в GitHub Container Registry с два тага: latest и хеша на конкретния commit.

## Технологии

Език: C++17

Билд система: CMake

Тестове: GoogleTest

HTTP сървър: cpp-httplib

Контрол на версиите: Git, GitHub

Автоматизация: GitHub Actions

Статичен анализ на сигурността: CodeQL

Сканиране за уязвимости: Trivy

Контейнеризация: Docker, multi-stage build

Оркестрация: Kubernetes

Регистър за Docker образи: GitHub Container Registry
