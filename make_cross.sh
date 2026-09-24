#!/bin/bash
# make_cross.sh - Make GCC Cross Compiler
# NOTE: This currently only works for RPM-based Linux distributions like Fedora.
# NOTE: This is also hardcoded to target the x86_64 Architecture, so this'll change if I ever decide to target other architectures.

set -euo pipefail

if (($EUID != 0)); then
	echo "This script must be ran as root or using sudo." >&2
	exit 1
fi

# --- Configuration ---
export PREFIX="/opt/cross"
export TARGET=x86_64-elf
export PATH="$PREFIX/bin:$PATH"
JOBS=$(nproc)

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

# Preparing for the Build
mkdir -p toolchain
cd toolchain

echo "Installing dependancies..."
sudo dnf install -y gcc gcc-c++ git gpg make bison flex gmp-devel libmpc-devel mpfr-devel texinfo wget

echo "Downloading GNU Keyring..."
wget -nc "${GNU_FTP}/gnu-keyring.gpg"

echo "Downloading Source Code for Binutils and GCC..."
# --- binutils ---
wget -nc "${GNU_FTP}/${BINUTILS_TAR}"
wget -nc "${GNU_FTP}/${BINUTILS_TAR}.sig"

# --- gcc ---
wget -nc "${GNU_FTP}/gcc/${GCC_DIR}/${GCC_TAR}"
wget -nc "${GNU_FTP}/gcc/${GCC_DIR}/${GCC_TAR}.sig"

echo "Verifying signatures..."
gpg --no-default-keyring --keyring gnu-keyring.gpg \
    --verify "${BINUTILS_TAR}.sig" "${BINUTILS_TAR}"

gpg --no-default-keyring --keyring gnu-keyring.gpg \
    --verify "${GCC_TAR}.sig" "${GCC_TAR}"

# Final Preparations
mkdir -p "$PREFIX"
echo "Preparations complete. Beginning build."

# The Build
echo "Extracing source archives..."
tar -xf "${BINUTILS_TAR}"
tar -xf "${GCC_TAR}"

# --- build binutils ---
echo "Building binutils..."

mkdir -p build-binutils
cd build-binutils
"../${BINUTILS_DIR}/configure" --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror --enable-default-execstack=no
make -j "$JOBS"
make install
cd ..

# --- Patch GCC for a red-zone free libgcc multilib ---
echo "Patching GCC config for no-red-zone libgcc..."

GCC_CONFIG="${GCC_DIR}/gcc/config.gcc"
TMAKE_FRAGMENT="${GCC_DIR}/gcc/config/i386/t-x86_64-elf"

# Create a new Makefile fragment
cat > "$TMAKE_FRAGMENT" << 'EOF'
MULTILIB_OPTIONS += mno-red-zone
MULTILIB_DIRNAMES += no-red-zone
EOF

# Point config.gcc at TMAKE_FRAGMENT
if ! grep -q "t-x86_64-elf" "$GCC_CONFIG"; then
    sed -i '/^x86_64-\*-elf\*/a\    tmake_file="${tmake_file} i386/t-x86_64-elf"' "$GCC_CONFIG"
fi

# --- Build GCC (C only) + libgcc ---
echo "Building GCC and libgcc..."
mkdir -p build-gcc
cd build-gcc
"../${GCC_DIR}/configure" --target="$TARGET" --prefix="$PREFIX" \
    --disable-nls --enable-languages=c --without-headers        \
    --disable-hosted-libstdcxx
make -j "$JOBS" all-gcc
make -j "$JOBS" all-target-libgcc
make install-gcc
make install-target-libgcc
cd ..

echo "$TARGET Cross compiler built at $PREFIX/bin"
exit 0