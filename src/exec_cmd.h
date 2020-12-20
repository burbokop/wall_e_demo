#ifndef EXEC_CMD_H
#define EXEC_CMD_H

#include <string>
#include <vector>
#include <functional>
#include <list>
#include <sproc/src/fork.h>

namespace sexy_proc {

inline sproc::process_result exec(const std::string &cmd) {
    return sproc::fork([cmd]{
        return std::system(cmd.c_str());
    });
}

bool contains_meta(const std::string& package, const std::string &path = "meta.list");
bool append_meta(const std::string& package, const std::string &path = "meta.list");
void foreach_meta(const std::function<void(const std::string&)>& callback, const std::string &path = "meta.list");
bool clear_meta(const std::string &path = "meta.list");

bool download_deb(const std::string& package);
bool list_dependencies(const std::string& package, std::list<std::string> *list, const std::vector<std::string> &args = {
        "--recurse",
        "--no-recommends",
        "--no-suggests",
        "--no-conflicts",
        "--no-breaks",
        "--no-replaces",
        "--no-enhances"
});
bool check_in_root(const std::string& package);
bool download_package(const std::string& package, bool forced = false, bool with_dependencies = true);
void local_install();
bool install_package(const std::string &package);
bool force_install_package(const std::string &package);
std::string home_directory();
std::string find_pack_by_cmd(const std::string& cmd);

class temporary_path {
    std::string m_prev_path;
public:
    temporary_path(const std::string &new_path);
    ~temporary_path();
};

class environment {
    std::string m_root_dir;
    bool m_forced = false;
public:
    environment(const std::string &root_dir, bool forced = false) { m_root_dir = root_dir; m_forced = forced; }
    bool install_package(const std::string &package) const;
    bool force_install_package(const std::string &package) const;
    void local_install() const;
    void local_install(const std::string &package) const;
    sproc::process_result exec(const std::string &cmd) const;
    sproc::process_result exec(const std::string &cmd, const std::string &package) const;
    sproc::process_result auto_exec(const std::string &cmd) const;
    std::string root_dir() const;
    bool clear() const;
    bool forced() const;
};

static inline const environment home = environment(home_directory() + "/.sexy_proc");
static inline const environment forced_home = environment(home_directory() + "/.sexy_proc_forced", true);

inline sproc::process_result system(const std::string& cmd, bool forced = true) {
    if(forced) {
        return forced_home.auto_exec(cmd);
    } else {
        return home.auto_exec(cmd);
    }
}

}

#endif // EXEC_CMD_H
