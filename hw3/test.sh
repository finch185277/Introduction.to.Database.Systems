#!/bin/sh

main() {
  DATA="data"
  for i in 1 2 3; do
    for TN in 1 2 4 8 16 32; do
      echo "${DATA}${i} [Thread Number: ${TN}]"
      ./main ${TN} ${DATA}/${DATA}${i}_out < ${DATA}/${DATA}${i}
      diff ${DATA}/${DATA}${i}_out ${DATA}/${DATA}${i}_answer
      rm ${DATA}/${DATA}${i}_out
    done
  done
}
main
