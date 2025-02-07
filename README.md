# ATM Management System

A comprehensive ATM (Automated Teller Machine) management system written in C, featuring user authentication, account management, secure transactions, and real-time notifications.

## Features

- **User Authentication**
  - Secure login/registration system
  - Password encryption using SHA-256
  - SQLite database storage

- **Account Management**
  - Create new accounts
  - Check account details
  - Update account information
  - Remove existing accounts
  - View list of owned accounts

- **Transaction System**
  - Deposit funds
  - Withdraw funds
  - Transfer between accounts
  - Real-time balance updates
  - Transaction history

- **Security Features**
  - Encrypted password storage
  - Secure session management
  - Input validation
  - Error handling

- **Real-time Notifications**
  - Account transfer notifications
  - Transaction alerts
  - Inter-process communication using pipes

## Prerequisites

- GCC Compiler
- SQLite3
- OpenSSL
- Make

### Installing Dependencies (Ubuntu/Debian)

```bash
sudo apt-get update
sudo apt-get install -y libsqlite3-dev libssl-dev
```

## Building the Project

1. Clone the repository:
```bash
git clone https://github.com/Stella-Achar-Oiro/atm-management-system.git
cd atm-management-system
```

2. Create necessary directories and files:
```bash
make setup
```

3. Build the project:
```bash
make
```

## Running the Program

After building, run the ATM system:
```bash
./atm
```

## Project Structure

```
.
├── src/
│   ├── main.c           # Main program entry
│   ├── auth.c           # Authentication functions
│   ├── system.c         # Core system functions
│   ├── notification.c   # Notification system
│   ├── database.c       # Database operations
│   ├── encryption.c     # Password encryption
│   └── header.h         # Header declarations
├── data/                # Data storage
│   ├── users.txt        # User records
│   └── records.txt      # Account records
├── build/               # Compiled objects
├── Makefile            # Build configuration
└── README.md           # This file
```

## Available Commands

- `make`: Build the project
- `make setup`: Create necessary directories and files
- `make clean`: Remove built files
- `make cleanall`: Remove all generated files including data
- `make install-deps`: Install required system libraries

## Features in Detail

### Account Types
- Current Account
- Savings Account (7% interest)
- Fixed01 (1 year, 4% interest)
- Fixed02 (2 year, 5% interest)
- Fixed03 (3 year, 8% interest)

### Transaction Rules
- Fixed accounts cannot perform transactions
- Savings accounts earn monthly interest
- Current accounts have no transaction limits

## Security Notes

- Passwords are hashed using SHA-256
- User data is stored in SQLite database
- File operations use secure temporary files
- Input validation prevents buffer overflows
- Process isolation for notification system

## Contributing

1. Fork the repository
2. Create your feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add some amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Open a Pull Request

## License

This project is licensed under the MIT License - see the LICENSE file for details.

## Author

- Stella Achar Oiro

## Acknowledgments

- Original project requirements from system design documentation
- OpenSSL and SQLite3 documentation
- C Programming best practices guides