## Introduction
* This repository provides an inplementation of facetts.
* This is an algorithm for estimating cardinality in inner product spaces
* As for the details about facetts, please read our IEEE Open Journal of the Computer Society paper,  
Cardinality Estimation in Inner Product Space

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

## Citation
If you use our implementation, please cite the following paper.
```
@article{?,  
    title={Cardinality Estimation in Inner Product Space},  
    author={Hirata, Kohei and Amagata, Daichi and Hara, Takahiro},  
    journal={IEEE Open Journal of the Computer Society}, 
    volume={?},
    pages={?--?},  
    year={2022},
    publisher={}
}
```

## License
Copyright (c) 2022 peitaw22  
This software is released under the [MIT license](https://github.com/peitaw22/facetts/blob/main/LICENSE).
