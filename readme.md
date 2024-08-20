## LeBQ+

### 1. Introduce

LeBQ+ is built upon the LeB index structure, which is an enhanced B+ tree that integrates set length information and bucket mapping into the index keys. By incorporating set lengths into the keys, LeB can directly exclude sets that do not meet the length requirements, thereby improving query efficiency.

### 2. Operating Environment

\- **Processor:** Intel i5-8300H 4-core processor @ 2.30GHz

-**Memory:** 24GB  RAM

-**Operating System:** Ubuntu 20.04

**GCC Version:** 9.0

### 3. Example

 **The test dataset must have the following format:**

- 11 6 55 586 587 446
- 11 314 6 3 566 567 568 569 438
- 554 11 314 6 2 89 555 556 557 241 3 558 102 104 559 119 560 561 562 126 563 564
- ...

**The query dataset must have the following format:**

- 11 6 55 586 587 446
- 11 314 6 3 566 567 568 569 438
- 554 11 314 6 2 89 555 556 557 241 3 558 102 104 559 119 560 561 562 126 563 564
- ...
