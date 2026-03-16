#!/usr/bin/env bash
# Build the extension and run the test suite.
# Expected to run inside a php-minimal (Alpine) container or Ubuntu from the repo root.
# Env vars:
#   LIBARCHIVE_VERSION  "system" (default) or a version tag like "v3.7.0".
#                       "system" installs the distro package; a version tag
#                       builds libarchive from source into .libarchive-cache/
#                       inside the workspace (cached by the CI workflow).
set -euo pipefail

LIBARCHIVE_VERSION="${LIBARCHIVE_VERSION:-system}"

# ── Install / build libarchive ────────────────────────────────────────────────

if [ "$LIBARCHIVE_VERSION" = "system" ]; then
    if command -v apk >/dev/null 2>&1; then
        apk add --no-cache libarchive-dev
    else
        # Non-root Ubuntu runner: use sudo.
        _sudo=""
        [ "$(id -u)" != "0" ] && _sudo="sudo"
        $_sudo apt-get update -qq
        $_sudo apt-get install -y -qq libarchive-dev
    fi
    CONFIGURE_LIBARCHIVE="--with-libarchive"
else
    # Build libarchive from source at the requested version.
    # The workflow caches ${GITHUB_WORKSPACE}/.libarchive-cache so subsequent
    # runs restore it and skip the build.
    LA_VERSION_NUM="${LIBARCHIVE_VERSION#v}"
    LA_PREFIX="${GITHUB_WORKSPACE}/.libarchive-cache"

    if [ ! -f "${LA_PREFIX}/lib/libarchive.so" ] && [ ! -f "${LA_PREFIX}/lib/libarchive.a" ]; then
        if command -v apk >/dev/null 2>&1; then
            apk add --no-cache build-base curl zlib-dev bzip2-dev xz-dev zstd-dev lz4-dev openssl-dev expat-dev
        else
            _sudo=""
            [ "$(id -u)" != "0" ] && _sudo="sudo"
            $_sudo apt-get update -qq
            $_sudo apt-get install -y -qq build-essential curl zlib1g-dev libbz2-dev liblzma-dev libzstd-dev liblz4-dev libssl-dev libexpat1-dev
        fi

        BUILD_DIR="$(mktemp -d)"
        curl -fsSL "https://github.com/libarchive/libarchive/releases/download/${LIBARCHIVE_VERSION}/libarchive-${LA_VERSION_NUM}.tar.gz" \
            | tar xz -C "$BUILD_DIR"
        cd "${BUILD_DIR}/libarchive-${LA_VERSION_NUM}"
        ./configure --prefix="${LA_PREFIX}" --disable-bsdtar --disable-bsdcat --disable-bsdcpio --disable-bsdunzip --without-xml2 --with-expat
        make -j"$(nproc)"
        make install
        cd -
        rm -rf "$BUILD_DIR"
    fi

    export LD_LIBRARY_PATH="${LA_PREFIX}/lib${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}"
    CONFIGURE_LIBARCHIVE="--with-libarchive=${LA_PREFIX}"
fi

# ── Build ─────────────────────────────────────────────────────────────────────

# Clean up all generated files so stale objects from a previous PHP version or
# build variant don't silently survive into this build.
if [ -f Makefile ]; then
    make -f Makefile distclean
fi
# Remove any leftover object files even if Makefile is gone.
find . -name '*.lo' -o -name '*.o' | xargs rm -f 2>/dev/null || true
find . -name '.libs' -type d | xargs rm -rf 2>/dev/null || true

phpize
./configure --with-php-config="$(which php-config)" "$CONFIGURE_LIBARCHIVE" ${EXTRA_CONFIGURE_FLAGS:-}
make -f Makefile -j"$(nproc)"

# ── Test ──────────────────────────────────────────────────────────────────────

# The generated Makefile silences and ignores errors on the `if` commands;
# undo that so test failures surface properly.
sed -i 's/-@if/@if/' Makefile

ret=0
TEST_PHP_EXECUTABLE="$(which php)" \
TEST_PHP_JUNIT=report.xml \
REPORT_EXIT_STATUS=1 \
NO_INTERACTION=1 \
TESTS="--set-timeout 300 --show-diff" \
    make -f Makefile test || ret=$?

found=$(find tests -name '*.mem' | wc -l)
if [ "$found" -gt 0 ]; then
    echo "Found $found memory leak(s):"
    find tests -name '*.mem' -print -exec cat {} \;
    ret=1
fi

exit "$ret"
