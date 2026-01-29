# С++ библиотека для работы с справочником сотрудников и их зарплатами

## Описание

Библиотека предоставляет ряд функций по:

- добавлению/удалению/поиску сотрудников

- добавлению/удалению отношений подчинения между сотрудниками

- поиску начальника конкретного сотрудника

- поиску прямых подчиненных/всех подчиненных конкретного сотрудника

- расчету зарплат (суммарно по всем сотрудникам, по определенной категории сотрудников, по конкретному сотруднику)

## Сборка

Сборка происходит с помощью утилиты CMake. Команды:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release # конфигурация
cmake --build build --parallel 8               # сборка
```

Недостаточно просто скачать репозиторий и запустить сборку, нужен ряд dev-зависимостей.

Зависимости deb:

- build-essential

- make

- cmake

- libboost-all-dev

- libgtest-dev

Зависимости rpm:

- libstdc++-static

- gcc-c++

- make

- cmake

- boost-devel

- gtest-devel

- libasan

- libubsan

## Тестирование

Для тестирования тоже используется утилита cmake (ctest)

Тесты создаются по пути `build/test`

Чтобы запустить тесты, достаточно воспользоваться командами:

```bash
cd build/test
ctest
```

## Поставка

Библиотека поставляется как динамически линкуемый файл (so - shared object).

Если нет возможности установить ряд перечисленных зависиимостей, то можно воспользоваться технологией docker для сборки so-файла под deb-подобный или rpm-подобный дистрибутивы:

```bash
# Команды сборки образов с помощью docker
docker build -f docker/rpm.dockerfile -t rpm_image . # для rpm
docker build -f docker/deb.dockerfile -t deb_image . # для deb
```

После успешного создания образов можно создать контейнер на их базе и скопировать бинарный файл оттуда:

```bash
docker create --name rpm_container rpm_image
docker cp rpm_container:/usr/src/app/build/src/libemployee-lib.so .

docker create --name deb_container deb_image
docker cp deb_container:/usr/src/app/build/src/libemployee-lib.so .
```