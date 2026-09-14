#!/bin/bash
# make_cross.sh - Make GCC Cross Compiler
# NOTE: This currently only works for RPM-based Linux distributions like Fedora.
set -euo pipefail

if (($EUID != 0)); then
	echo "This script must be ran as root or using sudo." >&2
	exit 1
fi

# Versions
GCC_VER="16.2.0"
BINUTILS_VER="2.47"

# Directories
GCC_DIR="gcc-${GCC_VER}"
BINUTILS_DIR="binutils-${BINUTILS_VER}"

# Filenames
GCC_TAR="${GCC_DIR}.tar.gz"
BINUTILS_TAR="${BINUTILS_DIR}.tar.gz"

# Base URLs
GNU_FTP="https://ftp.gnu.org/gnu"


echo "Installing GCC dependancies..."
sudo dnf install -y gcc gcc-c++ git gpg make bison flex gmp-devel libmpc-devel mpfr-devel texinfo wget

echo "Downloading GNU Keyring"
wget -nc "${GNU_FTP}/gnu-keyring.gpg"

echo "Downloading Source Code for Binutils + GCC..."
# --- binutils ---
wget -nc "${GNU_FTP}/${BINUTILS_TAR}"
wget -nc "${GNU_FTP}/${BINUTILS_TAR}.sig"

# --- gcc ---
wget -nc "${GNU_FTP}/gcc/${GCC_DIR}/${GCC_TAR}"
wget -nc "${GNU_FTP}/gcc/${GCC_DIR}/${GCC_TAR}.sig"

echo "Verifying signatures"
gpg --no-default-keyring --keyring gnu-keyring.gpg \
    --verify "${BINUTILS_TAR}.sig" "${BINUTILS_TAR}"

gpg --no-default-keyring --keyring gnu-keyring.gpg \
    --verify "${GCC_TAR}.sig" "${GCC_TAR}"

# incomplete
exit 0
