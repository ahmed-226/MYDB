import subprocess

def run_script(commands):
    """
    Runs the database program with the given commands and captures the output.
    """
    process = subprocess.Popen(
        ["./mydb"],  # Path to the compiled C program
        stdin=subprocess.PIPE,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        text=True  # Ensures input/output is treated as text
    )
    # Send commands to the program
    stdout, stderr = process.communicate("\n".join(commands) + "\n")
    return stdout.splitlines()

def test_insert_and_select():
    """
    Test inserting and selecting a row from the database.
    """
    result = run_script([
        "insert 1 user1 person1@example.com",
        "select",
        ".exit",
    ])
    expected = [
        "db > Executed.",
        "db > (1, user1, person1@example.com)",
        "Executed.",
        "db > ",
    ]
    assert result == expected, f"Expected {expected}, but got {result}"

def test_table_full_error():
    """
    Test that the database prints an error message when the table is full.
    """
    script = [f"insert {i} user{i} person{i}@example.com" for i in range(1, 1402)]
    script.append(".exit")
    result = run_script(script)
    assert result[-2] == "db > Error: Table full.", f"Expected 'db > Error: Table full.', but got {result[-2]}"

def test_maximum_length_strings():
    """
    Test inserting strings that are the maximum allowed length.
    """
    long_username = "a" * 32
    long_email = "a" * 255
    script = [
        f"insert 1 {long_username} {long_email}",
        "select",
        ".exit",
    ]
    result = run_script(script)
    expected = [
        "db > Executed.",
        f"db > (1, {long_username}, {long_email})",
        "Executed.",
        "db > ",
    ]
    assert result == expected, f"Expected {expected}, but got {result}"


def test_string_too_long_error():
    """
    Test that the database prints an error message when strings are too long.
    """
    long_username = "a" * 33  # Exceeds COLUMN_USERNAME_SIZE (32)
    long_email = "a" * 256    # Exceeds COLUMN_EMAIL_SIZE (255)
    script = [
        f"insert 1 {long_username} {long_email}",
        "select",
        ".exit",
    ]
    result = run_script(script)
    expected = [
        "db > String is too long.",
        "db > Executed.",
        "db > ",
    ]
    assert result[:3] == expected, f"Expected {expected}, but got {result[:3]}"

    
if __name__ == "__main__":
    test_insert_and_select()
    test_table_full_error()
    test_string_too_long_error()
    test_maximum_length_strings()
    print("All tests passed!")