#!/usr/bin/env bash
set -euo pipefail
if [[ -z "${DEVKITPRO:-}" ]]; then
  echo 'DEVKITPRO not set. Install devkitPro nds-dev and source its environment.' >&2
  exit 1
fi
export DEVKITARM="${DEVKITARM:-$DEVKITPRO/devkitARM}"
export PATH="$DEVKITARM/bin:$PATH"
export LIBNDS="${LIBNDS:-$DEVKITPRO/libnds}"
export LIBFAT="${LIBFAT:-$DEVKITPRO/libfat}"
for f in "$DEVKITARM/bin/arm-none-eabi-gcc" "$DEVKITARM/ds_rules" "$LIBNDS/include/nds.h" "$LIBFAT/include/fat.h"; do
  if [[ ! -e "$f" ]]; then echo "Missing dependency: $f" >&2; exit 1; fi
done
make clean
make -j2
if [[ -s elemental_legends.nds ]]; then
 echo "Built: $(pwd)/elemental_legends.nds"
else
 echo 'Build did not produce a ROM.' >&2
 exit 1
fi
