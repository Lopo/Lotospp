Lotos2 (Lopo's Talker Object System)
====================================

A telnet-based talker server, cross-platform, written in C++.

If you don't know what a talker is, read http://en.wikipedia.org/wiki/Talker .


Dependencies
============

- cmake
- Boost libraries (http://www.boost.org)
  - boost
  - boost-program-options
  - boost-system
  - boost-filesystem
  - boost-thread
  - boost-date-time
  - boost-random
  - boost-log
- MySQL
- make


Build
=====

	cmake . && make


Used source codes
=================

- NUTS-IV (http://www.ogham.demon.co.uk/nuts4.html)
- Open Tibia Server (https://github.com/opentibia/server)
  - base network
- Licq (https://github.com/licq-im/licq)
  - Cmake things
- MySQL (https://dev.mysql.com/downloads)
  - Cmake module
- Pushy (https://github.com/godexsoft/pushy)
  - logging
