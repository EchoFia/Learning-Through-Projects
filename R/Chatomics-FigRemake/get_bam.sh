#!/bin/bash

# NOTE: && Makes it so that it stops if any line fails

SRRs=('SRR1810912' 'SRR1810918')
for SRR in "${SRRs[@]}"
do
    fastq-dl --accession $SRR -o fastq/ &&
    mv fastq/fastq-run-info.tsv fastq/$SRR-run-info.tsv &&
    fastqc fastq/$SRR.fastq.gz --outdir qc_reports &&
    bowtie2 -x GRCh38_noalt_as/GRCh38_noalt_as -U fastq/$SRR.fastq.gz -S fastq/$SRR.sam --threads 8  --no-mixed --no-discordant -k 1 &&
    samtools view -bS fastq/$SRR.sam > fastq/$SRR.bam &&
    rm fastq/$SRR.sam &&
    samtools sort -@ 4 -o fastq/$SRR.sorted.bam fastq/$SRR.bam &&
    rm fastq/$SRR.bam &&
    samtools index fastq/$SRR.sorted.bam
done