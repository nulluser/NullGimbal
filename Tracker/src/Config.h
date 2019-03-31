/*
    Gimbal Tracker

    (C)2018 Jason Hunt nulluser@gmail.com

    FILE: Config.h
*/


#ifndef CONFIG_H
#define CONFIG_H

#include <string>
#include <vector>

using namespace std;

// Module local
#ifdef MODULE_CONFIG
#define MODULE "[Config]   "
static string toLower(std::string s);
#endif


class ConfigEntry
{
  public:
    ConfigEntry(string header, string name, string value) : header(header), name(name), value(value) {};

    string header, name, value;
};


class Config
{
  public:
    Config(const char *config);
    string getValue(string header, string name);

    bool getInt(string header, string name, int &int_vale, int def);
    bool getFloat(string header, string name, float &float_val, float def);
    void showConfig(void);

  private:
    bool loadConfig(void);
    void addValue(string header, string name, string value);
    std::vector<ConfigEntry> config;
    const char *filename;

};

#endif

