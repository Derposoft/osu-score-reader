import os

os.makedirs('maps', exist_ok=True)
os.makedirs('replays', exist_ok=True)
os.makedirs('replaysx', exist_ok=True)

with open('mapmods.csv', 'w') as mapmods:
    mapmods.write('[BeatmapSetID]#osu/[BeatmapID],[NM/HD/HR/DT/FM]')
with open('teams.csv', 'w') as teams:
    teams.write('test team,Derposoft,Duhop')

