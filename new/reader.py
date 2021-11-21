import os
import sys
import csv
import hashlib
import numpy as np
import pandas as pd
from osu_map_parser.osu_map_parser.map_parser import MapParser
from osrparse import parse_replay_file, GameMode, Mod

# -2. create player name -> team name
player_to_teamname = {}
with open('teams.csv', 'r') as teams:
    teams = teams.readlines()
    for team in teams:
        team = team.split(',')
        teamname = team[0]
        players = team[1:]
        for player in players:
            player_to_teamname[player] = teamname

# -1. create map ids -> allowed mods
mapmods = pd.read_csv('mapmods.csv', header=None)
map_to_mods = {}
for i in range(len(mapmods)):
    map_mod = mapmods.iloc[i]
    map_to_mods[map_mod[0]] = map_mod[1]

# 0. create md5 -> map ids
maps = os.listdir('maps')
mapmd5_to_id = {}
for m in maps:
    map_md5 = hashlib.md5(open('maps/' + m,'rb').read()).hexdigest()
    map = MapParser('maps/' + m)
    print(map.version)
    if int(map.version) < 11:
        continue
    map_id = map.metadata['BeatmapSetID'] + '#osu/' + map.metadata['BeatmapID']
    mapmd5_to_id[map_md5] = map_id

# 1. extract zips
zips = os.listdir('replays')
for zip in zips:
    os.system('7z e "./replays/"' + zip +  ' -o"./replaysx/" -aoa')

# 2. parse each map
replays = os.listdir('replaysx')
maps_to_team = {} # maps map -> { team -> { player: score } }
for r in replays:
    # 2.0. parse replay file
    replay = parse_replay_file('replaysx/' + r)
    # 2.1. check game mode
    if replay.game_mode != GameMode.STD:
        print('MAP NOT IN OSU!STD. SKIPPING...') # TODO
        continue
    # 2.2. check if replay is for map in pool
    replay_map_hash = replay.beatmap_hash
    if not replay_map_hash in mapmd5_to_id:
        print('REPLAY NOT FOR A MAP IN POOL! SKIPPING...')
        continue
    # 2.3. check for scorev2 (for debug prints)
    replay_map_id = mapmd5_to_id[replay_map_hash]
    allowed_mods = map_to_mods[replay_map_id]
    replay_mods = replay.mod_combination
    if not (replay_mods & Mod.ScoreV2):
        print('SCOREv2 NOT USED FOR THIS MAP. SKIPPING...')
        continue
    # 2.4. check the mods of the replay against allowed mods
    MOD_ERROR_MSG = 'INCORRECT MODS FOR MAP! SKIPPING...'
    def mod_check(mods, mod_flag):
        # we allow nf and require scorev2 for ALL mod flags. mod flag = 0 for NM
        if mods != (Mod.ScoreV2 | mod_flag) or mods != (Mod.ScoreV2 | Mod.NoFail | mod_flag):
            return True
        return False
    if allowed_mods == 'NM':
        if not mod_check(replay_mods, 0):
            print(MOD_ERROR_MSG)
            continue
    if allowed_mods == 'HD':
        if not mod_check(replay_mods, Mod.Hidden):
            print(MOD_ERROR_MSG)
            continue
    if allowed_mods == 'HR':
        if not mod_check(replay_mods, Mod.HardRock):
            print(MOD_ERROR_MSG)
            continue
    if allowed_mods == 'DT':
        if not mod_check(replay_mods, Mod.DoubleTime):
            print(MOD_ERROR_MSG)
            continue
    if allowed_mods == 'FM':
        # anything is allowed and we've already checked for scorev2, so we don't have to do anything.
        pass
    # 2.5. collect score and add to db
    player = replay.player_name
    if player not in player_to_teamname:
        print('PLAYER IS NOT IN A TEAM! WHY DID THIS GUY SUBMIT?? SKIPPING...')
        continue
    
    sys.exit()




    

