#!/usr/bin/env zsh

# build.zsh
# ‾‾‾‾‾‾‾‾‾

function -- () {

  # equivalent to running `dirname` on this file's path (w/o resolving links)
  local -r _proj_root="${${(%):-%x}:a:h}"

  local -r CC='clang'
  local -r STD=23

  # ————————————————————————————————————————————————————————————————————————— #

  local -ri 10 optimisation=0
  local -ra CFLAGS=( g O$optimisation ) DEFINES=( )

  # ———————————————————————————————————————————————————— #

  # all `-W...` warnings to enable
  local -a WARNINGS=( all extra pedantic vla )
  # all `-W-no-...` warnings to disable
  local -ra NO_WARN=( multichar )

  WARNINGS+=( "no-${(@)^NO_WARN}" )

  # ————————————————————————————————————————————————————————————————————————— #

  local -r EXEC_NAME='regex'

  # location of the outputted binary
  local -r TARGET="$_proj_root/out/$EXEC_NAME"

  # the command that should be run after compilation
  local -ra CMD=( "$TARGET" "$@" )

  # ———————————————————————————————————————————————————— #

  local -ra INCLUDES=( "$_proj_root/source" )
  # an array of all the program's source files
  local -ra SOURCE_FILES=( "$_proj_root/source/"**/*.c )

  # ———————————————————————————————————————————————————— #

  local -ra LIBPATHS=( ) LDLIBS=( ) FRAMEWORKS=( )

  local -ra SANITISE=( address undefined )
  local -ra ASAN_OPTS=(
    print_legend=0
    stack_trace_format=$'"  %n\t%f\t\t%S"'
  )

  # ————————————————————————————————————————————————————————————————————————— #

  # pack the build args into an array, adding each of their relevant prefixes,
  #  and making sure not to add any empty arrays
  local -a BUILD_ARGS=( -std="c$STD" )

  if (( $#CFLAGS     )) BUILD_ARGS+=(  "-${(@)^CFLAGS}"   )
  if (( $#LDLIBS     )) BUILD_ARGS+=( "-l${(@)^LDLIBS}"   )
  if (( $#DEFINES    )) BUILD_ARGS+=( "-D${(@)^DEFINES}"  )
  if (( $#INCLUDES   )) BUILD_ARGS+=( "-I${(@)^INCLUDES}" )
  if (( $#LIBPATHS   )) BUILD_ARGS+=( "-L${(@)^LIBPATHS}" )
  if (( $#WARNINGS   )) BUILD_ARGS+=( "-W${(@)^WARNINGS}" )
  if (( $#SANITISE   )) BUILD_ARGS+=( "-fsanitize=${(j:,:)SANITISE}" )
  if (( $#FRAMEWORKS )) BUILD_ARGS+=( "-framework ${(@)^FRAMEWORKS}" )

  # always add the target file
  BUILD_ARGS+=( --output "$TARGET" -- "${(@)SOURCE_FILES}" )

  # ———————————————————————————————————————————————————— #

  "$CC" "${(@)BUILD_ARGS}" \
    && ASAN_OPTIONS="${(j.:.)ASAN_OPTS}" "${(@)CMD}"

} "$@"

# ——————————————————————————————————————————————————————————————————————————— #

# spell:ignore asan ldlibs cflags
