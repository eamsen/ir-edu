# Exercise 5 Recap
The Wiki page is nice to see whether you get correct results, but not suitable
for performance comparisons due to the different systems used for the evaluation.

Therefore, I have compiled a quick overview over the performance of your submitted
applications on my own system. Note that not all student's results are included,
only those which returned correct results without major modifications from my
side.

The best performing system at each category is highlighted. *es* is excluded from
this due to his "cheating" by ignoring single-letter words and adding only one
padding character at each side instead of k-1.

## Performance Results

### System Specs
* CPU: Intel Core i5 750 x3  @2.67GHz (VM)
* 4GB RAM @800MHz (VM)
* OS: Ubuntu 12.04 LTS 32bit. 
* GCC 4.6.3

### Parsing
* ***ab*: 591ms**
* *ah*: 1235ms
* *as*: 613ms
* *cg*: 620ms
* *dp-mg*: 1000ms
* *es*: 769ms
* *fw*: 1470ms
* *jm*: 1470ms
* *ke*: 1768ms
* *mb*: 1446ms
* *mf205*: 1700ms

### Average Query Durations
* *ab*: 5.68ms
* *ah*: 20.82ms
* ***as*: 5.27ms**
* *cg*: 7.11ms
* *dp-mg*: 44.00ms
* *es*: 1.42ms
* *fw*: 5.69ms
* *jm*: 28.04ms
* *mf205*: 5.28ms
* *mf228*: 6.00ms 

### Average Edit Distance Durations
* *ab*: 3.79µs 
* *ah*: 3.00µs
* *as*: 3.10µs
* *cg*: 3.29µs
* *es*: 1.00µs
* ***fw*: 1.43µs**
* *jm*: 1.69µs
* *mf205*: 2.13µs
* *mf228*: 4.31µs
