#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <sstream>

//#include <openssl/md5.h>

// print an error!
void printerror(std::string file) {
    printf("ERROR PROCESSING: %s\n", file.c_str());
    exit(0);
}
// reads a string from a file as specified in the ".osr file format" page on osu.ppy.sh
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
// Get the size of the file by its file descriptor
unsigned long get_size_by_fd(int fd) {
    struct stat statbuf;
    if(fstat(fd, &statbuf) < 0) exit(-1);
    return statbuf.st_size;
}
/* return the md5 hash of a file given the filepath. code from 
    https://stackoverflow.com/questions/1220046/how-to-get-the-md5-hash-of-a-file-in-c 
    and adapted for my use.

std::string getMD5ofFile(std::string path) {
    int file_descript;
    unsigned long file_size;
    char *file_buffer;

    file_descript = open(path.c_str(), O_RDONLY);
    if (file_descript < 0)
        exit(-1);

    file_size = get_size_by_fd(file_descript);
    printf("file size:\t%lu\n", file_size);

    file_buffer = (char*)malloc(file_size * sizeof(char));//mmap(0, file_size, PROT_READ, MAP_SHARED, file_descript, 0);
    read(file_descript, file_buffer, file_size);
    char* md5_out;
    MD5((unsigned char *)file_buffer, file_size, (unsigned char *)md5_out);
    //munmap(file_buffer, file_size);
    free(file_buffer);
    std::string result = "";
    char resbuf[1];
    for (int i = 0; i < 16; i++) {
        sprintf(resbuf, "%x", md5_out[i]);
        result.append(resbuf);
    }
    return result;
}*/

std::string to_string_patch(int val) {
    std::stringstream ss;
    ss << val;
    return ss.str();
}

int main(int argc, char* argv[]) {
    // read in all map file names from argv input
    std::string inputs = "";
    for (int i = 2; i < argc; i++)
        inputs.append(argv[i]).append(" ");
    inputs = inputs.substr(0, inputs.size() - 1); // cut off final space
    // split input by ':::' (assuming maps don't have this sequence of characters. otherwise we screwed.)
    std::vector<std::string> map_ins; // order: dt, fm, hd, hr, nm
    std::string delimiter = ":::";
    size_t pos = 0;
    std::string token;
    while ((pos = inputs.find(delimiter)) != std::string::npos) {
        token = inputs.substr(0, pos);
        map_ins.push_back(token);
        inputs.erase(0, pos + delimiter.length());
    }
    // debug print for problems
    std::string dt_map = map_ins[0];
    std::string fm_map = map_ins[1];
    std::string hd_map = map_ins[2];
    std::string hr_map = map_ins[3];
    std::string nm_map = map_ins[4];
    // hash computations of beatmaps
    /*
    std::string dt_hash = getMD5ofFile("./maps/dt/" + dt_map);
    std::string fm_hash = getMD5ofFile("./maps/freemod/" + fm_map);
    std::string hd_hash = getMD5ofFile("./maps/hd/" + hd_map);
    std::string hr_hash = getMD5ofFile("./maps/hr/" + hr_map);
    std::string nm_hash = getMD5ofFile("./maps/nomod/" + nm_map);

    printf("hash test output: %s\n", dt_hash.c_str());
    */

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
        if (fp == NULL) {
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

        reportstring += to_string_patch(score);
        scores->push_back(reportstring);
    }
    for (int i = 0; i < scores->size(); i++) {
        printf("%s\n", (*scores)[i].c_str());
    }
    printf("done processing. total score of %s: %d\n", playername.c_str(), currscore);
}