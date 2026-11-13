### =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= ###

### Install Modules (First Time) ###

# if (!require("BiocManager", quietly = TRUE))
#     install.packages("BiocManager") #, repos = "https://cloud.r-project.org")
    
# BiocManager::install("GenomicRanges")
# BiocManager::install("rtracklayer")

# install.packages("here")# , repos = "https://cloud.r-project.org")

# install.packages("devtools", repos = "https://cloud.r-project.org")
# pak::pak("r-lib/devtools")
# pak::pak("js229/Vennerable")

# install.packages("tidyverse", repos = "https://cloud.r-project.org")

# install.packages("ggpmisc", repos = "https://cloud.r-project.org")

# BiocManager::install("TxDb.Hsapiens.UCSC.hg38.knownGene")

### =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= ###

### Set Up ###

# Import modules
suppressPackageStartupMessages({
    library(rtracklayer) # for reading in bed file
    library(here)
    library(GenomicRanges)
    library(Vennerable)
    library(dplyr)
    library(ggplot2)
    library(readr)
    library(ggpmisc)
    library(TxDb.Hsapiens.UCSC.hg38.knownGene)
})

# Create directory to store figure images
figures_dir = "Figures"
# Create the directory (if needed)
if (!file.exists(figures_dir)) {
    dir.create(figures_dir)
}


### MAYBE DO PDF's? Which one is better? compared to pngs



### =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= ###

### Fig. 1a ###

peak_folder <- "peak_reports/"

# 'GRanges' object
TAZ_peaks <- import(here(file.path(peak_folder, "SRR1810907_peak/SRR1810907_peaks.narrowPeak"))) # TAZ_1
YAP_peaks <- import(here(file.path(peak_folder, "SRR1810900_peak/SRR1810900_peaks.narrowPeak"))) # YAP_1

# TAZ_overlap_YAP_peaks <- subsetByOverlaps(TAZ_peaks, YAP_peaks)
# length(TAZ_overlap_YAP_peaks)

# NOTE: Warnings suppressed due to 3 'sequence levels' being present in only 1 (not both)
YAP_overlap_TAZ_peaks <- suppressWarnings(subsetByOverlaps(YAP_peaks, TAZ_peaks))
# length(YAP_overlap_TAZ_peaks)

n_YAP <- length(YAP_peaks)
n_TAZ <- length(TAZ_peaks)
n_YAP_TAZ <- length(YAP_overlap_TAZ_peaks)

venn_YAP_TAZ <- Venn(
                  SetNames = c("YAP", "TAZ"),
                  Weight = c(
                    "10" = n_YAP, # Unique to A
                    "01" = n_TAZ, # Unique to B
                    "11" = n_YAP_TAZ # Intersection
                ))

# Export Fig. 1a
png(file = file.path(figures_dir, "Fig_1a.png"))
plot(venn_YAP_TAZ)
garbage <- dev.off() # Returns 'null device 1'

### =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= ###

### Fig. 1b ###

TEAD4_peak <- import(here("peak_reports/SRR1810918_peak/SRR1810918_peaks.narrowPeak"))

YAP_overlap_TAZ_peaks_overlap_TEAD4<- subsetByOverlaps(YAP_overlap_TAZ_peaks, TEAD4_peak)

n_TEAD4 <- length(TEAD4_peak)
n_YAP_TAZ_TEAD4<- length(YAP_overlap_TAZ_peaks_overlap_TEAD4)

venn_YAP_TAZ_TEAD4 <- Venn(
                        SetNames = c("YAP/TAZ", "TEAD4"),
                        Weight = c(
                          "10" = n_YAP_TAZ, # Unique to A
                          "01" = n_TEAD4, # Unique to B
                          "11" = n_YAP_TAZ_TEAD4 # Intersection
                       ))

# Export Fig. 1b
png(file = file.path(figures_dir, "Fig_1b.png"))
plot(venn_YAP_TAZ_TEAD4)
garbage <- dev.off() # Returns 'null device 1'

### =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= ###

### Fig. 1c ###

TEAD4_summit <- import(here(file.path(peak_folder, "SRR1810918_peak/SRR1810918_summits.bed")))
YAP_summit <- import(here(file.path(peak_folder, "SRR1810900_peak/SRR1810900_summits.bed")))

YAP_TAZ_summit <- YAP_summit[YAP_summit$name %in% YAP_overlap_TAZ_peaks$name]

YAP_TAZ_500bp_window <- resize(YAP_TAZ_summit, width = 500, fix = "center")

hits <- findOverlaps(TEAD4_summit, YAP_TAZ_500bp_window)


# Compute signed distances
signed_distance <- function(A, B) {
  dist <- GenomicRanges::distance(A, B)
  sign <- ifelse(start(A) < start(B), -1, 1)
  return (dist * sign)
}

summit_distance <- signed_distance(TEAD4_summit[queryHits(hits)], YAP_TAZ_summit[subjectHits(hits)])

distance_df <- table(summit_distance) %>%
  tibble::as_tibble() 

df_binned <- distance_df %>%
  mutate(summit_distance = as.numeric(summit_distance)) %>%
  arrange(summit_distance) %>%
  mutate(bin = floor(summit_distance / 5) * 5) %>%  # Create bins by grouping every 5 bp
  group_by(bin) %>%
  summarise(n = mean(n, na.rm = TRUE))  # Calculate average 'n' for each bin


# Export Fig. 1c
# distance_df %>%
#   mutate(summit_distance = as.numeric(summit_distance)) %>%
#   arrange(summit_distance) %>%
#   ggplot(aes(x=summit_distance, y = n)) +
#   geom_line()

png(file = file.path(figures_dir, "Fig_1c.png"))
df_binned %>%
  ggplot(aes(x=bin, y = n)) +
  geom_line() +
  scale_x_continuous(breaks = c(-250, 0, 250)) +
  xlab("distance to the summit \nof YAP peaks (bp)") +
  ylab("peak density") +
  theme_classic(base_size = 14)
garbage <- dev.off() # Returns 'null device 1'

### =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= ###

### Fig. 1d ###

if (!file.exists(file.path(peak_folder, "YAP_TAZ_TEAD4_common.bed"))) {
    export(YAP_overlap_TAZ_peaks_overlap_TEAD4, con = here(file.path(peak_folder, "YAP_TAZ_TEAD4_common.bed")))
}

# NOTE: Must run some commands in terminal between these steps (Check log.ipynb)

num_reads <- read_tsv(here(file.path(peak_folder, "YAP_TAZ_TEAD4_counts.tsv")), col_names = FALSE, show_col_types = FALSE)
colnames(num_reads) <- c("chr", "start", "end", "name", "score", "value", "YAP", "TAZ", "TEAD4")

# Normalise to 'reads per million' - total aligned reads obtained in terminal (check log.ipynb)
# NOTE: Total aligned reads are 23653961, 26789648 and 34332907 for YAP, TAZ and TEAD4 respectively
num_reads <- num_reads %>%
    mutate(YAP = YAP / 23653961 * 10^6,
        TAZ = TAZ / 26789648 * 10^6,
        TEAD4 = TEAD4 / 34332907 * 10^6)

# Export Fig. 1d (Currently as 2 separate files - Merge !!!!)
png(file = file.path(figures_dir, "Fig_1d_i.png"))
ggplot(num_reads, aes(x=TEAD4, y= YAP)) +
  geom_point(color = "#ff4000") +
  geom_smooth(method = "lm", se = FALSE, color = "black") +  # Linear regression line
  stat_poly_eq(
    aes(label = after_stat(rr.label)),
    formula = y ~ x,
    parse = TRUE,
    color = "black"
  ) +
  scale_x_continuous(trans = 'log2') +
  scale_y_continuous(trans = 'log2') +
  theme_classic(base_size = 14) +
  xlab("TEAD4 signal") +
  ylab("YAP signal")
garbage <- dev.off() # Returns 'null device 1'

png(file = file.path(figures_dir, "Fig_1d_ii.png"))
ggplot(num_reads, aes(x=TEAD4, y= TAZ)) +
  geom_point(color = "#ff4000") +
  geom_smooth(method = "lm", se = FALSE, color = "black") +  # Linear regression line
  stat_poly_eq(
    aes(label = after_stat(rr.label)),
    formula = y ~ x,
    parse = TRUE,
    color = "black"
  ) +
  scale_x_continuous(trans = 'log2') +
  scale_y_continuous(trans = 'log2') +
  theme_classic(base_size = 14) +
  xlab("TEAD4 signal") +
  ylab("TAZ signal")
garbage <- dev.off() # Returns 'null device 1'

### =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= ###

### Fig. 1f ###

# Get the TSS's
hg38_transcripts <- transcripts(TxDb.Hsapiens.UCSC.hg38.knownGene)
tss_gr <- promoters(hg38_transcripts, upstream = 0, downstream = 1)

YAP_dist<- mcols(distanceToNearest(YAP_peaks, tss_gr))$distance
TAZ_dist<- mcols(distanceToNearest(TAZ_peaks, tss_gr))$distance
TEAD4_dist<- mcols(distanceToNearest(TEAD4_peak, tss_gr))$distance

tss_distance_df<- bind_rows(
          data.frame(factor = "YAP", distance = YAP_dist),
          data.frame(factor = "TAZ", distance = TAZ_dist),
          data.frame(factor = "TEAD4", distance = TEAD4_dist))



counts_per_category<- tss_distance_df %>%
  mutate(category = case_when(
    distance < 1000 ~ "<1kb",
    distance >=1000 & distance < 10000 ~ "1-10kb",
    distance >= 10000 & distance <=100000 ~ "10-100kb",
    distance > 100000 ~ ">100kb"
  )) %>%
  group_by(factor, category) %>%
  count()

total_counts <- tss_distance_df %>%
  mutate(category = case_when(
    distance < 1000 ~ "<1kb",
    distance >=1000 & distance < 10000 ~ "1-10kb",
    distance >= 10000 & distance <=100000 ~ "10-100kb",
    distance > 100000 ~ ">100kb"
  )) %>%
  count(factor, name = "total")

merged_df <- left_join(counts_per_category, total_counts)
merged_df$category <- factor(merged_df$category, levels = c("<1kb", "1-10kb", "10-100kb", ">100kb"))

# Export Fig. 1f
png(file = file.path(figures_dir, "Fig_1f.png"))
merged_df %>%
  mutate(Percentage = n/total * 100) %>%
  ggplot(aes(x= factor, y = Percentage, fill = category)) +
  geom_bar(stat = "identity", position = "stack") +
  labs(
    title = "Distance to TSS",
    x = "Group",
    y = "Percentage"
  ) +
  scale_y_continuous(labels = scales::percent_format(scale = 1)) +
  scale_fill_manual(values = c("#EF3E2B", "#F16161", "#F59595", "#FAD1C8")) +
  theme_classic(base_size = 14)
garbage <- dev.off() # Returns 'null device 1'

### =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= ###

### Fig. 1g ###

### =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= ###

### Fig. 1h ###

public_data_folder = "public_data/"
H3K4me1<- import(here(file.path(public_data_folder, "H3K4me1.bed")))
H3K4me3<- import(here(file.path(public_data_folder, "H3K4me3.bed")))
H3K27ac<- import(here(file.path(public_data_folder, "H3K27ac.bed")))

suppressWarnings({
    active_enhancers <- subsetByOverlaps(H3K4me1, H3K27ac)
    inactive_enhancers <- subsetByOverlaps(H3K4me1, H3K27ac, invert = TRUE)
    promoters <- subsetByOverlaps(H3K4me3, H3K4me1, invert = TRUE)

    n_active_enhancers <- length(subsetByOverlaps(YAP_overlap_TAZ_peaks_overlap_TEAD4, active_enhancers))
    n_inactive_enhancers <- length(subsetByOverlaps(YAP_overlap_TAZ_peaks_overlap_TEAD4, inactive_enhancers))
    n_promoters <- length(subsetByOverlaps(YAP_overlap_TAZ_peaks_overlap_TEAD4, promoters))
})

n_unclassified <- length(YAP_overlap_TAZ_peaks_overlap_TEAD4) - n_active_enhancers - n_inactive_enhancers - n_promoters


annotation_df <- data.frame(
                    category = c("active_enhancers", "inactive_enhancers", "promoters", "unclassified"),
                    peak_number = c(n_active_enhancers, n_inactive_enhancers, n_promoters, n_unclassified)
                 )

annotation_df$category <- factor(
                            annotation_df$category, 
                            levels = c("promoters", "active_enhancers", "inactive_enhancers", "unclassified")
                          )

# Calculate percentages and cumulative positions for labeling
annotation_df <- annotation_df %>%
  dplyr::mutate(
    percentage = peak_number / sum(peak_number) * 100,
    label = paste0(round(percentage, 1), "%")
  )

# Create the pie chart
png(file = file.path(figures_dir, "Fig_1h.png"))
ggplot(annotation_df, aes(x = "", y = peak_number, fill = category)) +
  geom_bar(stat = "identity", width = 1) + 
  coord_polar("y", start = 0) +
  theme_void() + # Remove unnecessary axes
  labs(title = "YAP/TAZ/TEAD4 peaks") +
  scale_fill_manual(values = c("#8D1E0F", "#F57D2B", "#FADAC4", "#D4DADA")) +
  geom_text(aes(label = label), position = position_stack(vjust = 0.5)) # Add percentage labels
garbage <- dev.off() # Returns 'null device 1'
