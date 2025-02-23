# ATM Management System

A secure and feature-rich ATM (Automated Teller Machine) management system implemented in C with SQLite database integration and enhanced UI.

## Features

### User Management
- Secure user registration with SHA-512 password hashing
- Login authentication with encrypted passwords
- Automatic password upgrade from plain text to hashed (backward compatibility)
- Color-coded user interface for better user experience

### Account Management
- Multiple account types support:
  - Savings (7% annual interest)
  - Current (no interest)
  - Fixed Deposits:
    - 1 year: 4% interest
    - 2 years: 5% interest
    - 3 years: 8% interest
- Account operations:
  - Create new accounts
  - View account details and balances
  - Update account information
  - List all owned accounts
  - Delete accounts
  - Transfer account ownership

### Transaction Management
- Secure deposit operations
- Withdraw funds with balance verification
- Transaction history logging
- Real-time balance updates
- Interest calculations based on account type

## Technical Stack

### Core Technologies
- Language: C
- Database: SQLite3
- Encryption: SHA-512 via libcrypt
- Build System: Make

### Project Structure
```
.
├── LICENSE
├── Makefile
├── README.md
├── atm                    # Main executable
├── data/
│   └── atm.db            # SQLite database
├── obj/                  # Compiled objects
│   ├── *.o              # Object files
│   └── *.d              # Dependency files
└── src/
    ├── database.c        # Database operations
    ├── header.h          # Main header
    ├── interface.c       # User interface
    ├── main.c           # Entry point
    ├── system.c         # Business logic
    └── ui.h             # UI components
```

### Dependencies
- GCC compiler
- SQLite3 development libraries
- libcrypt development libraries
- ANSI terminal support

## Getting Started

### Installation

1. Install required dependencies:
```bash
# Ubuntu/Debian
sudo apt-get install gcc make libsqlite3-dev libcrypt-dev

# Fedora
sudo dnf install gcc make sqlite-devel libxcrypt-devel

# MacOS (using Homebrew)
brew install sqlite3 openssl
```

2. Clone the repository:
```bash
git clone https://github.com/yourusername/atm-management-system.git
cd atm-management-system
```

3. Build the project:
```bash
make clean    # Clean any previous builds
make         # Build the project
```

### Running the Application
```bash
./atm        # Run the ATM system
```

## Usage Guide

### First Time Setup
1. Launch the application
2. Choose Register option
3. Enter username and password
4. Login with created credentials

### Account Operations
1. Create Account:
   - Select account type
   - Enter initial deposit
   - Provide required details

2. Transactions:
   - Select Make Transaction
   - Choose deposit or withdraw
   - Enter amount
   - Confirm transaction

3. Account Management:
   - View account details
   - Update information
   - Transfer ownership
   - Delete accounts

## Security Features
- SHA-512 password hashing
- Prepared SQL statements preventing injection
- Input validation and sanitization
- Hidden password entry
- Secure memory handling
- Transaction verification

## Development

### Building for Development
```bash
make debug   # Build with debug symbols
```

### Available Make Commands
- `make`: Build release version
- `make debug`: Build debug version
- `make clean`: Clean build files
- `make run`: Build and run
- `make help`: Show all commands

## Contributing
1. Fork the repository
2. Create your feature branch
3. Commit your changes
4. Push to the branch
5. Create a Pull Request

## Future Roadmap
- Multi-factor authentication
- Transaction limits
- PDF statement generation
- Joint accounts
- Admin dashboard
- Automated backups
- Mobile number verification
- Email notifications

## License
This project is licensed under the MIT License - see the LICENSE file for details.

## Acknowledgments
- SQLite for database management
- GNU C Library for encryption
- ANSI for terminal coloring support