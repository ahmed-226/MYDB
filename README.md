# Simple Database in C

This project implements a simple, file-based database in C, inspired by SQLite's architecture. It supports basic CRUD operations (INSERT and SELECT) and persists data to a file. The database operates as a command-line application, allowing users to interact with it via a REPL (Read-Eval-Print Loop).

## Table of Contents

- Features
- Requirements
- Installation
- Usage
- Database Structure
- Commands
- Implementation Details
- Limitations
- Contributing
- License

## Features

- **Persistent Storage**: Data is stored in a file, allowing data to persist between sessions.
- **Basic SQL-like Commands**: Supports `INSERT` and `SELECT` statements for adding and retrieving records.
- **Meta Commands**: Includes a `.exit` command to close the database gracefully.
- **Error Handling**: Provides feedback for syntax errors, invalid inputs, and table capacity issues.
- **Pager System**: Manages memory efficiently by loading data pages from disk as needed.

## Requirements

- A C compiler (e.g., `gcc`)
- POSIX-compliant operating system (e.g., Linux, macOS, or WSL on Windows)
- Standard C libraries (included in most systems)

## Installation

1. Clone or download the repository containing the source code.

2. Navigate to the project directory.

3. Compile the code using a C compiler:

   ```bash
   gcc -o db main.c
   ```

4. Run the compiled executable, specifying a database file:

   ```bash
   ./db my_database.db
   ```

## Usage

1. Start the database by running the compiled program with a filename:

   ```bash
   ./db my_database.db
   ```

2. You will see a prompt (`db >`) where you can enter commands.

3. Use SQL-like commands to interact with the database or meta commands to control the program.

4. To exit, type `.exit`.

### Example Session

```bash
$ ./db my_database.db
db > insert 1 alice alice@example.com
Executed.
db > insert 2 bob bob@example.com
Executed.
db > select
(1, alice, alice@example.com)
(2, bob, bob@example.com)
Executed.
db > .exit
```

## Database Structure

The database stores records in a table with the following schema:

- **id**: A 32-bit unsigned integer (unique identifier).
- **username**: A string of up to 32 characters.
- **email**: A string of up to 255 characters.

### Storage

- **Rows**: Each record (row) is serialized into a fixed-size structure and stored in pages.
- **Pages**: Data is divided into 4KB pages, with up to 100 pages supported (defined by `TABLE_MAX_PAGES`).
- **File**: The database is stored in a single file, with pages read and written as needed.

## Commands

### SQL-like Commands

- **INSERT**: Adds a new row to the table.

  ```
  insert <id> <username> <email>
  ```

  Example: `insert 1 alice alice@example.com`

- **SELECT**: Retrieves and displays all non-empty rows.

  ```
  select
  ```

### Meta Commands

- **.exit**: Closes the database file and exits the program.

  ```
  .exit
  ```

## Implementation Details

The database is implemented as a simple key-value store with a focus on persistence and basic querying. Key components include:

### Data Structures

- **Row**: Represents a single record with `id`, `username`, and `email` fields.
- **Table**: Manages the collection of rows and references a `Pager` for disk operations.
- **Pager**: Handles file I/O, caching pages in memory to reduce disk access.
- **InputBuffer**: Stores user input for processing commands.
- **Statement**: Represents a parsed SQL command (`INSERT` or `SELECT`).

### Key Functions

- **db_open**: Initializes the database by opening the file and setting up the table.
- **pager_open**: Creates a `Pager` for managing file operations.
- **serialize_row / deserialize_row**: Converts rows to/from a binary format for storage.
- **execute_statement**: Processes `INSERT` or `SELECT` statements.
- **do_meta_command**: Handles meta commands like `.exit`.
- **db_close**: Flushes data to disk, frees memory, and closes the file.

### Memory Management

- The database uses a pager system to load pages into memory only when needed.
- Pages are flushed to disk during `db_close` or when necessary to ensure data persistence.
- Memory is dynamically allocated for the `Table`, `Pager`, and `InputBuffer`, and freed appropriately.

### Error Handling

- **Syntax Errors**: Invalid commands or missing arguments result in a `PREPARE_SYNTAX_ERROR`.
- **String Length**: Usernames or emails exceeding size limits trigger `PREPARE_STRING_TOO_LONG`.
- **Negative IDs**: IDs less than 0 cause a `PREPARE_NEGATIVE_ID`.
- **Table Full**: Attempts to insert into a full table return `EXECUTE_TABLE_FULL`.
- **File Errors**: Issues with file operations (e.g., reading, writing) cause the program to exit with an error message.

## Limitations

- **Single Table**: Only one table is supported.
- **No Updates or Deletes**: Only `INSERT` and `SELECT` operations are implemented.
- **No Indexing**: Lookups are sequential, which may be slow for large datasets.
- **Fixed Schema**: The table structure (`id`, `username`, `email`) is hardcoded.
- **No Concurrency**: The database does not support multiple users or transactions.
- **Limited Error Recovery**: File corruption or unexpected errors may require manual intervention.

## Contributing

Contributions are welcome! To contribute:

1. Fork the repository.
2. Create a new branch for your feature or bug fix.
3. Submit a pull request with a clear description of your changes.

Please ensure your code follows the existing style and includes appropriate comments.

## License

This project is licensed under the MIT License. See the LICENSE file for details.