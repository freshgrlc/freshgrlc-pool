#ifndef UTIL_COMMANDLINEARGUMENTS_H
#define UTIL_COMMANDLINEARGUMENTS_H

#include <string>
#include <vector>
#include <memory>
#include <stdexcept>

class CommandLineArguments
{
    public:
        class Parameter
        {
            public:
                template <typename T>
                Parameter(T &valueRef, const std::string &name, char abbreviation, bool required, const char *helpText = "") :
                    name(name),
                    abbreviation(abbreviation),
                    required(required),
                    helpText(helpText),
                    storage(std::make_unique<ValueRef<T>>(valueRef)),
                    _isSet()
                {}

                template <typename T>
                Parameter(T &valueRef, const std::string &name, char abbreviation, const char *helpText = "") :
                    name(name),
                    abbreviation(abbreviation),
                    required(false),
                    helpText(helpText),
                    storage(std::make_unique<ValueRef<T>>(valueRef)),
                    _isSet()
                {}

                template <typename T>
                Parameter(T &valueRef, const std::string &name, bool required, const char *helpText = "") :
                    name(name),
                    abbreviation(0),
                    required(required),
                    helpText(helpText),
                    storage(std::make_unique<ValueRef<T>>(valueRef)),
                    _isSet()
                {}

                template <typename T>
                Parameter(T &valueRef, const std::string &name, const char *helpText = "") :
                    name(name),
                    abbreviation(0),
                    required(false),
                    helpText(helpText),
                    storage(std::make_unique<ValueRef<T>>(valueRef)),
                    _isSet()
                {}

                inline Parameter(bool &valueRef, const std::string &name, char abbreviation, const char *helpText = "") :
                    name(name),
                    abbreviation(abbreviation),
                    required(false),
                    helpText(helpText),
                    storage(std::make_unique<ImplicitBooleanValueRef>(valueRef)),
                    _isSet()
                {}

                inline Parameter(bool &valueRef, const std::string &name, const char *helpText = "") :
                    name(name),
                    abbreviation(0),
                    required(false),
                    helpText(helpText),
                    storage(std::make_unique<ImplicitBooleanValueRef>(valueRef)),
                    _isSet()
                {}

                inline Parameter(const Parameter &them) :
                    name(them.name),
                    abbreviation(them.abbreviation),
                    required(them.required),
                    helpText(them.helpText),
                    storage(them.storage->clone()),
                    _isSet(them._isSet)
                {}

                inline Parameter(Parameter &&old) :
                    name(old.name),
                    abbreviation(old.abbreviation),
                    required(old.required),
                    helpText(old.helpText),
                    storage(std::move(old.storage)),
                    _isSet(old._isSet)
                {}

                const std::string name;
                const char abbreviation;
                const bool required;
                const std::string helpText;

                inline bool requiresArgument(void) const                    { return this->storage->requiresArgument(); }
                inline void setArgument(const std::string &argument = "")   { this->storage->set(argument); _isSet = true; }
                inline bool isSet(void) const                               { return _isSet; }

            private:
                class AbstractStorage
                {
                    public:
                        virtual ~AbstractStorage(void) = 0;
                        virtual void set(const std::string &argument) = 0;
                        virtual std::unique_ptr<AbstractStorage> clone(void) = 0;

                        inline virtual bool requiresArgument(void)
                        {
                            return true;
                        }
                };

                typedef std::unique_ptr<AbstractStorage> StorageRef;

                template <typename T>
                class ValueRef : public AbstractStorage
                {
                    public:
                        inline ValueRef(T &ref) : AbstractStorage(), valueRef(ref) {}
                        inline ~ValueRef(void) {}

                        inline virtual StorageRef clone(void) override
                        {
                            return std::make_unique<ValueRef<T>>(*this);
                        }

                        inline virtual void set(const std::string &argument) override
                        {
                            this->valueRef = argument;
                        }

                    private:
                        T &valueRef;
                };

                class ImplicitBooleanValueRef : public AbstractStorage
                {
                    public:
                        inline ImplicitBooleanValueRef(bool &ref) : AbstractStorage(), valueRef(ref)
                        {
                            this->valueRef = false;
                        }

                        inline ~ImplicitBooleanValueRef(void) {}

                        inline virtual StorageRef clone(void) override
                        {
                            return std::make_unique<ImplicitBooleanValueRef>(*this);
                        }

                        inline void set(const std::string &argument) override
                        {
                            this->valueRef = true;
                        }

                        inline virtual bool requiresArgument(void) override
                        {
                            return false;
                        }

                    private:
                        bool &valueRef;
                };

                StorageRef storage;
                bool _isSet;
        };

        typedef std::vector<Parameter> Template;

        class parse_error : public std::runtime_error
        {
            public:
                inline parse_error(const std::string &reason) : runtime_error(reason) {}
                inline parse_error(std::string &&reason) : runtime_error(std::move(reason)) {}
        };

        class print_help : public std::runtime_error
        {
            public:
                inline print_help(void) : runtime_error("User requested help screen") {}
        };

        CommandLineArguments(int argc, char *argv[], Template &&templ);

        void printUsage(void);
        void printHelp(const std::string &helpParamName = "help");

        void process(const std::string &helpParamName = "help");
        void processOrPrintUsageAndExit(const std::string &helpParamName = "help");

        inline const std::string &programName(void)  { return _programName; }

    private:
        std::vector<const char *> _raw;
        std::string _programName;
        Template templ;

        Parameter *getParameter(const std::string &name);
        Parameter *getParameter(char abbreviated);
};

template <>
inline void CommandLineArguments::Parameter::ValueRef<int>::set(const std::string &argument)
{
    try
    {
        this->valueRef = std::stoi(argument);
    }
    catch (const std::invalid_argument &e)
    {
        throw parse_error(std::string("Unable to interpret as integer: ") + argument);
    }
}

template <>
inline void CommandLineArguments::Parameter::ValueRef<bool>::set(const std::string &argument)
{
    if (!argument.compare("yes") ||
        !argument.compare("true") ||
        !argument.compare("enabled") ||
        !argument.compare("1"))
    {
        this->valueRef = true;
    }
    else if (!argument.compare("no") ||
             !argument.compare("false") ||
             !argument.compare("disabled") ||
             !argument.compare("0"))
    {
        this->valueRef = false;
    }
    else throw parse_error(std::string("Unable to interpret as boolean value: ") + argument);
}

#endif
