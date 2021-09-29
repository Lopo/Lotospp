/* vi: set ts=4 sw=4 ai: */
#define MAINFILE

#include "Lotospp/buildinfo.h"
#include "Log/Logger.h"
#include "Strings/misc.h"
#include "Network/ServiceManager.h"
#include "Network/Protocols/Telnet.h"
#ifdef __EXCEPTION_TRACER__
#	include "Common/ExceptionHandler.h"
#endif
#include "globals.h"
#include "System/build_config.h"
#include <boost/program_options.hpp>
#include <boost/program_options/detail/utf8_codecvt_facet.hpp>
#include <boost/filesystem.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/erase.hpp>


using namespace LotosPP;


bool configure(int ac, char **av)
{
	std::string cf{"etc/config.ini"},
		pidFile,
		logDir;
	LotosPP::Log::severity_t logLevelC,
		logLevelF;

	namespace po=boost::program_options;
	namespace fs=boost::filesystem;
	namespace pt=boost::property_tree;

	po::options_description generic("Allowed options");
	generic.add_options()
		("configFile,c", po::value<std::string>(&cf)->default_value("etc/config.ini"), "config file")
		("help,h", "this help")
		("version,V", "")
#ifdef HAVE_FORK
		("daemon,d", "fork to background as daemon")
#endif
		("logLevelC", po::value<LotosPP::Log::severity_t>(&logLevelC)->default_value(LotosPP::Log::severity_t("warning")), "console log level (none, trace, debug, info, warning, error, fatal)")
		("logLevelF", po::value<LotosPP::Log::severity_t>(&logLevelF)->default_value(LotosPP::Log::severity_t("info")), "file log level (trace, debug, info, warning, error, fatal)")
		("logDir,L", po::value<std::string>(&logDir)->default_value("log"), "")
		("pidFile,p", po::value<std::string>(&pidFile)->default_value(std::string(av[0])+".pid"), "")
		("suppress,s", "suppress config info")
		;
	po::positional_options_description p;
	p.add("configFile", -1);

	po::variables_map vm;
	po::store(po::command_line_parser(ac, av).options(generic).positional(p).run(), vm);
	po::notify(vm);

	if (vm.count("version")) {
		std::cout << Lotospp_get_buildinfo()->project_version << std::endl;
		return false;
		}
	if (vm.count("help")) {
		std::cout << generic << std::endl;
		return false;
		}

	if (cf!="") {
		if (!fs::exists(cf)) {
			std::cerr << "ERROR: The config file '" << cf << "' not found." << std::endl;
			exit(1);
			}
		if (!fs::is_regular_file(cf)) {
			std::cerr << "ERROR: The config file '" << cf << "' isn't regular file." << std::endl;
			exit(1);
			}
		pt::ini_parser::read_ini(cf, options);
		}
#ifdef HAVE_FORK
	if (vm.count("daemon")) {
		options.put("global.daemon", true);
		}
	else {
		options.put("global.daemon", false);
		}
#endif
	if (vm.count("suppress")) {
		options.put("global.suppress_config_info", true);
		}
	options.put("global.log.console.level", logLevelC.to_string());
	options.put("global.log.file.level", logLevelF.to_string());
	options.put("global.log.dir", logDir);
	if (int64_t userPort=options.get<int64_t>("global.userPort", 0); userPort<=1024 || userPort>65535) {
		std::cout << "Main port must be between 1024 and 65535, actual: " << userPort << std::endl;
		return false;
		}
	if (pidFile!="") {
		options.put("global.pidFile", pidFile);
		}
	return true;
}

void parseConfig(void)
{
	namespace fs=boost::filesystem;

	std::string serverName(options.get("global.serverName", ""));
	if (serverName=="") {
		std::cerr << "ERROR: Server name not specified" << std::endl;
		exit(1);
		}
	if (serverName.length()>10) {
		std::cerr << "ERROR: Server name is too long" << std::endl;
		exit(1);
		}
	if (Strings::hasWhitespace(serverName)) {
		std::cerr << "ERROR: Server name can't contain whitespace" << std::endl;
		exit(1);
		}
	std::string workingDir(options.get("global.workingDir", ""));
	if (boost::ends_with(workingDir, "/")) {
		boost::algorithm::erase_last(workingDir, "/");
		options.put("global.workingDir", workingDir);
		}
	if (!fs::exists(workingDir) || !fs::is_directory(workingDir)) {
		std::cerr << "ERROR: Working dir '" << workingDir << "' don't exist or isn't dir" << std::endl;
		exit(1);
		}
	std::string logDir(options.get("global.log.dir", ""));
	if (boost::ends_with(logDir, "/")) {
		boost::algorithm::erase_last(logDir, "/");
		options.put("global.log.dir", logDir);
		}
	if (!fs::exists(logDir) || !fs::is_directory(logDir)) {
		std::cerr << "ERROR: Log dir '" << logDir << "' don't exist or isn't dir" << std::endl;
		exit(1);
		}
	if (int64_t userPort=options.get<int64_t>("global.userPort", 0); userPort<=1024 || userPort>65535) {
		std::cerr << "ERROR: Invalid user port number " << userPort << ". Range is 1025 - 65535." << std::endl;
		exit(1);
		}
}

void closePidFile(void)
{
	if (std::string pidFile=options.get("global.pidFile", ""); pidFile!="") {
		remove(pidFile.c_str());
		}
}

void init(void)
{
	setlocale(LC_ALL, "C");
	boost::system::error_code ec;
	boost::filesystem::path original_dir(boost::filesystem::current_path(ec));
	if (ec) {
		std::cerr << "ERROR: current_path(): " << ec.message() << std::endl;
		exit(1);
		}
	options.put("runtime.originalDir", original_dir);
	time_t t0=time(NULL);
	options.put("runtime.bootTime", t0);
	options.put("runtime.serverTime", t0);
	boost::date_time::c_time::localtime(&t0, &serverTimeTms);

	parseConfig();
	LotosPP::Log::Logger::getInstance()->init();

	if (!options.get<bool>("global.suppress_config_info", false)) {
		LOG(LINFO) << "Server name: " << options.get("global.serverName", "");
		LOG(LINFO) << "Original dir: " << options.get("runtime.originalDir", "");
		LOG(LINFO) << "Working dir: " << boost::filesystem::canonical(options.get("global.workingDir", ""));
		LOG(LINFO) << "Log dir: " << options.get("global.log.dir", "");
		LOG(LINFO) << "User port: " << options.get("global.userPort", 0);
		LOG(LINFO) << "Done.";
		}

	LOG(LINFO) << "Number of found CPUs: " << boost::thread::hardware_concurrency();

	if (std::string pidFile{options.get("global.pidFile", "")}; pidFile!="") {
		std::ofstream f(pidFile, std::ios::trunc|std::ios::out);
		f << getpid();
		f.close();
		atexit(closePidFile);
		}

	//SIGNALS
#ifdef OS_WIN
#else
//	closeGuard consoleGuard(boost::bind(handleSignal, boost::ref(io_service)));
#endif
}

void ErrorMessage(const std::string& message)
{
	std::cout << std::endl << std::endl << "Error: " << message << std::endl;
#ifdef OS_WIN
	std::string s;
	std::cin >> s;
#endif
}

boost::mutex g_loaderLock;
boost::condition_variable g_loaderSignal;
boost::unique_lock<boost::mutex> g_loaderUniqueLock(g_loaderLock);

void mainLoader(Network::ServiceManager* service_manager)
{
	// Tie ports and register services
	service_manager->add<Network::Protocols::Telnet>(options.get<uint16_t>("global.userPort"));

	g_talker.start(service_manager);
	g_loaderSignal.notify_all();
}


extern "C"
int main(int argc, char **argv)
{
#ifdef OS_OPENBSD // must be called before any mallocs
	extern char *malloc_options;
	malloc_options=(char*)"S";
#endif
#ifndef OS_WIN
	if (!getuid() || !geteuid()) {
		std::cout << "executed as root - login as normal user" << std::endl;
		return 1;
		}
#endif
	if (!configure(argc, argv)) {
		return 0;
		}

#ifdef HAVE_FORK
	if (false && options.get("global.daemon", false)) { // XXX
		switch (fork()) {
			case -1:
				std::cerr << "ERROR: fork()" << std::endl;
				exit(1);
			case 0:
				std::cout << "forked to background, pid " << getpid() << std::endl;
				options.put("runtime.main_pid", getpid());
				break;
			default:
				exit(0);
			}
		}
#endif

	// Provides stack traces when the server crashes, if compiled in.
#ifdef __EXCEPTION_TRACER__
	Common::ExceptionHandler mainExceptionHandler;
	mainExceptionHandler.InstallHandler();
#endif

	init();
	Network::ServiceManager servicer;

	// Start scheduler and dispatcher threads
	g_dispatcher.start();
	g_scheduler.start();
	// Add load task
	g_dispatcher.addTask(LotosPP::Common::createTask(boost::bind(mainLoader, &servicer)));

	// Wait for loading to finish
	g_loaderSignal.wait(g_loaderUniqueLock);

	if (servicer.isRunning()) {
		servicer.run();
		}
	else {
		ErrorMessage("No services running. Server is not online.");
        }

#ifdef __EXCEPTION_TRACER__
	mainExceptionHandler.RemoveHandler();
#endif
	g_scheduler.shutdownAndWait();
	g_dispatcher.shutdownAndWait();
	// Don't run destructors, may hang!

	return 0;
}
