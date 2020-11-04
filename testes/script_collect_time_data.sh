#!/bin/bash

PISTA_PEQUENA=250
PISTA_MEDIA=500
PISTA_GRANDE=1000

CICLISTAS_POUCOS=10
CICLISTAS_NORMAL=100
CICLISTAS_MUITOS=1000

rm data.csv

echo "c_n p_p, c_n p_m, c_n p_g, p_m c_p, p_m c_n, p_m c_m" >> data.csv
for i in {1..30}; do
  ciclista=$CICLISTAS_NORMAL
  for pista in $PISTA_PEQUENA $PISTA_MEDIA $PISTA_GRANDE; do
      { cat relatorio$pista-$ciclista-$i.txt | grep real | sed 's/m/ /' | sed 's/s//' | sed 's/,/./' | awk '{printf "%d", (60*$2) + $3}' | sed 's/,/./'
      } >> data.csv
      echo -n "," >> data.csv
      echo $pista-$ciclista-$i
  done

  pista=$PISTA_MEDIA
  for ciclista in $CICLISTAS_POUCOS $CICLISTAS_NORMAL $CICLISTAS_MUITOS; do
      { cat relatorio$pista-$ciclista-$i.txt | grep real | sed 's/m/ /' | sed 's/s//' | sed 's/,/./' | awk '{printf "%d", (60*$2) + $3}' | sed 's/,/./'
      } >> data.csv
      echo -n "," >> data.csv
      echo $pista-$ciclista-$i
  done
  echo "" >> data.csv
done
