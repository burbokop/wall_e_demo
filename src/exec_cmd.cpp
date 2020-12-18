#include "exec_cmd.h"

#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>
#include <sstream>
#include <filesystem>
#include "wall_e/src/color.h"
#include <fstream>
#include "wall_e/src/lex.h"
#include <iostream>


namespace clang_interface {
    decltype (close) *__close = close;
    decltype (pipe) *__pipe = pipe;
    decltype (fork) *__fork = fork;
};


template <typename T>
T read_unit(int fd, bool *ok = nullptr) {
    T v;
    const auto size = read(fd, &v, sizeof (T));
    printf("size: %d ", static_cast<int>(size));
    if(ok)
        *ok = (size == sizeof (T));
    return v;
}

sexy_proc::process_result sexy_proc::fork(const std::function<int()> &callback) {
    process_result result;
    constexpr static size_t PIPES_COUNT = 3;
    enum {
        Write = 0,
        Read = 1,
        ReadErr = 2,
    };

    cpipe_container pipes(PIPES_COUNT);
    const auto pid = clang_interface::__fork();
    if(pid) {
        pipes.rclose(Write);
        pipes.wclose(Read);
        pipes.wclose(ReadErr);

        //write(pipes.pipe(PARENT_WRITE_PIPE, WRITE_FD), "gogadoda", 8);

        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            result.valid = true;
            result.code = WEXITSTATUS(status);
            result.out = pipes.read_all(Read);
            result.err = pipes.read_all(ReadErr);
        }
    } else {
        pipes.rdup2(Write, STDIN_FILENO);
        pipes.wdup2(Read, STDOUT_FILENO);
        pipes.wdup2(ReadErr, STDERR_FILENO);
        const auto code = callback();
        pipes.close();
        exit(code);
    }
    return result;
}

std::string sexy_proc::cpipe_container::__read_all(sexy_proc::cpipe_container::file_des fd) {
    std::string result;
    while (true) {
        char c[256];
        auto size = read(fd, &c, sizeof (c));
        if(size > 0) {
            result.append(c, size);
        }
        if(size < static_cast<decltype (size)>(sizeof (c))) {
            break;
        }
    }
    return result;
}

sexy_proc::cpipe_container::cpipe_container(size_t count) {
    pipes = cpipe_vector(count);
    for(size_t i = 0; i < count; ++i)
        clang_interface::__pipe(pipes[i]);
}

void sexy_proc::cpipe_container::rclose(size_t index) { clang_interface::__close(rfile(index)); }

void sexy_proc::cpipe_container::wclose(size_t index) { clang_interface::__close(wfile(index)); }

void sexy_proc::cpipe_container::rdup2(size_t index, sexy_proc::cpipe_container::file_des fd) { dup2(rfile(index), fd); }

void sexy_proc::cpipe_container::wdup2(size_t index, sexy_proc::cpipe_container::file_des fd) { dup2(wfile(index), fd); }

void sexy_proc::cpipe_container::close() {
    if(!closed) {
        for(size_t i = 0; i < pipes.size(); ++i) {
            rclose(i);
            wclose(i);
        }
        closed = true;
    }
}


bool sexy_proc::install_package(const std::string &package) {
    if(download_package(package)) {
        local_install();
        return true;
    }
    return false;
}


std::string sexy_proc::env::root_dir() const {
    return m_root_dir;
}

bool sexy_proc::env::install_package(const std::string &package) {
    temporary_path tmp_p(m_root_dir);
    return sexy_proc::install_package(package);
}


static int aaaaa = [] {
    std::cout << "TEST STARTED...\n";
    sexy_proc::env e("./env");
    e.install_package("clang-10");
    std::cout << "TEST FINISHED.\n";
    return 0;
}();

sexy_proc::temporary_path::temporary_path(const std::string &new_path) {
    std::filesystem::create_directory(new_path);
    m_prev_path = std::filesystem::current_path();
    std::filesystem::current_path(new_path);
}

sexy_proc::temporary_path::~temporary_path() { std::filesystem::current_path(m_prev_path); }


bool sexy_proc::download_package(const std::string &package) {
    std::cout << "Downloading pack..: " << wall_e::color::BrightCyan(package) << "\n";
    if(contains_meta(package)) {
        std::cout << wall_e::color::BrightYellow("Pack..: ") << wall_e::color::BrightCyan(package) << " already downloaded." << "\n";
        return true;
    }

    std::filesystem::create_directories("./deb");
    std::filesystem::current_path("./deb");
    const auto download_res = exec("apt-get download " + package);
    std::filesystem::current_path("..");
    if(download_res.err.size() == 0) {
        append_meta(package);
        const auto search_res = exec("apt-cache rdepends " + package);
        if(search_res.err.size() == 0) {
            std::istringstream ss(search_res.out);
            while (!ss.eof()) {
                std::string line;
                std::getline(ss, line, '\n');
                if(line.size() > 2 && line[0] == ' ' && line[1] == ' ') {
                    const std::string pname = wall_e::lex::trim(line);
                    if(!download_package(pname)) {
                        throw std::runtime_error("Dependency solving failed: " + pname);
                    }
                }
            }
            std::cout << "Downloading finished with code: " << download_res.code << "\n";
            return true;
        } else {
            std::cerr << "Getting dependencies failed: " << search_res.err << "\n";
        }
    } else {
        std::cerr << "Downloading failed: " << download_res.err << "\n";
    }
    return false;
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
    std::filesystem::recursive_directory_iterator it("./deb"), end;
    while (it != end) {
        if(it->exists()) {
            const auto pname = it->path().string();
            if(!contains_meta(pname, meta_file_name)) {
                std::cout << wall_e::color::BrightMagenta("Unpucking pack..: ", pname) << "\n";
                const auto res = exec("dpkg-deb --extract " + pname + " ./");
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
