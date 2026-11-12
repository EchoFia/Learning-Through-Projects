R Project No. 1: Recreating Figures from "Genome-wide association between YAP/TAZ/TEAD and AP-1 at enhancers drives oncogenic growth"

**Tutorial**: https://crazyhottommy.github.io/reproduce_genomics_paper_figures/index.html

**Original Paper Citation**: Zanconato, F. et al. Genome-wide association between YAP/TAZ/TEAD and AP-1 at enhancers drives oncogenic growth. Nat Cell Biol 17, 1218–1227 (2015).

## Summary of the Paper

# Project Summary - Change slightly

Following the above tutorial, we shall remake the figures from Zanconato et al.’s paper. We shall look mainly look at 4 proteins:

- YAP - SRR1810900
- TAZ - SRR1810907
- TEAD4 - SRR1810918
- IgG - SRR1810912 (Control)

In the Gene Expression Omnibus (GEO), an `SRX` code is for a given set of experiments for one protein, and an `SRR` code is for the raw files of each fun. From the same experiment set, there are multiple sets of data for each of the above proteins; however, due to large file sizes, we shall only take one `SRR` set for each (listed above).

As much of the work is directly in the terminal, the commands are stored in the `log.ipynb` file, including the initial set-up of the **Conda** environment.

# Phase 1: Preprocessing the Data

1. Download the raw ChIP-Seq data from **GEO** using the `fastq-dl` module
   - It’s good practice to ensure the data is good quality, which we can do using `fastqc`
2. Download the reference human genome, **hg38**, using `refgenie` - specifically, we want the `fasta` file
3. Using `bowtie2`, align the ChIP-Seq data to the human genome - this will output `SAM` files for each
4. Convert the `SAM` files to an indexed, sorted `BAM` files with the `samtools` module
5. With `deeptools`, convert the sorted `BAM` files into **BigWig** (`.bw`) files (except for **IgG**)
6. Using IgG as a control, find the peaks of each protein with `macs3`

# Phase 2: Creating the Figures

- **Fig. 1a & 1b**:
  - NOTE: Unique peaks for TAZ and YAP overlap depends on which you do first - essentially, one TAZ peak might overlap with 3 YAP peaks - if you do TAZ first, it spits back the 1 TAZ peak as it does overlap with at least 1 YAP peak. Do YAP peaks first, and it spits back 3, cause all 3 overlap with a TAZ peak, it just happens to be the same one. - WHAT TO DO ABOUT THIS
- **Fig. 1c**:
  - “For those that overlap, what is the difference between the indexes of their respective peaks” - essentially, how bang on do they overlap?

## New Concepts

- Using `.sh` files to run multiple terminal commands
  - **NOTE**: Must run `chmod 744 <filepath>` to grant the `.sh` file access to the terminal
-

## New Coding Terminology

- `%>%` https://stackoverflow.com/questions/24536154/what-does-mean-in-r
- `$` extracts a thing (like `pos.x` the . in that)
- `%in%` s
- `c(Elem1, Elem2, Elem3)` makes a list (`c` for concatonate)

## New R Libraries

-

## New Conda Modules

- `fastq-dl`
- `fastqc`
- `refgenie`
- `bowtie2`
- `samtools`
- `deeptools`
- `macs3`
