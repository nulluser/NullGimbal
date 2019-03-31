/*
    Gimbal Tracker

    (C)2018 Jason Hunt nulluser@gmail.com

    FILE: Config.cpp
*/

#define MODULE_CONFIG

#include <fstream>
#include <algorithm>

#include "Config.h"
#include "Utility.h"


using namespace std;

/*
    Interface
*/



Config::Config(const char *filename): filename(filename)
{
    loadConfig();
    showConfig();
}


/* Load config file */
bool Config::loadConfig(void)
{
    console(MODULE "Loading config from %s\n", filename);

    ifstream file(filename);

    string header;

    for( string line; getline( file, line ); )
    {
        if (line.length() == 0) continue;

        //alert(MODULE "Line: %s\n", line.c_str());

        // Check for new header
        int p1 = line.find('['), p2 = line.find(']');

        // Is header?
        if (p1 != -1 && p2 != -1)
        {
            header = line.substr(p1 + 1, p2 - p1 - 1);
            continue;
        }

        int index = 0;
        string name, value;

        // Get name
        while(line[index] && line[index] != ' ' && line[index] != '\t' && line[index] != '#') name += line[index++];

        // Eat white
        while(line[index] && (line[index] == ' ' || line[index] == '\t')) index++;

        // Get value
        while(line[index] && line[index] != ' ' && line[index] != '\t') value += line[index++];

        addValue(header, name, value);
    }




    return false;
}



/* Add config Value */
void Config::addValue(string header, string name, string value)
{
    //console(MODULE "[%s] (%s) = (%s)\n", header.c_str(), name.c_str(), value.c_str());

    if (name == "") return;
    if (value == "") return;

    config.push_back(ConfigEntry(toLower(header), toLower(name), toLower(value)));
}


/* Show config data */
void Config::showConfig(void)
{
    console(MODULE "[Config]\n");

    for (size_t i = 0; i < config.size(); i++)
    {
        console(MODULE "%s::%-20s\t%s\n", config[i].header.c_str(),  config[i].name.c_str(),  config[i].value.c_str());
    }
}


/* Get config value */
string Config::getValue(string header, string name)
{
    header = toLower(header);
    name = toLower(name);

    for (size_t i = 0; i < config.size(); i++)
    {
        if (config[i].header == header && config[i].name == name)
            return config[i].value;
    }

    return "";
}


/* Get config value */
bool Config::getInt(string header, string name, int &val, int def)
{
    string s= getValue(header, name);

    val = def;

    if (s == "") return true;

    try
    {
        val = stoi(s);
    }
    catch(exception e)
    {
        return true;
    }

    return false;
}



/* Get config value */
bool Config::getFloat(string header, string name, float &val, float def)
{
    string s = getValue(header, name);
    val = def;

    if (s == "") return true;

    try
    {
        val = stof(s);
    }
    catch(exception e)
    {
        return true;
    }

    return false;
}


/* Helper, convert to lower */
static string toLower(string s)
{
    string up = s;
    transform(up.begin(), up.end(), up.begin(), ::tolower);
    return up;
}









