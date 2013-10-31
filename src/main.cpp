/* vi: set ts=4 sw=4 ai: */
#define MAINFILE

#include <stdint.h>
#include <time.h>

#include <string>
#include <iostream>

#include <boost/program_options.hpp>
#include <boost/program_options/detail/utf8_codecvt_facet.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/erase.hpp>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/date_time/c_time.hpp>
//#include <boost/random.hpp>

#include "config.h"
#include "Lotos2/globals.h"
#include "Lotos2/misc.h"


using std::cout;
using std::cerr;
using std::string;
using boost::asio::ip::tcp;


bool configure(int ac, char **av);
void init(void);
void startNetwork(void);


int main(int argc, char **argv)
{
	if (!configure(argc, argv)) {
		return 0;
		}

#ifdef HAVE_FORK
	if (options.get("global.daemon", false)) {
		switch (fork()) {
			case -1:
				cerr << "ERROR: fork()" << std::endl;
				exit(1);
			case 0:
				cout << "forked to background, pid " << getpid() << std::endl;
				options.put("runtime.main_pid", getpid());
				break;
			default:
				exit(0);
			}
		}
#endif

	init();
	startNetwork();

	return 0;
}

bool configure(int ac, char **av)
{
	string cf="config.ini",
		logFile;

	namespace po=boost::program_options;
	namespace fs=boost::filesystem;
	namespace pt=boost::property_tree;

	po::options_description generic("Allowed options");
	generic.add_options()
		("configFile,c", po::value<string>(&cf)->default_value("config.ini"), "config file")
		("help,h", "this help")
		("version,V", "")
#ifdef HAVE_FORK
		("daemon,d", "")
#endif
		("logFile,l", po::value<string>(&logFile), "")
		("suppress,s", "suppress config info")
		;
	po::positional_options_description p;
	p.add("configFile", -1);

	po::variables_map vm;
	po::store(po::command_line_parser(ac, av).options(generic).positional(p).run(), vm);
	po::notify(vm);

	if (vm.count("version")) {
		cout << "0.0.1a" << std::endl;
		return false;
		}
	if (vm.count("help")) {
		cout << generic << std::endl;
		return false;
		}

	if (cf!="") {
		if (!fs::exists(cf)) {
			cerr << "ERROR: The config file '" << cf << "' not found." << std::endl;
			exit(1);
			}
		if (!fs::is_regular_file(cf)) {
			cerr << "ERROR: The config file '" << cf << "' isn't regular file." << std::endl;
			exit(1);
			}
		string scf(cf);
		pt::ini_parser::read_ini(scf, options);
		}

#ifdef HAVE_FORK
	if (vm.count("daemon")) {
		if (vm["daemon"].empty())
			options.put("global.daemon", true);
		else 
			switch (vm["daemon"].as<int>()) {
				case 1:
					options.put("global.daemon", true);
					break;
				case 0:
					options.put("global.daemon", false);
					break;
				default:
					cerr << "ERROR: invalid value for --daemon" << std::endl;
					exit(1);
				}
		}
#endif
	if (vm.count("suppress"))
		options.put("global.suppress_config_info", true);
	if (logFile!="")
		options.put("global.logFile", logFile);
	else
		logFile=options.get<string>("global.logFile", "");

	if (logFile!="" && logFile!="/dev/null") {
		if (fs::exists(logFile) && !fs::is_regular_file(logFile)) {
			cerr << "ERROR: The log file must either be a regular file or /dev/null." << std::endl;
			return false;
			}
		unlink(logFile.c_str());
		options.put("global.logFile", logFile);
		}
	int userPort=options.get<int>("global.userPort", 0);
	if (userPort<=1024) {
		cout << "Main port must be higher then 1024, actual: " << userPort << std::endl;
		return false;
		}
	if (options.get("global.daemon", false)) {
		if (logFile=="") {
			cerr << "ERROR: You must specify a log file if the server is to be run as a daemon" << std::endl;
			return false;
			}
		}
	return true;
}

void parse_config(void)
{
	namespace fs=boost::filesystem;

	string serverName(options.get("global.serverName", ""));
	if (serverName=="") {
		cerr << "ERROR: Server name not specified" << std::endl;
		exit(1);
		}
	if (serverName.length()>10) {
		cerr << "ERROR: Server name is too long" << std::endl;
		exit(1);
		}
	if (has_whitespace(serverName.c_str())) {
		cerr << "ERROR: Server name can't contain whitespace" << std::endl;
		exit(1);
		}
	string workingDir(options.get("global.workingDir", ""));
	if (boost::ends_with(workingDir, "/")) {
		boost::algorithm::erase_last(workingDir, "/");
		options.put("global.workingDir", workingDir);
		}
	if (!fs::exists(workingDir) || !fs::is_directory(workingDir)) {
		cerr << "ERROR: Working dir '" << workingDir << "' don't exist or isn't dir" << std::endl;
		exit(1);
		}
	int userPort=options.get<int>("global.userPort", 0);
	if (userPort<=1024 || userPort>=65535) {
		cerr << "ERROR: Invalid user port number " << userPort << ". Range is 1025 - 65535." << std::endl;
		exit(1);
		}

	if (!options.get<bool>("global.suppress_config_info", false)) {
		log("Server name: %s", serverName.c_str());
		log("Original dir: %s", options.get<string>("runtime.originalDir").c_str());
		log("Working dir: %s", workingDir.c_str());
		log("Log file: %s", options.get<string>("global.logFile", "")!=""? options.get<string>("global.logFile", "").c_str() : "<stdout>");
		log("User port: %d", options.get<int>("global.userPort"));

		log("Done.");
		}
}

void init(void)
{
	setlocale(LC_ALL, "C");
	char* original_dir=getcwd(NULL, 0);
	if (!original_dir) {
		cerr << "ERROR: getcwd()" << std::endl;
		exit(1);
		}
	options.put("runtime.originalDir", original_dir);
	free(original_dir);
	time_t t0=time(0);
	options.put("runtime.bootTime", t0);
	options.put("runtime.serverTime", t0);
	boost::date_time::c_time::localtime(&t0, &serverTimeTms);
//	srandom((u_int)time(0));
//	boost::random::mt19937 rng;

	parse_config();

	pthread_mutex_init(&log_mutex, NULL);

	//SIGNALS
}

class session
: public std::enable_shared_from_this<session>
{
	public:
		session(tcp::socket socket)
			: socket_(std::move(socket))
		{}

		void start()
		{
			do_read();
		}

	private:
		void do_read()
		{
			auto self(shared_from_this());
			socket_.async_read_some(boost::asio::buffer(data_, max_length),
					[this, self](boost::system::error_code ec, std::size_t length) {
						if (!ec) {
							do_write(length);
						}
					});
		}

		void do_write(std::size_t length)
		{
			auto self(shared_from_this());
			boost::asio::async_write(socket_, boost::asio::buffer(data_, length),
					[this, self](boost::system::error_code ec, std::size_t /*length*/) {
						if (!ec) {
							do_read();
						}
					});
		}

		tcp::socket socket_;

		enum {
			max_length = 1024
			};
		char data_[max_length];
};

class server
{
	public:
		server(boost::asio::io_service& io_service, short port)
#ifdef USE_IPV6
			: acceptor_(io_service, tcp::endpoint(tcp::v6(), port))
#else
			: acceptor_(io_service, tcp::endpoint(tcp::v4(), port))
#endif
				, socket_(io_service)
		{
			do_accept();
		}
	private:
		void do_accept()
		{
			acceptor_.async_accept(socket_, [this](boost::system::error_code ec)
			{
				if (!ec)
					std::make_shared<session>(std::move(socket_))->start();
				do_accept();
			});
		}
		tcp::acceptor acceptor_;
		tcp::socket socket_;
};

void startNetwork(void)
{
	std::size_t cpuCnt=boost::thread::hardware_concurrency();
//	std::size_t workThreadCnt= cpuCnt<2? 2 : cpuCnt;
	boost::asio::io_service WorkIoService(cpuCnt);
	server s(WorkIoService, options.get<int>("global.userPort"));
	WorkIoService.run();
//	tcp::endpoint endpoint(tcp::v4(), options.get<int>("global.userPort"));
}
