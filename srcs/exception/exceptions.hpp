#ifndef EXCEPTIONS_HPP
 #define EXCEPTIONS_HPP

#include <exception>
#include <string>
#include <iostream>

// enum class ErrorType {
// 	SOCKET_CREATION,
// 	SET_SOCKET,
// 	BIND,
// 	LISTEN,
// 	ACCEPT_SIGNAL,
// 	RECV,
// 	POLL,
// 	POLLERR_POLLHUP,
// 	FORK,
// 	WAITPID
// 	// ... other error types ...
// };

// class MyException : public std::exception {

// 	ErrorType	type;
// 	std::string	errMsg;

// 	public:
// 		MyException(ErrorType type, const std::string& message) : type(type), errMsg(message) {}

// 		const char* what() const noexcept override {
// 			return errMsg.c_str();
// 		}

// 		ErrorType getType() const {
// 			return type;
// 		}

// };


/* EXAMPLE OF HOW TO USE THIS EXCEPTION CLASS */

// void someFunction() {
//     // ... some code ...

// 	bool fileErrorOccurred = true; // Suppose a file error occurs
// 	if (fileErrorOccurred) {
// 		throw MyException(ErrorType::FileError, "File error occurred in someFunction");
// 	}

//     // ... more code ...
// }

// int main() {
// 	try {
// 		someFunction();
// 	} catch (const MyException& e) {
// 		std::cerr << "Exception of type " << static_cast<int>(e.getType())
// 					<< " caught: " << e.what() << std::endl;
// 	}

// 	return 0;
// }


#endif

