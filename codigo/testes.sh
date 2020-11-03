#!/bin/bash

PISTA_PEQUENA=10
PISTA_MEDIA=30
PISTA_GRANDE=50

CICLISTAS_POUCOS=10
CICLISTAS_NORMAL=20
CICLISTAS_MUITOS=30

for pista in $PISTA_PEQUENA $PISTA_MEDIA $PISTA_GRANDE; do
  for ciclista in $CICLISTAS_POUCOS $CICLISTAS_NORMAL $CICLISTAS_MUITOS; do
    for i in {1..30}; do
      { time ./ep2 $pista $ciclista 2> teste$pista-$ciclista-$i.txt
      } 2>> relatorio$pista-$ciclista-$i.txt &
      pmap $(pgrep ep2) | grep total - >> relatorio$pista-$ciclista-$i.txt &
      wait $(pmap $(pgrep time))
    done
  done
done

