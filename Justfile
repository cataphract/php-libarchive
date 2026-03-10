# Build and test inside the matching CI Docker image.
# Image SHAs are read from .github/docker-image-shas.yml (multi-arch OCI index digests).
# To refresh SHAs: .github/scripts/update-docker-shas.sh

# ── Images ────────────────────────────────────────────────────────────────────

_base := "datadog/dd-appsec-php-ci@"

image_8_0_debug       := _base + `grep 'php-8.0-debug:'       .github/docker-image-shas.yml | cut -d'"' -f2`
image_8_0_release_zts := _base + `grep 'php-8.0-release-zts:' .github/docker-image-shas.yml | cut -d'"' -f2`
image_8_1_debug       := _base + `grep 'php-8.1-debug:'       .github/docker-image-shas.yml | cut -d'"' -f2`
image_8_1_release_zts := _base + `grep 'php-8.1-release-zts:' .github/docker-image-shas.yml | cut -d'"' -f2`
image_8_2_debug       := _base + `grep 'php-8.2-debug:'       .github/docker-image-shas.yml | cut -d'"' -f2`
image_8_2_release_zts := _base + `grep 'php-8.2-release-zts:' .github/docker-image-shas.yml | cut -d'"' -f2`
image_8_3_debug       := _base + `grep 'php-8.3-debug:'       .github/docker-image-shas.yml | cut -d'"' -f2`
image_8_3_release_zts := _base + `grep 'php-8.3-release-zts:' .github/docker-image-shas.yml | cut -d'"' -f2`
image_8_4_debug       := _base + `grep 'php-8.4-debug:'       .github/docker-image-shas.yml | cut -d'"' -f2`
image_8_4_release_zts := _base + `grep 'php-8.4-release-zts:' .github/docker-image-shas.yml | cut -d'"' -f2`
image_8_5_debug       := _base + `grep 'php-8.5-debug:'       .github/docker-image-shas.yml | cut -d'"' -f2`
image_8_5_release_zts := _base + `grep 'php-8.5-release-zts:' .github/docker-image-shas.yml | cut -d'"' -f2`

_run := "docker run --rm --entrypoint bash -v \"$PWD:/src:ro\" --tmpfs /overlay:exec,size=1g --cap-add SYS_ADMIN --security-opt apparmor=unconfined -w /workspace --user root"
_cmd := "-c 'mkdir -p /overlay/upper /overlay/work /workspace && mount -t overlay overlay -o lowerdir=/src,upperdir=/overlay/upper,workdir=/overlay/work /workspace && cd /workspace && .github/scripts/build-and-test.sh'"

# ── Default ───────────────────────────────────────────────────────────────────

default:
    @just --list

# ── Formatting ────────────────────────────────────────────────────────────────

_clang_format := "/opt/homebrew/opt/llvm@21/bin/clang-format"

# Format C source files with clang-format 21
format:
    {{_clang_format}} -i libarchive.c stream.c php_libarchive.h php_compat.h

# ── Stub generation ───────────────────────────────────────────────────────────

# Regenerate libarchive_arginfo.h from libarchive.stub.php
gen-arginfo:
    php "$(php-config --prefix)/lib/php/build/gen_stub.php" libarchive.stub.php

# ── Individual targets ────────────────────────────────────────────────────────

test-8_0-debug:
    {{_run}} {{image_8_0_debug}} {{_cmd}}
test-8_0-release-zts:
    {{_run}} {{image_8_0_release_zts}} {{_cmd}}

test-8_1-debug:
    {{_run}} {{image_8_1_debug}} {{_cmd}}
test-8_1-release-zts:
    {{_run}} {{image_8_1_release_zts}} {{_cmd}}

test-8_2-debug:
    {{_run}} {{image_8_2_debug}} {{_cmd}}
test-8_2-release-zts:
    {{_run}} {{image_8_2_release_zts}} {{_cmd}}

test-8_3-debug:
    {{_run}} {{image_8_3_debug}} {{_cmd}}
test-8_3-release-zts:
    {{_run}} {{image_8_3_release_zts}} {{_cmd}}

test-8_4-debug:
    {{_run}} {{image_8_4_debug}} {{_cmd}}
test-8_4-release-zts:
    {{_run}} {{image_8_4_release_zts}} {{_cmd}}

test-8_5-debug:
    {{_run}} {{image_8_5_debug}} {{_cmd}}
test-8_5-release-zts:
    {{_run}} {{image_8_5_release_zts}} {{_cmd}}

# ── Per-version aggregates (sequential to avoid workspace conflicts) ───────────

test-8_0: test-8_0-debug test-8_0-release-zts
test-8_1: test-8_1-debug test-8_1-release-zts
test-8_2: test-8_2-debug test-8_2-release-zts
test-8_3: test-8_3-debug test-8_3-release-zts
test-8_4: test-8_4-debug test-8_4-release-zts
test-8_5: test-8_5-debug test-8_5-release-zts

# ── All Linux targets ─────────────────────────────────────────────────────────

test-linux: test-8_0 test-8_1 test-8_2 test-8_3 test-8_4 test-8_5
