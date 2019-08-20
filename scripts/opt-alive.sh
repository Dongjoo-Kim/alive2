#!/bin/bash

set -e

# IPO passes aren't supported ATM
PASSES="argpromotion deadargelim hotcoldsplit inline ipconstprop ipsccp mergefunc partial-inliner -Os -O2 -O3"

TV="-tv"
for p in $PASSES; do
  for arg in $@; do
    if [[ $arg == *"$p"* ]]; then
      TV=""
      break
    fi
  done
done

timeout 1000 $HOME/memopt/llvm/build/bin/opt -load=$HOME/memopt/alive2/build/tv/tv.so -tv-exit-on-error $TV $@ $TV -tv-smt-to=10000 -tv-report-dir=$HOME/memopt/alive2/build/logs -tv-smt-stats
