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

### =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= ###

### Import Modules ###

# File Handling
suppressPackageStartupMessages({
    library(rtracklayer) # for reading in bed file
    library(here)
    library(GenomicRanges)
    library(Vennerable)
    library(dplyr)
    library(ggplot2)
})

### =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= ###

### Preparing Fig. 1a ###

peak_folder <- "peak_reports/"

# 'GRanges' object
TAZ_peaks <- import(here(file.path(peak_folder, "SRR1810907_peak/SRR1810907_peaks.narrowPeak"))) # TAZ_1
YAP_peaks <- import(here(file.path(peak_folder, "SRR1810900_peak/SRR1810900_peaks.narrowPeak"))) # YAP_1

# TAZ_overlap_YAP_peaks <- subsetByOverlaps(TAZ_peaks, YAP_peaks)
# length(TAZ_overlap_YAP_peaks)

# NOTE: Warnings suppressed due 3 'sequence levels' being present in only 1 (not both)
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

### =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= ###

### Preparing Fig. 1b ###

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

### =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= ###

### Preparing Fig. 1c ###

TEAD4_summit <- import(here(file.path(peak_folder, "SRR1810918_peak/SRR1810918_summits.bed")))
YAP_summit <- import(here(file.path(peak_folder, "SRR1810900_peak/SRR1810900_summits.bed")))

YAP_TAZ_summit <- YAP_summit[YAP_summit$name %in% YAP_overlap_TAZ_peaks$name]

YAP_TAZ_500bp_window <- resize(YAP_TAZ_summit, width = 500, fix = "center")

hits <- findOverlaps(TEAD4_summit, YAP_TAZ_500bp_window)


# Compute signed distances
signed_distance <- function(A, B) {
  dist <- GenomicRanges::distance(A, B)
  sign <- ifelse(start(A) < start(B), -1, 1)
  dist * sign
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

### =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-= ###

### Exporting Figures ###

figures_dir = "Figures"
# Create the directory (if needed)
if (!file.exists(figures_dir)) {
    dir.create(figures_dir)
}

# Fig. 1a
png(file = file.path(figures_dir, "Fig_1a.png"))
plot(venn_YAP_TAZ)
garbage <- dev.off() # Returns 'null device 1'

# Fig. 1b
png(file = file.path(figures_dir, "Fig_1b.png"))
plot(venn_YAP_TAZ_TEAD4)
garbage <- dev.off() # Returns 'null device 1'

# Fig. 1c
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

### MAYBE DO PDF's? Which one is better?