# PS1 Development Environment
This repository provides a Docker-based development environment for PlayStation 1 (PS1) game development. It includes tools and libraries necessary for building PS1 games, along with a Makefile to streamline the build process.

<p align="center">
  <img src=".github/screenshot1.png" alt="PS1 Development Environment" width="600">
</p>

## Dependencies
- Docker
- Make
- PS1 emulator (e.g., DuckStation)

## How to build
To build the project, follow these steps:
1. Clone the repository:
```bash
git clone https://github.com/marconvcm/ps1_dev.git
cd ps1_dev
```
2. Build the Docker image:
```bash
make prepare
```

3. Build game:
```bash
make start
```
