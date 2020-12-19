#ifndef EXEC_CMD_H
#define EXEC_CMD_H

#include <string>
#include <vector>
#include <functional>
#include <list>

namespace sexy_proc {

struct cpipe_container {
    typedef int file_des;
    typedef file_des cpipe[2];
    typedef std::vector<cpipe> cpipe_vector;
private:
    cpipe_vector pipes;
    bool closed = false;
    std::string __read_all(file_des fd);
public:
    cpipe_container(size_t count);
    inline file_des &rfile(size_t index) { return pipes[index][0]; }
    inline file_des &wfile(size_t index) { return pipes[index][1]; }
    inline file_des rfile(size_t index) const { return pipes[index][0]; }
    inline file_des wfile(size_t index) const { return pipes[index][1]; }
    inline void rclose(size_t index);
    inline void wclose(size_t index);
    inline void rdup2(size_t index, file_des fd);
    inline void wdup2(size_t index, file_des fd);
    inline size_t size() const { return pipes.size(); }
    inline std::string read_all(size_t index) { return __read_all(rfile(index)); }
    inline std::string read_all_reversed(size_t index) { return __read_all(wfile(index)); }
    void close();
    inline ~cpipe_container() { close(); }
};


struct process_result {
    std::string out;
    std::string err;
    int code;
    bool valid = false;
};

std::ostream &operator<<(std::ostream& stream, const process_result& r);

process_result fork(const std::function<int()> &callback);

inline process_result exec(const std::string &cmd) { return fork([cmd]{ return std::system(cmd.c_str()); }); }


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
public:
    environment(const std::string &root_dir) { m_root_dir = root_dir; }
    bool install_package(const std::string &package) const;
    bool force_install_package(const std::string &package) const;
    void local_install() const;
    process_result exec(const std::string &cmd) const;
    process_result exec(const std::string &cmd, const std::string &package) const;
    process_result auto_exec(const std::string &cmd) const;
    std::string root_dir() const;
    bool clear() const;
};

static inline const environment home = environment(home_directory() + "/.sexy_proc");

}

#endif // EXEC_CMD_H
