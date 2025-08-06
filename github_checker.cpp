#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <iomanip>
#include <algorithm>
#include <memory>

#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
    #define CURL_STATICLIB
#else
    #include <termios.h>
    #include <unistd.h>
#endif

#include <curl/curl.h>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct WriteCallbackData {
    std::string data;
};

// Callback function to write response data
size_t WriteCallback(void* contents, size_t size, size_t nmemb, WriteCallbackData* data) {
    size_t totalSize = size * nmemb;
    data->data.append((char*)contents, totalSize);
    return totalSize;
}

// Cross-platform secure password input
std::string getPassword() {
    std::string password;
    char ch;
    
#ifdef _WIN32
    std::cout << "Enter GitHub password/token: ";
    while ((ch = _getch()) != '\r') {
        if (ch == '\b' && !password.empty()) {
            password.pop_back();
            std::cout << "\b \b";
        } else if (ch != '\b') {
            password += ch;
            std::cout << '*';
        }
    }
    std::cout << std::endl;
#else
    struct termios old_termios, new_termios;
    tcgetattr(STDIN_FILENO, &old_termios);
    new_termios = old_termios;
    new_termios.c_lflag &= ~(ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_termios);
    
    std::cout << "Enter GitHub password/token: ";
    std::getline(std::cin, password);
    
    tcsetattr(STDIN_FILENO, TCSANOW, &old_termios);
    std::cout << std::endl;
#endif
    
    return password;
}

// Function to make GitHub API request
std::string makeGitHubRequest(const std::string& url, const std::string& username, const std::string& password) {
    CURL* curl;
    CURLcode res;
    WriteCallbackData data;
    
    curl = curl_easy_init();
    if (!curl) {
        throw std::runtime_error("Failed to initialize CURL");
    }
    
    // Set URL
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    
    // Set authentication
    std::string auth = username + ":" + password;
    curl_easy_setopt(curl, CURLOPT_USERPWD, auth.c_str());
    
    // Set user agent (required by GitHub API)
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "GitHubFollowerChecker/1.0");
    
    // Set callback function
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
    
    // Follow redirects
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    
    // Set timeout
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);
    
    // Perform request
    res = curl_easy_perform(curl);
    
    // Check for errors
    if (res != CURLE_OK) {
        curl_easy_cleanup(curl);
        throw std::runtime_error("CURL request failed: " + std::string(curl_easy_strerror(res)));
    }
    
    // Check HTTP response code
    long response_code;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response_code);
    
    curl_easy_cleanup(curl);
    
    if (response_code == 401) {
        throw std::runtime_error("Authentication failed. Please check your credentials.");
    } else if (response_code == 404) {
        throw std::runtime_error("User not found.");
    } else if (response_code != 200) {
        throw std::runtime_error("HTTP Error: " + std::to_string(response_code));
    }
    
    return data.data;
}

// Function to get all users from paginated API
std::vector<std::string> getAllUsers(const std::string& baseUrl, const std::string& username, const std::string& password) {
    std::vector<std::string> users;
    int page = 1;
    const int perPage = 100;
    
    while (true) {
        std::string url = baseUrl + "?page=" + std::to_string(page) + "&per_page=" + std::to_string(perPage);
        std::string response = makeGitHubRequest(url, username, password);
        
        json jsonResponse = json::parse(response);
        
        if (jsonResponse.empty()) {
            break;
        }
        
        for (const auto& user : jsonResponse) {
            users.push_back(user["login"]);
        }
        
        if (jsonResponse.size() < perPage) {
            break;
        }
        
        page++;
    }
    
    return users;
}

// Function to display results in tabular format
void displayResults(const std::vector<std::string>& notFollowingBack, 
                   const std::set<std::string>& followers,
                   const std::set<std::string>& following) {
    
    if (notFollowingBack.empty()) {
        std::cout << "\n🎉 Great news! All your followers are following you back!\n" << std::endl;
        return;
    }
    
    std::cout << "\n📊 Users who are not following you back:\n" << std::endl;
    
    // Table headers
    std::cout << std::left << std::setw(25) << "Names" 
              << std::setw(15) << "Followers" 
              << std::setw(15) << "Following" << std::endl;
    std::cout << std::string(55, '-') << std::endl;
    
    // Table data
    for (const auto& user : notFollowingBack) {
        bool isFollower = followers.count(user) > 0;
        bool isFollowing = following.count(user) > 0;
        
        std::cout << std::left << std::setw(25) << user
                  << std::setw(15) << (isFollower ? "✓" : "✗")
                  << std::setw(15) << (isFollowing ? "✓" : "✗") << std::endl;
    }
    
    std::cout << "\nSummary:" << std::endl;
    std::cout << "- Total followers: " << followers.size() << std::endl;
    std::cout << "- Total following: " << following.size() << std::endl;
    std::cout << "- Not following back: " << notFollowingBack.size() << std::endl;
}

int main() {
    try {
        // Initialize CURL globally
        curl_global_init(CURL_GLOBAL_DEFAULT);
        
        std::string username, password;
        
        // Get credentials interactively
        std::cout << "🔐 GitHub Follower Checker" << std::endl;
        std::cout << "=========================\n" << std::endl;
        std::cout << "Enter GitHub username: ";
        std::getline(std::cin, username);
        
        password = getPassword();
        
        if (username.empty() || password.empty()) {
            std::cerr << "❌ Username and password cannot be empty!" << std::endl;
            return 1;
        }
        
        std::cout << "\n🔍 Fetching your GitHub data..." << std::endl;
        
        // Get followers and following
        std::string followersUrl = "https://api.github.com/users/" + username + "/followers";
        std::string followingUrl = "https://api.github.com/users/" + username + "/following";
        
        std::cout << "📥 Getting followers..." << std::endl;
        std::vector<std::string> followersVec = getAllUsers(followersUrl, username, password);
        std::set<std::string> followers(followersVec.begin(), followersVec.end());
        
        std::cout << "📤 Getting following..." << std::endl;
        std::vector<std::string> followingVec = getAllUsers(followingUrl, username, password);
        std::set<std::string> following(followingVec.begin(), followingVec.end());
        
        // Find users you're following but who aren't following you back
        std::vector<std::string> notFollowingBack;
        for (const auto& user : following) {
            if (followers.find(user) == followers.end()) {
                notFollowingBack.push_back(user);
            }
        }
        
        // Sort alphabetically for better presentation
        std::sort(notFollowingBack.begin(), notFollowingBack.end());
        
        // Display results
        displayResults(notFollowingBack, followers, following);
        
        // Cleanup
        curl_global_cleanup();
        
    } catch (const std::exception& e) {
        std::cerr << "❌ Error: " << e.what() << std::endl;
        curl_global_cleanup();
        return 1;
    }
    
    return 0;
}