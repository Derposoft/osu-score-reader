#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

void printerror(std::string file) {
    printf("ERROR PROCESSING: %s\n", file.c_str());
    exit(0);
}

std::string readstring(FILE* fp) {
    std::string result = "";
    char exists[1];
    fread(&exists, sizeof(exists), 1, fp);
    if (exists[0] == 0x00) {
        return result;
    }
    // parse uleb128
    int mask = 0b01111111;
    int size = 0;
    int shift = 0;
    while (1) {
        char currbyte[1];
        fread(&currbyte, 1, 1, fp);
        size |= (currbyte[0] & mask) << shift;
        if (currbyte[0] >> 7 == 0x00)
            break;
        shift += 7;
    }
    char res[size + 1];
    fread(&res, size, 1, fp);
    res[size] = '\0';
    return std::string(res);
}

int main(int argc, char* argv[]) {
    std::string path;
    if (argc != 2)
        exit(0);
    else
        path = argv[1];
    DIR* dir = opendir(path.c_str());
    if (!dir) {
        printf("Error opening directory. Exiting");
        exit(0);
    }
    struct dirent* currsong;
    struct stat currstat;
    int currmode = 0;
    int currscore = 0;
    std::string playername = "";
    std::vector<std::string>* scores = new std::vector<std::string>();
    // begin reading each folder
    while ((currsong = readdir(dir)) != NULL) {
        stat(currsong->d_name, &currstat);
        if (S_ISDIR(currstat.st_mode))
            continue;

        // looping through each of the player's songs
        std::string filename(currsong->d_name);
        filename = path + "\\" + filename;
        FILE* fp = fopen(filename.c_str(), "rb");
        if (fp == nullptr) {
            printf("ERROR OPENING: %s\n", filename.c_str());
            exit(0);
        }
        
        // check gamemode
        char mode[1];
        fread(&mode, 1, 1, fp);
        if (mode[0] != 0)
            printerror(path);
        // version
        int ver;
        fread(&ver, sizeof(ver), 1, fp);
        // hash
        std::string shash = readstring(fp);
        // playername
        if (playername == "")
            playername = readstring(fp);
        else if (readstring(fp) != playername) {
            printf("Player has submitted replays of multiple people! Exiting");
            exit(0);
        }
        // replay hash
        std::string replayhash = readstring(fp);
        // num notes
        short num300, num100, num50, numgeki, numkatu, nummiss;
        fread(&num300, sizeof(num300), 1, fp);
        fread(&num100, sizeof(num300), 1, fp);
        fread(&num50, sizeof(num300), 1, fp);
        fread(&numgeki, sizeof(num300), 1, fp);
        fread(&numkatu, sizeof(num300), 1, fp);
        fread(&nummiss, sizeof(num300), 1, fp);
        // total score
        int score;
        fread(&score, sizeof(score), 1, fp);
        // max combo
        short maxcombo;
        fread(&maxcombo, sizeof(maxcombo), 1, fp);
        // fc?
        char fc;
        fread(&fc, sizeof(fc), 1, fp);
        // mods?
        int mods;
        fread(&mods, sizeof(mods), 1, fp);
        if ((mods & 536870912) == 0) {
            printf("%s has not used scorev2 on a map! Scores ignored.\n", playername.c_str());
            exit(0);
        }

        currscore += score;
        // build score report
        std::string reportstring(currsong->d_name);
        reportstring += ": ";
        reportstring += std::to_string(score);
        scores->push_back(reportstring);
    }
    for (int i = 0; i < scores->size(); i++) {
        printf("%s\n", (*scores)[i].c_str());
    }
    printf("done processing. total score of %s: %d\n", playername.c_str(), currscore);
}