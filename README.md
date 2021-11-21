# Hello friend!

This is a score reader/parser for [Duhop](https://osu.ppy.sh/users/Duhop)'s "Retry Paradise 2" osu tourname [here](https://osu.ppy.sh/community/forums/topics/1461824?n=1)! The instructions to use this are simple:

1. run a recursive-submodule git clone like so:
git clone --recurse-submodules https://github.com/Derposoft/osu-score-reader.git

2. download relevant dependencies:
pip install pandas
pip install osrparse

3. change directory into ./new and run setup.py:
cd new/
python3 setup.py

4. fill in teams.csv and mapmods.csv with teams and map mods as outlined in the example lines

5. throw all .osr replays and zip/rar osu! replay files into the replays/ folder (not the replaysx/ folder -- just ignore that one :heh:)

6. run reader.py

7. output.csv file should be outputted in current directory!


disclaimers:
- the old score reader only works on windows, but the new one does not!
- 7zip must still be installed
~~- only works on windows.
- requires 7zip to be installed into default directory (Program Files)
~~
The scores of each player for each song are outputted and also totalled after the following steps. This program was created for the sole purpose of simplifying the scoring process for the retry paradise osu! tournament. The input to the program are a set of archives of the replays of a given player; e.g. a  file replays.rar or replays.zip which expands into:

replays.rar -> {
    replay1.osr,
    replay2.osr, ...
}

osu! is a popular circle-clicking rhythm game. Documentation on the .osr osu! replay file format can be found at https://osu.ppy.sh/help/wiki/osu!_File_Formats/Osr_(file_format) and more information about the game can be found at https://osu.ppy.sh/home.

Step 1: put all compressed replay archives (zips or rars) into "zips"

Step 2: click run.bat

Step 3: ????

Step 4: profit

