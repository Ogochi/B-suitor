# B-suitor
It's a project done as a part of __Concurrent Programming Class__ at University of Warsaw.

Program is meant to solve __B-Matching Problem__. It is a generalisation of ordinary matching problem. 
Everything is the same despite the fact you can match vertice `v` in graph with at most `b(v)` other vertices(`b(v)` is provided function).
Project has focus on undirected wighted graphs. 

Our aim was to find approximation(not less than half of the best solution) 
of matching with the greatest sum of weights on edges. There was made such a decision, because precise algorithms are slow or really complex.

## Algorithm
Algorithm we were supposed to use is described in detail [here](https://www.cs.purdue.edu/homes/apothen/Papers/bMatching-SISC-2016.pdf).

## Performance
I was testing implementation on graphs from [Stanford Database](http://snap.stanford.edu/data/as-skitter.html).

I present performance for graph __as-Skitter__ with random weights(from 1 to 4).

Program run on processor:  __AMD Ryzen 5 1500X, 3.5GHz__

Number of vertices|Number of edges|Average vertex degree|Max vertex degree|
|---|---|---|---|
|1696415|11095298|12|35455|

|Number of threads|1|2|3|4|5|6|7|8|
|----|---|---|---|---|---|---|---|---|
|Time(sec)|1.852|1.481|1.341|1.285|1.177|1.126|1.099|1.049|
|Speed-up|1|1.25|1.381|1.441|1.573|1.645|1.684|1.765|

## University mark
Project received __9 out of 10__ points.

|Marked part|My score|Max possible score|
|----|---|---|
|Correctness|5|5|
|Performance|2|3|
|Report|2|2|
