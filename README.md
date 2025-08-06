
[![DOI](https://zenodo.org/badge/1033021985.svg)](https://doi.org/10.5281/zenodo.16751339)

# github-follower-checker
A cross-platform C++ application that analyzes your GitHub followers and identifies users who are not following you back. This tool provides an interactive terminal interface with secure authentication and presents results in a clean tabular format.

## Features

- **Cross-Platform Compatibility**: Works seamlessly on Windows, macOS, and Linux
- **Secure Authentication**: Masked password/token input for enhanced security
- **Interactive Interface**: User-friendly prompts for username and credentials
- **GitHub API Integration**: Uses official GitHub REST API with proper authentication
- **Pagination Support**: Handles accounts with large numbers of followers/following
- **Tabular Output**: Clean, organized display with Names, Followers, and Following columns
- **Comprehensive Error Handling**: Robust error messages and validation
- **Real-time Feedback**: Progress indicators during data fetching

## System Architecture

The application consists of several key components:

1. **Authentication Module**: Secure, cross-platform password input handling
2. **GitHub API Client**: RESTful API communication with pagination support  
3. **Data Processing Engine**: Follower/following analysis and comparison
4. **Output Formatter**: Clean tabular display with summary statistics

## Prerequisites

### Required Dependencies

- **C++17 compatible compiler** (GCC 7+, Clang 5+, MSVC 2017+)
- **libcurl** - HTTP client library for API requests
- **nlohmann/json** - Modern JSON parsing library

### Platform-Specific Requirements

#### Linux (Ubuntu/Debian)
```bash
sudo apt-get update
sudo apt-get install build-essential libcurl4-openssl-dev nlohmann-json3-dev
```

#### macOS
```bash
# Install Homebrew if not already installed
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"

# Install dependencies
brew install curl nlohmann-json
```

#### Windows
```bash
# Using vcpkg package manager
vcpkg install curl nlohmann-json
```

## Installation

### 1. Clone the Repository

```bash
git clone https://github.com/asifpatankar/github-follower-checker.git
cd github-follower-checker
```

### 2. Compile the Application

#### Linux/macOS
```bash
g++ -std=c++17 github_checker.cpp -lcurl -o github_checker
```

#### Windows (Visual Studio)
```bash
g++ -std=c++17 github_checker.cpp -lcurl -o github_checker.exe
```

#### Windows (MinGW)
```bash
g++ -std=c++17 github_checker.cpp -lcurl -lws2_32 -o github_checker.exe
```

## Configuration

### GitHub Authentication

For optimal security and rate limiting, use a **Personal Access Token** instead of your password:

1. Visit [GitHub Token Settings](https://github.com/settings/tokens)
2. Click "Generate new token (classic)"
3. Select the `read:user` scope (minimal required permission)
4. Copy the generated token for use with the application

## Usage

### 1. Run the Application

```bash
# Linux/macOS
./github_checker

# Windows
github_checker.exe
```

### 2. Interactive Authentication

The application will prompt you for:
- **GitHub username**: Your GitHub username
- **Password/Token**: Your password or Personal Access Token (input will be masked)

### 3. Data Analysis

The application will automatically:
1. Fetch your followers list
2. Fetch your following list  
3. Compare the data to identify non-mutual follows
4. Display results in tabular format

### 4. Sample Output

```
🔐 GitHub Follower Checker
=========================

Enter GitHub username: asifpatankar
Enter GitHub password/token: ********

🔍 Fetching your GitHub data...
📥 Getting followers...
📤 Getting following...

📊 Users who are not following you back:

Names                    Followers       Following      
-------------------------------------------------------
AfrojSatwilkar           ✗               ✓             

Summary:
- Total followers: 42
- Total following: 43
- Not following back: 1
```

## API Rate Limits

The GitHub API has the following rate limits:
- **Authenticated requests**: 5,000 requests per hour
- **Unauthenticated requests**: 60 requests per hour

This application uses authenticated requests and implements pagination to efficiently handle large follower/following lists while staying within rate limits.

## Security Considerations

### Best Practices

1. **Use Personal Access Tokens**: More secure than passwords and can be easily revoked
2. **Minimal Permissions**: Only request `read:user` scope for basic functionality
3. **Secure Storage**: Never store credentials in plain text or version control
4. **Network Security**: All API communications use HTTPS encryption

### Token Management

- Store tokens securely using your system's credential manager
- Rotate tokens periodically for enhanced security
- Revoke unused tokens immediately

## Troubleshooting

### Common Issues

#### Authentication Failed (HTTP 401)
- Verify your username and password/token are correct
- Ensure your token has the `read:user` permission
- Check if your token has expired

#### User Not Found (HTTP 404)
- Confirm the GitHub username exists and is spelled correctly
- Ensure the user profile is public

#### Network Connection Issues
- Check your internet connection
- Verify firewall settings allow HTTPS connections
- Try running from a different network location

#### Compilation Errors
```bash
# If libcurl is not found
export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:$PKG_CONFIG_PATH

# If nlohmann/json headers are missing
sudo ln -s /usr/include/nlohmann /usr/local/include/nlohmann
```

## Advanced Configuration

### Customizing Output Format

Modify the `displayResults()` function to change:
- Column widths and alignment
- Display symbols (✓/✗ vs True/False)
- Summary statistics format

### Adding Features

The modular design allows easy extension:
- Export results to CSV/JSON
- Add filtering options
- Include follower statistics
- Implement caching for better performance

## Contributing

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## Performance Metrics

- **Memory Usage**: ~2MB for typical accounts (<1000 followers)
- **Processing Time**: ~5-15 seconds depending on follower count
- **Network Efficiency**: Minimized API calls through pagination
- **Cross-Platform**: Consistent performance across all supported OS

## Licensing

This project is available under a dual licensing model:

### Non-Commercial Use
For academic, personal, and non-commercial use, this project is available under the MIT License. See the [LICENSE](LICENSE) file for details.

### Commercial Use
For commercial use, please see [COMMERCIAL_LICENSE.md](COMMERCIAL_LICENSE.md) for detailed terms and contact information.

All commercial inquiries will be handled on a case-by-case basis.

**Built with ❤️ for the GitHub community**

For support, please open an issue or contact asifpatankar@rocketmail.com
