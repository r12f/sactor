#!/usr/bin/env just --justfile

COLOR_GREEN := '(ansi green)'
COLOR_BLUE := '(ansi blue)'
COLOR_RED := '(ansi r)'
COLOR_NONE := '(ansi reset)'

VERSION := '0.1.0'

ROOT_DIR := replace(justfile_directory(), "\\", "/")
EXAMPLE_DIR := ROOT_DIR + "/examples"
OUT_DIR := ROOT_DIR + "/outputs"
RELEASE_PACKAGE := OUT_DIR + "/sactor-package" + "-" + VERSION + ".zip"

set shell := ["nu", "-c"]

build:
    just build-example "blinky-hello"

build-example EXAMPLE_NAME:
    @just _log-head "Building example: {{EXAMPLE_NAME}}"
    @cd "{{EXAMPLE_DIR}}/{{EXAMPLE_NAME}}"; pio run

upload EXAMPLE_NAME:
    @cd "{{EXAMPLE_DIR}}/{{EXAMPLE_NAME}}"; pio run --target upload

pack:
    @cd "{{ROOT_DIR}}"; rm --force "{{RELEASE_PACKAGE}}"; 7z -tzip a "{{RELEASE_PACKAGE}}" "./sactor"; 7z -tzip a "{{RELEASE_PACKAGE}}" "./LICENSE"; 7z -tzip a "{{RELEASE_PACKAGE}}" "./README.md";

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