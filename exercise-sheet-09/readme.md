# Exercise 9 Recap
The Wiki page is nice to see whether you get correct results, but not suitable
for performance comparisons due to the different systems used for the evaluation.

Therefore, I have compiled a quick overview over the performance of your submitted
applications on my own system. Note that not all students' results are included,
only those which returned correct results without major modifications from my
side.

The best performing system (excluding *es*) at each category is highlighted.
Multithreaded programs are marked with `mt`.

## System Specs
* CPU: Intel Core i5 750 x3 @2.67GHz (VM)
* 4GB RAM @800MHz (VM)
* OS: Ubuntu 12.04 LTS 32bit. 
* GCC 4.6.3

## Performance Results **(not final!)**
The **minimum rate of change** was set to **0.1**, if possible.

*Notation: RSS/iterations/duration*

### Maximum 10 iterations
*ab:* 22335/10/358s `mt`  
*ah:* 26856/10/612s  
*jm:* 22384/10/long  
*js:* 22371/10/unknown  
*ke:* 22157/10/734s  
*mb:* 22229/10/574s  
*es:* 22074/10/134s  
*es:* 22074/10/49s `mt`  

### Maximum 20 iterations
*ab:* 22232/20/699s `mt`  
*ah:* 26809/20/1334s  
*jm:* 22264/20/long  
*js:* 22310/20/unknown  
*ke:* 22094/20/1279s  
*mb:* 22100/20/1235s  
*es:* 22015/20/198s  
*es:* 22015/20/73s `mt`  

### Maximum 50 iterations  
*ab:* 22168/33/1150s `mt`  
*ah:* 26785/50/3320s  
*ke:* 22102/50/3838s  
*mb:* 22051/50/3081s  
*es:* 21996/42/345s  
*es:* 21996/42/118s `mt`  
