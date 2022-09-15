## Facetts
* An algorithm for estimating cardinality in inner product spaces

## Environment
* Linux OS (Ubuntu)
* g++ 5.5.0 (or newer version)

## How to use
* Datasets
    * Create your datasets from, e.g., rating data, by using Matrix Factorization (https://www.csie.ntu.edu.tw/~cjlin/libmf/).
    * Place your datasets at ``dataset`` directory.
        * Ex.)　An example dataset ``netflix_mf-50.txt`` is at the directory.
    * If you want to test accuracy, the ground truth datasets have to be at ``exact`` directory．
        * Ex.)　An example file ``netflix_thr4.csv`` is at this direcotry.
        * Each line is ``id, cardinality``.
* Test result is obtained by a csv file.
    * Each line consists of ``query id, real cardinality, estimated cardinality, error, running time``.
* Parameter setting is via ``threshold.txt & sampling_number.txt`` at ``parameter`` directory.
* Compile: ``g++ -O3 -o facetts.out facetts.cpp -std=c++11``
* Run: ``./facetts.out``
