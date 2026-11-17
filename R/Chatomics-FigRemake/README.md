R Project No. 1: Recreating Figures from "Genome-wide association between YAP/TAZ/TEAD and AP-1 at enhancers drives oncogenic growth"

**Tutorial**: https://crazyhottommy.github.io/reproduce_genomics_paper_figures/index.html

**Original Paper Citation**: Zanconato, F. et al. Genome-wide association between YAP/TAZ/TEAD and AP-1 at enhancers drives oncogenic growth. Nat Cell Biol 17, 1218–1227 (2015).

# Project Summary

Following the above tutorial, we shall remake the figures from Zanconato et al.’s paper. We shall look mainly look at 4 proteins (listed below), and our main aim is to explore if they interact. To do so, we shall use ChIP-Seq data to confirm that they overlap in which loci of the human genome they act on, and compare these to known promoter and enhancer regions. The 4 proteins are:

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

- **Fig. 1a & 1b**: The total number of peaks of each of YAP, TAZ & TEAD4, and the number of overlapping peaks of (a) YAP with TAZ, and (b) YAP/TAZ with TEAD4. The range of each peak was calculated using `macs3`, HOW DOES IT DECIDE? and the number of overlaps using `subsetByOverlaps` from the`GenomicRanges` in **R**.
  - **NOTE**: `subsetByOverlaps` is **NOT** commutative - if 3 peaks of `A` overlap with 1 peak of `B`, then `subsetByOverlaps(A, B)` will yield `3` peaks but `subsetByOverlaps(B, A)` will yield `1` peak. The difference is fairly small here, less than a 1% difference,
- **Fig. 1c**: The distribution of distances between the summits of YAP/TAZ peaks and their overlapping TEAD4 peaks. Distances are calculated using the `distance` function from the `GenomicRanges` library.
- **Fig. 1d**: Linear correlation between the signals of YAP or TAZ with TEAD4 within the YAP/TAZ/TEAD4 peaks. The number of reads are calculated using `multicov` from `bedtools`, and the total number of aligned reads for each protein is given by `flagstat` from `samtools`. The $r^2$ value is calculated using the `gppmisc` library in **R**.
  - **NOTE**: There is an outlier with high signals in both - this is **NOT** from a blacklisted region (where errors frequently occur)
- **Fig. 1f**: The distribution of distances between the YAP/TAZ/TEAD4 peaks and their nearest TSS. The list of TSS’s comes from the `TxDb.Hsapiens.UCSC.hg38.knownGene` library in **R**, and the distances are calculated using `distanceToNearest` from the `GenomicRanges` library.
  - **NOTE**: More TSS’s are labelled now (2026) than when the tutorial was made (2024, I believe). More importantly, substantially different to that of the paper (2018) - the paper discusses most YAP/TAZ/TEAD4 peaks being **>100 kb** away from a TSS, and thus potentially in enhancer regions, but we find the opposite is true.
- **Fig. 1g**: To be completed later.
- **Fig. 1h**: Distribution of YAP/TAZ/TEAD4 peaks at expected promoters and active/inactive enhancers sites. Datasets for H3K4me3, H3K4me1 and H3K27Ac were downloaded from https://www.ncbi.nlm.nih.gov/geo/query/acc.cgi?acc=GSE49651 and converted from `hg19` to `hg38` (described in `log.ipynb`). From these, (a) promoters are defined as H3K4me3 peaks **NOT** overlapping with H3K4me1 peaks (and <5 kb from a TSS), and (b) enhancers as H3K4me1 peaks **NOT** overlapping with H3K4me3 peaks - (i) active if overlapping with H3K27Ac peaks and (ii) inactive if not.
  - **NOTE**: For where H3K4me1 and H3K4me3 peaks overlap, the paper has discussed manually deciding based on the signal strength of each and proximity to a TSS. In our code, we have taken all H3K4me1 peaks as enhancers, ignoring if they overlap with H3K4me3.
- **Fig. 1i**:
- **Fig. 1j**:
- **Fig. 1k**: The distribution of distances between the YAP/TAZ peaks and their nearest TSS. Calculated the same as **Fig. 1f**.

## New Concepts

- Using `.sh` files to run multiple terminal commands
  - **NOTE**: Must run `chmod 744 <filepath>` to grant the `.sh` file access to the terminal

## New R Libraries

- `rtracklayer`
- `here`
- `GenomicRanges`
- `Vennerable`
- `dplyr` data manipulation (mutate(), etc…)
- `ggplot2`
- `readr`
- `ggpmisc`
- `TxDb.Hsapiens.UCSC.hg38.knownGene`
- `EnrichedHeatmap`
- `magick`

## New Conda Modules

- `fastq-dl`
- `fastqc`
- `refgenie`
- `bowtie2`
- `samtools`
- `deeptools`
- `macs3`

## New Coding Terminology

- `data.frame()`
- `png(file = <file-name>) ... garbage <- dev.off()` saves plotted images between the 2 above lines to `<file-name>`
- `GenomicRanges::subsetByOverlaps(A, B)` takes in 2 `GRanges` objects and returns a `GRanges` subset of `A` that overlaps with `B`
- `tibble::as_tibble()` is similar to `data.frame()` but with some improvements
- `dplyr::mutate()` creates new rows from functions of other rows, or modifies existing rows
- `EnrichedHeatmap::normalizeToMatrix()` prepares data for heatmaps (Unclear on exist working)

- Silencers:
  - `suppressPackageStartupMessages()` silences the warnings about masked functions
  - `suppressWarnings()`
  - `suppressMessages()`
- `x %>% f()` is the same as f(x)
  - **NOTE**: `x %>% f(a)` is the same as f(x, a)
- `A %in% B` loops through all elements of `A`, and returns `TRUE` if in `B`, else `FALSE`
- `c(Elem1, Elem2, Elem3)` makes a list (`c` for concatonate)
- `$` works like `pos$x` which in Python is `pos.x`

# Further Work

- For **Fig. 1d**, we get slightly different $r^2$ values. This could be due to having a third of the data - how does reducing data size affect $r^2$ values (assuming they’re randomly removed)?
- Make **Fig. 1g** and look at what **IGV** is
- Investigate how promoters/enhancers are defined in **Fig. 1h**, as discussed in the **NOTE**
- Add in H3K4me1 and H3K4me3 to **Fig. 1i**
- Understand **Fig. 1i & j**
