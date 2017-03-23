Dumbo
=======

Introduction
------------

Dumbo is an experimental MPP database engine as-a-service node built using Boost Fibers, Memoria and LLVM JIT.

* It's [Seastar](http://www.seastar-project.org/)-like  event-driven framework providing fast lock-free 
    inter-thread communication, Asynchrous IO for SSDs and network, and fibers. Building around Boost Fibers for 
    code simplicity. Unilike Seastar, continuation-passing stile is not enforced.
* [Memoria](https://bitbucket.org/vsmirnov/memoria/wiki/Home) is a C++14 framework providing 
    general purpose dynamic composable data structures on top of key-value, object, file, block or in-memory storage. 
    Out of the box the following data structures are provided: map, vector, multimap, 
    table, wide table and others. 
* [LLVM JIT](http://llvm.org/) to compile queries, filters, transformers and other logic over Memoria's 
    data structures dynamically, on the fly.

In the wild Dumbo is family of an adorable [deep sea octopuses](http://oceana.org/marine-life/cephalopods-crustaceans-other-shellfish/dumbo-octopuses).

