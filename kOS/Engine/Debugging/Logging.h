/******************************************************************/
/*!
\file      Logging.h
\author    Rayner Tan
\par       raynerweichen.tan@digipen.edu
\date      Sept 28, 2024
\brief     This file contains the definition of the Logger class and
           related macros for logging at different log levels (INFO,
           WARN, ERROR, DEBUG, CRASH) along with signal handling
           for program crashes.

The Logger class provides formatted output of log messages to
both the console and file, as well as macros to easily log messages
with varying severity.

Copyright (C) 2024 DigiPen Institute of Technology.
Reproduction or disclosure of this file or its contents without the
prior written consent of DigiPen Institute of Technology is prohibited.
*/
/******************************************************************/

#ifndef LOGGING_H
#define LOGGING_H

#include "Config/pch.h"
#include <backward/backward.hpp>


/*
* @brief Variadic Macro for logging Information. This macro takes in a string message, followed by the
* necessary arguments.
* @param Takes an std::string_view or string in the form of "This is a log value: {0}, and {1}", followed by
* the arguments
*/
#define LOGGING_INFO(x, ...) logging::Logger::m_GetInstance(). m_Info( x , ##__VA_ARGS__ )

/*
* @brief Variadic Macro for logging warnings. This macro takes in a string message, followed by the
* necessary arguments.
* @param Takes an std::string_view or string in the form of "This is a log value: {0}, and {1}", followed by
* the arguments
*/
#define LOGGING_WARN( x, ... ) logging::Logger::m_GetInstance().m_Warn( x , ##__VA_ARGS__ )

/*
* @brief Variadic Macro for logging Errors. This macro takes in a string message, followed by the
* necessary arguments.
* @param Takes an std::string_view or string in the form of "This is a log value: {0}, and {1}", followed by
* the arguments
*/
#define LOGGING_ERROR(x, ...) logging::Logger::m_GetInstance().m_Error(std::source_location::current(), x,  ##__VA_ARGS__)
#define LOGGING_ERROR_NO_SOURCE_LOCATION(x, ...) logging::Logger::m_GetInstance().m_Error(x, ##__VA_ARGS__)
#define LOGGING_CRASH(x, ...) logging::Logger::m_GetInstance().m_Crash(x,  ##__VA_ARGS__)
/*
* @brief Variadic Macro for logging Debug. This macro takes in a string message, followed by the
* necessary arguments.
* @param Takes an std::string_view or string in the form of "This is a log value: {0}, and {1}", followed by
* the arguments
*/
#define LOGGING_DEBUG(x, ...) logging::Logger:: m_GetInstance().m_Debug( x, ##__VA_ARGS__)

/******************************************************************/
/*!
\def       LOGGING_ASSERT(x)
\brief     Macro for making assertions. If the condition is false, an assertion
            failure occurs.
\param[in] x    The condition to assert.
*/
/******************************************************************/
#define LOGGING_ASSERT(x) \
    if (!(x)) { \
        assert(x); \
    }



#define LOGGING_ASSERT_WITH_MSG(x,...) logging::Logger::m_GetInstance().m_Assert(std::source_location::current(),x,##__VA_ARGS__);

/******************************************************************/
/*!
\def       LOGGING_INIT_LOGS(filename)
\brief     Macro to initialize the Logger with the specified file.
\param[in] filename The name of the file where log messages will be stored.
*/
/******************************************************************/
#define LOGGING_INIT_LOGS( filename ) logging::Logger::m_GetInstance().m_Init( filename )

#define LOGGING_POPUP(x, ...) logging::Logger::m_GetInstance().m_Popup( x, ##__VA_ARGS__)
#define LOGGING_TOGGLE(toggle) logging::Logger::m_GetInstance().setToggle(toggle);

namespace logging {

    static const std::string s_GREEN = "\033[0;32m";
    static const std::string s_YELLOW = "\033[0;33m";
    static const std::string s_RED = "\033[0;31m";
    static const std::string s_WHITE = "\033[0;30m";
    static const std::string s_BLUE = "\033[0;36m";
    static const std::string s_CLOSE = "\033[0m";

    /******************************************************************/
    /*!
    \enum      LogLevel
    \brief     Defines the different levels of logging available: DEBUG, INFO,
               WARNING, ERROR.
    */
    /******************************************************************/
    enum class LogLevel {
        LOG_DEBUG,
        LOG_INFO,
        LOG_WARNING,
        LOG_ERROR,
        LOG_LEVEL_SIZE
    };

    class Logger {
    public:

        /******************************************************************/
        /*!
        \fn      Logger::Logger()
        \brief   Default constructor for the Logger class.
        \details Initializes default values without opening a log file.
        */
        /******************************************************************/
        Logger() = default;

        /******************************************************************/
        /*!
        \fn      Logger::Logger(const std::string& filename)
        \brief   Constructor that opens the log file in append mode.
        \param   filename - The name of the log file to open.
        \details Opens the specified log file for appending log messages.
        */
        /******************************************************************/
        Logger(const std::string& filename);

        /******************************************************************/
        /*!
        \fn      Logger::~Logger()
        \brief   Destructor that closes the log file.
        \details Ensures that the log file is properly closed when the Logger is destroyed.
        */
        /******************************************************************/
        ~Logger();


        /******************************************************************/
        /*!
        \fn        void Logger::m_Init(const std::string& filename)
        \brief     Initializes the Logger by opening the specified log file.
        \param[in] filename  The name of the file to log messages to.
        \warning   Must be called before using the Logger to log messages.
        */
        /******************************************************************/
        void  m_Init(const std::string& filename);

        /******************************************************************/
        /*!
        \fn        Logger& Logger::m_GetInstance()
        \brief     Returns the singleton instance of the Logger class.
        \return    Reference to the Logger instance.
        */
        /******************************************************************/
        static Logger& m_GetInstance();

        /******************************************************************/
        /*!
        \fn      void Logger::m_Log(LogLevel level, const std::string& message)
        \brief   Logs a message at the specified log level.
        \param   level - The severity level of the log (e.g., INFO, WARN, ERROR).
        \param   message - The message to be logged.
        \details Outputs the provided message to both the console and the log file, with a specified log level.
        */
        /******************************************************************/
        void m_Log(LogLevel level, const std::string& message);

        /******************************************************************/
        /*!
        \fn      void Logger::m_Info(const std::string_view message, Args&&... args)
        \brief   Logs an informational message.
        \param   message - The message format string.
        \param   args - Additional arguments for formatting the message.
        \details Logs an informational message, used for general operational information.
        */
        /******************************************************************/
        template <typename... Args>
        void m_Info(const std::string_view message, Args&&... args);

        /******************************************************************/
        /*!
        \fn      void Logger::m_Warn(const std::string_view message, Args&&... args)
        \brief   Logs a warning message.
        \param   message - The message format string.
        \param   args - Additional arguments for formatting the message.
        \details Logs a warning, used to indicate a potential issue that should be noted.
        */
        /******************************************************************/
        template <typename... Args>
        void m_Warn(const std::string_view message, Args&&... args);

        /******************************************************************/
        /*!
        \fn      void Logger::m_Error(std::source_location location, const std::string_view message, Args&&... args)
        \brief   Logs an error message with source location.
        \param   location - Source location data (e.g., file, line, function).
        \param   message - The message format string.
        \param   args - Additional arguments for formatting the message.
        \details Logs an error with source location details, useful for diagnosing issues.
        */
        /******************************************************************/
        template <typename... Args>
        void m_Error(std::source_location location, const std::string_view message, Args&&... args);

        /******************************************************************/
        /*!
        \fn      void Logger::m_Error(const std::string_view message, Args&&... args)
        \brief   Logs an error message without source location.
        \param   message - The message format string.
        \param   args - Additional arguments for formatting the message.
        \details Logs an error message without specifying source location.
        */
        /******************************************************************/
        template <typename... Args>
        void m_Error(const std::string_view message, Args&&... args);

        /******************************************************************/
        /*!
        \fn      void Logger::m_Crash(const std::string_view message, Args&&... args)
        \brief   Logs a crash message.
        \param   message - The message format string.
        \param   args - Additional arguments for formatting the message.
        \details Logs a critical error that results in a program crash.
        */
        /******************************************************************/
        template <typename... Args>
        void m_Crash(const std::string_view message, Args&&... args);

        /******************************************************************/
        /*!
        \fn      void Logger::m_Assert(std::source_location location, const std::string_view message, Args&&... args)
        \brief   Logs an assertion failure.
        \param   location - Source location data (e.g., file, line, function).
        \param   message - The message format string.
        \param   args - Additional arguments for formatting the message.
        \details Logs an assertion failure, used for debugging critical issues that violate expected logic.
        */
        /******************************************************************/
        template <typename... Args>
        void m_Assert(std::source_location location, const std::string_view message, Args&&... args);

        /******************************************************************/
        /*!
        \fn      void Logger::m_Debug(const std::string_view message, Args&&... args)
        \brief   Logs a debug message.
        \param   message - The message format string.
        \param   args - Additional arguments for formatting the message.
        \details Logs a message for debugging purposes, often containing detailed internal state information.
        */
        /******************************************************************/
        template <typename... Args>
        void m_Debug(const std::string_view message, Args&&... args);

        /******************************************************************/
        /*!
        \fn      void Logger::m_Popup(const std::string_view message, Args&&... args)
        \brief   Logs a message and shows it in a popup window.
        \param   message - The message format string.
        \param   args - Additional arguments for formatting the message.
        \details Logs a message and displays it as a popup window, useful for notifying the user directly.
        */
        /******************************************************************/
        template <typename... Args>
        void m_Popup(const std::string_view message, Args&&... args);


        //template <typename... Args>
        //void  m_Popup(const std::string_view message, Args&&... args);
        /******************************************************************/
        /*!
        \fn        std::string Logger::m_GetCurrentTimestamp()
        \brief     Gets the current timestamp in the format YYYY-MM-DD HH:MM:SS.
        \return    A string containing the current timestamp.
        */
        /******************************************************************/
        std::string m_GetCurrentTimestamp();
        /******************************************************************/
        /*!
        \fn        std::vector<std::string> Logger::m_GetLogList()
        \brief     Returns the list of logs
        */
        /******************************************************************/
        std::vector<std::string>  m_GetLogList();

        /******************************************************************/
        /*!
        \fn      void Logger::m_TestingLog()
        \brief   Logs a test message to verify the logger functionality.
        \details This function is used to log a test message, ensuring that the logger is working correctly.
        */
        /******************************************************************/
        void m_TestingLog();

        /******************************************************************/
        /*!
        \fn        void Logger::m_Setup_Abort_Handler()
        \brief     Sets up signal handlers for various abort and crash signals
                   (SIGABRT, SIGSEGV, SIGTERM, etc.).
        */
        /******************************************************************/
        void  m_Setup_Abort_Handler();
        /******************************************************************/
        /*!
        \fn        static void Logger::m_Abort_Handler(int signal)
        \brief     Handles the received abort signal and logs the stack trace
                   to the log file.
        \param[in] signal The signal received (e.g., SIGABRT, SIGSEGV).
        */
        /******************************************************************/
        static void  m_Abort_Handler(int);

        bool getToggle();
        void setToggle(bool toggle);

    private:
        bool toggle_Debug = false;


        /******************************************************************/
        /*!
        \var     std::ofstream m_logFile
        \brief   File stream for the log file.
        \details This stream is used to write log messages to an external log file.
        */
        /******************************************************************/
        std::ofstream m_logFile;

        /******************************************************************/
        /*!
        \var     backward::Printer m_printer
        \brief   Printer for logging crash stack traces.
        \details Used to print stack traces into the log file during crashes, providing debug information.
        */
        /******************************************************************/
        backward::Printer m_printer;

        /******************************************************************/
        /*!
        \var     backward::StackTrace m_st
        \brief   Stack trace object for capturing program state during crashes.
        \details This object captures the call stack, which can be used to debug crashes by logging the stack trace.
        */
        /******************************************************************/
        backward::StackTrace m_st;

        /******************************************************************/
        /*!
        \var     bool m_bInitialized
        \brief   Indicates whether the logger has been initialized.
        \details This flag is used to ensure that the logger is properly set up before use.
        */
        /******************************************************************/
        bool m_bInitialized{ false };

        /******************************************************************/
        /*!
        \fn      std::string Logger::m_LevelToString(LogLevel level)
        \brief   Converts the log level to a string representation.
        \param   level - The log level to convert.
        \return  A string representing the log level (e.g., "INFO", "ERROR").
        */
        /******************************************************************/
        std::string m_LevelToString(LogLevel level);

        /******************************************************************/
        /*!
        \fn      std::string Logger::m_ColorToString(LogLevel level)
        \brief   Converts the log level to a corresponding color code.
        \param   level - The log level to convert.
        \return  A string containing the ANSI color code for the log level.
        */
        /******************************************************************/
        std::string m_ColorToString(LogLevel level);

        /******************************************************************/
        /*!
        \var     std::vector<std::string> m_log_list
        \brief   List of all logged messages.
        \details This vector stores all logged messages, providing an in-memory list of log entries.
        */
        /******************************************************************/
        std::vector<std::string> m_log_list;

    };


    template <typename... Args>
    void Logger::m_Info(const std::string_view message, Args&&... args)
    {
        //assert(m_bInitialized && "The logger must be initialized before it is used!");

        if (!m_bInitialized)
        {

            std::cout << "The logger must be initialized before it is used!" << std::endl;

            return;
        }
        if (!toggle_Debug) {
            std::stringstream logEntry;
            logEntry << "[INFO]: " << m_GetCurrentTimestamp() << " - " << std::vformat(message, std::make_format_args(args...));

            std::cout << s_BLUE << logEntry.str() << s_CLOSE << std::endl;
            m_log_list.push_back(logEntry.str());
            // Output to log file
            if (m_logFile.is_open()) {
                m_logFile << logEntry.str() << "\n";
                m_logFile.flush(); // Ensure immediate write to file
            }
        }
    }


    template <typename... Args>
    void Logger::m_Warn(const std::string_view message, Args&&... args)
    {
#ifdef _WIN32
        //assert(m_bInitialized && "The logger must be initialized before it is used!");

        if (!m_bInitialized)
        {

            std::cout << "The logger must be initialized before it is used!" << std::endl;

            return;
        }
        if (toggle_Debug) {
            std::stringstream logEntry;
            logEntry << "[WARN]: " << m_GetCurrentTimestamp() << " - " << std::vformat(message, std::make_format_args(args...));

            std::cout << s_YELLOW << logEntry.str() << s_CLOSE << std::endl;

            m_log_list.push_back(logEntry.str());

            // Output to log file
            if (m_logFile.is_open()) {
                m_logFile << logEntry.str() << "\n";
                m_logFile.flush(); // Ensure immediate write to file
            }
            else {
                std::cout << "Not storing" << std::endl;
            }
        }
#endif
    }
    template <typename... Args>
    void Logger::m_Error(std::source_location location, const std::string_view message, Args&&... args)
    {
#ifdef _WIN32
        //assert(m_bInitialized && "The logger must be initialized before it is used!");

        if (!m_bInitialized)
        {

            std::cout << "The logger must be initialized before it is used!" << std::endl;

            return;
        }
        if (!toggle_Debug) {
            std::stringstream logEntry;
            logEntry << "[ERROR]: " << m_GetCurrentTimestamp() << " - " << std::vformat(message, std::make_format_args(args...))
                << "\nFUNC: " << location.function_name() << " LINE: " << location.line() << " FILE: " << location.file_name();

            std::cout << s_RED << logEntry.str() << s_CLOSE << std::endl;



            m_log_list.push_back(logEntry.str());

            std::string title = "Error";
            std::wstring tile_wstring = std::wstring(title.begin(), title.end());
            std::string entry = logEntry.str();
            std::wstring entry_W = std::wstring(entry.begin(), entry.end());

            int msgboxID = MessageBoxW(
                NULL,
                entry_W.c_str(),
                tile_wstring.c_str(),
                MB_ICONQUESTION | MB_OK | MB_DEFBUTTON2
            );

            switch (msgboxID)
            {
            case IDOK:
                // Output to log file
                if (m_logFile.is_open()) {
                    m_logFile << logEntry.str() << "\n";
                    m_logFile.flush(); // Ensure immediate write to file
                }
                break;
            default:
                break;
            }
            m_st.load_here(32);
            m_printer.print(m_st, m_logFile);
        }
        

#endif
    }

    template <typename... Args>
    void Logger::m_Error(const std::string_view message , Args&&... args)
    {
#ifdef _WIN32
        //assert(m_bInitialized && "The logger must be initialized before it is used!");

        if (!m_bInitialized)
        {

            std::cout << "The logger must be initialized before it is used!" << std::endl;

            return;
        }
        if (!toggle_Debug) {
            std::stringstream logEntry;
            logEntry << "[ERROR]: " << m_GetCurrentTimestamp() << " - " << std::vformat(message, std::make_format_args(args...));


            std::cout << s_RED << logEntry.str() << s_CLOSE << std::endl;


            m_log_list.push_back(logEntry.str());

            std::string title = "Error";
            std::wstring tile_wstring = std::wstring(title.begin(), title.end());
            std::string entry = logEntry.str();
            std::wstring entry_W = std::wstring(entry.begin(), entry.end());

            int msgboxID = MessageBoxW(
                NULL,
                entry_W.c_str(),
                tile_wstring.c_str(),
                MB_ICONQUESTION | MB_OK | MB_DEFBUTTON2
            );

            switch (msgboxID)
            {
            case IDOK:
                // Output to log file
                if (m_logFile.is_open()) {
                    m_logFile << logEntry.str() << "\n";
                    m_logFile.flush(); // Ensure immediate write to file
                }
                break;
            default:
                break;
            }
        }
#endif
    }

    template <typename... Args>
    void Logger::m_Crash(const std::string_view message, Args&&... args)
    {
        //assert(m_bInitialized && "The logger must be initialized before it is used!");

        if (!m_bInitialized)
        {

            std::cout << "The logger must be initialized before it is used!" << std::endl;

            return;
        }

        if (!toggle_Debug) {
            std::stringstream logEntry;
            logEntry << "[CRASH]: " << m_GetCurrentTimestamp() << " - " << std::vformat(message, std::make_format_args(args...));

            std::cout << s_RED << logEntry.str() << s_CLOSE << std::endl;

            m_log_list.push_back(logEntry.str());

            // Output to log file
            if (m_logFile.is_open()) {
                m_logFile << logEntry.str() << "\n";
                m_logFile.flush(); // Ensure immediate write to file
            }
        }
    }

    template <typename... Args>
    void Logger::m_Assert(std::source_location location, const std::string_view message, Args&&... args)
    {
#ifdef _WIN32
        //assert(m_bInitialized && "The logger must be initialized before it is used!");

        if (!m_bInitialized)
        {

            std::cout << "The logger must be initialized before it is used!" << std::endl;

            return;
        }
        if (!toggle_Debug) {
            std::stringstream logEntry;
            logEntry << "[ASSERTION]: " << m_GetCurrentTimestamp() << " - " << std::vformat(message, std::make_format_args(args...))
                << "\nFUNC: " << location.function_name() << " LINE: " << location.line() << " FILE: " << location.file_name();

            std::cout << s_RED << logEntry.str() << s_CLOSE << std::endl;

            m_log_list.push_back(logEntry.str());

            std::string title = "Assertion Failed";
            std::wstring tile_wstring = std::wstring(title.begin(), title.end());
            std::string entry = logEntry.str();
            std::wstring entry_W = std::wstring(entry.begin(), entry.end());

            int msgboxID = MessageBoxW(
                NULL,
                entry_W.c_str(),
                tile_wstring.c_str(),
                MB_ICONQUESTION | MB_OK | MB_DEFBUTTON2
            );

            switch (msgboxID)
            {
            case IDOK:
                // Output to log file
                if (m_logFile.is_open()) {
                    m_logFile << logEntry.str() << "\n";
                    m_logFile.flush(); // Ensure immediate write to file
                }
                //LOGGING_ASSERT(false);
                abort();
                break;
            default:
                break;
            }
        }
#endif
    }


    template <typename... Args>
    void Logger::m_Debug( const std::string_view message, Args&&... args)
    {
        //assert(m_bInitialized && "The logger must be initialized before it is used!");

        if (!m_bInitialized)
        {

            std::cout << "The logger must be initialized before it is used!" << std::endl;

            return;
        }
        if (!toggle_Debug) {
            std::stringstream logEntry;
            logEntry << "[DEBUG]: " << m_GetCurrentTimestamp() << " - " << std::vformat(message, std::make_format_args(args...));

            std::cout << s_GREEN << logEntry.str() << s_CLOSE << std::endl;

            m_log_list.push_back(logEntry.str());

            // Output to log file
            if (m_logFile.is_open()) {
                m_logFile << logEntry.str() << "\n";
                m_logFile.flush(); // Ensure immediate write to file
            }
        }
    }

    template <typename... Args>
    void Logger::m_Popup(const std::string_view message, Args&&... args)
    {
#ifdef _WIN32
        //assert(m_bInitialized && "The logger must be initialized before it is used!");

        if (!m_bInitialized)
        {

            std::cout << "The logger must be initialized before it is used!" << std::endl;

            return;
        }
        if (!toggle_Debug) {
            std::stringstream logEntry;
            logEntry << "[POPUP]: " << m_GetCurrentTimestamp() << " - " << std::vformat(message, std::make_format_args(args...));


            std::cout << s_GREEN << logEntry.str() << s_CLOSE << std::endl;


            m_log_list.push_back(logEntry.str());

            std::string title = "POP UP";
            std::wstring tile_wstring = std::wstring(title.begin(), title.end());
            std::string entry = logEntry.str();
            std::wstring entry_W = std::wstring(entry.begin(), entry.end());

            int msgboxID = MessageBoxW(
                NULL,
                entry_W.c_str(),
                tile_wstring.c_str(),
                MB_ICONQUESTION | MB_OK | MB_DEFBUTTON2
            );

            switch (msgboxID)
            {
            case IDOK:
                // Output to log file
                if (m_logFile.is_open()) {
                    m_logFile << logEntry.str() << "\n";
                    m_logFile.flush(); // Ensure immediate write to file
                }
                break;
            default:
                break;
            }
        }   
#endif
    }

}

/**************************************************

HOW TO USE?

Logging::Logger log(<insert file name>)
log.log(<enum>, <text>);

USING DEFINE
LOGGING_INIT_LOGS(<FILE PATH>);
LOGGING_INFO(<MESSAGE>, 50);
inside MESSAGE need to have {} as this is the placeholder for 50

************************************************/

#endif

