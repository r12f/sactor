#!/usr/bin/env just --justfile

COLOR_GREEN := '(ansi green)'
COLOR_BLUE := '(ansi blue)'
COLOR_RED := '(ansi r)'
COLOR_NONE := '(ansi reset)'

VERSION := '0.2.1'

ROOT_DIR := replace(justfile_directory(), "\\", "/")
EXAMPLE_DIR := ROOT_DIR + "/examples"
OUT_DIR := ROOT_DIR + "/outputs"
RELEASE_PACKAGE := OUT_DIR + "/sactor-package" + "-" + VERSION + ".zip"

set shell := ["nu", "-c"]

build-all:
    just build "blinky_hello"
    just build "actor_pool"
    just build "timer"

build EXAMPLE_NAME="blinky_hello" ENV="seeed_xiao_esp32c3":
    @just _log-head "Building example: {{EXAMPLE_NAME}}"
    @cd "{{EXAMPLE_DIR}}/{{EXAMPLE_NAME}}"; pio run -e "{{ENV}}"

upload EXAMPLE_NAME="blinky_hello" ENV="seeed_xiao_esp32c3":
    @just _log-head "Building and uploading example: {{EXAMPLE_NAME}}"
    @cd "{{EXAMPLE_DIR}}/{{EXAMPLE_NAME}}"; pio run --target upload -e "{{ENV}}"

pack:
    @just _log-head "Packing source code to {{RELEASE_PACKAGE}} ..."
    @cd "{{ROOT_DIR}}"; rm --force "{{RELEASE_PACKAGE}}"; 7z -tzip a "{{RELEASE_PACKAGE}}" "./src"; 7z -tzip a "{{RELEASE_PACKAGE}}" "./LICENSE"; 7z -tzip a "{{RELEASE_PACKAGE}}" "./README.md"; 7z -tzip a "{{RELEASE_PACKAGE}}" "./library.*"; 7z -tzip a "{{RELEASE_PACKAGE}}" "./examples";

publish:
    pio pkg publish

#
# Utility tasks
#
_log-head LOG_LINE:
    @just _log-inner "{{COLOR_GREEN}}" "INFO!" "{{LOG_LINE}}"

_log-info LOG_LINE:
    @just _log-inner "{{COLOR_BLUE}}" "INFO " "{{LOG_LINE}}"

_log-error LOG_LINE:
    @just _log-inner "{{COLOR_RED}}" "ERROR" "{{LOG_LINE}}"

_log-inner COLOR LOG_LEVEL LOG_LINE:
    @if "{{LOG_LINE}}" == "" { echo ""; } else { echo $"{{COLOR}}[(date now | date format `%Y-%m-%d %H:%M:%S`)][{{LOG_LEVEL}}] {{LOG_LINE}}{{COLOR_NONE}}"; }