#include "CommandLineArguments.h"

#include <cstring>
#include <iostream>
#include <stdlib.h>

CommandLineArguments::Parameter::AbstractStorage::~AbstractStorage(void) {}

CommandLineArguments::CommandLineArguments(int argc, char *argv[], CommandLineArguments::Template &&templ) :
    _raw(argc-1),
    _programName(argv[0]),
    templ(std::move(templ))
{
    memcpy(&_raw[0], &argv[1], _raw.size() * sizeof(argv[0]));
}

static void setEmptyParameterArgument(CommandLineArguments::Parameter *currentParameter)
{
    if (!currentParameter->requiresArgument())
        currentParameter->setArgument();
    else
        throw CommandLineArguments::parse_error(std::string("Parameter --") + currentParameter->name + " requires an argument");
}

CommandLineArguments::Parameter *CommandLineArguments::getParameter(const std::string &name)
{
    for (Parameter &param : this->templ)
    {
        if (param.name == name)
            return &param;
    }

    return nullptr;
}

CommandLineArguments::Parameter *CommandLineArguments::getParameter(char abbreviated)
{
    for (Parameter &param : this->templ)
    {
        if (param.abbreviation == abbreviated)
            return &param;
    }

    return nullptr;
}

void CommandLineArguments::process(const std::string &helpParamName)
{
    Parameter *currentParameter = NULL;

    for (const char *arg : _raw)
    {
        if (arg[0] == '-')
        {
            if (arg[1] == '-')
            {
                if (helpParamName == &arg[2])
                    throw print_help();

                currentParameter = this->getParameter(&arg[2]);

                if (!currentParameter)
                    throw parse_error(std::string("Invalid parameter: ") + arg);
            }
            else
            {
                for (char abbreviatedParam : std::string(&arg[1]))
                {
                    if (currentParameter)
                        setEmptyParameterArgument(currentParameter);

                    currentParameter = this->getParameter(abbreviatedParam);

                    if (!currentParameter)
                        throw parse_error(std::string("Invalid parameter: -") + abbreviatedParam);
                }
            }
        }
        else if (currentParameter)
        {
            if (currentParameter->requiresArgument())
            {
                currentParameter->setArgument(arg);
                currentParameter = NULL;
            }
            else
                throw parse_error(std::string("Parameter --") + currentParameter->name + " does not require an argument");
        }
        else
            throw parse_error(std::string("Unexpected argument: ") + arg);
    }

    if (currentParameter)
        setEmptyParameterArgument(currentParameter);

    for (const auto &param : this->templ)
    {
        if (param.required && !param.isSet())
            throw parse_error(std::string("Required parameter --") + param.name + " not provided");
    }
}

static const char tab[] = "    ";
static const int tabSize = sizeof(tab) - 1;

static int calculateUsageTabs(int programNameLength)
{
    return programNameLength > 24 ? 3 : 3 + programNameLength / 4;
}

static void fillUsageWhitespace(int programNameLength)
{
    if (programNameLength > 24)
        std::cerr << std::endl << tab << tab << tab;
    else
    {
        for (int spaces = 4 - programNameLength % 4; spaces--; )
            std::cerr << ' ';
    }
}

void CommandLineArguments::printUsage()
{
    int tabs = calculateUsageTabs(this->programName().length());

    static const char usage_s[] = "Usage:  ";

    std::cerr << std::endl;

    std::cerr << usage_s << this->programName();
    fillUsageWhitespace(this->programName().length());
    std::cerr << "--help" << std::endl << std::endl;

    std::cerr << usage_s << this->programName();
    fillUsageWhitespace(this->programName().length());

    for (const auto &param : this->templ)
    {
        if (!param.required)
            std::cerr << "[ ";

        if (param.abbreviation)
            std::cerr << "[ --" << param.name << " | -" << param.abbreviation << " ]";
        else
            std::cerr << "--" << param.name;

        if (param.requiresArgument())
            std::cerr << " <" << param.name << ">";

        if (!param.required)
            std::cerr << " ]";

        std::cerr << std::endl;

        for (int i = 0; i < tabs; i++)
            std::cerr << tab;
    }

    std::cerr << std::endl;
}

static int calculateTotalParamNameColumnSize(const std::string &paramName)
{
    return tabSize + 2 + paramName.length() + 1;
}

static int calculateTotalParamNameColumnSize(const CommandLineArguments::Parameter &param)
{
    return calculateTotalParamNameColumnSize(param.name) + (param.abbreviation ? 5 : 0) + (param.requiresArgument() ? 8 : 0);
}

static void fillColumn(int columnSizeInTabs, int columnDataSize)
{
    for (int spaces = 4 - columnDataSize % 4; spaces--; )
        std::cerr << ' ';

    for (int filler = columnSizeInTabs - columnDataSize / 4 - 1; filler--; )
        std::cerr << tab;
}

void CommandLineArguments::printHelp(const std::string &helpParamName)
{
    int tabs;

    this->printUsage();

    std::cerr << "Parameters:" << std::endl << std::endl;

    for (const auto &param : this->templ)
    {
        int tabsRequired = (calculateTotalParamNameColumnSize(param) + tabSize - 1) / 4;

        if (tabsRequired > tabs)
            tabs = tabsRequired;
    }

    std::cerr << tab << "--" << helpParamName;
    fillColumn(tabs, calculateTotalParamNameColumnSize(helpParamName));
    std::cerr << "| Print this help and exit." << std::endl;

    for (const auto &param : this->templ)
    {
        std::cerr << tab << "--" << param.name;

        if (param.abbreviation)
            std::cerr << " | -" << param.abbreviation;

        if (param.requiresArgument())
            std::cerr << " <value>";

        fillColumn(tabs, calculateTotalParamNameColumnSize(param));
        std::cerr << "| " << (param.required ? "Required." : "Optional.") << " " << param.helpText << std::endl;
    }

    std::cerr << std::endl;
}


void CommandLineArguments::processOrPrintUsageAndExit(const std::string &helpParamName)
{
    try
    {
        this->process(helpParamName);
    }
    catch (const parse_error &e)
    {
        std::cerr << "Error: " << e.what() << std::endl;

        this->printUsage();
        exit(1);
    }
    catch (const print_help &e)
    {
        this->printHelp();
        exit(1);
    }
}

