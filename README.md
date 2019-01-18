# Improvement of the partially assembled genome with long reads
This repository contains the project for a [Bioinformatics course](https://www.fer.unizg.hr/en/course/bio).

Algorithm implementation is based on the work:
* [Assembly of chromosome-scale contigs by efficiently resolving repetitive sequences with long reads](https://www.biorxiv.org/content/early/2018/06/13/345983).

## Authors
See the list of [contributors](https://github.com/Dan901/bioinformatics/graphs/contributors) who participated in this project.

## Recommended parameters 
For Ecoli:
1000 0.1 300000 0.2 200 -> also random paths can be set to 0
For Cjejuni (same also works for bgrahmii):
500 0.3 300000 0.2 200
1000 0.3 500000 0.2 200
500 0.4 300000 0.05 500
500 0.3 300000 0.1 500

## Usage - for linux users
Requirements: g++-8

First one should clone the project with git. 

Second, start the 'compile' script. If the script is not executable, type: 
```bash
chmod +x compile
```

Start the script by writing:
```bash
./compile
 ```
 
 As an output there should be a file named main. Here is an example of usage:
 ```bash
 ./main data/EColi/ecoli_test_contigs.fasta data/EColi/ecoli_test_reads.fasta data/EColi/overlaps_reads_contigs.paf data/EColi/overlaps_reads.paf data/EColi/prefix  1000 0.1 300000 0.2 200
 ```
 The last 5 arguments are parameters that effect the way the algorithm behaves. The given parameters are recommended settings for Ecoli. Other then thoose you are free to adjust them and play until you are satisfied with the result. As there is a radnom factor involved please understand that you can get different paths with the same arguments. Also if you with to remove random paths(random search) set the last argument to 0. 
 
 Arguments in order are:
 1. Relative or absolute path to the files with contigs in fasta or fastq format = data/EColi/ecoli_test_contigs.fasta
 2. Relative or absolute path to the files with reads in fasta or fastq format = data/EColi/ecoli_test_reads.fasta
 3.	Relative or absolute path to the files with overlaps from contigs and reads in paf format = data/EColi/overlaps_reads_contigs.paf
 4.	Relative or absolute path to the files with overlaps from reads with reads in paf format = data/EColi/overlaps_reads.paf
 5. Relative or absolute path to the output file = data/Ecoli/prefix
  * after the prefix of the output path contig sequence will follow
 6.	overlapThreshold, overlaps containing less nucleobases then this will be discarded, minimal value of 500
 7.	siThreshold, double that represents the lowest sequence identity that will not be discarded, only overlabs with a bigger Si will be used  
 8.	maxPathLength, maximum number of bases a connectiong between two contigs can contain
 9. maxOverhangExtensionRatio, double that represents how big can the overhang be in relation to an extension 
  * overhang < maxOverhangExtensionRatio * extensionLength 
 10.	randomPathTrials, number of random paths that shall be created using the monteCarlo method, it should be a natural number
 
 
