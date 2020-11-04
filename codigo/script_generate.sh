#!/bin/bash

PISTA_PEQUENA=250
PISTA_MEDIA=500
PISTA_GRANDE=1000

CICLISTAS_POUCOS=10
CICLISTAS_NORMAL=100
CICLISTAS_MUITOS=1000

# rm teste*
# rm relatorio*

ciclista=$CICLISTAS_NORMAL
for pista in $PISTA_PEQUENA $PISTA_MEDIA $PISTA_GRANDE; do
  for i in {1..10}; do
    { time ./ep2 $pista $ciclista $i
    } 2>> relatorio$pista-$ciclista-$i.txt
    echo $pista-$ciclista-$i
  done
done

pista=$PISTA_MEDIA
for ciclista in $CICLISTAS_POUCOS $CICLISTAS_MUITOS; do
  for i in {1..10}; do
    { time ./ep2 $pista $ciclista $i
    } 2>> relatorio$pista-$ciclista-$i.txt
    echo $pista-$ciclista-$i
  done
done
