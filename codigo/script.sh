#!/bin/bash

PISTA_PEQUENA=250
PISTA_MEDIA=500
PISTA_GRANDE=1000

CICLISTAS_POUCOS=10
CICLISTAS_NORMAL=100
CICLISTAS_MUITOS=1000

rm teste*
rm relatorio*

for pista in $PISTA_PEQUENA $PISTA_MEDIA $PISTA_GRANDE; do
  for ciclista in $CICLISTAS_POUCOS $CICLISTAS_NORMAL $CICLISTAS_MUITOS; do
    for i in {1..30}; do
      { time ./ep2 $pista $ciclista $i 2> teste$pista-$ciclista-$i.txt
      } 2>> relatorio$pista-$ciclista-$i.txt
    done
  done
done

