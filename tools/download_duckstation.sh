#!/bin/bash
set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
TARGET_DIR="${ROOT_DIR}/.duckstation"

mkdir -p "${TARGET_DIR}"

# Determine target OS
TARGET_OS="$1"

if [ -z "${TARGET_OS}" ]; then
    case "$(uname -s)" in
        Linux*)  TARGET_OS="linux" ;;
        Darwin*) TARGET_OS="mac" ;;
        CYGWIN*|MINGW*|MSYS*) TARGET_OS="windows" ;;
        *)
            echo "Unknown operating system: $(uname -s)"
            echo "Usage: $0 [linux|mac|windows]"
            exit 1
            ;;
    esac
fi

# Normalize OS argument
TARGET_OS="$(echo "${TARGET_OS}" | tr '[:upper:]' '[:lower:]')"

# Determine CPU architecture
ARCH="$(uname -m)"
case "${ARCH}" in
    x86_64|amd64) ARCH_TYPE="x64" ;;
    aarch64|arm64) ARCH_TYPE="arm64" ;;
    *) ARCH_TYPE="x64" ;;
esac

DOWNLOADER=""
if command -v curl >/dev/null 2>&1; then
    DOWNLOADER="curl -fL --progress-bar -o"
elif command -v wget >/dev/null 2>&1; then
    DOWNLOADER="wget -q --show-progress -O"
else
    echo "Error: Neither curl nor wget was found."
    exit 1
fi

echo "=== DuckStation Downloader ==="
echo "Target OS:   ${TARGET_OS}"
echo "Arch:        ${ARCH_TYPE}"
echo "Install Dir: ${TARGET_DIR}"
echo "=============================="

case "${TARGET_OS}" in
    linux)
        if [ "${ARCH_TYPE}" = "arm64" ]; then
            FILE_NAME="DuckStation-arm64.AppImage"
        else
            FILE_NAME="DuckStation-x64.AppImage"
        fi
        URL="https://github.com/stenzek/duckstation/releases/download/latest/${FILE_NAME}"
        DEST="${TARGET_DIR}/${FILE_NAME}"

        echo "Downloading ${URL}..."
        ${DOWNLOADER} "${DEST}" "${URL}"
        chmod +x "${DEST}"

        # Create symlink
        ln -sf "${FILE_NAME}" "${TARGET_DIR}/duckstation"
        echo "DuckStation AppImage downloaded successfully to ${DEST}"
        echo "Symlink created at ${TARGET_DIR}/duckstation"
        ;;

    mac|macos|darwin)
        FILE_NAME="duckstation-mac-release.zip"
        URL="https://github.com/stenzek/duckstation/releases/download/latest/${FILE_NAME}"
        DEST="${TARGET_DIR}/${FILE_NAME}"

        echo "Downloading ${URL}..."
        ${DOWNLOADER} "${DEST}" "${URL}"

        echo "Extracting ${DEST}..."
        if command -v unzip >/dev/null 2>&1; then
            unzip -qo "${DEST}" -d "${TARGET_DIR}"
            rm -f "${DEST}"
            echo "DuckStation extracted to ${TARGET_DIR}/DuckStation.app"
        else
            echo "Warning: unzip utility not found. The zip archive was saved to ${DEST}"
        fi
        ;;

    windows|win)
        if [ "${ARCH_TYPE}" = "arm64" ]; then
            FILE_NAME="duckstation-windows-arm64-release.zip"
        else
            FILE_NAME="duckstation-windows-x64-release.zip"
        fi
        URL="https://github.com/stenzek/duckstation/releases/download/latest/${FILE_NAME}"
        DEST="${TARGET_DIR}/${FILE_NAME}"

        echo "Downloading ${URL}..."
        ${DOWNLOADER} "${DEST}" "${URL}"

        echo "Extracting ${DEST}..."
        if command -v unzip >/dev/null 2>&1; then
            unzip -qo "${DEST}" -d "${TARGET_DIR}"
            rm -f "${DEST}"
            echo "DuckStation extracted to ${TARGET_DIR}"
        else
            echo "Zip archive saved to ${DEST}. Please unzip it in ${TARGET_DIR}"
        fi
        ;;

    *)
        echo "Error: Unsupported target OS: ${TARGET_OS}"
        echo "Valid options: linux, mac, windows"
        exit 1
        ;;
esac

echo "Done! DuckStation is ready in ${TARGET_DIR}"
