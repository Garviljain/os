#include <iostream>
#include <vector>
#include <string>
#include <filesystem>
#include <cstdlib>
#include <memory>

using namespace std;
namespace fs = std::filesystem;

struct Node {
	string name;
	bool is_directory;
	string permissions;
	size_t size;
	vector<shared_ptr<Node>> children;

	Node(const string& name, bool is_directory, const string& permissions, size_t size)
    	: name(name), is_directory(is_directory), permissions(permissions), size(size) {}
};

class DirectoryTree {
public:
	explicit DirectoryTree(const fs::path& root) : root_path(root) {
    	build_tree(root, root_node);
	}

	void print_tree(bool show_all, bool show_details, int level = 0) {
    	print_node(root_node, show_all, show_details, level);
	}

private:
	fs::path root_path;
	shared_ptr<Node> root_node = make_shared<Node>(root_path.filename().string(), true, "-", 0);
	/*
	void build_tree(const fs::path& path, shared_ptr<Node>& node) {
   
	std::vector<fs::directory_entry> entries;

    
	for (const auto& entry : fs::directory_iterator(path)) {
    	entries.push_back(entry);
	}

 
	std::sort(entries.begin(), entries.end(), [](const fs::directory_entry& a, const fs::directory_entry& b) {
    	return a.path().filename().string() < b.path().filename().string();
	});

 
	for (const auto& entry : entries) {
    	if (entry.is_directory()) {
        	auto child = make_shared<Node>(entry.path().filename().string(), true, get_permissions(entry.path()), 0);
        	build_tree(entry.path(), child); // Recursively build the tree for directories
        	node->children.push_back(child); // Add child to the current node's children
    	} else {
        	auto child = make_shared<Node>(entry.path().filename().string(), false, get_permissions(entry.path()), fs::file_size(entry.path()));
        	node->children.push_back(child); // Add child to the current node's children
    	}
	}
}*/

	void build_tree(const fs::path& path, shared_ptr<Node>& node) {
    	for (const auto& entry : fs::directory_iterator(path)) {
        	if (entry.is_directory()) {
            	auto child = make_shared<Node>(entry.path().filename().string(), true, get_permissions(entry.path()), 0);
            	build_tree(entry.path(), child);
            	node->children.push_back(child);
        	} else {
            	auto child = make_shared<Node>(entry.path().filename().string(), false, get_permissions(entry.path()), fs::file_size(entry.path()));
            	node->children.push_back(child);
        	}
    	}
	}
    

	string get_permissions(const fs::path& path) {
 	 
    	return "-rwxr-xr-x";
	}

	void print_node(const shared_ptr<Node>& node, bool show_all, bool show_details, int level) {
    	if (!show_all && node->name.find('.') == 0) return;

    	for (int i = 0; i < level; ++i) cout << "  ";
    	if (show_details) {
        	cout << (node->is_directory ? "[DIR] " : "[FILE] ") << node->name << " (" << node->size << " bytes, " << node->permissions << ")\n";
    	} else {
        	cout << node->name << "\n";
    	}

    	for (const auto& child : node->children) {
        	print_node(child, show_all, show_details, level + 1);
    	}
	}
};

int main(int argc, char* argv[]) {
	if (argc < 2) {
    	cerr << "Usage: " << argv[0] << " <directory_path> [-a] [-d] [-f] [-l] [-x] [-P]\n";
    	return 1;
	}

	fs::path directory_path = argv[1];
	DirectoryTree tree(directory_path);

	bool show_all = false;
	bool show_details = false;

	for (int i = 2; i < argc; ++i) {
    	string arg = argv[i];
    	if (arg == "-a") show_all = true;
    	if (arg == "-d") show_details = true;
	}

	tree.print_tree(show_all, show_details);

	return 0;
}
