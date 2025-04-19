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
    std::string message;

public:
    InvalidFunctionNameException(const std::string& msg) : message(msg) {}
    const char* what() const noexcept override
    {
        return message.c_str();
    }
};

class InvalidFileException : public std::exception
{
private:
    std::string message;

public:
    InvalidFileException(const std::string& msg) : message(msg) {}
    const char* what() const noexcept override
    {
        return message.c_str();
    }
};

class ModuleNotLoadedException : public std::exception
{
private:
    std::string message;

public:
    ModuleNotLoadedException(const std::string& msg) : message(msg) {}
    const char* what() const noexcept override
    {
        return message.c_str();
    }
};

class UnknownInstruction : public std::exception
{
private:
    std::string message;

public:
    UnknownInstruction(const std::string& msg) : message(msg) {}
    const char* what() const noexcept override
    {
        return message.c_str();
    }
};

class InvalidatedValue : public std::exception
{
private:
    std::string message;

public:
InvalidatedValue(const std::string& msg) : message(msg) {}
    const char* what() const noexcept override
    {
        return message.c_str();
    }
};