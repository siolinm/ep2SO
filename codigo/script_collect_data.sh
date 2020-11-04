#!/bin/bash

PISTA_PEQUENA=250
PISTA_MEDIA=500
PISTA_GRANDE=1000

CICLISTAS_POUCOS=10
CICLISTAS_NORMAL=100
CICLISTAS_MUITOS=1000

ciclista=$CICLISTAS_NORMAL
for pista in $PISTA_PEQUENA $PISTA_MEDIA $PISTA_GRANDE; do
  for i in {1..30}; do
    { cat relatorio$pista-$ciclista-$i.txt | grep real | awk '{printf "%s", $2}'
    } >> data.csv
    echo -n "," >> data.csv
    echo $pista-$ciclista-$i
  done
  echo "" >> data.csv
done

pista=$PISTA_MEDIA
for ciclista in $CICLISTAS_POUCOS $CICLISTAS_NORMAL $CICLISTAS_MUITOS; do
  for i in {1..30}; do
    { cat relatorio$pista-$ciclista-$i.txt | grep real | awk '{printf "%s", $2}'
    } >> data.csv
    echo -n "," >> data.csv
    echo $pista-$ciclista-$i
  done
  echo "" >> data.csv
done
