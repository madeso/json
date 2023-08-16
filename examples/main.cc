#include <fstream>
#include <iostream>

#include "jsonh/jsonh.h"

// todo: time the tests https://github.com/serde-rs/json-benchmark

void print_help(const std::string& name)
{
    const std::string i = "  ";
    std::cout
        << name << " -s -S -p -P -i[] -o[] filename\n"
        << i << "-s   make silent\n"
        << i << "-S   make not silent\n"
        << i << "-p   print input (ignored by silent mode)\n"
        << i << "-P   don't print input\n"
        << i << "-i   INPUT flags\n"
        << i << i << "d   use the last duplicate key instead of error\n"
        << i << i << "c   ignore commas\n"
        << i << i << "i   parse identifiers as strings\n"
        << i << "-o   OUTPUT flags\n"
        << i << i << "c   don't write commas\n";
}

std::string th(int i)
{
    switch (i)
    {
    case 1:
        return "st";
    case 2:
        return "nd";
    default:
        return "th";
    }
}

int main(int argc, char* const argv[])
{
    const std::string appname = argv[0];

    if (argc <= 1)
    {
        print_help(appname);
        return 0;
    }

    int status = 0;
    bool is_silent = false;
    bool please_print = true;
    auto input_flags = jsonh::parse_flags::None;
    auto output_flags = jsonh::print_flags::Json;
    auto pretty_print = jsonh::Pretty;

    for (int argvi = 1; argvi < argc; argvi += 1)
    {
        const std::string cmd = argv[argvi];
        if (cmd.empty())
            continue;

        if (cmd[0] == '-')
        {
            const auto cmdsize = cmd.size();
            if (cmdsize == 1)
            {
                std::cerr << "The " << argvi << th(argvi) << " option is missing options\n";
                print_help(appname);
                return -1;
            }
            else if (cmdsize == 2)
            {
                const char c = cmd[1];
                switch (c)
                {
                case 'h':
                case '?':
                    print_help(appname);
                    return 0;
                case 's':
                    is_silent = true;
                    break;
                case 'S':
                    is_silent = false;
                    break;
                case 'p':
                    please_print = true;
                    break;
                case 'P':
                    please_print = false;
                    break;
                default:
                    std::cerr << "Unknown option " << cmd << "\n";
                    print_help(appname);
                    return -1;
                }
            }
            else
            {
                const bool is_input = cmd[1] == 'i';
                if (is_input == false && cmd[1] != 'o')
                {
                    std::cerr << "Unknown I/O option " << cmd << "\n";
                    print_help(appname);
                    return -1;
                }

                for (size_t cmdi = 2; cmdi < cmdsize; cmdi += 1)
                {
                    const char c = cmd[cmdi];
                    if (is_input)
                    {
                        switch (c)
                        {
                        case 'd': input_flags = static_cast<jsonh::parse_flags::Type>(input_flags | jsonh::parse_flags::DuplicateKeysOnlyLatest); break;
                        case 'i': input_flags = static_cast<jsonh::parse_flags::Type>(input_flags | jsonh::parse_flags::IgnoreAllCommas); break;
                        case 'c': input_flags = static_cast<jsonh::parse_flags::Type>(input_flags | jsonh::parse_flags::IdentifierAsString); break;
                        default:
                            std::cerr << "Unknown Input option " << c << " in " << cmd << "\n";
                            print_help(appname);
                            return -1;
                        }
                    }
                    else
                    {
                        switch (c)
                        {
                        case 'P': pretty_print = jsonh::Pretty; break;
                        case 'C': pretty_print = jsonh::Compact; break;
                        case 'c': output_flags = static_cast<jsonh::print_flags::Type>(output_flags | jsonh::print_flags::SkipCommas); break;
                        default:
                            std::cerr << "Unknown output option " << c << " in " << cmd << "\n";
                            print_help(appname);
                            return -1;
                        }
                    }
                }
            }
        }
        else
        {
            std::ifstream file_stream(cmd);
            if (!file_stream.good())
            {
                if (!is_silent)
                {
                    std::cerr << "Failed to load " << cmd << "\n";
                    status = -1;
                }
                continue;
            }

            // todo: implement a custom json buffer to we don't have to load the whole file into a string

            // load while file into string
            std::string file_data;
            file_stream.seekg(0, std::ios::end);
            file_data.reserve(file_stream.tellg());
            file_stream.seekg(0, std::ios::beg);
            file_data.assign((std::istreambuf_iterator<char>(file_stream)),
                             std::istreambuf_iterator<char>());

            const auto parse_result = jsonh::Parse(file_data, input_flags);

            if (parse_result.HasError())
            {
                status = -1;

                if (!is_silent)
                {
                    std::cerr << parse_result << "\n";
                }
            }
            else
            {
                if (please_print)
                {
                    std::cout << jsonh::Print(parse_result.value.get(), output_flags, pretty_print);
                }
            }
        }
    }

    return status;
}
