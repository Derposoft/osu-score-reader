import os
import hashlib
import pandas as pd
from osu_map_parser.osu_map_parser.map_parser import MapParser
from osrparse import parse_replay_file, GameMode, Mod

# -2. create player name -> team name
player_to_teamname = {}
with open('teams.csv', 'r', encoding='utf-8-sig') as teams:
    teams = teams.readlines()
    for team in teams:
        team = [x.strip() for x in team.split(',') if x.strip() != '']
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
    if int(map.version) < 11: # TODO FIX!! IGNORING ALL OLD OSU MAP FILES!
        continue
    map_id = map.metadata['BeatmapSetID'] + '#osu/' + map.metadata['BeatmapID']
    mapmd5_to_id[map_md5] = (map_id, map.metadata['Title'])

# 1. extract zips
zips = os.listdir('replays')
for zip in zips:
    if zip.endswith('.zip') or zip.endswith('.rar'):
    #    if str(os.name) == 'nt':
    #        os.system('C:"Program Files"\\7-Zip\\7z.exe e ' + '.\\replays\\' + zip + ' -o ".\\replaysx\\" -aoa')
    #    else:
        os.system('7z e "./replays/"' + zip +  ' -o"./replaysx/" -aoa')
    else:
        os.rename('./replays/' + zip, './replaysx/' + zip)

# 2. parse each replay and add score to results if valid replay
replays = os.listdir('replaysx')
maps_to_team = {} # maps map -> { team -> { player: score } }
for r in replays:
    # 2.0. parse replay file
    replay = parse_replay_file('replaysx/' + r)
    player = replay.player_name
    # 2.1. check game mode
    if replay.game_mode != GameMode.STD:
        print(player + '\'s replay isn\t osu!std. SKIPPING...')
        continue
    # 2.2. check if replay is for map in pool
    replay_map_hash = replay.beatmap_hash
    if not replay_map_hash in mapmd5_to_id:
        print(player + ' submitted a map not in the pool! SKIPPING...')
        continue
    # 2.3. check for scorev2 (for debug prints)
    replay_map_id = mapmd5_to_id[replay_map_hash][0]
    replay_map_name = mapmd5_to_id[replay_map_hash][1]
    allowed_mods = map_to_mods[replay_map_id]
    replay_mods = replay.mod_combination
    if not (replay_mods & Mod.ScoreV2):
        print('SCOREv2 NOT USED FOR THIS MAP. SKIPPING...')
        continue
    # 2.4. check the mods of the replay against allowed mods
    MOD_ERROR_MSG = player + ' submitted ' + replay_map_name + ' with wrong map mods! SKIPPING...'
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
    # 2.5. collect score and add to result
    if player not in player_to_teamname:
        print(player + ' isn\'t even a player... why did they submit?? SKIPPING...')
        continue
    player_name = player
    team_name = player_to_teamname[player]
    score = replay.score
    map_name = replay_map_name
    if map_name not in maps_to_team:
        maps_to_team[map_name] = {}
    if team_name not in maps_to_team[map_name]:
        maps_to_team[map_name][team_name] = {}
    maps_to_team[map_name][team_name][player_name] = score
print(maps_to_team)
# 3. output all results as a csv
output = pd.DataFrame()
for map in maps_to_team:
    # 3.1. for each team, get top scorer
    for team in maps_to_team[map]:
        top_player_score = 0
        for player in maps_to_team[map][team]:
            if maps_to_team[map][team][player] > top_player_score:
                top_player_score = maps_to_team[map][team][player]
        output = output.append([[map, team, player, top_player_score]])
# if there are any outputs
if len(output):
    output.to_csv('outputs.csv', index=False, header=['map', 'team', 'player', 'score'])
