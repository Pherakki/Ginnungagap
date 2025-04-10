#pragma once

#include <fstream>
#include <filesystem>
#include <set>
#include <string>
#include <vector>
#include <GinnungagapCore/utils/Paths.hpp>
#include <GinnungagapCore/utils/StringUtils.hpp>

using CSVRow = std::vector<std::string>;

struct CSVFile
{
private:
    std::vector<CSVRow> rows;
public:
    auto begin() { return rows.begin(); }
    auto end  () { return rows.end  (); }

    CSVRow& operator[](size_t idx) { return this->rows[idx]; }
    size_t rowCount() { return this->rows.size(); }

    static std::vector<std::filesystem::path> getCSVFiles(const std::filesystem::path& path)
    {
        std::vector<std::filesystem::path> out;
        for (const auto& f : getFiles(path))
        {
            if (stringmanip::toLower(f.extension().string()) == ".csv")
                out.push_back(f);
        }
        return out;
    }

    static CSVFile fromFile(const std::filesystem::path& path)
    {
        std::set<char> delimiters = {','};

        CSVFile out;
        std::ifstream reader(path);

        std::stringstream buf;
        buf << reader.rdbuf();
        std::string strbuf = buf.str();

        bool quoted_cell      = false;
        bool init_new_cell    = false;
        
        CSVRow cur_row;
        std::string cur_cell;
        for (int idx=0; idx < strbuf.size(); ++idx)
        {
            char c = strbuf[idx];
            if (init_new_cell)
            {
                quoted_cell = c == '\"';
                init_new_cell = false;
                if (quoted_cell)
                    continue;
            };

            if (quoted_cell)
            {
                // Hit a quote
                if (c == '\"')
                {
                    // TODO: perform any whitespace-eating here if allowed
                    if (idx+1 < strbuf.size())
                    {
                        idx++;
                        char next_char = strbuf[idx];

                        if (testEndOfCellOrRow(next_char, delimiters, cur_cell, cur_row, out, init_new_cell, idx, strbuf))
                            continue;
                        else
                            idx--;
                    }
                }
                cur_cell += c;
            }
            else
            {
                if (!testEndOfCellOrRow(c, delimiters, cur_cell, cur_row, out, init_new_cell, idx, strbuf))
                {
                    cur_cell += c;
                }
            }
        }

        // Clean up any cells/rows that were not terminated
        if (!cur_cell.empty())
            cur_row.push_back(cur_cell);
        if (!cur_row.empty())
            out.rows.push_back(cur_row);

        return out;
    }
private:
    static bool testEndOfCellOrRow(char c, const std::set<char>& delimiters, std::string& cur_cell, std::vector<std::string>& cur_row, CSVFile& out, bool& init_new_cell, int& idx, const std::string& strbuf)
    {
        // Hit a delimiter: always create a new cell in the unquoted context
        if (delimiters.contains(c))
        {        
            cur_row.push_back(cur_cell);
            init_new_cell = true;
            cur_cell.erase();

            return true;
        }
        // Hit a linefeed: always create new row in the unquoted context
        else if (c == '\n')
        {
            cur_row.push_back(cur_cell);
            init_new_cell = true;
            cur_cell.erase();

            out.rows.push_back(cur_row);
            cur_row.clear();
            return true;
        }
        // Hit a carriage return: either \r or \r\n will be counted as a valid newline; always create new row
        else if (c == '\r')
        {
            // Eat \n if it exists
            if (idx + 1 < strbuf.size() && strbuf[idx+1] == '\n')
                idx++;
            
            cur_row.push_back(cur_cell);
            init_new_cell = true;
            cur_cell.erase();

            out.rows.push_back(cur_row);
            cur_row.clear();
            return true;
        }
        return false;
    }
};

