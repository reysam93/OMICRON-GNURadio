#!/bin/bash

dir=$1

if [ "$#" -ne 1 ]; then
  echo "usage: mv_exp_files.sh dir"
  exit 1
fi

filesType=( time_ freq_ )

mkdir $dir

for files in "${filesType[@]}"
do
  scp lab1@10.1.147.216:/tmp/$files* /tmp;
  cp /tmp/$files* $dir
done
