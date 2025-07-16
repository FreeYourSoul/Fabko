#!/usr/bin/env sh

# support arbitrary commands trough symlinks to this file
# e.g. create a symlink "make" to "nix-run.sh" and execution
# of the symlink will run "make" in the nix-shell environment
if [ "$(basename "$0")" != "nix-run.sh" ]; then
  CMD="$(basename "$0")"
fi

# we need to check, if we're already running inside the
# nix-shell environment, as some nix-run.sh symlinks may
# call other nix-run.sh symlinks, which wouldn't work.
# this is because nix-shell itself will not be available
# when run in --pure mode
if [ -n "$IN_NIX_RUN" ]; then
  exec "$CMD" "$@"
else
  #relative path containing default.nix / shell.nix
  PROJECT_DIR=$(dirname ./.
  QUOT_ARGS=$(printf '"%s" ' "$@")
  IN_NIX_RUN=1 nix-shell --pure --keep IN_NIX_RUN --run "$CMD $QUOT_ARGS" "$PROJECT_DIR"
  exit $?
fi
