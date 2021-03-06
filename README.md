# Szkolenie - Zaawansowane programowanie w C++ #

## Linki

* https://www.youtube.com/watch?v=2olsGf6JIkU&ab_channel=CppCon

## Libs
 * Menadżer pakietów: vcpkg
 * fmt - formatowanie stringów - https://github.com/fmtlib/fmt
 * folly - Facebook Open Library - https://github.com/facebook/folly
 * abseil - Google basic library
 * boost - https://www.boost.org/doc/libs/

## Dokumentacja + slajdy

* https://infotraining.bitbucket.io/cpp-adv
* https://gitpitch.com/infotraining-dev/cpp-adv/develop?grs=bitbucket&p=slides_pl#/
* https://infotraining.bitbucket.io/cpp-stl/iteratory.html
* https://learning.oreilly.com/library/view/discovering-modern-c/9780134383682/

## Books

* https://learning.oreilly.com/library/view/the-c-standard/9780132978286/
* https://learning.oreilly.com/library/view/c-concurrency-in/9781617294693/
* https://learning.oreilly.com/library/view/designing-audio-effect/9780240825151/ - Designing Audio Effect Plug-Ins in C++

## Zdalna maszyna wirtualna (SSH)

* link do rejestracji: https://labs.azure.com/register/rrteny9y
  - uczestnik powinien mieć konto Github lub Microsoft
  - ustawić swoje hasło przy rejestracji

## Konfiguracja lokalna - toolchain

Jeśli uczestnik chce skonfigurować lokalnie swoje środowisko, to należy zainstalować:

### Kompilator + CMake

* Dowolny kompilator C++ (gcc, clang, Visual C++) wspierający C++17
  * polecany kompilator pod Windows (https://nuwen.net/mingw.html) - instalacja polega na rozpakowaniu archiwum do katalogu C:\MinGW. Szczegółowy opis instalacji znajduje się na stronie.

* [CMake > 3.15](https://cmake.org/)
  * proszę sprawdzić wersję w lini poleceń

  ```
  cmake --version
  ```

## Visual Studio Code

* [Visual Studio Code](https://code.visualstudio.com/)

### Zainstalować wtyczki C++

* C/C++
* CMakeTools

### Zainstalować wtyczki do pracy zdalnej

* Remote - SSH
  - opis pracy z wtyczką: https://code.visualstudio.com/docs/remote/ssh
  - UWAGA! Windows 10 - [wymagana jest instalacja klienta OpenSSH lub Git for Windows (wcześniejsze wersje Windows)](https://code.visualstudio.com/docs/remote/troubleshooting#_installing-a-supported-ssh-client)
  - sprawdzić, czy klient SSH jest dostępny w terminalu (dodany do zmiennej PATH)
  - przy połączeniu z maszyną zdalną należy kliknąć na link **details** w pop-upie w prawym dolnym rogu. Umożliwi to wprowadzenie ustawionego wcześniej hasła.

* Live Share

* W przypadku pracy ze zdalną maszyna przez SSH należy za(re)instalować wtyczki C++ po połączeniu z maszyną.