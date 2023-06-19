
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <functional>
#include <ios>
#include <iostream>
#include <memory>
#include <new>
#include <regex>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

static bool practice = false;
static bool info = false;

#ifndef _GNU_SOURCE
	#define _GNU_SOURCE
#endif

#include <errno.h>
#include <ext/stdio_filebuf.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

template <typename CharT> class FileDescriptorBuffer
{
	friend class Process;

	typedef __gnu_cxx::stdio_filebuf<CharT> BufferType;

	alignas(BufferType) char _buffer[sizeof(BufferType)];
	int _fd;
	std::ios_base::openmode _mode;

	FileDescriptorBuffer(int fd, std::ios_base::openmode mode) noexcept : _fd(fd), _mode(mode) { }
	FileDescriptorBuffer(const FileDescriptorBuffer&) = delete;

	FileDescriptorBuffer &operator=(const FileDescriptorBuffer&) = delete;
	FileDescriptorBuffer &operator=(FileDescriptorBuffer&&) = delete;

public:
	FileDescriptorBuffer(FileDescriptorBuffer &&other) noexcept : _fd(other._fd), _mode(other.mode) { other._fd = -1; }
	~FileDescriptorBuffer() { close(); }

	void close() noexcept
	{
		if (_fd >= 0)
			(void)::close(_fd);
		else if (_fd == -2)
			reinterpret_cast<BufferType*>(_buffer)->~BufferType();

		_fd = -1;
	}

	std::basic_streambuf<CharT> &open()
	{
		const int fd = _fd;

		if (fd == -1)
			throw std::runtime_error("Cannot open invalid file descriptor");
		if (fd >= 0)
		{
			_fd = -2;
			return *new(_buffer) BufferType(fd, _mode);
		}

		return *reinterpret_cast<BufferType*>(_buffer);
	}
};

class Process
{
	struct Initializer
	{
		Initializer() { signal(SIGPIPE, SIG_IGN); }
	};

	typedef int Handle;

	static constexpr Handle INVALID_HANDLE = Handle(-1);
	static Initializer _initializer;

	Handle _process;
	Handle _stdin;
	Handle _stdout;
	Handle _stderr;

	static void closeIOHandle(Handle &handle) noexcept
	{
		if (handle != INVALID_HANDLE)
		{
			(void)close(handle);
			handle = INVALID_HANDLE;
		}
	}

	static bool createPipe(Handle &in, Handle &out) noexcept
	{
		Handle fds[2];

		if (pipe2(fds, O_CLOEXEC) != 0)
			return false;

		in = fds[1];
		out = fds[0];
		return true;
	}

	template <typename CharT> static FileDescriptorBuffer<CharT> getBuffer(Handle &handle, std::ios_base::openmode mode)
	{
		Handle bufferHandle = handle;

		handle = INVALID_HANDLE;
		return FileDescriptorBuffer<CharT>(bufferHandle, mode);
	}

	static bool remapIOHandle(Handle handle, Handle newHandle) noexcept
	{
		return handle == INVALID_HANDLE || dup2(handle, newHandle) >= 0;
	}

public:
	enum IOConnections
	{
		IO_NONE = 0,
		IO_STDIN = 1,
		IO_STDOUT = 2,
		IO_STDERR = 4,
		IO_ALL = IO_STDIN | IO_STDOUT | IO_STDERR
	};

	Process() noexcept : _process(INVALID_HANDLE), _stdin(INVALID_HANDLE), _stdout(INVALID_HANDLE), _stderr(INVALID_HANDLE) { }
	Process(const std::vector<std::string> &args, IOConnections ioConnections = IO_ALL) noexcept : _process(INVALID_HANDLE), _stdin(INVALID_HANDLE), _stdout(INVALID_HANDLE), _stderr(INVALID_HANDLE) { start(args, ioConnections); }

	FileDescriptorBuffer<char> cinBuffer() { return getBuffer<char>(_stdin, std::ios_base::out); }
	FileDescriptorBuffer<char> coutBuffer() { return getBuffer<char>(_stdout, std::ios_base::in); }
	FileDescriptorBuffer<char> cerrBuffer() { return getBuffer<char>(_stderr, std::ios_base::in); }

	template <typename CharT> FileDescriptorBuffer<CharT> inBuffer() { return getBuffer<CharT>(_stdin, std::ios_base::out); }
	template <typename CharT> FileDescriptorBuffer<CharT> outBuffer() { return getBuffer<CharT>(_stdout, std::ios_base::in); }
	template <typename CharT> FileDescriptorBuffer<CharT> errBuffer() { return getBuffer<CharT>(_stderr, std::ios_base::in); }

	Process &start(const std::vector<std::string> &args, IOConnections ioConnections = IO_ALL)
	{
		if (_process != INVALID_HANDLE)
			throw std::runtime_error("Process is already started");

		std::vector<const char*> argv;
		argv.reserve(args.size() + 1);

		for (const std::string &arg : args)
			argv.push_back(arg.data());

		argv.push_back(nullptr);

		if (practice || info)
		{
			const char *separator = "";

			for (const std::string &arg : args)
				std::cout << separator << arg, separator = " ";

			std::cout << std::endl;

			if (practice)
				return *this;
		}

		Handle stdinOut = INVALID_HANDLE, stdoutIn = INVALID_HANDLE, stderrIn = INVALID_HANDLE;
		bool createdPipes = true;

		if (ioConnections & IO_STDIN)
			createdPipes = createPipe(_stdin, stdinOut);

		if (createdPipes && (ioConnections & IO_STDOUT))
			createdPipes = createPipe(stdoutIn, _stdout);

		if (createdPipes && (ioConnections & IO_STDERR))
			createdPipes = createPipe(stderrIn, _stderr);

		if (createdPipes)
			_process = fork();

		if (_process == 0)
		{
			// Remap IO streams
			remapIOHandle(stdinOut, STDIN_FILENO);
			remapIOHandle(stdoutIn, STDOUT_FILENO);
			remapIOHandle(stderrIn, STDERR_FILENO);

			(void)execv(argv[0], (char * const *)argv.data());
			(void)execvp(argv[0], (char * const *)argv.data());
			std::exit(EXIT_FAILURE);
		}

		// Parent process
		closeIOHandle(stdinOut);
		closeIOHandle(stdoutIn);
		closeIOHandle(stderrIn);

		if (_process == -1)
		{
			(void)wait();
			throw std::runtime_error("Failed to start process");
		}

		return *this;
	}

	int wait() noexcept
	{
		if (practice)
			return 0;

		closeIOHandle(_stdin);
		closeIOHandle(_stdout);
		closeIOHandle(_stderr);

		if (_process != INVALID_HANDLE)
		{
			int exitStatus;

			exitStatus = (waitpid(_process, &exitStatus, 0) >= 0 && WIFEXITED(exitStatus)) ? WEXITSTATUS(exitStatus) : -1;
			_process = INVALID_HANDLE;
			return exitStatus;
		}

		return -1;
	}
};

Process::Initializer Process::_initializer;

class File;

class Symbol
{
	const File &_file;
	std::string _name;
	char _type;
	std::string _address;

public:
	Symbol(const File &file, std::string name, char type, std::string address) noexcept : _file(file), _name(std::move(name)), _type(type), _address(std::move(address)) { }

	const File &file() const noexcept { return _file; }
	const std::string &name() const noexcept { return _name; }
	char type() const noexcept { return _type; }
	const std::string &address() const noexcept { return _address; }

	struct Hash { std::size_t operator()(const Symbol &symbol) const { return std::hash<std::string>()(symbol._name); } };
	struct Equal { bool operator()(const Symbol &symbol1, const Symbol &symbol2) const noexcept { return symbol1._name == symbol2._name; } };
};

class File
{
	std::string _name;
	mutable std::vector<std::shared_ptr<Symbol>> _provides;
	mutable std::vector<std::shared_ptr<Symbol>> _dependsOn;
	mutable std::vector<std::shared_ptr<Symbol>> _internal;

public:
	struct Hash { std::size_t operator()(const std::shared_ptr<File> &file) const { return std::hash<std::string>()(file->name()); } };

	File(std::string name) : _name(std::move(name)) { }

	const std::string &name() const noexcept { return _name; }
	std::vector<std::shared_ptr<Symbol>> &provides() const noexcept { return _provides; }
	std::vector<std::shared_ptr<Symbol>> &dependsOn() const noexcept { return _dependsOn; }
	std::vector<std::shared_ptr<Symbol>> &internal() const noexcept { return _internal; }
};

inline bool operator==(const File &file1, const File &file2) noexcept { return file1.name() == file2.name(); }

class Pattern : public std::string
{
	bool _negated;
	std::regex _regEx;

public:
	typedef std::hash<std::string> Hash;

	Pattern() noexcept : _negated() { }
	Pattern(std::string string) : std::string(std::move(string)), _negated(data()[0] == '-'), _regEx(data() + _negated, size() - _negated, std::regex::optimize) { }

	bool matches(const std::string &value) const { return std::regex_match(value, _regEx); }
	bool negated() const noexcept { return _negated; }
};

// Loads `nm`-style output
std::unordered_set<std::shared_ptr<File>, File::Hash> loadNMOutput(std::istream &input)
{
	std::unordered_set<std::shared_ptr<File>, File::Hash> files;
	std::shared_ptr<File> currentFile;

	// Parse input
	while (input)
	{
		std::string line;
		std::getline(input, line);

		if (line.empty())
			continue; // Skip empty lines
		else if (line[line.length() - 1] == ':')
			currentFile = *files.emplace(std::make_shared<File>(line.substr(0, line.length() - 1))).first;
		else
		{
			if (!currentFile)
				currentFile = *files.emplace(std::make_shared<File>("")).first;

			std::string address;
			char type;
			std::string name;

			if (line[0] != ' ')
			{
				address = line.substr(0, line.find(" "));
				type = line[address.length() + 1];
				name = line.substr(address.length() + 3);
			}
			else
			{
				std::size_t typePos = line.find_first_not_of(" ");
				type = line[typePos];
				name = line.substr(typePos + 2);
			}

			if (type == 'U')
				currentFile->dependsOn().emplace_back(std::make_shared<Symbol>(*currentFile, std::move(name), type, std::move(address)));
			else if (type >= 'A' && type <= 'Z')
				currentFile->provides().emplace_back(std::make_shared<Symbol>(*currentFile, std::move(name), type, std::move(address)));
			else
				currentFile->internal().emplace_back(std::make_shared<Symbol>(*currentFile, std::move(name), type, std::move(address)));
		}
	}

	return files;
}

// Adds all comma-separated items to a list
template <typename T, typename CreateFn> void addCommaListToOption(const char *value, T &option, CreateFn create)
{
	for (const char *next = std::strchr(value, ','); next; value = next + 1, next = std::strchr(value, ','))
		option.emplace_back(create(value, next - value));

	option.emplace_back(create(value, std::strlen(value)));
}

// Adds all comma-separated items to a map
template <typename T, typename CreateFn> void addCommaListToMap(const char *value, T &option, CreateFn create)
{
	for (const char *next = std::strchr(value, ','); next; value = next + 1, next = std::strchr(value, ','))
		option.emplace(create(value, next - value));

	option.emplace(create(value, std::strlen(value)));
}

// Converts a const reference temporary to a non-const reference temporary
template <typename T> T &asNonconst(const T &ref) { return const_cast<T&>(ref); }

// Checks if any item in the list matches the value
template <typename T, typename U> bool matches(const T &list, const U &value)
{
	bool matches = false;

	for (const auto &item : list)
	{
		if (matches == item.negated() && item.matches(value))
			matches = !matches;
	}

	return matches;
}

int main(int argc, const char *argv[])
{
	try
	{
		if (argc <= 2 || std::strcmp(argv[1], "-help") == 0)
		{
			std::cerr << argv[0] << " [options...] file [outputFile]" << std::endl;
			std::cerr << " -stub stubFile" << std::endl;
			std::cerr << " -ar ar" << std::endl;
			std::cerr << " -nm nm" << std::endl;
			std::cerr << " -objcopy objcopy" << std::endl;
			std::cerr << " -ignore symbol1,..." << std::endl; // Ignore these symbols and copy them into the output archive as-is
			std::cerr << " -defined symbol1,..." << std::endl; // Assume these symbols are defined by some other library and don't add them to the output archive
			std::cerr << " -exclude symbol1,..." << std::endl; // Excluded symbols are not added to the output archive as well as any symbols that require them
			std::cerr << " -rename oldSymbol1=newSymbol1,..." << std::endl;
			std::cerr << " -weaken" << std::endl;
			std::cerr << " -practice" << std::endl;
			std::cerr << " -info" << std::endl;
			std::cerr << "Patches an archive file by treating defined symbols as symbols provided by another library and removing excluded symbols and symbols that depend on them." << std::endl;
			return 1;
		}

		// Parse all options
		const char *file = nullptr;
		const char *output = nullptr;
		const char *stub = nullptr;
		const char *ar = "ar";
		const char *nm = "nm";
		const char *objcopy = "objcopy";
		std::vector<Pattern> ignore;
		std::vector<Pattern> defined;
		std::vector<Pattern> exclude;
		std::unordered_map<std::string, std::string> rename;
		auto createPattern = [](const char *value, std::size_t length) { return Pattern(std::string(value, length)); };
		bool weaken = false;
		bool parsedPractice = false;

		for (int i = 1; i < argc; i++)
		{
			if (argv[i][0] == '-')
			{
				const char *equalsI = std::strchr(argv[i] + 1, '=');
				const std::string option = equalsI ? std::string(&argv[i][1], equalsI) : std::string(&argv[i][1]);
				const char *value = equalsI ? equalsI + 1 : (++i < argc ? argv[i] : nullptr);

				if (option == "weaken" && !equalsI)
					weaken = true, i--;
				else if (option == "practice" && !equalsI)
					parsedPractice = true, i--;
				else if (option == "info" && !equalsI)
					info = true, i--;
				else if (!value)
					throw std::runtime_error("No value provided for option \"" + option + "\"");
				else if (option == "stub")
					stub = value;
				else if (option == "ar")
					ar = value;
				else if (option == "nm")
					nm = value;
				else if (option == "objcopy")
					objcopy = value;
				else if (option == "ignore")
					addCommaListToOption(value, ignore, createPattern);
				else if (option == "defined")
					addCommaListToOption(value, defined, createPattern);
				else if (option == "exclude")
					addCommaListToOption(value, exclude, createPattern);
				else if (option == "rename")
					addCommaListToMap(value, rename, [](const char *value, std::size_t length) {
							const char *equals = std::strchr(value, '=');

							if (!equals)
								throw std::runtime_error("Invalid rename value \"" + std::string(value, length) + "\"");

							return std::make_pair(std::string(value, equals - value), std::string(equals + 1, length - (equals + 1 - value)));
						});
				else
					throw std::runtime_error("Unrecognized option \"" + option + "\"");
			}
			else if (!file)
				file = argv[i];
			else if (!output)
				output = argv[i];
			else
				throw std::runtime_error("Extra argument \"" + std::string(argv[i]) + "\" found");
		}

		if (!file)
			throw std::runtime_error("No file argument found");
		else if (!output)
			output = file;

		// Get the symbols from the file
		Process process({nm, file}, Process::IO_STDOUT);
		auto files = loadNMOutput(asNonconst(std::istream(&process.coutBuffer().open())));
		int nmExit = process.wait();

		if (nmExit)
			return nmExit;

		practice = parsedPractice;

		// Determine which files to keep
		std::vector<std::shared_ptr<File>> filesToKeep;
		std::unordered_map<std::reference_wrapper<const std::string>, std::shared_ptr<Symbol>, std::hash<std::string>, std::equal_to<std::string>> stubSymbols;
		std::unordered_set<std::string> includedSymbols;
		std::vector<std::pair<std::shared_ptr<File>, std::vector<std::shared_ptr<Symbol>>>> symbolsToHide;

		for (std::size_t size = std::size_t(-1); size != filesToKeep.size(); )
		{
			size = filesToKeep.size();

			for (auto it = files.begin(); it != files.end(); )
			{
				// Check if a file should be included based on the required symbols
				bool included = true;

				for (const auto &dependency : (*it)->dependsOn())
				{
					auto renameIt = rename.find(dependency->name());
					const std::string &name = renameIt == rename.end() ? dependency->name() : renameIt->second;

					if (includedSymbols.find(name) == includedSymbols.end() && !matches(ignore, name) && !matches(defined, name))
					{
						included = false;
						break;
					}
				}

				if (included)
				{
					// Check each symbol in the file and hide excluded symbols
					filesToKeep.push_back(*it);
					std::size_t providedSymbols = 0;
					std::vector<std::shared_ptr<Symbol>> hideSymbols;

					for (const auto &provides : (*it)->provides())
					{
						auto renameIt = rename.find(provides->name());
						const std::string &name = renameIt == rename.end() ? provides->name() : renameIt->second;

						if (matches(ignore, name))
							; // Do nothing
						else if (matches(defined, name))
						{
							hideSymbols.emplace_back(provides);
							stubSymbols.emplace(name, provides);
						}
						else if (matches(exclude, name))
							hideSymbols.emplace_back(provides);
						else
						{
							includedSymbols.emplace(name);
							providedSymbols++;

							if (info)
								std::cout << "Including symbol " << provides->name() << " from " << (*it)->name() << std::endl;
						}
					}

					if (providedSymbols == 0)
						filesToKeep.pop_back();
					else if (!hideSymbols.empty())
						symbolsToHide.emplace_back(*it, std::move(hideSymbols));

					it = files.erase(it);
				}
				else
					++it;
			}
		}

		// Write out the stub file
		if (stub)
		{
			for (const auto &file : files)
			{
				for (const auto &provides : file->provides())
					stubSymbols.emplace(provides->name(), provides);
			}

			std::fstream stubStream;

			if (!practice)
				stubStream.open(stub, std::ios_base::out | std::ios_base::trunc);

			for (const auto &pair : stubSymbols)
			{
				if (pair.second->type() >= 'A' && pair.second->type() <= 'Z')
				{
					if (info)
						std::cout << "Stubbing symbol " << static_cast<const std::string &>(pair.first) << std::endl;

					if (pair.second->type() == 'T' || pair.second->type() == 'W')
						stubStream << "__attribute__((__weak__)) void " << static_cast<const std::string &>(pair.first) << "() { }" << std::endl;
					else
						stubStream << "__attribute__((__weak__)) void *" << static_cast<const std::string &>(pair.first) << ";" << std::endl;
				}
			}
		}

		// Determine symbols to rename
		std::vector<std::string> archiveFiles = {ar, "x", file};
		std::vector<std::string> removeFiles = {"rm", "-f"};
		std::vector<std::pair<std::shared_ptr<File>, std::vector<std::string>>> symbolsToRename;

		for (const auto &file : filesToKeep)
		{
			std::vector<std::string> renameSymbols;

			for (const auto &provides : file->provides())
			{
				auto it = rename.find(provides->name());

				if (it != rename.end())
					renameSymbols.emplace_back(it->first + '=' + it->second);
			}

			for (const auto &dependency : file->dependsOn())
			{
				auto it = rename.find(dependency->name());

				if (it != rename.end())
					renameSymbols.emplace_back(it->first + '=' + it->second);
			}

			if (!renameSymbols.empty())
				symbolsToRename.emplace_back(file, std::move(renameSymbols));

			archiveFiles.emplace_back(file->name());
			removeFiles.emplace_back(file->name());
		}

		// Extract files
		int extractExit = process.start(archiveFiles, Process::IO_NONE).wait();

		if (extractExit != 0)
			return extractExit;

		// Weaken symbols
		if (weaken)
		{
			for (const auto &file : filesToKeep)
			{
				std::vector<std::string> weakenSymbols = {objcopy};

				for (const auto &provides : file->provides())
					weakenSymbols.insert(weakenSymbols.end(), {"--weaken-symbol", provides->name()});

				if (weakenSymbols.size() > 1)
				{
					weakenSymbols.emplace_back(file->name());

					int weakenExit = process.start(weakenSymbols, Process::IO_NONE).wait();

					if (weakenExit != 0)
						return weakenExit;
				}
			}
		}

		// Rename symbols for each file individually
		for (auto &pair : symbolsToRename)
		{
			std::vector<std::string> renameSymbols = {objcopy};

			for (const auto &symbol : pair.second)
				renameSymbols.insert(renameSymbols.end(), {"--redefine-sym", std::move(symbol)});

			renameSymbols.emplace_back(pair.first->name());

			int renameExit = process.start(renameSymbols, Process::IO_NONE).wait();

			if (renameExit != 0)
				return renameExit;
		}

		// Hide symbols for each file individually
		for (auto &pair : symbolsToHide)
		{
			std::vector<std::string> hideSymbols = {objcopy};

			for (const auto &symbol : pair.second)
			{
				hideSymbols.insert(hideSymbols.end(), {"--localize-symbol", symbol->name()});
			}

			hideSymbols.emplace_back(pair.first->name());

			int hideSymbolsExit = process.start(hideSymbols, Process::IO_NONE).wait();

			if (hideSymbolsExit != 0)
				return hideSymbolsExit;
		}

		const bool removedOutputFile = practice || unlink(output) == 0 || errno == ENOENT;

		archiveFiles[1] = "cr";
		archiveFiles[2] = output;
		const int replaceExit = removedOutputFile ? process.start(archiveFiles, Process::IO_NONE).wait() : 0;

		if (replaceExit != 0)
			return replaceExit;

		const int hideSymbolsExit = process.start(removeFiles, Process::IO_NONE).wait();

		if (!removedOutputFile)
			throw std::runtime_error("Output file could not be replaced");

		if (hideSymbolsExit != 0)
			return hideSymbolsExit;
	}
	catch(const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return 2;
	}
}
