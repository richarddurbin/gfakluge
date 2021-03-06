#ifndef TINY_FA_HPP
#define TINY_FA_HPP

#pragma once
#define _FILE_OFFSET_BITS 64
#ifdef WIN32
#define ftell64(a)     _ftelli64(a)
#define fseek64(a,b,c) _fseeki64(a,b,c)
typedef __int64 off_type;
#elif defined(__APPLE__) || defined(__FreeBSD__)
#define ftell64(a)     ftello(a)
#define fseek64(a,b,c) fseeko(a,b,c)
typedef off_t off_type;
#else
#define ftell64(a)     ftello(a)
#define fseek64(a,b,c) fseeko(a,b,c)
typedef __off64_t off_type;
#endif



#include <string>
#include <sstream>
#include <ostream>
#include <cstdio>
#include <vector>
#include <fstream>
#include <unordered_map>
#include <map>
#include <cstdint>
#include <assert.h>
#include <algorithm>
#include <sys/stat.h>
#include "pliib.hpp"


namespace TFA{

    // From https://stackoverflow.com/questions/4157687/using-char-as-a-key-in-stdmap
struct custom_char_comparator
{
   bool operator()(char const *a, char const *b) const
   {
      return std::strcmp(a, b) < 0;
   }
};





struct tiny_faidx_entry_t {
    char* name;
    int name_len;
    int32_t line_char_len;
    int32_t line_byte_len;
    int64_t seq_len;
    int64_t raw_len;
    uint64_t offset;
    tiny_faidx_entry_t(){
        line_char_len = -1;
        line_byte_len = 0;
        seq_len = 0;
        raw_len = 0;
        offset = -1;
    };
    tiny_faidx_entry_t(std::vector<std::string> splits){
        assert(splits.size() == 5);
        this->name_len = splits[0].length();
        this->name = new char[this->name_len + 1];
        memcpy(this->name, splits[0].c_str(), splits[0].length() * sizeof(char));
        this->seq_len = std::stol(splits[1]);
        this->offset = std::stoull(splits[2]);
        this->line_char_len = std::stoi(splits[3]);
        this->line_byte_len = std::stoi(splits[4]);
    };
    // Five columns
    // seqname  seqlen  offset  line_char_len   line_byte_len
    std::string to_string(){
        std::stringstream st;
        st << name << '\t' << seq_len << '\t' << offset << '\t' << line_char_len << '\t' << line_byte_len << endl;
        return st.str();
    };
    void write_to_stream(std::ostream& os){
        os << name << '\t' << seq_len << '\t' << offset << '\t' << line_char_len << '\t' << line_byte_len << endl;
    };
};

struct custom_faidx_entry_t_comparator
{
   bool operator()(tiny_faidx_entry_t const *a, tiny_faidx_entry_t const *b) const
   {
      return a->offset < b->offset;
   }
};

struct tiny_faidx_t{
    std::map<char*, tiny_faidx_entry_t*, custom_char_comparator> seq_to_entry;
    FILE* fasta;
    void close(){
        for (auto k : seq_to_entry){
            delete k.second->name;
            delete k.second;
        }
    };

    void add(tiny_faidx_entry_t* entry){
        seq_to_entry[entry->name] = entry;
    };
    ~tiny_faidx_t(){
        close();
    };

    bool hasSeqID(const char* s) const {
        return (seq_to_entry.count( (char*) s) != 0);
    };

    void get(const char* seqname, tiny_faidx_entry_t*& entry) const {
        entry = seq_to_entry.at((char*) seqname);
    };

    void write(ostream& os) const {
        std::vector<tiny_faidx_entry_t*> sorted_entries;
        for (auto x : seq_to_entry){
                sorted_entries.push_back(x.second);
        }
        std::sort(sorted_entries.begin(), sorted_entries.end(), custom_faidx_entry_t_comparator());
        for (auto x : sorted_entries){
            x->write_to_stream(os);
        }
    };

    void write(const char* filename) const{
        std::ofstream ofi;
        ofi.open(filename);
        if (ofi.good()){
            write(ofi);
        }

    };

} ;


// typedef struct {
//     std::unordered_map<char*, char*, custom_char_comparator> name_to_seq;

//     tiny_fasta_map_t(){

//     }

//     tiny_fasta_map_t(const char* filename){

//     }

//     void get(const char* name, char*&s){
//         if (name_to_seq.count(name) != 0){
//             s = name_to_seq[name];
//         }
//     };

//     void put(const char* name, const char* s){
//         name_to_seq[name] = s;
//     }
// } tiny_fasta_map_t;


// inline void parseFasta(const char* filename, tiny_faidx_t& fai, tiny_fasta_map_t& fam){

// };

inline void createFAIndex(const char* fastaName, tiny_faidx_t& fai){
    
    uint64_t line_number = 0;
    int32_t line_length = 0;
    uint64_t offset = 0;
    
    std::string line;

    std::ifstream faFile;
    faFile.open(fastaName);

    tiny_faidx_entry_t* entry = new tiny_faidx_entry_t();

    if (faFile.is_open()){
        while(std::getline(faFile, line)){
            ++line_number;
            line_length = line.length();
            if (line[0] == '>' || line[0] == '@'){
                // This is a valid fasta name line
                // Start a new entry and begin filling it.
                if (entry->name_len != 0){
                    fai.add(entry);
                    entry = new tiny_faidx_entry_t();
                }
                line = line.substr(1, line_length);
                char* name = new char[line_length];
                std::strcpy(name, line.c_str());
                pliib::strip(name, line_length - 1, ' ');
                pliib::trim_after_char(name, strlen(name), ' ');
                //cerr << name << endl;
                entry->name_len = std::strlen(name);
                entry->name = new char[entry->name_len];
                std::strcpy(entry->name, name);

            }
            else if (line[0] == '+'){
                std::getline(faFile, line);
                line_length = line.length();
                offset += line_length + 1;
                std::getline(faFile, line);
                line_length = line.length();
                offset += line_length + 1;
            }
            else{
                if (entry->offset == -1){
                    entry->line_char_len = line_length;
                    entry->line_byte_len = line_length + 1;
                    entry->offset = offset;
                }

                entry->seq_len += line_length;
                entry->raw_len += line_length + 1;
            }
            offset += line_length + 1;
        }
        if (entry->seq_len >= 0){
            fai.add(entry);
        }
    }
};

inline void writeFAIndex(const char* fastaName, const tiny_faidx_t& fai){
    // Create index outfile with the correct name
    std::string fn(fastaName);
    fn = fn + ".fai";
    std::ofstream ofi(fn.c_str());
    if (ofi.good()){
        fai.write(ofi);
    }
};

inline bool checkFAIndexFileExists(const char* fastaName){
    struct stat statFileInfo; 
    string indexFileName(fastaName);
    indexFileName = indexFileName + ".fai"; 
    return stat(indexFileName.c_str(), &statFileInfo) == 0;
};

inline char* indexFileName(const char* fastaName){
    int len = strlen(fastaName);
    static const char* file_ext = ".fai";
    char* ret = new char[len + 5];
    ret[len + 4] = '\0';
    strcpy(ret, fastaName);
    strcpy(ret + len, file_ext);
    return ret;
};

inline void parseFAIndex(const char* fastaFileName, tiny_faidx_t& fai){
    std::ifstream ifi;
    char* ifn = indexFileName(fastaFileName);
    ifi.open((const char*) ifn);

    if (!(fai.fasta = fopen(fastaFileName, "r"))){
        cerr << "Error: couldn't open fasta file " << fastaFileName << endl;
    }

    if (ifi.is_open()){
        
        string line;
        while(std::getline(ifi, line)){
            vector<string> splits = pliib::split(line.c_str(), '\t');
            tiny_faidx_entry_t* t = new tiny_faidx_entry_t(splits);
            fai.add(t);
        }
    }
    else{
        cerr << "Couldn't open index " << ifn << "." << endl;
    }
    delete ifn;
};

inline void getSequenceLength(const tiny_faidx_t& fai, const char* seqname, uint32_t& length){

    tiny_faidx_entry_t* entry;
    if (fai.hasSeqID(seqname)){
        fai.get(seqname, entry);
        length = entry->seq_len;
    }
};

inline void getSequence( const tiny_faidx_t& fai, const char* seqname, char*& seq){
    uint32_t sz = 0;
    
    tiny_faidx_entry_t* entry;
    if (fai.hasSeqID(seqname)){
        fai.get(seqname, entry);
        sz = entry->seq_len + 1;
        seq = new char[sz];
        seq[sz - 1] = '\0';
        fseek64(fai.fasta, entry->offset, SEEK_SET);
        if (fread(seq, sizeof(char), entry->seq_len, fai.fasta)){
            pliib::remove_nulls_and_whitespace(seq, entry->seq_len);
        }
    }
    else{
        cerr << "No sequence found for ID: " << seqname << "." << endl;
    }

};

inline void getSequence( const tiny_faidx_t& fai, const char* seqname,
                         char*& seq, int start, int end){
    getSequence(fai, seqname, seq);
    end = min(end, (int) strlen(seq));
    start = max(0, (int) start);
    char* ret = new char[end - start + 1];
    ret[end - start] = '\0';
    memcpy(ret, seq + start, (end - start) * sizeof(char) );
    delete seq;
    seq = ret;
};

}

#endif
