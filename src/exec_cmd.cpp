#include "exec_cmd.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <wall_e/src/color.h>
#include <unistd.h>
#include <pwd.h>

bool sexy_proc::install_package(const std::string &package) {
    if(download_package(package)) {
        local_install();
        return true;
    }
    return false;
}

std::string sexy_proc::environment::root_dir() const {
    return m_root_dir;
}

bool sexy_proc::environment::clear() const {
    temporary_path tmp_p(m_root_dir);
    return clear_meta();
}

bool sexy_proc::environment::forced() const {
    return m_forced;
}

bool sexy_proc::environment::install_package(const std::string &package) const {
    temporary_path tmp_p(m_root_dir);
    return sexy_proc::install_package(package);
}

bool sexy_proc::environment::force_install_package(const std::string &package) const {
    temporary_path tmp_p(m_root_dir);
    return sexy_proc::force_install_package(package);
}

void sexy_proc::environment::local_install() const {
    temporary_path tmp_p(m_root_dir);
    sexy_proc::local_install();
}

void sexy_proc::environment::local_install(const std::string &package) const {
    temporary_path tmp_p(m_root_dir);
    std::filesystem::copy_file(package, "./deb");
    sexy_proc::local_install();
}

sproc::process_result sexy_proc::environment::exec(const std::string &cmd, const std::string &package) const {
    if(m_forced ? force_install_package(package) : install_package(package)) {
        return this->exec(cmd);
    } else {
        return {};
    }
}

sproc::process_result sexy_proc::environment::auto_exec(const std::string &cmd) const {
    const auto pos = cmd.find(' ');
    std::string cmd_name;
    if(pos >= 0 && pos < cmd.size()) {
        cmd_name = cmd.substr(0, pos);
    } else {
        cmd_name = cmd;
    }

    const auto package = find_pack_by_cmd(cmd_name);
    if(package.size() > 0)
        return this->exec(cmd, package);

    return {};
}

sproc::process_result sexy_proc::environment::exec(const std::string &cmd) const {
    const auto path_setup_cmd = "export PATH=" + m_root_dir + "/usr/bin:" + m_root_dir + "/bin:$PATH";
    return sexy_proc::exec(path_setup_cmd + " && " + cmd);
}

sexy_proc::temporary_path::temporary_path(const std::string &new_path) {
    std::filesystem::create_directory(new_path);
    m_prev_path = std::filesystem::current_path();
    std::filesystem::current_path(new_path);
}

sexy_proc::temporary_path::~temporary_path() { std::filesystem::current_path(m_prev_path); }


bool sexy_proc::download_package(const std::string &package, bool forced, bool with_dependencies) {
    std::cout << "Downloading pack .:. " << wall_e::color::BrightCyan(package) << "\n";
    if(check_in_root(package) && !forced) {
        std::cout << "                 ::: " << wall_e::color::BrightGreen("Found in root.") << "\n";
        return true;
    }
    if(contains_meta(package)) {
        std::cout << "                 ::: " << wall_e::color::BrightYellow("Already downloaded.") << "\n";
        return true;
    }

    if(with_dependencies) {
        std::list<std::string> packs_to_install;
        if(list_dependencies(package, &packs_to_install)) {
            for(const auto &p : packs_to_install) {
                if(!download_package(p, forced, false)) {
                    return false;
                }
            }
        } else {
            std::cerr << "                 ::: " << wall_e::color::BrightRed("Getting dependencies failed.") << "\n";
            return false;
        }
    }

    if(download_deb(package)) {
        std::cout << "                 ::: " << wall_e::color::BrightGreen("Success: ", package) << "\n";
        return true;
    } else {
        std::cerr << "                 ::: " << wall_e::color::BrightRed("Downloading failed: ") << "\n";
        return false;
    }
}

bool sexy_proc::contains_meta(const std::string &package, const std::string &path) {
    std::ifstream imeta(path, std::ios::in);
    if(imeta.is_open()) {
        while(!imeta.eof()) {
            std::string line;
            std::getline(imeta, line, '\n');
            if(line == package) {
                imeta.close();
                return true;
            }
        }
    }
    imeta.close();
    return false;
}

bool sexy_proc::append_meta(const std::string &package, const std::string &path) {
    if(contains_meta(package))
        return false;

    std::ofstream (path, std::ios::app) << package << std::endl;
    return true;
}

void sexy_proc::foreach_meta(const std::function<void (const std::string &)> &callback, const std::string &path) {
    std::ifstream imeta(path, std::ios::in);
    if(imeta.is_open()) {
        while(!imeta.eof()) {
            std::string line;
            std::getline(imeta, line, '\n');
            callback(line);
        }
    }
}

void sexy_proc::local_install() {
    static const std::string meta_file_name = "installed.list";
    static const std::string deb_dir = "./deb";
    if(std::filesystem::exists(deb_dir) && std::filesystem::is_directory(deb_dir)) {
        std::filesystem::recursive_directory_iterator it(deb_dir), end;
        while (it != end) {
            if(it->exists()) {
                const auto pname = it->path().string();
                if(!contains_meta(pname, meta_file_name)) {
                    std::cout << wall_e::color::BrightMagenta("Unpucking pack..: ", pname) << "\n";
                    const auto res = sexy_proc::exec("dpkg-deb --extract " + pname + " ./");
                    if(res.err.size() == 0) {
                        append_meta(pname, meta_file_name);
                        std::cout << wall_e::color::BrightGreen("Pack unpucked: ", pname) << "\n";
                    } else {
                        std::cout << wall_e::color::BrightRed("Unpucking failed. Pack: ", pname, " E:", res.err) << "\n";
                    }
                }
            }
            ++it;
        }
    }
}

std::string sexy_proc::home_directory() {
    struct passwd *pw = getpwuid(getuid());
    if(pw)
        return pw->pw_dir;
    return {};
}

bool sexy_proc::check_in_root(const std::string &package) {
    const auto out = sexy_proc::exec("dpkg -s " + package).out;
    const auto i = out.find("Status: install ok installed");
    return i >= 0 && i < out.size();
}

bool sexy_proc::list_dependencies(const std::string &package, std::list<std::string> *list, const std::vector<std::string> &args) {
    static const std::string filter_sort_cmd = " | grep \"^\\w\" | sort -u";

    std::string joined_args;
    for(const auto& a : args)
        joined_args += a + " ";

    const auto search_res = sexy_proc::exec("apt-cache depends " + joined_args + package + filter_sort_cmd);
    if(search_res.err.size() == 0) {
        std::istringstream ss(search_res.out);
        while (!ss.eof()) {
            std::string line;
            std::getline(ss, line, '\n');
            if(line.size() > 0) {
                if(line != package) {
                    std::cout << "DEP: " << line << "\n";
                    list->push_back(line);
                } else {
                    std::cout << wall_e::color::BrightYellow("DUBLICATE: ", line) << "\n";
                }
            }
        }
        return true;
    }
    return false;
}

bool sexy_proc::download_deb(const std::string &package) {
    std::filesystem::create_directories("./deb");
    std::filesystem::current_path("./deb");
    const auto download_res = sexy_proc::exec("apt-get download " + package);
    std::filesystem::current_path("..");
    if(download_res.err.size() == 0) {
        append_meta(package);
        return true;
    }
    return false;
}

bool sexy_proc::clear_meta(const std::string &path) {
    return std::filesystem::remove(path);
}

bool sexy_proc::force_install_package(const std::string &package) {
    if(download_package(package, true)) {
        local_install();
        return true;
    }
    return false;
}

std::string sexy_proc::find_pack_by_cmd(const std::string &cmd) {
    const static std::vector<std::string> cmds = { "/usr/lib/command-not-found", "/usr/share/command-not-found/command-not-found" };
    for(const auto &c : cmds) {
        if(std::filesystem::exists(c)) {
            const auto res = sexy_proc::exec(c + " " + cmd);
            if(res.err.size() > 0) {
                std::istringstream ss(res.err);
                while (!ss.eof()) {
                    std::string line;
                    std::getline(ss, line, '\n');
                    const auto pos = line.find("apt");
                    if(pos >= 0 && pos < line.size()) {
                        const auto begin = line.find("install") + sizeof ("install");
                        if(begin >= 0 && begin < line.size()) {
                            const auto end = line.find(' ', begin);
                            if(end >= 0 && end <= line.size()) {
                                return line.substr(begin, end - begin);
                            } else {
                                return line.substr(begin, line.size() - begin);
                            }
                        }
                    }
                }
            }
        }
    }
    return {};
}
