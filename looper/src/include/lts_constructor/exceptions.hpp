/**
 * @file exceptions.hpp
 * @author Jan Pánek (xpanek11@stud.fit.vutbr.cz)
 * @brief
 * @version 0.1
 * @date 2024-12-07
 *
 * @copyright Copyright (c) 2024
 *
 */

#include <exception>

class InvalidFunctionNameException : public std::exception
{
private:
    char *message;

public:
    InvalidFunctionNameException(char *msg) : message(msg) {}
    char *what()
    {
        return message;
    }
};

class InvalidFileException : public std::exception
{
private:
    char *message;

public:
    InvalidFileException(char *msg) : message(msg) {}
    char *what()
    {
        return message;
    }
};

class ModuleNotLoadedException : public std::exception
{
private:
    char *message;

public:
    ModuleNotLoadedException(char *msg) : message(msg) {}
    char *what()
    {
        return message;
    }
};

class UnknownInstruction : public std::exception
{
private:
    char *message;

public:
    UnknownInstruction(char *msg) : message(msg) {}
    char *what()
    {
        return message;
    }
};
