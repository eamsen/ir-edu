# Exercise 9 Recap
The Wiki page is nice to see whether you get correct results, but not suitable
for performance comparisons due to the different systems used for the evaluation.

Therefore, I have compiled a quick overview over the performance of your submitted
applications on my own system. Note that not all students' results are included,
only those which returned correct results without major modifications from my
side.

The best performing systems (excluding *es*) at each category are marked with
`p` for best performance and `q` for best quality (RSS value).
Multithreaded programs are additionally marked with `mt`.

By comparing the execution times the systems by *js* and *es* significantly stand
out from the rest. This is mostly due to using a vector of pairs for the sparse
vectors, instead of an ordered map. The vector is a very cache-efficient data
structure and since the bottleneck of this application is in the distance
calculation, which relies on fast iteration over the vectors, the vector is
strongly favored over the ordered map.

As you can see, the quality results of the same system can vary, this is because
of the randomazation of the initial centroids. For more predictable and provably
better results, k-means++ seeding should be prefered.

## System Specs
* CPU: Intel Core i5 750 x3 @2.67GHz (VM)
* 4GB RAM @800MHz (VM)
* OS: Ubuntu 12.04 LTS 32bit. 
* GCC 4.6.3

## Performance Results (not final!)
The **minimum rate of change** was set to **0.1**, if possible.

*Notation: RSS/iterations/duration*

### Maximum 10 iterations
*ab:* 22335/10/358s `mt`  
*ah:* 22145/10/652s `q`   
*cs:* 22195/10/1494s  
*jm:* 22384/10/long  
*js:* 22441/10/185s `p`   
*jw:* 25367/10/538s `mt`  
*ke:* 22157/10/734s  
*mb:* 22229/10/574s  
*mf:* 22179/10/1588s  
*es:* 22074/10/134s  
*es:* 22074/10/49s `mt`  

### Maximum 20 iterations
*ab:* 22232/20/699s `mt`  
*ah:* 22071/20/1328s    
*cs:* 22041/20/long  `q`  
*jm:* 22264/20/long  
*js:* 22339/20/344s  `p`   
*jw:* 25182/20/944s `mt`  
*ke:* 22094/20/1279s  
*mb:* 22100/20/1235s  
*es:* 22015/20/198s  
*es:* 22015/20/73s `mt`  

### Maximum 50 iterations  
*ab:* 22168/33/1150s `mt`  
*ah:* 22031/50/3394  `q`  
*cs:* 22148/50/long  
*js:* 22328/24/346s  `p`    
*jw:* 25345/46/2137s `mt`  
*ke:* 22102/50/3838s  
*mb:* 22051/50/3081s  
*es:* 21996/42/345s  
*es:* 21996/42/118s `mt`  
