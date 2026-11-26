-- @Block
-- Import Data --

-- Shockingly difficult to import a .csv file
-- Used Python to format data and then import

-- Tried this after creating the table (but failed):
-- SET GLOBAL local_infile = 1;

-- LOAD DATA LOCAL INFILE
-- "spotify_data.csv"
-- INTO TABLE spotify_data
-- COLUMNS TERMINATED BY ','
-- LINES TERMINATED BY '\n';

-- @Block
-- Size of Dataset --

SELECT 
    COUNT(track_id) AS total_tracks,
    COUNT(DISTINCT track_genre) AS total_genres
FROM songs;

-- @Block
-- Most Popular Songs --

SELECT 
    album_name,
    AVG(popularity) AS avg_popularity
FROM songs
GROUP BY album_name
ORDER BY avg_popularity DESC
LIMIT 10;

-- @Block
-- Reset --

-- DROP TABLE songs