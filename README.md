# Úkol č. 2

Autor: Ondřej Surý, P23010

## Zadání

Vytvořte projekt v jazyku C, který bude splňovat následující úkoly:

- vytvořte dynamické dvourozměrné pole-matici celých čísel, která bude mít 10 řádků a 12 sloupců
- pole bude reprezentováno datovou strukturou pointer na pointer 
- naplňte matici náhodnými celými čísly v rozsahu 0 až 1000
- vypočtěte součet všech čísel v matici
- nalezněte nejmenší hodnotu a její pozici v matici
- vypište obsah matice na obrazovku
- nahraďte všechny 0 v matici hodnotou 1000 a matici opět vypište

## Řešení

Řešení odpovídá zadání, nicméně u tak malých C polí by bylo lepší pole
alokovat v celku a ideálně pouze na stacku, nikoli na heapu.  Při
alokaci v celku bychom pak mohli použít jednoduše
malloc(sizeof(array));

Poznámka, pro alokaci sloupců a řádků je použit calloc() nikoli pro
nulování obsahu, ale abychom nemuseli řešit overflow násobků
velikosti.  V tomto případě víme, že velikost je 12*10 a nikdy nám
nepřeteče SIZE_MAX, ale použití calloc() v případě alokace polí je
best-practice.

Řešení není rozděleno do menších funkcí, tudiž je pouze dvou
průchodové, při rozdělení do sub-funkcí (generate, print, count_sum,
find_min, replace_1000, print) by bylo pole procházeno 6x (worst case).
